# FUNCTION-PARSER

## WORK IN PROGRESS

## Descripción
Librería en C++20 destinada a la evaluación de funciones matemáticas.
El principal objetivo es aprender el diseño de APIs y librerías, así como servir de complemento para otro de mis proyectos actuales
[Render de funciones](https://github.com/mraxeliuss-cloud/Function-render)

## Estado actual
- Hace un análisis léxico y sintáctico de las expresiones
- Construye un AST con precedencia, los operadores básicos (+, -, *, /) y funciones
- Números, constantes (pi, e) y paréntesis funcionales.
- Evaluación recursiva de las expresiones

## Aún en desarrollo:
- Muestreo sobre rangos
- Fachada pública de la librería

## Ejemplos de sintaxis de las expresiones esperadas:
´´´
pi + e - 10
´´´

Las funciones están diseñadas para funcionar así:

```
pow(base, exponente) = base ^ exponente
log(base, argumento) = log_b a
nrt(indice, radicando) = indice√ radicando
sin(argumento)/cos(argumento)/tan(argumento)
```
