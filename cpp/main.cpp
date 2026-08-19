#include "sga_do.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace {

std::string pedirTexto(const std::string& mensaje) {
    std::cout << mensaje;
    std::string valor;
    std::getline(std::cin, valor);
    return trim(valor);
}

double pedirNota() {
    std::string texto = pedirTexto("Nota (0-20): ");
    std::replace(texto.begin(), texto.end(), ',', '.');
    try {
        std::size_t caracteresLeidos = 0;
        const double nota = std::stod(texto, &caracteresLeidos);
        if (caracteresLeidos != texto.size()) {
            throw std::invalid_argument("entrada incompleta");
        }
        return validarNota(nota);
    } catch (const std::exception&) {
        throw ValidationError("Ingrese un valor numérico válido entre 0 y 20.");
    }
}

std::string resolverRutaDeDatos() {
    std::ifstream desdeRaiz("cpp/alumnos.txt");
    return desdeRaiz.good() ? "cpp" : ".";
}

void imprimirMenu() {
    std::cout << "\n==================================================\n"
              << "SGA-DO: SISTEMA DIPLOMADOSONLINE\n"
              << "==================================================\n"
              << "1. Registrar Alumno\n"
              << "2. Registrar Profesor\n"
              << "3. Registrar Notas a un Alumno\n"
              << "4. Deshacer Último Registro de Nota\n"
              << "5. Generar Cola de Certificados\n"
              << "6. Mostrar Reporte General\n"
              << "7. Salir\n"
              << "==================================================\n"
              << "Seleccione una opción (1-7): ";
}

void registrarAlumno(SistemaGestionAcademica& sistema) {
    Alumno* alumno = sistema.registrarAlumno(
        pedirTexto("Cédula/ID: "),
        pedirTexto("Nombre completo: "),
        pedirTexto("Correo electrónico: "),
        pedirTexto("Tipo de programa (Curso/Diplomado/Bootcamp): ")
    );
    std::cout << "Alumno registrado: " << alumno->getNombre() << "\n";
}

void registrarProfesor(SistemaGestionAcademica& sistema) {
    Profesor* profesor = sistema.registrarProfesor(
        pedirTexto("Cédula/ID: "),
        pedirTexto("Nombre completo: "),
        pedirTexto("Correo electrónico: "),
        pedirTexto("Especialidad académica: "),
        pedirTexto("Materia asignada: ")
    );
    std::cout << "Profesor registrado: " << profesor->getNombre() << "\n";
}

void registrarNota(SistemaGestionAcademica& sistema) {
    Alumno* alumno = sistema.registrarNota(pedirTexto("Cédula/ID del alumno: "), pedirNota());
    std::cout << "Nota registrada para " << alumno->getNombre()
              << ". Estado actual: " << alumno->obtenerEstatus() << "\n";
}

} 

int main() {
    try {
        SistemaGestionAcademica sistema(resolverRutaDeDatos());
        bool salir = false;
        while (!salir) {
            imprimirMenu();
            std::string opcion;
            std::getline(std::cin, opcion);
            opcion = trim(opcion);
            try {
                if (opcion == "1") {
                    registrarAlumno(sistema);
                } else if (opcion == "2") {
                    registrarProfesor(sistema);
                } else if (opcion == "3") {
                    registrarNota(sistema);
                } else if (opcion == "4") {
                    Alumno* alumno = sistema.deshacerUltimaNota();
                    std::cout << "Se deshizo la última nota de " << alumno->getNombre() << ".\n";
                } else if (opcion == "5") {
                    const std::vector<Alumno*> graduandos = sistema.generarColaCertificados();
                    std::cout << "Cola procesada en orden FIFO. Graduandos exportados: "
                              << graduandos.size() << "\n"
                              << "Archivo generado: " << sistema.getRepositorio().getCertificadosPath() << "\n";
                } else if (opcion == "6") {
                    std::cout << sistema.reporteGeneral();
                } else if (opcion == "7") {
                    sistema.getRepositorio().guardarAlumnos();
                    sistema.getRepositorio().guardarProfesores();
                    std::cout << "Cambios guardados. SGA-DO finalizado de forma segura.\n";
                    salir = true;
                } else {
                    throw ValidationError("Ingrese una opción válida entre 1 y 7.");
                }
            } catch (const ValidationError& error) {
                std::cout << "Error: " << error.what() << "\n";
            }
        }
    } catch (const ValidationError& error) {
        std::cerr << "Error al iniciar SGA-DO: " << error.what() << "\n";
        return 1;
    }
    return 0;
}
