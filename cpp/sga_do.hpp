#ifndef SGA_DO_HPP
#define SGA_DO_HPP

#include <queue>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

class ProgramaAcademico {
private:
    std::string nombre_;

public:
    explicit ProgramaAcademico(std::string nombre);
    virtual ~ProgramaAcademico() = default;

    const std::string& getNombre() const;
    virtual bool evaluarAprobacion(const std::vector<double>& notas) const = 0;
    virtual std::string obtenerCriterio() const = 0;
};

class Curso final : public ProgramaAcademico {
public:
    Curso();
    bool evaluarAprobacion(const std::vector<double>& notas) const override;
    std::string obtenerCriterio() const override;
};

class Diplomado final : public ProgramaAcademico {
public:
    Diplomado();
    bool evaluarAprobacion(const std::vector<double>& notas) const override;
    std::string obtenerCriterio() const override;
};

class Bootcamp final : public ProgramaAcademico {
public:
    Bootcamp();
    bool evaluarAprobacion(const std::vector<double>& notas) const override;
    std::string obtenerCriterio() const override;
};

class ValidationError : public std::runtime_error {
public:
    explicit ValidationError(const std::string& mensaje);
};

class Persona {
private:
    std::string cedula_;
    std::string nombre_;
    std::string correo_;

public:
    Persona(std::string cedula, std::string nombre, std::string correo);
    virtual ~Persona() = default;

    const std::string& getCedula() const;
    const std::string& getNombre() const;
    const std::string& getCorreo() const;
};

class Alumno final : public Persona {
private:
    ProgramaAcademico* programa_;
    std::vector<double> notas_;

public:
    Alumno(std::string cedula, std::string nombre, std::string correo,
           ProgramaAcademico* programa, std::vector<double> notas = {});
    ~Alumno() override;

    Alumno(const Alumno&) = delete;
    Alumno& operator=(const Alumno&) = delete;

    const ProgramaAcademico& getPrograma() const;
    const std::vector<double>& getNotas() const;
    void agregarNota(double nota);
    double eliminarUltimaNota();
    double calcularPromedio() const;
    bool estaAprobado() const;
    std::string obtenerEstatus() const;
    std::string toFileLine() const;
};

class Profesor final : public Persona {
private:
    std::string especialidad_;
    std::string materia_;

public:
    Profesor(std::string cedula, std::string nombre, std::string correo,
             std::string especialidad, std::string materia);

    const std::string& getEspecialidad() const;
    const std::string& getMateria() const;
    std::string toFileLine() const;
};

class Repositorio {
private:
    std::string alumnosPath_;
    std::string profesoresPath_;
    std::string certificadosPath_;
    std::vector<Alumno*> alumnos_;
    std::vector<Profesor*> profesores_;

    void crearArchivosSiNoExisten();
    void cargarAlumnos();
    void cargarProfesores();
    ProgramaAcademico* crearPrograma(const std::string& nombre) const;
    void liberarMemoria();

public:
    explicit Repositorio(const std::string& basePath);
    ~Repositorio();

    Repositorio(const Repositorio&) = delete;
    Repositorio& operator=(const Repositorio&) = delete;

    void cargar();
    void guardarAlumnos() const;
    void guardarProfesores() const;
    std::vector<Alumno*>& getAlumnos();
    const std::vector<Alumno*>& getAlumnos() const;
    std::vector<Profesor*>& getProfesores();
    const std::vector<Profesor*>& getProfesores() const;
    const std::string& getCertificadosPath() const;
    Alumno* buscarAlumno(const std::string& cedula) const;
    bool existeCedula(const std::string& cedula) const;
};

struct RegistroDeshacer {
    std::string cedula;
    double nota;
};

class SistemaGestionAcademica {
private:
    Repositorio repositorio_;
    std::stack<RegistroDeshacer> pilaDeshacer_;
    std::queue<Alumno*> colaCertificados_;

    ProgramaAcademico* crearPrograma(const std::string& nombre) const;

public:
    explicit SistemaGestionAcademica(const std::string& basePath);

    Repositorio& getRepositorio();
    const Repositorio& getRepositorio() const;
    Alumno* registrarAlumno(const std::string& cedula, const std::string& nombre,
                            const std::string& correo, const std::string& tipoPrograma);
    Profesor* registrarProfesor(const std::string& cedula, const std::string& nombre,
                                const std::string& correo, const std::string& especialidad,
                                const std::string& materia);
    Alumno* registrarNota(const std::string& cedula, double nota);
    Alumno* deshacerUltimaNota();
    std::vector<Alumno*> generarColaCertificados();
    std::string reporteGeneral() const;
};

double validarNota(double valor);
std::string trim(const std::string& valor);
std::string formatoNumero(double valor);
std::string formatoPromedio(double valor);

#endif
