public class Profesor extends Persona {
    private final String especialidad;
    private final String materia;

    public Profesor(String cedula, String nombre, String correo, String especialidad, String materia) {
        super(cedula, nombre, correo);
        this.especialidad = especialidad;
        this.materia = materia;
    }

    public String getEspecialidad() {
        return especialidad;
    }

    public String getMateria() {
        return materia;
    }

    public String toFileLine() {
        return String.join(",", getCedula(), getNombre(), getCorreo(), especialidad, materia);
    }
}
