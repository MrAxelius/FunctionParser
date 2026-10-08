#pragma once

#include "Nodo.h"
#include "Token.h"
#include "Excepciones.h"

#include <cmath>
#include <memory>
#include <cassert>
#include <limits>

namespace Evaluador
{
    // Asumir que el Shunting yard funciona, y devuelve los nodos bien, así no revisar los hijos
    // La invarianza es correcta en el algoritmo, que solo genera nodos válidos, así que aquí no hay que revisar.
    /*
    +
    | \
    4  *
        |\
        3 2
    */
    inline double evaluacionRecursiva(const Nodo &nodo, double x)
    {
        switch (nodo.token.tipo)
        {
        case TokenType::NUMERO:
            return nodo.token.getValorNumerico();
        case TokenType::VARIABLE:
            return x;
        case TokenType::SIN:
        {
            assert(nodo.hijos.size() == 1);
            // Es necesario el * para acceder al objeto entero y desreferenciarlo
            auto arg = evaluacionRecursiva(*nodo.hijos[0], x);
            return std::sin(arg);
        }
        case TokenType::NEGACION:
        {
            assert(nodo.hijos.size() == 1);
            auto arg = evaluacionRecursiva(*nodo.hijos[0], x);
            return -(arg);
        }
        case TokenType::COS:
        {
            assert(nodo.hijos.size() == 1);
            auto arg = evaluacionRecursiva(*nodo.hijos[0], x);
            return std::cos(arg);
        }
        case TokenType::TAN:
        {
            assert(nodo.hijos.size() == 1);
            auto arg = evaluacionRecursiva(*nodo.hijos[0], x);
            return std::tan(arg);
        }
        case TokenType::POW:
        {
            assert(nodo.hijos.size() == 2);
            auto base = evaluacionRecursiva(*nodo.hijos[0], x);
            auto exponente = evaluacionRecursiva(*nodo.hijos[1], x);
            return std::pow(base, exponente);
        }
        case TokenType::NRT:
        {
            assert(nodo.hijos.size() == 2);
            auto radicando = evaluacionRecursiva(*nodo.hijos[0], x);
            auto indice = evaluacionRecursiva(*nodo.hijos[1], x);

            // Negar todo radicando negativo, incluso impares, cuando debería ser válido
            // No descarto soportarlo más adelante, ahora es por simplicidad de diseño
            if (radicando < 0 || indice == 0)
            {
                return std::numeric_limits<double>::quiet_NaN();
            }
            return std::pow(radicando, 1.0 / indice);
        }

        case TokenType::LOG:
        {
            assert(nodo.hijos.size() == 2);
            auto argumento = evaluacionRecursiva(*nodo.hijos[0], x);
            auto base = evaluacionRecursiva(*nodo.hijos[1], x);
            if (base == 0 || !std::isfinite(base))
            {
                return std::numeric_limits<double>::quiet_NaN();
            }
            return (std::log(argumento) / std::log(base));
        }
        case TokenType::ADD:
        {
            assert(nodo.hijos.size() == 2);
            auto arg1 = evaluacionRecursiva(*nodo.hijos[0], x);
            auto arg2 = evaluacionRecursiva(*nodo.hijos[1], x);
            return arg1 + arg2;
        }

        case TokenType::SUB:
        {
            assert(nodo.hijos.size() == 2);
            auto arg1 = evaluacionRecursiva(*nodo.hijos[0], x);
            auto arg2 = evaluacionRecursiva(*nodo.hijos[1], x);
            return arg1 - arg2;
        }
        case TokenType::DIVIDE:
        {
            assert(nodo.hijos.size() == 2);
            auto dividendo = evaluacionRecursiva(*nodo.hijos[0], x);
            auto divisor = evaluacionRecursiva(*nodo.hijos[1], x);

            return dividendo / divisor;
        }

        case TokenType::MULT:
        {
            assert(nodo.hijos.size() == 2);
            auto arg1 = evaluacionRecursiva(*nodo.hijos[0], x);
            auto arg2 = evaluacionRecursiva(*nodo.hijos[1], x);

            return (arg1) * (arg2);
        }
        case TokenType::ABRE_PARENTESIS:
        case TokenType::CIERRA_PARENTESIS:
        case TokenType::COMA:
            throw ErrorEnDesarrollo("This token cannot be a node");
        }
        throw ErrorEnDesarrollo("Token holds an undeclared type");
    }
}