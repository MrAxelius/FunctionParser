# FunctionParser

> **Work in progress.** Funcionalidad básica operativa.

Librería en C++20 para el análisis y la evaluación de funciones matemáticas
de una variable.

El objetivo principal es aprender diseño de APIs y de librerías. Sirve además
como componente de [Function-render](https://github.com/MrAxelius/Function-render),
un visor de funciones.

## Estado actual

- Análisis léxico y sintáctico con errores posicionados.
- Construcción de un AST con precedencia, operadores básicos (`+`, `-`, `*`, `/`)
  y funciones.
- Números, constantes (`pi`, `e`), paréntesis y anidamiento de funciones.
- Evaluación recursiva y muestreo sobre un rango.
- Fachada pública con pImpl y versionado mediante `inline namespace`.

### Pendiente

- Menos unario (`-x`, `2 * -3`).
- Traducción de excepciones internas a la jerarquía pública.
- Funciones de dos y tres variables: no implementado. La versión `v1` cubre
  una sola variable.

## Sintaxis

Ejemplo de expresión válida:
pi + e - 10

Funciones disponibles:
- pow(base, exponente) = base ^ exponente
- log(argumento, base) = log_base(argumento)
- nrt(radicando, indice) = raíz de índice n sobre el radicando
- sin(x) / cos(x) / tan(x)


## Valores no evaluables

Cuando la función no tiene valor en un punto —división por cero, logaritmo
de un argumento no positivo, raíz de radicando negativo— el muestreo devuelve
igualmente el punto, con la coordenada `y` marcada como `NaN`. El vector
siempre contiene `pasos + 1` elementos, de modo que el índice se corresponde
con la posición en el eje.

Es responsabilidad del consumidor comprobar cada punto antes de usarlo:

```cpp
for (const auto& punto : puntos) {
    if (!std::isfinite(punto.y)) continue;  // sin valor en esta x
    // ...
}
```

Nunca compares con `==`: `NaN == NaN` es falso.
