import os


class ArchivoService:

    @staticmethod
    def guardar_linea(nombre_archivo, contenido):
        with open(nombre_archivo, "a", encoding="utf-8") as archivo:
            archivo.write(contenido + "\n")

    @staticmethod
    def leer_archivo(nombre_archivo):

        if not os.path.exists(nombre_archivo):
            return []

        with open(nombre_archivo, "r", encoding="utf-8") as archivo:
            return archivo.readlines()
