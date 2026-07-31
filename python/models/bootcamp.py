from .programa_academico import ProgramaAcademico

class Bootcamp(ProgramaAcademico):

    def __init__(self):
        super().__init__("Bootcamp")

    def evaluar_aprobacion(self, notas):
        return all(nota >= 14 for nota in notas)
