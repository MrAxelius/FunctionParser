#pragma once

#include "Nodo.h"
#include "Token.h"
#include "Excepciones.h"

#include <optional>
#include <cmath>
#include <memory>

namespace Evaluador
{
    // Asumir que el Shunting yard funciona, y devuelve los nodos bien, así no revisar los hijos
    // La invarianza es correcta en el algoritmo, que solo genera nodos válidos, así que aquí no hay que revisar.
    std::optional<double> evaluacionRecursiva(const Nodo &nodo, double x)
    {
        // Es necesario el * para acceder al objeto entero y desreferenciarlo
        // no necesita () porque es el operando de menor preferencia
        switch (nodo.token.tipo)
        {
        case TokenType::NUMERO:
            return nodo.token.getValorNumerico();
        case TokenType::VARIABLE:
            return x;
        case TokenType::SIN:
        {
            auto arg = evaluacionRecursiva(*nodo.hijos[0], x);
            if (!arg)
                return std::nullopt;

            return std::sin(*arg);
        }
        case TokenType::COS:
        {
            auto arg = evaluacionRecursiva(*nodo.hijos[0], x);
            if (!arg)
                return std::nullopt;

            return std::cos(*arg);
        }
        case TokenType::TAN:
        {
            auto arg = evaluacionRecursiva(*nodo.hijos[0], x);
            if (!arg)
                return std::nullopt;

            return std::tan(*arg);
        }
        case TokenType::POW:
        {
            assert(nodo.hijos.size() == 2);
            auto base = evaluacionRecursiva(*nodo.hijos[0], x);
            auto exponente = evaluacionRecursiva(*nodo.hijos[1], x);

            if (!base || !exponente)
            {
                return std::nullopt;
            }

            return std::pow(*base, *exponente);
        }
        case TokenType::NRT:
        {
            assert(nodo.hijos.size() == 2);
            auto radicando = evaluacionRecursiva(*nodo.hijos[0], x);
            auto indice = evaluacionRecursiva(*nodo.hijos[1], x);
            if (!radicando || !indice)
            {
                return std::nullopt;
            }
            // Negar todo radicando negativo, incluso impares, cuando debería ser válido
            // No descarto soportarlo más adelante, ahora es por simplicidad de diseño
            if (*radicando < 0 || *indice == 0)
            {
                return std::nullopt;
            }
            return std::pow(*radicando, 1.0 / *indice);
        }

        case TokenType::LOG:
        {
            assert(nodo.hijos.size() == 2);
            auto argumento = evaluacionRecursiva(*nodo.hijos[0], x);
            auto base = evaluacionRecursiva(*nodo.hijos[1], x);
            if (!base || !argumento)
            {
                return std::nullopt;
            }
            if (*base <= 0 || *base == 1)
            {
                return std::nullopt;
            }

            if (*argumento <= 0)
            {
                return std::nullopt;
            }
            return (std::log(*argumento) / std::log(*base));
        }
        case TokenType::ADD:
        {
            assert(nodo.hijos.size() == 2);
            auto arg1 = evaluacionRecursiva(*nodo.hijos[0], x);
            auto arg2 = evaluacionRecursiva(*nodo.hijos[1], x);
            if (!arg1 || !arg2)
            {
                return std::nullopt;
            }
            return *arg1 + *arg2;
        }

        case TokenType::SUB:
        {
            assert(nodo.hijos.size() == 2);
            auto arg1 = evaluacionRecursiva(*nodo.hijos[0], x);
            auto arg2 = evaluacionRecursiva(*nodo.hijos[1], x);
            if (!arg1 || !arg2)
            {
                return std::nullopt;
            }
            return *arg1 - *arg2;
        }
        case TokenType::DIVIDE:
        {
            assert(nodo.hijos.size() == 2);
            auto divisor = evaluacionRecursiva(*nodo.hijos[1], x);
            auto dividendo = evaluacionRecursiva(*nodo.hijos[0], x);
            if (!dividendo)
            {
                return std::nullopt;
            }
            if (!divisor || *divisor == 0)
            {
                return std::nullopt;
            }
            return *dividendo / *divisor;
        }

        case TokenType::MULT:
        {
            assert(nodo.hijos.size() == 2);
            auto arg1 = evaluacionRecursiva(*nodo.hijos[0], x);
            auto arg2 = evaluacionRecursiva(*nodo.hijos[1], x);
            if (!arg1 || !arg2)
            {
                return std::nullopt;
            }
            return (*arg1) * (*arg2);
        }
        default:
            throw ErrorEnDesarrollo("Algo ha salido mal al evaluar la función");
        }
    }
}
/*
+
| \
4  *
    |\
    3 2
*/