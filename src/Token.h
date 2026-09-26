#pragma once

#include <utility>
#include <variant>
#include <string>

#include "Excepciones.h"

enum class TokenType
{
    // OPERADORES
    ADD,
    SUB,
    MULT,
    DIVIDE,
    // Por el momento 0-3
    POW,
    NRT,
    SIN,
    COS,
    TAN,
    LOG, // 10
    NEGACION,
    // FIN OPERADORES

    ABRE_PARENTESIS,
    CIERRA_PARENTESIS,
    COMA,
    VARIABLE,
    NUMERO,
};

struct Token
{
    TokenType tipo;
    std::variant<double, char> valor;
    size_t posicion;

    // Constructor para números
    Token(double num, size_t pos) : tipo(TokenType::NUMERO), valor(num), posicion(pos) {}
    // Constructor para variables
    Token(char var, size_t pos) : tipo(TokenType::VARIABLE), valor(var), posicion(pos) {}
    // Constructor para tokens sin valor
    Token(TokenType t, size_t pos) : tipo(t), valor(0.0), posicion(pos) {}

    [[nodiscard]] double getValorNumerico() const
    {
        if(std::holds_alternative<double>(valor))
        {
            return std::get<double>(valor);
        }else
        {
            throw ErrorNoEsValorEsperado("El token no tiene un valor numerico ", posicion);
        }
    }
    [[nodiscard]] char getNombre() const
    {
        if(std::holds_alternative<char>(valor)){
            return std::get<char>(valor);
        }
        else{
            throw ErrorNoEsValorEsperado("El token no tiene un valor de carácter", posicion);
        }
    }
};
