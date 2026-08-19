from pathlib import Path
from tempfile import TemporaryDirectory

from python import sga_do as app


def test_reglas_lifo_fifo_y_validaciones() -> None:
    with TemporaryDirectory() as folder:
        root = Path(folder)
        app.ALUMNOS_FILE = root / "alumnos.txt"
        app.PROFESORES_FILE = root / "profesores.txt"
        app.CERTIFICADOS_FILE = root / "certificados_pendientes.txt"
        app.ALUMNOS_FILE.write_text(
            "V-101,Ana Silva,ana@email.com,Curso,10,10,10\n"
            "V-202,Carlos Pérez,carlos@email.com,Diplomado,14,14,13\n"
            "V-303,María Gómez,maria@email.com,Bootcamp,20,20,13\n"
            "V-404,Luis Rojas,luis@email.com,Bootcamp,14,15,14\n",
            encoding="utf-8",
        )
        app.PROFESORES_FILE.write_text(
            "P-505,Dr. Pedro Infante,pedro@email.com,Backend,C++ y Algoritmos\n",
            encoding="utf-8",
        )

        sistema = app.SistemaSGADO()
        estados = {alumno.nombre: alumno.estatus() for alumno in sistema.repositorio.alumnos}
        assert estados == {
            "Ana Silva": "APROBADO",
            "Carlos Pérez": "REPROBADO",
            "María Gómez": "REPROBADO",
            "Luis Rojas": "APROBADO",
        }

        alumno = sistema.registrar_alumno("V-999", "Prueba LIFO", "test@email.com", "Curso")
        sistema.registrar_nota(alumno.cedula, 15)
        sistema.deshacer_ultima_nota()
        assert sistema.repositorio.buscar_alumno("V-999").notas == []

        graduandos = sistema.generar_cola_certificados()
        assert [alumno.nombre for alumno in graduandos] == ["Ana Silva", "Luis Rojas"]
        assert "Total de graduandos en cola: 2" in app.CERTIFICADOS_FILE.read_text(encoding="utf-8")

        try:
            app.leer_nota("veinte")
        except app.ValidationError:
            pass
        else:
            raise AssertionError("La entrada no numérica no fue rechazada")


if __name__ == "__main__":
    test_reglas_lifo_fifo_y_validaciones()
    print("Python Core OK: polimorfismo, LIFO, FIFO, persistencia y validaciones")
