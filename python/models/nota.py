class Nota:

    def __init__(self, cedula_alumno, valor):
        self.cedula_alumno = cedula_alumno
        self.valor = valor

    def __str__(self):
        return f"{self.cedula_alumno}: {self.valor}"
