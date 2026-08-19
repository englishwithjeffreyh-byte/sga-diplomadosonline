import java.util.List;

public class Diplomado extends ProgramaAcademico {
    private final double notaMinima = 14.0;

    public Diplomado() {
        super("Diplomado");
    }

    @Override
    public boolean evaluarAprobacion(List<Double> notas) {
        return !notas.isEmpty() && notas.stream().mapToDouble(Double::doubleValue).average().orElse(0) >= notaMinima;
    }

    @Override
    public String obtenerCriterio() {
        return "promedio mayor o igual a 14/20";
    }
}
