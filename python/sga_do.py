from __future__ import annotations

import math
from dataclasses import dataclass, field
from pathlib import Path
from typing import Callable, Optional
from collections import deque

BASE_DIR = Path(__file__).resolve().parent
ALUMNOS_FILE = BASE_DIR / "alumnos.txt"
PROFESORES_FILE = BASE_DIR / "profesores.txt"
CERTIFICADOS_FILE = BASE_DIR / "certificados_pendientes.txt"


class ValidationError(ValueError):
    pass


def validar_nota(valor: float) -> float:
    if not 0 <= valor <= 20:
        raise ValidationError("La nota debe estar entre 0 y 20.")
    return valor


def leer_nota(texto: str) -> float:
    try:
        return validar_nota(float(texto.strip().replace(",", ".")))
    except ValueError as error:
        raise ValidationError("Ingrese un valor numérico válido.") from error


def mostrar_nota(valor: float) -> str:
    return f"{valor:.1f}" if valor % 1 else str(int(valor))


def mostrar_promedio(valor: float) -> str:
    promedio_truncado = math.floor(valor * 10) / 10
    return f"{promedio_truncado:.1f}"


@dataclass
class Persona:
    cedula: str
    nombre: str
    correo: str


class ProgramaAcademico:
    nombre = "Programa"

    def evaluar_aprobacion(self, notas: list[float]) -> bool:
        raise NotImplementedError

    def descripcion_regla(self) -> str:
        raise NotImplementedError


class Curso(ProgramaAcademico):
    nombre = "Curso"

    def evaluar_aprobacion(self, notas: list[float]) -> bool:
        return bool(notas) and sum(notas) / len(notas) >= 10

    def descripcion_regla(self) -> str:
        return "promedio mayor o igual a 10/20"


class Diplomado(ProgramaAcademico):
    nombre = "Diplomado"

    def evaluar_aprobacion(self, notas: list[float]) -> bool:
        return bool(notas) and sum(notas) / len(notas) >= 14

    def descripcion_regla(self) -> str:
        return "promedio mayor o igual a 14/20"


class Bootcamp(ProgramaAcademico):
    nombre = "Bootcamp"

    def evaluar_aprobacion(self, notas: list[float]) -> bool:
        return len(notas) == 3 and all(nota >= 14 for nota in notas)

    def descripcion_regla(self) -> str:
        return "ninguna nota individual menor a 14/20"


PROGRAMAS: dict[str, Callable[[], ProgramaAcademico]] = {
    "curso": Curso,
    "diplomado": Diplomado,
    "bootcamp": Bootcamp,
}


def crear_programa(nombre: str) -> ProgramaAcademico:
    try:
        return PROGRAMAS[nombre.strip().lower()]()
    except KeyError as error:
        raise ValidationError("Seleccione Curso, Diplomado o Bootcamp.") from error


@dataclass
class Alumno(Persona):
    programa: ProgramaAcademico
    notas: list[float] = field(default_factory=list)

    def agregar_nota(self, nota: float) -> None:
        if len(self.notas) >= 3:
            raise ValidationError("Cada alumno puede tener máximo 3 notas.")
        self.notas.append(validar_nota(nota))

    def deshacer_nota(self) -> float:
        if not self.notas:
            raise ValidationError("El alumno no tiene notas para deshacer.")
        return self.notas.pop()

    def promedio(self) -> float:
        return sum(self.notas) / len(self.notas) if self.notas else 0

    def esta_aprobado(self) -> bool:
        return self.programa.evaluar_aprobacion(self.notas)

    def estatus(self) -> str:
        if len(self.notas) < 3:
            return "PENDIENTE"
        return "APROBADO" if self.esta_aprobado() else "REPROBADO"

    def to_line(self) -> str:
        notas = self.notas + [0] * (3 - len(self.notas))
        return ",".join(
            [
                self.cedula,
                self.nombre,
                self.correo,
                self.programa.nombre,
                *(mostrar_nota(nota) for nota in notas),
            ]
        )


@dataclass
class Profesor(Persona):
    especialidad: str
    materia: str

    def to_line(self) -> str:
        return ",".join([self.cedula, self.nombre, self.correo, self.especialidad, self.materia])


@dataclass
class RegistroNota:
    cedula: str
    nota: float


class Repositorio:
    def __init__(self) -> None:
        self.alumnos: list[Alumno] = []
        self.profesores: list[Profesor] = []
        self.cargar()

    def preparar_archivos(self) -> None:
        ALUMNOS_FILE.touch(exist_ok=True)
        PROFESORES_FILE.touch(exist_ok=True)
        CERTIFICADOS_FILE.touch(exist_ok=True)

    def cargar(self) -> None:
        self.preparar_archivos()
        self.alumnos = self._cargar_alumnos()
        self.profesores = self._cargar_profesores()

    def _cargar_alumnos(self) -> list[Alumno]:
        alumnos: list[Alumno] = []
        for linea in ALUMNOS_FILE.read_text(encoding="utf-8").splitlines():
            if not linea.strip():
                continue
            partes = [parte.strip() for parte in linea.split(",")]
            if len(partes) != 7:
                continue
            try:
                notas = [validar_nota(float(valor)) for valor in partes[4:7]]
                notas_reales = notas if any(notas) else []
                alumnos.append(Alumno(partes[0], partes[1], partes[2], crear_programa(partes[3]), notas_reales))
            except (ValidationError, ValueError):
                continue
        return alumnos

    def _cargar_profesores(self) -> list[Profesor]:
        profesores: list[Profesor] = []
        for linea in PROFESORES_FILE.read_text(encoding="utf-8").splitlines():
            if not linea.strip():
                continue
            partes = [parte.strip() for parte in linea.split(",", maxsplit=4)]
            if len(partes) == 5:
                profesores.append(Profesor(*partes))
        return profesores

    def guardar_alumnos(self) -> None:
        ALUMNOS_FILE.write_text(
            "".join(f"{alumno.to_line()}\n" for alumno in self.alumnos),
            encoding="utf-8",
        )

    def guardar_profesores(self) -> None:
        PROFESORES_FILE.write_text(
            "".join(f"{profesor.to_line()}\n" for profesor in self.profesores),
            encoding="utf-8",
        )

    def buscar_alumno(self, cedula: str) -> Optional[Alumno]:
        return next((alumno for alumno in self.alumnos if alumno.cedula.lower() == cedula.strip().lower()), None)

    def existe_cedula(self, cedula: str) -> bool:
        cedula_normalizada = cedula.strip().lower()
        return any(persona.cedula.lower() == cedula_normalizada for persona in [*self.alumnos, *self.profesores])


class SistemaSGADO:
    def __init__(self) -> None:
        self.repositorio = Repositorio()
        self.pila_notas: list[RegistroNota] = []

    def registrar_alumno(self, cedula: str, nombre: str, correo: str, tipo_programa: str) -> Alumno:
        if not cedula.strip() or not nombre.strip() or not correo.strip():
            raise ValidationError("Cédula, nombre y correo son obligatorios.")
        if self.repositorio.existe_cedula(cedula):
            raise ValidationError("La cédula/ID ya está registrada.")
        alumno = Alumno(cedula.strip(), nombre.strip(), correo.strip(), crear_programa(tipo_programa))
        self.repositorio.alumnos.append(alumno)
        self.repositorio.guardar_alumnos()
        return alumno

    def registrar_profesor(self, cedula: str, nombre: str, correo: str, especialidad: str, materia: str) -> Profesor:
        if not all(valor.strip() for valor in [cedula, nombre, correo, especialidad, materia]):
            raise ValidationError("Todos los campos del profesor son obligatorios.")
        if self.repositorio.existe_cedula(cedula):
            raise ValidationError("La cédula/ID ya está registrada.")
        profesor = Profesor(cedula.strip(), nombre.strip(), correo.strip(), especialidad.strip(), materia.strip())
        self.repositorio.profesores.append(profesor)
        self.repositorio.guardar_profesores()
        return profesor

    def registrar_nota(self, cedula: str, nota: float) -> Alumno:
        alumno = self.repositorio.buscar_alumno(cedula)
        if alumno is None:
            raise ValidationError("No se encontró un alumno con esa cédula/ID.")
        alumno.agregar_nota(nota)
        self.pila_notas.append(RegistroNota(alumno.cedula, nota))
        self.repositorio.guardar_alumnos()
        return alumno

    def deshacer_ultima_nota(self) -> tuple[Alumno, float]:
        if not self.pila_notas:
            raise ValidationError("No hay registros de notas para deshacer.")
        registro = self.pila_notas.pop()
        alumno = self.repositorio.buscar_alumno(registro.cedula)
        if alumno is None:
            raise ValidationError("El alumno asociado ya no existe.")
        nota_eliminada = alumno.deshacer_nota()
        self.repositorio.guardar_alumnos()
        return alumno, nota_eliminada

    def generar_cola_certificados(self) -> list[Alumno]:
        cola: deque[Alumno] = deque(
            alumno for alumno in self.repositorio.alumnos if len(alumno.notas) == 3 and alumno.esta_aprobado()
        )
        graduandos: list[Alumno] = []
        while cola:
            graduandos.append(cola.popleft())

        lineas = [
            "=========================================",
            "REPORTE DE CERTIFICADOS PENDIENTES",
            "=========================================",
            f"Total de graduandos en cola: {len(graduandos)}",
            "",
        ]
        for indice, alumno in enumerate(graduandos, start=1):
            regla_bootcamp = " (Cumple regla de ninguna nota < 14)" if alumno.programa.nombre == "Bootcamp" else ""
            lineas.extend(
                [
                    f"{indice}. [{alumno.cedula}] {alumno.nombre}",
                    f"- Programa: {alumno.programa.nombre}",
                    f"- Promedio Final: {mostrar_promedio(alumno.promedio())}",
                    f"- Estatus: APROBADO{regla_bootcamp}",
                    "",
                ]
            )
        lineas.extend(["=========================================", "* Fin del reporte - Generado por SGA-DO *", ""])
        CERTIFICADOS_FILE.write_text("\n".join(lineas), encoding="utf-8")
        return graduandos

    def reporte_general(self) -> str:
        lineas = [
            "=========================================",
            "REPORTE GENERAL SGA-DO",
            "=========================================",
            "",
            "PROFESORES ACTIVOS",
        ]
        if self.repositorio.profesores:
            for profesor in self.repositorio.profesores:
                lineas.append(f"- [{profesor.cedula}] {profesor.nombre} | {profesor.especialidad} | {profesor.materia}")
        else:
            lineas.append("- No hay profesores registrados.")

        lineas.extend(["", "ALUMNOS REGISTRADOS"])
        if self.repositorio.alumnos:
            for alumno in self.repositorio.alumnos:
                lineas.append(
                    f"- [{alumno.cedula}] {alumno.nombre} | {alumno.programa.nombre} | "
                    f"Notas: {', '.join(mostrar_nota(nota) for nota in alumno.notas) or 'Sin notas'} | "
                    f"Promedio: {mostrar_promedio(alumno.promedio())} | {alumno.estatus()}"
                )
        else:
            lineas.append("- No hay alumnos registrados.")
        return "\n".join(lineas)


def pedir_texto(mensaje: str) -> str:
    return input(mensaje).strip()


def pedir_opcion(mensaje: str, opciones: set[str]) -> str:
    valor = input(mensaje).strip()
    if valor not in opciones:
        if opciones == {str(numero) for numero in range(1, 8)} and not valor.isdigit():
            raise ValidationError("Ingrese un valor numérico válido.")
        raise ValidationError("Ingrese una opción válida.")
    return valor


def pedir_programa() -> str:
    valor = input("Tipo de programa (Curso/Diplomado/Bootcamp): ").strip().lower()
    equivalencias = {"curso": "Curso", "diplomado": "Diplomado", "bootcamp": "Bootcamp"}
    if valor not in equivalencias:
        raise ValidationError("Seleccione Curso, Diplomado o Bootcamp.")
    return equivalencias[valor]


def imprimir_menu() -> None:
    print(
        "\n==================================================\n"
        "SGA-DO: SISTEMA DIPLOMADOSONLINE\n"
        "==================================================\n"
        "1. Registrar Alumno\n"
        "2. Registrar Profesor\n"
        "3. Registrar Notas a un Alumno\n"
        "4. Deshacer Último Registro de Nota\n"
        "5. Generar Cola de Certificados\n"
        "6. Mostrar Reporte General\n"
        "7. Salir\n"
        "=================================================="
    )


def ejecutar_opcion(sistema: SistemaSGADO, opcion: str) -> None:
    if opcion == "1":
        alumno = sistema.registrar_alumno(
            pedir_texto("Cédula/ID: "),
            pedir_texto("Nombre completo: "),
            pedir_texto("Correo electrónico: "),
            pedir_programa(),
        )
        print(f"Alumno registrado: {alumno.nombre}")
    elif opcion == "2":
        profesor = sistema.registrar_profesor(
            pedir_texto("Cédula/ID: "),
            pedir_texto("Nombre completo: "),
            pedir_texto("Correo electrónico: "),
            pedir_texto("Especialidad académica: "),
            pedir_texto("Materia asignada: "),
        )
        print(f"Profesor registrado: {profesor.nombre}")
    elif opcion == "3":
        alumno = sistema.registrar_nota(pedir_texto("Cédula/ID del alumno: "), leer_nota(pedir_texto("Nota (0-20): ")))
        print(f"Nota registrada para {alumno.nombre}. Estado actual: {alumno.estatus()}")
    elif opcion == "4":
        alumno, nota = sistema.deshacer_ultima_nota()
        print(f"Se deshizo la nota {mostrar_nota(nota)} de {alumno.nombre}.")
    elif opcion == "5":
        graduandos = sistema.generar_cola_certificados()
        print(f"Cola procesada en orden FIFO. Graduandos exportados: {len(graduandos)}")
        print(f"Archivo generado: {CERTIFICADOS_FILE}")
    elif opcion == "6":
        print(sistema.reporte_general())


def main() -> None:
    sistema = SistemaSGADO()
    print("Sistema cargado. Los datos se persisten en archivos .txt.")
    while True:
        imprimir_menu()
        try:
            opcion = pedir_opcion("Seleccione una opción (1-7): ", {str(numero) for numero in range(1, 8)})
            if opcion == "7":
                sistema.repositorio.guardar_alumnos()
                sistema.repositorio.guardar_profesores()
                print("Cambios guardados. SGA-DO finalizado de forma segura.")
                break
            ejecutar_opcion(sistema, opcion)
        except (ValidationError, EOFError, KeyboardInterrupt) as error:
            if isinstance(error, (EOFError, KeyboardInterrupt)):
                print("\nSalida cancelada. Cambios guardados de forma segura.")
                sistema.repositorio.guardar_alumnos()
                sistema.repositorio.guardar_profesores()
                break
            print(f"Error: {error}")


if __name__ == "__main__":
    main()
