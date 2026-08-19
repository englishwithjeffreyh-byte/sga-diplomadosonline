import java.util.List;

public class Curso extends ProgramaAcademico {
    private final double notaMinima = 10.0;

    public Curso() {
        super("Curso");
    }

    @Override
    public boolean evaluarAprobacion(List<Double> notas) {
        return !notas.isEmpty() && notas.stream().mapToDouble(Double::doubleValue).average().orElse(0) >= notaMinima;
    }

    @Override
    public String obtenerCriterio() {
        return "promedio mayor o igual a 10/20";
    }
}
