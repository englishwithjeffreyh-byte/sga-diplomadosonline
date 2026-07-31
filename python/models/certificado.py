class Certificado:

    def __init__(self, cedula_alumno, nombre_alumno,
                 programa, promedio_final, estatus):

        self.cedula_alumno = cedula_alumno
        self.nombre_alumno = nombre_alumno
        self.programa = programa
        self.promedio_final = promedio_final
        self.estatus = estatus

    def generar_reporte(self):
        return (
            f"[{self.cedula_alumno}] {self.nombre_alumno}\n"
            f"Programa: {self.programa}\n"
            f"Promedio Final: {self.promedio_final:.2f}\n"
            f"Estatus: {self.estatus}\n"
        )
