from .programa_academico import ProgramaAcademico

class Curso(ProgramaAcademico):

    def __init__(self):
        super().__init__("Curso")

    def evaluar_aprobacion(self, notas):
        promedio = sum(notas) / len(notas)
        return promedio >= 10










