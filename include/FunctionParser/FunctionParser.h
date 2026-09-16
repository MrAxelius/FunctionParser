#pragma once
// Archivo maestro para el contrato público
#include <FunctionParser/Expression.h> // La clase principal (depende de las anteriores)
#include <FunctionParser/Sample.h>
#include <FunctionParser/Rango.h> // Estructuras de entrada (Range2D, Range3D)

// Compilación condicional, por si crece y versiona
#define MATH_EVAL_VERSION_MAJOR 1
#define MATH_EVAL_VERSION_MINOR 0
#define MATH_EVAL_VERSION_PATCH 0
#define MATH_EVAL_VERSION_STRING "1.0.0"