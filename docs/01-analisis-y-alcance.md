# SGA-DO — Análisis y alcance inicial

## Contexto

DiplomadosOnline ha crecido hasta gestionar miles de estudiantes, profesores, programas y evidencias académicas. El proceso actual depende de un archivo Excel compartido, lo que provoca duplicidad de registros, errores de digitación, pérdida de información, ausencia de trazabilidad y retrasos en la certificación.

## Objetivo general

Construir un Sistema de Gestión Académica que centralice la operación, automatice la evaluación y facilite la generación de certificados mediante persistencia local en archivos de texto.

## Alcance del MVP

- Registrar alumnos con cédula/ID, nombre, correo y programa.
- Registrar profesores con cédula/ID, nombre, correo, especialidad y materia.
- Registrar hasta tres notas por alumno.
- Calcular promedio y estatus académico según el programa.
- Deshacer el último registro de nota.
- Generar una cola de graduandos elegibles para certificación.
- Exportar el reporte de certificados pendientes.
- Mostrar un reporte general de alumnos y profesores.
- Persistir la información en archivos `.txt`.
- Validar entradas y evitar IDs duplicados.

## Fuera del alcance del MVP

- Autenticación multiusuario.
- Base de datos relacional.
- Emisión de certificados en PDF.
- Integraciones externas de correo o pagos.
- Panel de administración web conectado al motor de consola.

## Datos semilla de QA

| ID | Persona | Programa / especialidad | Notas | Resultado |
| --- | --- | --- | --- | --- |
| V-101 | Ana Silva | Curso | 10, 10, 10 | Aprobada |
| V-202 | Carlos Pérez | Diplomado | 14, 14, 13 | Reprobado |
| V-303 | María Gómez | Bootcamp | 20, 20, 13 | Reprobada |
| V-404 | Luis Rojas | Bootcamp | 14, 15, 14 | Aprobado |
| P-505 | Dr. Pedro Infante | Backend | N/A | Profesor |
