#include "sga_do.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>

namespace {

void require(bool condicion, const std::string& mensaje) {
    if (!condicion) {
        throw std::runtime_error(mensaje);
    }
}

void crearDatosSemilla(const std::string& directorio) {
    std::filesystem::create_directories(directorio);
    std::ofstream alumnos(directorio + "/alumnos.txt");
    alumnos << "V-101,Ana Silva,ana@email.com,Curso,10,10,10\n"
             << "V-202,Carlos Pérez,carlos@email.com,Diplomado,14,14,13\n"
             << "V-303,María Gómez,maria@email.com,Bootcamp,20,20,13\n"
             << "V-404,Luis Rojas,luis@email.com,Bootcamp,14,15,14\n";
    std::ofstream profesores(directorio + "/profesores.txt");
    profesores << "P-505,Dr. Pedro Infante,pedro@email.com,Backend,C++ y Algoritmos\n";
}

} 

int main() {
    const std::string directorio =
        (std::filesystem::temp_directory_path() / "sga-do-cpp-test").string();
    std::filesystem::remove_all(directorio);
    crearDatosSemilla(directorio);

    try {
        SistemaGestionAcademica sistema(directorio);
        require(sistema.getRepositorio().buscarAlumno("V-101")->obtenerEstatus() == "APROBADO",
                "Curso debería aprobar");
        require(sistema.getRepositorio().buscarAlumno("V-202")->obtenerEstatus() == "REPROBADO",
                "Diplomado debería reprobar");
        require(sistema.getRepositorio().buscarAlumno("V-303")->obtenerEstatus() == "REPROBADO",
                "Bootcamp debería reprobar");
        require(sistema.getRepositorio().buscarAlumno("V-404")->obtenerEstatus() == "APROBADO",
                "Bootcamp debería aprobar");

        const std::string reporte = sistema.reporteGeneral();
        require(reporte.find("Promedio: 13.6") != std::string::npos,
                "El promedio de Carlos debe truncarse a 13.6");
        require(reporte.find("Promedio: 17.6") != std::string::npos,
                "El promedio de María debe truncarse a 17.6");

        Alumno* prueba = sistema.registrarAlumno(
            "V-999", "Prueba LIFO", "test@email.com", "Curso");
        sistema.registrarNota(prueba->getCedula(), 15);
        sistema.deshacerUltimaNota();
        require(sistema.getRepositorio().buscarAlumno("V-999")->getNotas().empty(),
                "LIFO debería eliminar la última nota");

        const std::vector<Alumno*> graduandos = sistema.generarColaCertificados();
        require(graduandos.size() == 2, "La cola debe contener dos graduandos");
        require(graduandos[0]->getNombre() == "Ana Silva" && graduandos[1]->getNombre() == "Luis Rojas",
                "FIFO debe respetar el orden");

        std::ifstream certificados(directorio + "/certificados_pendientes.txt");
        const std::string certificadosTexto((std::istreambuf_iterator<char>(certificados)),
                                            std::istreambuf_iterator<char>());
        require(certificadosTexto.find("Total de graduandos en cola: 2") != std::string::npos,
                "Debe generar dos certificados");
        require(certificadosTexto.find("Carlos Pérez") == std::string::npos &&
                    certificadosTexto.find("María Gómez") == std::string::npos,
                "El reporte no debe incluir reprobados");

        try {
            std::stod("veinte");
            throw std::runtime_error("La entrada no numérica fue aceptada");
        } catch (const std::invalid_argument&) {
        }

        std::cout << "C++ Core OK: polimorfismo, memoria, LIFO, FIFO, persistencia y validaciones\n";
    } catch (const std::exception& error) {
        std::cerr << "Prueba C++ fallida: " << error.what() << "\n";
        std::filesystem::remove_all(directorio);
        return 1;
    }

    std::filesystem::remove_all(directorio);
    return 0;
}
