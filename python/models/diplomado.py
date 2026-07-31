from .programa_academico import ProgramaAcademico

class Diplomado(ProgramaAcademico):

    def __init__(self):
        super().__init__("Diplomado")

    def evaluar_aprobacion(self, notas):
        promedio = sum(notas) / len(notas)
        return promedio >= 14











