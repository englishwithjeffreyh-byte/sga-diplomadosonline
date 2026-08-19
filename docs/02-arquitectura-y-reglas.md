# SGA-DO — Arquitectura y reglas de negocio

## Jerarquía de personas

```text
Persona
├── Alumno
│   ├── cedula
│   ├── nombre
│   ├── correo
│   ├── programaAcademico
│   └── notas[0..3]
└── Profesor
    ├── cedula
    ├── nombre
    ├── correo
    ├── especialidad
    └── materia
```

## Jerarquía de programas

```text
ProgramaAcademico
├── Curso
├── Diplomado
└── Bootcamp
```

Cada clase hija implementa `evaluarAprobacion()` o su equivalente idiomático. La lógica de aprobación no debe estar concentrada en el menú principal.

## Corrección del modelo

`Nota` y `Certificado` no son clases del dominio en este MVP:

- Las notas son valores numéricos almacenados en `Alumno.notas[0..3]`.
- La cola FIFO trabaja directamente con objetos `Alumno` aprobados.
- `certificados_pendientes.txt` es un reporte de salida, no una entidad persistente.
- La pila LIFO almacena referencias simples de la operación de deshacer, sin crear una clase `Nota`.

## Reglas de aprobación

- **Curso:** promedio de las notas mayor o igual a 10/20.
- **Diplomado:** promedio de las notas mayor o igual a 14/20.
- **Bootcamp:** las tres notas deben ser mayores o iguales a 14/20; el promedio no sustituye esta regla.
- Un alumno con menos de tres notas permanece en estado `PENDIENTE`.

## Estructuras de datos

- **Pila LIFO:** almacena cada registro de nota realizado durante la sesión para permitir `Deshacer Último Registro de Nota`.
- **Cola FIFO:** recibe a los alumnos aprobados en el mismo orden en que aparecen en `alumnos.txt` y los procesa para generar el reporte de certificados.

## Persistencia

### `alumnos.txt`

```text
Cedula,Nombre,Correo,TipoPrograma,Nota1,Nota2,Nota3
```

### `profesores.txt`

```text
Cedula,Nombre,Correo,Especialidad,Materia
```

### `certificados_pendientes.txt`

Reporte de salida generado cada vez que se procesa la cola de certificados.

## Principios de implementación

1. Leer los archivos al iniciar el programa.
2. Guardar inmediatamente los registros nuevos y las notas actualizadas.
3. Validar datos en los límites de entrada.
4. Evitar IDs duplicados entre alumnos y profesores.
5. Mantener el comportamiento equivalente en cada lenguaje.
