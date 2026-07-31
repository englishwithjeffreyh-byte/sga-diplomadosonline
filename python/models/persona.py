class Persona:
    def __init__(self, cedula, nombre_completo, correo):
        self.cedula = cedula
        self.nombre_completo = nombre_completo
        self.correo = correo

    def mostrar_datos(self):
        return (
            f"Cédula: {self.cedula}\n"
            f"Nombre: {self.nombre_completo}\n"
            f"Correo: {self.correo}"
        )





