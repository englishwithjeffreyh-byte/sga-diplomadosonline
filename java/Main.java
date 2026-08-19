import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Path basePath = resolverRutaDeDatos();
        SistemaGestionAcademica sistema = new SistemaGestionAcademica(basePath);
        Scanner scanner = new Scanner(System.in);
        boolean salir = false;

        System.out.println("Sistema cargado. Los datos se persisten en archivos .txt.");
        while (!salir) {
            imprimirMenu();
            try {
                String opcion = scanner.nextLine().trim();
                if (!opcion.matches("[1-7]")) {
                    throw new ValidationException("Ingrese un valor numérico válido.");
                }
                switch (opcion) {
                    case "1" -> registrarAlumno(sistema, scanner);
                    case "2" -> registrarProfesor(sistema, scanner);
                    case "3" -> registrarNota(sistema, scanner);
                    case "4" -> {
                        Alumno alumno = sistema.deshacerUltimaNota();
                        System.out.println("Se deshizo la última nota de " + alumno.getNombre() + ".");
                    }
                    case "5" -> {
                        int cantidad = sistema.generarColaCertificados().size();
                        System.out.println("Cola procesada en orden FIFO. Graduandos exportados: " + cantidad);
                        System.out.println("Archivo generado: " + sistema.getRepositorio().getCertificadosPath());
                    }
                    case "6" -> System.out.println(sistema.reporteGeneral());
                    case "7" -> {
                        sistema.getRepositorio().guardarAlumnos();
                        sistema.getRepositorio().guardarProfesores();
                        System.out.println("Cambios guardados. SGA-DO finalizado de forma segura.");
                        salir = true;
                    }
                    default -> throw new ValidationException("Ingrese una opción válida.");
                }
            } catch (ValidationException error) {
                System.out.println("Error: " + error.getMessage());
            }
        }
        scanner.close();
    }

    private static void registrarAlumno(SistemaGestionAcademica sistema, Scanner scanner) {
        Alumno alumno = sistema.registrarAlumno(
                pedirTexto(scanner, "Cédula/ID: "),
                pedirTexto(scanner, "Nombre completo: "),
                pedirTexto(scanner, "Correo electrónico: "),
                pedirTexto(scanner, "Tipo de programa (Curso/Diplomado/Bootcamp): ")
        );
        System.out.println("Alumno registrado: " + alumno.getNombre());
    }

    private static void registrarProfesor(SistemaGestionAcademica sistema, Scanner scanner) {
        Profesor profesor = sistema.registrarProfesor(
                pedirTexto(scanner, "Cédula/ID: "),
                pedirTexto(scanner, "Nombre completo: "),
                pedirTexto(scanner, "Correo electrónico: "),
                pedirTexto(scanner, "Especialidad académica: "),
                pedirTexto(scanner, "Materia asignada: ")
        );
        System.out.println("Profesor registrado: " + profesor.getNombre());
    }

    private static void registrarNota(SistemaGestionAcademica sistema, Scanner scanner) {
        String cedula = pedirTexto(scanner, "Cédula/ID del alumno: ");
        String textoNota = pedirTexto(scanner, "Nota (0-20): ");
        try {
            double nota = Validaciones.nota(Double.parseDouble(textoNota.replace(",", ".")));
            Alumno alumno = sistema.registrarNota(cedula, nota);
            System.out.println("Nota registrada para " + alumno.getNombre() + ". Estado actual: " + alumno.obtenerEstatus());
        } catch (NumberFormatException error) {
            throw new ValidationException("Ingrese un valor numérico válido.");
        }
    }

    private static String pedirTexto(Scanner scanner, String mensaje) {
        System.out.print(mensaje);
        return scanner.nextLine().trim();
    }

    private static void imprimirMenu() {
        System.out.println("\n==================================================");
        System.out.println("SGA-DO: SISTEMA DIPLOMADOSONLINE");
        System.out.println("==================================================");
        System.out.println("1. Registrar Alumno");
        System.out.println("2. Registrar Profesor");
        System.out.println("3. Registrar Notas a un Alumno");
        System.out.println("4. Deshacer Último Registro de Nota");
        System.out.println("5. Generar Cola de Certificados");
        System.out.println("6. Mostrar Reporte General");
        System.out.println("7. Salir");
        System.out.println("==================================================");
        System.out.print("Seleccione una opción (1-7): ");
    }

    private static Path resolverRutaDeDatos() {
        Path desdeRaiz = Path.of("java").toAbsolutePath();
        if (Files.exists(desdeRaiz.resolve("alumnos.txt"))) {
            return desdeRaiz;
        }
        return Path.of(".").toAbsolutePath();
    }
}
