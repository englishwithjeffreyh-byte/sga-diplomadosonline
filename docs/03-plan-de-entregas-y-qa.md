# SGA-DO — Plan de entregas y QA

## Estructura inicial

```text
sga-diplomadosonline/
├── docs/
├── python/
├── java/
└── cpp/
```

## Orden de implementación solicitado

1. Python: motor Core de consola con persistencia, herencia, polimorfismo, pila y cola.
2. JavaScript: réplica funcional del motor manteniendo las mismas reglas y archivos.
3. C++: réplica compilada con `fstream` y liberación explícita de memoria cuando corresponda.

> La consigna de carpetas del PDF define `/java` y `/cpp`. La segunda implementación solicitada actualmente es JavaScript; antes de comenzar esa fase se debe confirmar si el código JavaScript debe vivir en `/java` o si se agregará una carpeta `/javascript`.

## Casos de prueba principales

### CP-01 — Cálculo polimórfico

- Ana Silva debe aprobar Curso con promedio 10.0.
- Carlos Pérez debe reprobar Diplomado con promedio 13.6.
- María Gómez debe reprobar Bootcamp aunque tenga promedio 17.6 porque una nota es 13.
- Luis Rojas debe aprobar Bootcamp con notas 14, 15 y 14.

### CP-02 — Deshacer LIFO

Registrar una nota y ejecutar la opción de deshacer. La última nota debe desaparecer del historial del alumno y de la persistencia.

### CP-03 — Exportación FIFO

El reporte debe contener exclusivamente a Ana Silva y Luis Rojas, en ese orden.

### EVAL-01 — Persistencia

Un alumno y sus notas deben mantenerse después de cerrar y volver a abrir el programa.

### EVAL-02 — Polimorfismo real

Las reglas deben vivir en las clases de programa, no en una cadena de condicionales del menú.

### EVAL-03 — Entrada inválida

Una opción no numérica, una entrada vacía o una nota como `veinte` deben mostrar un error amable y volver al menú sin cerrar la aplicación.
