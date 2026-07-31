from abc import ABC, abstractmethod

class ProgramaAcademico(ABC):

    def __init__(self, nombre):
        self.nombre = nombre

    @abstractmethod
    def evaluar_aprobacion(self, notas):
        pass
