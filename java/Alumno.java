import java.util.ArrayList;
import java.util.List;

public class Alumno extends Persona {
    private final ProgramaAcademico programa;
    private final List<Double> notas;

    public Alumno(String cedula, String nombre, String correo, ProgramaAcademico programa) {
        this(cedula, nombre, correo, programa, new ArrayList<>());
    }

    public Alumno(String cedula, String nombre, String correo, ProgramaAcademico programa, List<Double> notas) {
        super(cedula, nombre, correo);
        this.programa = programa;
        this.notas = new ArrayList<>(notas);
    }

    public ProgramaAcademico getPrograma() {
        return programa;
    }

    public List<Double> getNotas() {
        return List.copyOf(notas);
    }

    public void agregarNota(double nota) {
        if (notas.size() >= 3) {
            throw new ValidationException("Cada alumno puede tener máximo 3 notas.");
        }
        notas.add(Validaciones.nota(nota));
    }

    public double eliminarUltimaNota() {
        if (notas.isEmpty()) {
            throw new ValidationException("El alumno no tiene notas para deshacer.");
        }
        return notas.remove(notas.size() - 1);
    }

    public double calcularPromedio() {
        return notas.stream().mapToDouble(Double::doubleValue).average().orElse(0);
    }

    public boolean estaAprobado() {
        return programa.evaluarAprobacion(notas);
    }

    public String obtenerEstatus() {
        if (notas.size() < 3) {
            return "PENDIENTE";
        }
        return estaAprobado() ? "APROBADO" : "REPROBADO";
    }

    public String toFileLine() {
        List<Double> valores = new ArrayList<>(notas);
        while (valores.size() < 3) {
            valores.add(0.0);
        }
        return String.join(",", getCedula(), getNombre(), getCorreo(), programa.getNombre(),
                Formato.numero(valores.get(0)), Formato.numero(valores.get(1)), Formato.numero(valores.get(2)));
    }
}
