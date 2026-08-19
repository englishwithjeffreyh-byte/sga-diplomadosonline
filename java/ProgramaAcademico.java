import java.util.List;

public abstract class ProgramaAcademico {
    private final String nombre;

    protected ProgramaAcademico(String nombre) {
        this.nombre = nombre;
    }

    public String getNombre() {
        return nombre;
    }

    public abstract boolean evaluarAprobacion(List<Double> notas);

    public abstract String obtenerCriterio();
}
