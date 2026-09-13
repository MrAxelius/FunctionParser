#pragma once

#include "Nodo.h"
#include "Token.h"
#include "Excepciones.h"

#include <cmath>

namespace Evaluador
{
    double evaluacionRecursiva(const Nodo &nodo, double x)
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
            return std::sin(evaluacionRecursiva(*nodo.hijos[0], x));
        case TokenType::COS:
            return std::cos(evaluacionRecursiva(*nodo.hijos[0], x));
        case TokenType::TAN:
            return std::tan(evaluacionRecursiva(*nodo.hijos[0], x));

        case TokenType::POW:
            return std::pow(evaluacionRecursiva(*nodo.hijos[0], x), evaluacionRecursiva(*nodo.hijos[1], x));
        case TokenType::NRT:
        {
            double radicando = evaluacionRecursiva(*nodo.hijos[0], x);
            double indice = evaluacionRecursiva(*nodo.hijos[1], x);
            if (radicando < 0 || indice == 0)
            {
                throw ErrorNumerico("Hay un valor inesperado en la raiz", nodo.token.posicion);
            }
            return std::pow(radicando, 1.0 / indice);
        }

        case TokenType::LOG:
        {
            double argumento = evaluacionRecursiva(*nodo.hijos[0], x);
            double base = evaluacionRecursiva(*nodo.hijos[1], x);
            if (base <= 0)
            {
                throw ErrorNoEsValorEsperado("La base del logaritmo es cero o negativa", nodo.token.posicion);
            }
            if (base == 1)
            {
                throw ErrorNoEsValorEsperado("Ningun exponente de 1 da un valor válido", nodo.token.posicion);
            }
            if (argumento <= 0)
            {
                throw ErrorNoEsValorEsperado("El argumento del logaritmo es cero o negativo", nodo.token.posicion);
            }
            return (std::log(argumento) / std::log(base));
        }
        case TokenType::ADD:
            return evaluacionRecursiva(*nodo.hijos[0], x) + evaluacionRecursiva(*nodo.hijos[1], x);

        case TokenType::SUB:
            return evaluacionRecursiva(*nodo.hijos[0], x) - evaluacionRecursiva(*nodo.hijos[1], x);

        case TokenType::DIVIDE:
        {
            double divisor = evaluacionRecursiva(*nodo.hijos[1], x);
            if(divisor == 0){
                throw ErrorNoEsValorEsperado("No se puede dividir por cero", nodo.token.posicion);
            }
            return evaluacionRecursiva(*nodo.hijos[0], x) / divisor;
        }

        case TokenType::MULT:
            return evaluacionRecursiva(*nodo.hijos[0], x) * evaluacionRecursiva(*nodo.hijos[1], x);

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