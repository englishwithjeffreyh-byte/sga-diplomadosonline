import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.List;
import java.util.Queue;
import java.util.Stack;

public class SistemaGestionAcademica {
    private final Repositorio repositorio;
    private final Stack<String> pilaDeshacer = new Stack<>();
    private final Queue<Alumno> colaCertificados = new ArrayDeque<>();

    public SistemaGestionAcademica(Path basePath) {
        repositorio = new Repositorio(basePath);
    }

    public Repositorio getRepositorio() {
        return repositorio;
    }

    public Alumno registrarAlumno(String cedula, String nombre, String correo, String tipoPrograma) {
        if (cedula.isBlank() || nombre.isBlank() || correo.isBlank()) {
            throw new ValidationException("Cédula, nombre y correo son obligatorios.");
        }
        if (repositorio.existeCedula(cedula)) {
            throw new ValidationException("La cédula/ID ya está registrada.");
        }
        Alumno alumno = new Alumno(cedula.trim(), nombre.trim(), correo.trim(), crearPrograma(tipoPrograma));
        repositorio.getAlumnos().add(alumno);
        repositorio.guardarAlumnos();
        return alumno;
    }

    public Profesor registrarProfesor(String cedula, String nombre, String correo, String especialidad, String materia) {
        if (List.of(cedula, nombre, correo, especialidad, materia).stream().anyMatch(String::isBlank)) {
            throw new ValidationException("Todos los campos del profesor son obligatorios.");
        }
        if (repositorio.existeCedula(cedula)) {
            throw new ValidationException("La cédula/ID ya está registrada.");
        }
        Profesor profesor = new Profesor(cedula.trim(), nombre.trim(), correo.trim(), especialidad.trim(), materia.trim());
        repositorio.getProfesores().add(profesor);
        repositorio.guardarProfesores();
        return profesor;
    }

    public Alumno registrarNota(String cedula, double nota) {
        Alumno alumno = repositorio.buscarAlumno(cedula);
        if (alumno == null) {
            throw new ValidationException("No se encontró un alumno con esa cédula/ID.");
        }
        alumno.agregarNota(nota);
        pilaDeshacer.push(alumno.getCedula() + "|" + nota);
        repositorio.guardarAlumnos();
        return alumno;
    }

    public Alumno deshacerUltimaNota() {
        if (pilaDeshacer.empty()) {
            throw new ValidationException("No hay registros de notas para deshacer.");
        }
        String[] registro = pilaDeshacer.pop().split("\\|", 2);
        Alumno alumno = repositorio.buscarAlumno(registro[0]);
        if (alumno == null) {
            throw new ValidationException("El alumno asociado ya no existe.");
        }
        alumno.eliminarUltimaNota();
        repositorio.guardarAlumnos();
        return alumno;
    }

    public List<Alumno> generarColaCertificados() {
        colaCertificados.clear();
        repositorio.getAlumnos().stream()
                .filter(alumno -> alumno.getNotas().size() == 3 && alumno.estaAprobado())
                .forEach(colaCertificados::offer);

        List<Alumno> graduandos = new ArrayList<>();
        while (!colaCertificados.isEmpty()) {
            graduandos.add(colaCertificados.poll());
        }

        List<String> lineas = new ArrayList<>(List.of(
                "=========================================",
                "REPORTE DE CERTIFICADOS PENDIENTES",
                "=========================================",
                "Total de graduandos en cola: " + graduandos.size(),
                ""
        ));
        for (int indice = 0; indice < graduandos.size(); indice++) {
            Alumno alumno = graduandos.get(indice);
            String regla = alumno.getPrograma().getNombre().equals("Bootcamp")
                    ? " (Cumple regla de ninguna nota < 14)"
                    : "";
            lineas.add((indice + 1) + ". [" + alumno.getCedula() + "] " + alumno.getNombre());
            lineas.add("- Programa: " + alumno.getPrograma().getNombre());
            lineas.add("- Promedio Final: " + Formato.promedio(alumno.calcularPromedio()));
            lineas.add("- Estatus: APROBADO" + regla);
            lineas.add("");
        }
        lineas.add("=========================================");
        lineas.add("* Fin del reporte - Generado por SGA-DO *");
        lineas.add("");
        try {
            Files.write(repositorio.getCertificadosPath(), lineas);
        } catch (IOException error) {
            throw new ValidationException("No se pudo generar certificados_pendientes.txt: " + error.getMessage());
        }
        return graduandos;
    }

    public String reporteGeneral() {
        List<String> lineas = new ArrayList<>(List.of(
                "=========================================",
                "REPORTE GENERAL SGA-DO",
                "=========================================",
                "",
                "PROFESORES ACTIVOS"
        ));
        if (repositorio.getProfesores().isEmpty()) {
            lineas.add("- No hay profesores registrados.");
        } else {
            repositorio.getProfesores().forEach(profesor -> lineas.add(
                    "- [" + profesor.getCedula() + "] " + profesor.getNombre() + " | "
                            + profesor.getEspecialidad() + " | " + profesor.getMateria()));
        }

        lineas.addAll(List.of("", "ALUMNOS REGISTRADOS"));
        if (repositorio.getAlumnos().isEmpty()) {
            lineas.add("- No hay alumnos registrados.");
        } else {
            repositorio.getAlumnos().forEach(alumno -> {
                String notas = alumno.getNotas().isEmpty()
                        ? "Sin notas"
                        : alumno.getNotas().stream().map(Formato::numero).reduce((a, b) -> a + ", " + b).orElse("");
                lineas.add("- [" + alumno.getCedula() + "] " + alumno.getNombre() + " | "
                        + alumno.getPrograma().getNombre() + " | Notas: " + notas + " | Promedio: "
                        + Formato.promedio(alumno.calcularPromedio()) + " | " + alumno.obtenerEstatus());
            });
        }
        return String.join("\n", lineas);
    }

    private ProgramaAcademico crearPrograma(String nombre) {
        return switch (nombre.trim().toLowerCase()) {
            case "curso" -> new Curso();
            case "diplomado" -> new Diplomado();
            case "bootcamp" -> new Bootcamp();
            default -> throw new ValidationException("Seleccione Curso, Diplomado o Bootcamp.");
        };
    }
}
