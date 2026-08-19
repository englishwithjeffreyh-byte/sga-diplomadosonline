import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class Repositorio {
    private final Path alumnosPath;
    private final Path profesoresPath;
    private final Path certificadosPath;
    private final List<Alumno> alumnos = new ArrayList<>();
    private final List<Profesor> profesores = new ArrayList<>();

    public Repositorio(Path basePath) {
        alumnosPath = basePath.resolve("alumnos.txt");
        profesoresPath = basePath.resolve("profesores.txt");
        certificadosPath = basePath.resolve("certificados_pendientes.txt");
        cargar();
    }

    public List<Alumno> getAlumnos() {
        return alumnos;
    }

    public List<Profesor> getProfesores() {
        return profesores;
    }

    public Path getCertificadosPath() {
        return certificadosPath;
    }

    public void cargar() {
        try {
            crearArchivosSiNoExisten();
            alumnos.clear();
            profesores.clear();
            cargarAlumnos();
            cargarProfesores();
        } catch (IOException error) {
            throw new ValidationException("No se pudieron abrir los archivos de persistencia: " + error.getMessage());
        }
    }

    private void crearArchivosSiNoExisten() throws IOException {
        Path basePath = alumnosPath.getParent();
        if (basePath != null) {
            Files.createDirectories(basePath);
        }
        if (Files.notExists(alumnosPath)) {
            Files.createFile(alumnosPath);
        }
        if (Files.notExists(profesoresPath)) {
            Files.createFile(profesoresPath);
        }
        if (Files.notExists(certificadosPath)) {
            Files.createFile(certificadosPath);
        }
    }

    private void cargarAlumnos() throws IOException {
        for (String linea : Files.readAllLines(alumnosPath)) {
            if (linea.isBlank()) {
                continue;
            }
            String[] partes = linea.split(",", -1);
            if (partes.length != 7) {
                continue;
            }
            try {
                List<Double> notas = Arrays.stream(partes, 4, 7).map(Double::parseDouble).toList();
                List<Double> notasReales = notas.stream().anyMatch(nota -> nota != 0) ? notas : List.of();
                alumnos.add(new Alumno(partes[0].trim(), partes[1].trim(), partes[2].trim(), crearPrograma(partes[3]), notasReales));
            } catch (RuntimeException error) {
                // Ignora registros mal formados sin detener la carga completa.
            }
        }
    }

    private void cargarProfesores() throws IOException {
        for (String linea : Files.readAllLines(profesoresPath)) {
            if (linea.isBlank()) {
                continue;
            }
            String[] partes = linea.split(",", 5);
            if (partes.length == 5) {
                profesores.add(new Profesor(partes[0].trim(), partes[1].trim(), partes[2].trim(), partes[3].trim(), partes[4].trim()));
            }
        }
    }

    private ProgramaAcademico crearPrograma(String nombre) {
        return switch (nombre.trim().toLowerCase()) {
            case "curso" -> new Curso();
            case "diplomado" -> new Diplomado();
            case "bootcamp" -> new Bootcamp();
            default -> throw new ValidationException("Programa académico no válido.");
        };
    }

    public void guardarAlumnos() {
        try {
            Files.write(alumnosPath, alumnos.stream().map(Alumno::toFileLine).toList());
        } catch (IOException error) {
            throw new ValidationException("No se pudo guardar alumnos.txt: " + error.getMessage());
        }
    }

    public void guardarProfesores() {
        try {
            Files.write(profesoresPath, profesores.stream().map(Profesor::toFileLine).toList());
        } catch (IOException error) {
            throw new ValidationException("No se pudo guardar profesores.txt: " + error.getMessage());
        }
    }

    public Alumno buscarAlumno(String cedula) {
        return alumnos.stream().filter(alumno -> alumno.getCedula().equalsIgnoreCase(cedula.trim())).findFirst().orElse(null);
    }

    public boolean existeCedula(String cedula) {
        return alumnos.stream().anyMatch(persona -> persona.getCedula().equalsIgnoreCase(cedula.trim()))
                || profesores.stream().anyMatch(persona -> persona.getCedula().equalsIgnoreCase(cedula.trim()));
    }
}
