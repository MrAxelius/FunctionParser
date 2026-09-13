#pragma once

#include <stdexcept>

struct ErrorLexico : std::runtime_error
{
    size_t posicion;
    ErrorLexico(const std::string &mensaje, size_t posicion)
        : std::runtime_error(mensaje), posicion(posicion) {}
};
struct ErrorDeFormato : std::runtime_error
{
    size_t posicion;
    ErrorDeFormato(const std::string &mensaje, size_t posicion)
        : std::runtime_error(mensaje), posicion(posicion) {}
};
struct ErrorNoEsValorEsperado : std::runtime_error
{
    size_t posicion;
    ErrorNoEsValorEsperado(const std::string &mensaje, size_t posicion)
        : std::runtime_error(mensaje), posicion(posicion) {}
};
struct ErrorParentesis : std::runtime_error
{
    size_t posicion;
    ErrorParentesis(const std::string &mensaje, size_t posicion)
        : std::runtime_error(mensaje), posicion(posicion) {}
};
struct ErrorDeNodos : std::runtime_error
{
    ErrorDeNodos(const std::string &mensaje)
        : std::runtime_error(mensaje) {}
};
struct ErrorEnDesarrollo : std::runtime_error
{
    ErrorEnDesarrollo(const std::string &mensaje)
        : std::runtime_error(mensaje) {}
};
struct ErrorNumerico : std::runtime_error
{
    size_t posicion;
    ErrorNumerico(const std::string &mensaje, size_t posicion)
        : std::runtime_error(mensaje), posicion(posicion) {}
};
