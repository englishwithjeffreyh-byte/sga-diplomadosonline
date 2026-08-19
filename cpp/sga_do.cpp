#include "sga_do.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <stdexcept>

namespace {

std::string rutaArchivo(const std::string& basePath, const std::string& archivo) {
    if (basePath.empty() || basePath == ".") {
        return archivo;
    }
    const char ultimo = basePath.back();
    if (ultimo == '/' || ultimo == '\\') {
        return basePath + archivo;
    }
    return basePath + "/" + archivo;
}

std::string minusculas(std::string valor) {
    std::transform(valor.begin(), valor.end(), valor.begin(),
                   [](unsigned char caracter) { return static_cast<char>(std::tolower(caracter)); });
    return valor;
}

std::vector<std::string> separar(const std::string& linea, char delimitador, std::size_t maxPartes = 0) {
    std::vector<std::string> partes;
    std::stringstream flujo(linea);
    std::string parte;
    while (std::getline(flujo, parte, delimitador)) {
        if (maxPartes != 0 && partes.size() + 1 == maxPartes) {
            std::string restante = parte;
            std::string cola;
            while (std::getline(flujo, cola)) {
                restante += delimitador + cola;
            }
            partes.push_back(restante);
            return partes;
        }
        partes.push_back(parte);
    }
    if (!linea.empty() && linea.back() == delimitador) {
        partes.emplace_back();
    }
    return partes;
}

bool esLineaVacia(const std::string& linea) {
    return trim(linea).empty();
}

} 

std::string trim(const std::string& valor) {
    const std::string espacios = " \t\r\n";
    const std::size_t inicio = valor.find_first_not_of(espacios);
    if (inicio == std::string::npos) {
        return "";
    }
    const std::size_t fin = valor.find_last_not_of(espacios);
    return valor.substr(inicio, fin - inicio + 1);
}

double validarNota(double valor) {
    if (!std::isfinite(valor) || valor < 0.0 || valor > 20.0) {
        throw ValidationError("La nota debe estar entre 0 y 20.");
    }
    return valor;
}

std::string formatoNumero(double valor) {
    if (std::floor(valor) == valor) {
        return std::to_string(static_cast<int>(valor));
    }
    std::ostringstream salida;
    salida << std::fixed << std::setprecision(1) << valor;
    return salida.str();
}

std::string formatoPromedio(double valor) {
    const double truncado = std::floor(valor * 10.0) / 10.0;
    std::ostringstream salida;
    salida << std::fixed << std::setprecision(1) << truncado;
    return salida.str();
}

ProgramaAcademico::ProgramaAcademico(std::string nombre)
    : nombre_(std::move(nombre)) {}

const std::string& ProgramaAcademico::getNombre() const {
    return nombre_;
}

Curso::Curso() : ProgramaAcademico("Curso") {}

bool Curso::evaluarAprobacion(const std::vector<double>& notas) const {
    if (notas.empty()) {
        return false;
    }
    const double suma = std::accumulate(notas.begin(), notas.end(), 0.0);
    return suma / notas.size() >= 10.0;
}

std::string Curso::obtenerCriterio() const {
    return "promedio mayor o igual a 10/20";
}

Diplomado::Diplomado() : ProgramaAcademico("Diplomado") {}

bool Diplomado::evaluarAprobacion(const std::vector<double>& notas) const {
    if (notas.empty()) {
        return false;
    }
    const double suma = std::accumulate(notas.begin(), notas.end(), 0.0);
    return suma / notas.size() >= 14.0;
}

std::string Diplomado::obtenerCriterio() const {
    return "promedio mayor o igual a 14/20";
}

Bootcamp::Bootcamp() : ProgramaAcademico("Bootcamp") {}

bool Bootcamp::evaluarAprobacion(const std::vector<double>& notas) const {
    return notas.size() == 3 &&
           std::all_of(notas.begin(), notas.end(), [](double nota) { return nota >= 14.0; });
}

std::string Bootcamp::obtenerCriterio() const {
    return "ninguna nota individual menor a 14/20";
}

ValidationError::ValidationError(const std::string& mensaje)
    : std::runtime_error(mensaje) {}

Persona::Persona(std::string cedula, std::string nombre, std::string correo)
    : cedula_(std::move(cedula)), nombre_(std::move(nombre)), correo_(std::move(correo)) {}

const std::string& Persona::getCedula() const {
    return cedula_;
}

const std::string& Persona::getNombre() const {
    return nombre_;
}

const std::string& Persona::getCorreo() const {
    return correo_;
}

Alumno::Alumno(std::string cedula, std::string nombre, std::string correo,
               ProgramaAcademico* programa, std::vector<double> notas)
    : Persona(std::move(cedula), std::move(nombre), std::move(correo)),
      programa_(programa), notas_(std::move(notas)) {}

Alumno::~Alumno() {
    delete programa_;
}

const ProgramaAcademico& Alumno::getPrograma() const {
    return *programa_;
}

const std::vector<double>& Alumno::getNotas() const {
    return notas_;
}

void Alumno::agregarNota(double nota) {
    if (notas_.size() >= 3) {
        throw ValidationError("Cada alumno puede tener máximo 3 notas.");
    }
    notas_.push_back(validarNota(nota));
}

double Alumno::eliminarUltimaNota() {
    if (notas_.empty()) {
        throw ValidationError("El alumno no tiene notas para deshacer.");
    }
    const double nota = notas_.back();
    notas_.pop_back();
    return nota;
}

double Alumno::calcularPromedio() const {
    if (notas_.empty()) {
        return 0.0;
    }
    const double suma = std::accumulate(notas_.begin(), notas_.end(), 0.0);
    return suma / notas_.size();
}

bool Alumno::estaAprobado() const {
    return programa_->evaluarAprobacion(notas_);
}

std::string Alumno::obtenerEstatus() const {
    if (notas_.size() < 3) {
        return "PENDIENTE";
    }
    return estaAprobado() ? "APROBADO" : "REPROBADO";
}

std::string Alumno::toFileLine() const {
    std::ostringstream salida;
    salida << getCedula() << ',' << getNombre() << ',' << getCorreo() << ','
           << programa_->getNombre() << ',';
    for (std::size_t indice = 0; indice < 3; ++indice) {
        if (indice < notas_.size()) {
            salida << formatoNumero(notas_[indice]);
        } else {
            salida << '0';
        }
        if (indice < 2) {
            salida << ',';
        }
    }
    return salida.str();
}

Profesor::Profesor(std::string cedula, std::string nombre, std::string correo,
                   std::string especialidad, std::string materia)
    : Persona(std::move(cedula), std::move(nombre), std::move(correo)),
      especialidad_(std::move(especialidad)), materia_(std::move(materia)) {}

const std::string& Profesor::getEspecialidad() const {
    return especialidad_;
}

const std::string& Profesor::getMateria() const {
    return materia_;
}

std::string Profesor::toFileLine() const {
    return getCedula() + "," + getNombre() + "," + getCorreo() + "," +
           especialidad_ + "," + materia_;
}

Repositorio::Repositorio(const std::string& basePath)
    : alumnosPath_(rutaArchivo(basePath, "alumnos.txt")),
      profesoresPath_(rutaArchivo(basePath, "profesores.txt")),
      certificadosPath_(rutaArchivo(basePath, "certificados_pendientes.txt")) {
    cargar();
}

Repositorio::~Repositorio() {
    liberarMemoria();
}

void Repositorio::liberarMemoria() {
    for (Alumno* alumno : alumnos_) {
        delete alumno;
    }
    for (Profesor* profesor : profesores_) {
        delete profesor;
    }
    alumnos_.clear();
    profesores_.clear();
}

void Repositorio::crearArchivosSiNoExisten() {
    std::ofstream alumnosArchivo(alumnosPath_, std::ios::app);
    std::ofstream profesoresArchivo(profesoresPath_, std::ios::app);
    std::ofstream certificadosArchivo(certificadosPath_, std::ios::app);
    if (!alumnosArchivo || !profesoresArchivo || !certificadosArchivo) {
        throw ValidationError("No se pudieron crear los archivos de persistencia.");
    }
}

ProgramaAcademico* Repositorio::crearPrograma(const std::string& nombre) const {
    const std::string tipo = minusculas(trim(nombre));
    if (tipo == "curso") {
        return new Curso();
    }
    if (tipo == "diplomado") {
        return new Diplomado();
    }
    if (tipo == "bootcamp") {
        return new Bootcamp();
    }
    throw ValidationError("Programa académico no válido.");
}

void Repositorio::cargarAlumnos() {
    std::ifstream archivo(alumnosPath_);
    std::string linea;
    while (std::getline(archivo, linea)) {
        if (esLineaVacia(linea)) {
            continue;
        }
        const std::vector<std::string> partes = separar(linea, ',');
        if (partes.size() != 7) {
            continue;
        }
        try {
            std::vector<double> notas;
            for (std::size_t indice = 4; indice < 7; ++indice) {
                notas.push_back(std::stod(trim(partes[indice])));
            }
            bool hayNota = std::any_of(notas.begin(), notas.end(), [](double nota) { return nota != 0.0; });
            if (!hayNota) {
                notas.clear();
            }
            alumnos_.push_back(new Alumno(trim(partes[0]), trim(partes[1]), trim(partes[2]),
                                          crearPrograma(partes[3]), notas));
        } catch (const std::exception&) {
        }
    }
}

void Repositorio::cargarProfesores() {
    std::ifstream archivo(profesoresPath_);
    std::string linea;
    while (std::getline(archivo, linea)) {
        if (esLineaVacia(linea)) {
            continue;
        }
        const std::vector<std::string> partes = separar(linea, ',', 5);
        if (partes.size() == 5) {
            profesores_.push_back(new Profesor(trim(partes[0]), trim(partes[1]), trim(partes[2]),
                                               trim(partes[3]), trim(partes[4])));
        }
    }
}

void Repositorio::cargar() {
    crearArchivosSiNoExisten();
    liberarMemoria();
    cargarAlumnos();
    cargarProfesores();
}

void Repositorio::guardarAlumnos() const {
    std::ofstream archivo(alumnosPath_, std::ios::trunc);
    if (!archivo) {
        throw ValidationError("No se pudo guardar alumnos.txt.");
    }
    for (const Alumno* alumno : alumnos_) {
        archivo << alumno->toFileLine() << '\n';
    }
}

void Repositorio::guardarProfesores() const {
    std::ofstream archivo(profesoresPath_, std::ios::trunc);
    if (!archivo) {
        throw ValidationError("No se pudo guardar profesores.txt.");
    }
    for (const Profesor* profesor : profesores_) {
        archivo << profesor->toFileLine() << '\n';
    }
}

std::vector<Alumno*>& Repositorio::getAlumnos() {
    return alumnos_;
}

const std::vector<Alumno*>& Repositorio::getAlumnos() const {
    return alumnos_;
}

std::vector<Profesor*>& Repositorio::getProfesores() {
    return profesores_;
}

const std::vector<Profesor*>& Repositorio::getProfesores() const {
    return profesores_;
}

const std::string& Repositorio::getCertificadosPath() const {
    return certificadosPath_;
}

Alumno* Repositorio::buscarAlumno(const std::string& cedula) const {
    const std::string buscada = trim(cedula);
    for (Alumno* alumno : alumnos_) {
        if (minusculas(alumno->getCedula()) == minusculas(buscada)) {
            return alumno;
        }
    }
    return nullptr;
}

bool Repositorio::existeCedula(const std::string& cedula) const {
    const std::string buscada = minusculas(trim(cedula));
    for (const Alumno* alumno : alumnos_) {
        if (minusculas(alumno->getCedula()) == buscada) {
            return true;
        }
    }
    for (const Profesor* profesor : profesores_) {
        if (minusculas(profesor->getCedula()) == buscada) {
            return true;
        }
    }
    return false;
}

SistemaGestionAcademica::SistemaGestionAcademica(const std::string& basePath)
    : repositorio_(basePath) {}

Repositorio& SistemaGestionAcademica::getRepositorio() {
    return repositorio_;
}

const Repositorio& SistemaGestionAcademica::getRepositorio() const {
    return repositorio_;
}

ProgramaAcademico* SistemaGestionAcademica::crearPrograma(const std::string& nombre) const {
    const std::string tipo = minusculas(trim(nombre));
    if (tipo == "curso") {
        return new Curso();
    }
    if (tipo == "diplomado") {
        return new Diplomado();
    }
    if (tipo == "bootcamp") {
        return new Bootcamp();
    }
    throw ValidationError("Seleccione Curso, Diplomado o Bootcamp.");
}

Alumno* SistemaGestionAcademica::registrarAlumno(const std::string& cedula, const std::string& nombre,
                                                  const std::string& correo, const std::string& tipoPrograma) {
    if (trim(cedula).empty() || trim(nombre).empty() || trim(correo).empty()) {
        throw ValidationError("Cédula, nombre y correo son obligatorios.");
    }
    if (repositorio_.existeCedula(cedula)) {
        throw ValidationError("La cédula/ID ya está registrada.");
    }
    Alumno* alumno = new Alumno(trim(cedula), trim(nombre), trim(correo), crearPrograma(tipoPrograma));
    repositorio_.getAlumnos().push_back(alumno);
    repositorio_.guardarAlumnos();
    return alumno;
}

Profesor* SistemaGestionAcademica::registrarProfesor(const std::string& cedula, const std::string& nombre,
                                                     const std::string& correo, const std::string& especialidad,
                                                     const std::string& materia) {
    if (trim(cedula).empty() || trim(nombre).empty() || trim(correo).empty() ||
        trim(especialidad).empty() || trim(materia).empty()) {
        throw ValidationError("Todos los campos del profesor son obligatorios.");
    }
    if (repositorio_.existeCedula(cedula)) {
        throw ValidationError("La cédula/ID ya está registrada.");
    }
    Profesor* profesor = new Profesor(trim(cedula), trim(nombre), trim(correo), trim(especialidad), trim(materia));
    repositorio_.getProfesores().push_back(profesor);
    repositorio_.guardarProfesores();
    return profesor;
}

Alumno* SistemaGestionAcademica::registrarNota(const std::string& cedula, double nota) {
    Alumno* alumno = repositorio_.buscarAlumno(cedula);
    if (alumno == nullptr) {
        throw ValidationError("No se encontró un alumno con esa cédula/ID.");
    }
    alumno->agregarNota(nota);
    pilaDeshacer_.push({alumno->getCedula(), nota});
    repositorio_.guardarAlumnos();
    return alumno;
}

Alumno* SistemaGestionAcademica::deshacerUltimaNota() {
    if (pilaDeshacer_.empty()) {
        throw ValidationError("No hay registros de notas para deshacer.");
    }
    const RegistroDeshacer registro = pilaDeshacer_.top();
    pilaDeshacer_.pop();
    Alumno* alumno = repositorio_.buscarAlumno(registro.cedula);
    if (alumno == nullptr) {
        throw ValidationError("El alumno asociado ya no existe.");
    }
    alumno->eliminarUltimaNota();
    repositorio_.guardarAlumnos();
    return alumno;
}

std::vector<Alumno*> SistemaGestionAcademica::generarColaCertificados() {
    while (!colaCertificados_.empty()) {
        colaCertificados_.pop();
    }
    for (Alumno* alumno : repositorio_.getAlumnos()) {
        if (alumno->getNotas().size() == 3 && alumno->estaAprobado()) {
            colaCertificados_.push(alumno);
        }
    }

    std::vector<Alumno*> graduandos;
    while (!colaCertificados_.empty()) {
        graduandos.push_back(colaCertificados_.front());
        colaCertificados_.pop();
    }

    std::ofstream archivo(repositorio_.getCertificadosPath(), std::ios::trunc);
    if (!archivo) {
        throw ValidationError("No se pudo generar certificados_pendientes.txt.");
    }
    archivo << "=========================================\n"
            << "REPORTE DE CERTIFICADOS PENDIENTES\n"
            << "=========================================\n"
            << "Total de graduandos en cola: " << graduandos.size() << "\n\n";
    for (std::size_t indice = 0; indice < graduandos.size(); ++indice) {
        Alumno* alumno = graduandos[indice];
        archivo << indice + 1 << ". [" << alumno->getCedula() << "] " << alumno->getNombre() << "\n"
                << "- Programa: " << alumno->getPrograma().getNombre() << "\n"
                << "- Promedio Final: " << formatoPromedio(alumno->calcularPromedio()) << "\n"
                << "- Estatus: APROBADO";
        if (alumno->getPrograma().getNombre() == "Bootcamp") {
            archivo << " (Cumple regla de ninguna nota < 14)";
        }
        archivo << "\n\n";
    }
    archivo << "=========================================\n"
            << "* Fin del reporte - Generado por SGA-DO *\n";
    return graduandos;
}

std::string SistemaGestionAcademica::reporteGeneral() const {
    std::ostringstream reporte;
    reporte << "=========================================\n"
            << "REPORTE GENERAL SGA-DO\n"
            << "=========================================\n\n"
            << "PROFESORES ACTIVOS\n";
    if (repositorio_.getProfesores().empty()) {
        reporte << "- No hay profesores registrados.\n";
    } else {
        for (const Profesor* profesor : repositorio_.getProfesores()) {
            reporte << "- [" << profesor->getCedula() << "] " << profesor->getNombre() << " | "
                    << profesor->getEspecialidad() << " | " << profesor->getMateria() << "\n";
        }
    }
    reporte << "\nALUMNOS REGISTRADOS\n";
    if (repositorio_.getAlumnos().empty()) {
        reporte << "- No hay alumnos registrados.\n";
    } else {
        for (const Alumno* alumno : repositorio_.getAlumnos()) {
            reporte << "- [" << alumno->getCedula() << "] " << alumno->getNombre() << " | "
                    << alumno->getPrograma().getNombre() << " | Notas: ";
            if (alumno->getNotas().empty()) {
                reporte << "Sin notas";
            } else {
                for (std::size_t indice = 0; indice < alumno->getNotas().size(); ++indice) {
                    if (indice > 0) {
                        reporte << ", ";
                    }
                    reporte << formatoNumero(alumno->getNotas()[indice]);
                }
            }
            reporte << " | Promedio: " << formatoPromedio(alumno->calcularPromedio())
                    << " | " << alumno->obtenerEstatus() << "\n";
        }
    }
    return reporte.str();
}
