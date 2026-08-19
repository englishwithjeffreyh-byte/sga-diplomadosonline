public final class Validaciones {
    private Validaciones() {
    }

    public static double nota(double valor) {
        if (!Double.isFinite(valor) || valor < 0 || valor > 20) {
            throw new ValidationException("La nota debe estar entre 0 y 20.");
        }
        return valor;
    }
}
