from .persona import Persona


class Alumno(Persona):

    def __init__(self, cedula, nombre_completo, correo, programa):
        super().__init__(cedula, nombre_completo, correo)
        self.notas = []
        self.programa = programa

    def agregar_nota(self, nota):
        if len(self.notas) < 3:
            self.notas.append(nota)
            return True

        print("El alumno ya posee el máximo de 3 notas.")
        return False

    def eliminar_ultima_nota(self):
        if self.notas:
            return self.notas.pop()

        print("No hay notas registradas.")
        return None

    def calcular_promedio(self):
        if not self.notas:
            return 0

        return sum(self.notas) / len(self.notas)

    def esta_aprobado(self):
        if len(self.notas) != 3:
            return False

        return self.programa.evaluar_aprobacion(self.notas)
