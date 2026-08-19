import java.util.Locale;

public final class Formato {
    private Formato() {
    }

    public static String numero(double valor) {
        if (valor == Math.rint(valor)) {
            return String.valueOf((int) valor);
        }
        return String.format(Locale.US, "%.1f", valor);
    }

    public static String promedio(double valor) {
        double truncado = Math.floor(valor * 10) / 10;
        return String.format(Locale.US, "%.1f", truncado);
    }
}
