from .persona import Persona


class Profesor(Persona):

    def __init__(self, cedula, nombre_completo, correo,
                 especialidad, materia_asignada):

        super().__init__(cedula, nombre_completo, correo)

        self.especialidad = especialidad
        self.materia_asignada = materia_asignada

    def mostrar_informacion(self):
        return (
            f"{self.mostrar_datos()}\n"
            f"Especialidad: {self.especialidad}\n"
            f"Materia: {self.materia_asignada}"
        )
