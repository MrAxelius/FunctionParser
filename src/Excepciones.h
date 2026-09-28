#pragma once

#include <stdexcept>
#include <FunctionParser/Excepciones.h>


// Internos
struct ErrorNoEsValorEsperado : std::runtime_error
{
    size_t posicion;
    ErrorNoEsValorEsperado(const std::string &mensaje, size_t posicion)
        : std::runtime_error(mensaje), posicion(posicion) {}
};
struct ErrorEnDesarrollo : std::runtime_error
{
    ErrorEnDesarrollo(const std::string &mensaje)
        : std::runtime_error(mensaje) {}
};



// Públicos
struct ErrorLexico : FunctionParser::ExpressionError
{
    ErrorLexico(const std::string &mensaje, size_t posicion)
        : ExpressionError(mensaje, posicion) {}
};
struct ErrorDeFormato : FunctionParser::ExpressionError
{
    ErrorDeFormato(const std::string &mensaje, size_t posicion)
        : ExpressionError(mensaje, posicion) {}
};
struct ErrorParentesis : FunctionParser::ExpressionError
{
    ErrorParentesis(const std::string &mensaje, size_t posicion)
        : ExpressionError(mensaje, posicion) {}
};

struct ErrorDeNodos : FunctionParser::ExpressionError
{
    ErrorDeNodos(const std::string &mensaje, size_t posicion)
        : ExpressionError(mensaje, posicion) {}
};
