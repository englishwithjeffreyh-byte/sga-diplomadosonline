import java.util.List;

public class Bootcamp extends ProgramaAcademico {
    private final double notaMinimaGlobal = 14.0;

    public Bootcamp() {
        super("Bootcamp");
    }

    @Override
    public boolean evaluarAprobacion(List<Double> notas) {
        return notas.size() == 3 && notas.stream().allMatch(nota -> nota >= notaMinimaGlobal);
    }

    @Override
    public String obtenerCriterio() {
        return "ninguna nota individual menor a 14/20";
    }
}
