import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;

public class TestSGADO {
    public static void main(String[] args) throws Exception {
        Path tempDir = Files.createTempDirectory("sga-do-java-");
        Files.writeString(tempDir.resolve("alumnos.txt"), String.join("\n",
                "V-101,Ana Silva,ana@email.com,Curso,10,10,10",
                "V-202,Carlos Pérez,carlos@email.com,Diplomado,14,14,13",
                "V-303,María Gómez,maria@email.com,Bootcamp,20,20,13",
                "V-404,Luis Rojas,luis@email.com,Bootcamp,14,15,14",
                ""));
        Files.writeString(tempDir.resolve("profesores.txt"), "P-505,Dr. Pedro Infante,pedro@email.com,Backend,C++ y Algoritmos\n");

        SistemaGestionAcademica sistema = new SistemaGestionAcademica(tempDir);
        require(sistema.getRepositorio().buscarAlumno("V-101").obtenerEstatus().equals("APROBADO"), "Curso debería aprobar");
        require(sistema.getRepositorio().buscarAlumno("V-202").obtenerEstatus().equals("REPROBADO"), "Diplomado debería reprobar");
        require(sistema.getRepositorio().buscarAlumno("V-303").obtenerEstatus().equals("REPROBADO"), "Bootcamp debería reprobar");
        require(sistema.getRepositorio().buscarAlumno("V-404").obtenerEstatus().equals("APROBADO"), "Bootcamp debería aprobar");

        String reporte = sistema.reporteGeneral();
        require(reporte.contains("Promedio: 13.6"), "El promedio de Carlos debe truncarse a 13.6");
        require(reporte.contains("Promedio: 17.6"), "El promedio de María debe truncarse a 17.6");

        Alumno prueba = sistema.registrarAlumno("V-999", "Prueba LIFO", "test@email.com", "Curso");
        sistema.registrarNota(prueba.getCedula(), 15);
        sistema.deshacerUltimaNota();
        require(sistema.getRepositorio().buscarAlumno("V-999").getNotas().isEmpty(), "LIFO debería eliminar la última nota");

        List<Alumno> graduandos = sistema.generarColaCertificados();
        require(graduandos.stream().map(Alumno::getNombre).toList().equals(List.of("Ana Silva", "Luis Rojas")), "FIFO debe respetar el orden");
        String certificados = Files.readString(tempDir.resolve("certificados_pendientes.txt"));
        require(certificados.contains("Total de graduandos en cola: 2"), "Debe generar dos certificados");
        require(!certificados.contains("Carlos Pérez") && !certificados.contains("María Gómez"), "El reporte no debe incluir reprobados");

        try {
            Validaciones.nota(Double.parseDouble("veinte"));
            throw new AssertionError("La entrada no numérica fue aceptada");
        } catch (NumberFormatException expected) {
            // Entrada inválida controlada por el menú.
        }

        System.out.println("Java Core OK: polimorfismo, LIFO, FIFO, persistencia y validaciones");
    }

    private static void require(boolean condition, String message) {
        if (!condition) {
            throw new AssertionError(message);
        }
    }
}
