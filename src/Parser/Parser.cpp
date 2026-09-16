#include "Parser.h"

#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <stack>
#include <optional>

#include "Token.h"
#include "Nodo.h"
#include "Excepciones.h"
namespace
{
    void desapilarOperador(std::stack<Token> &pilaOperadores,
                           std::stack<std::unique_ptr<Nodo>> &pilaOperandos,
                           int numeroHijos)
    {
        std::vector<std::unique_ptr<Nodo>> hijos;
        hijos.resize(numeroHijos);
        auto sacado = pilaOperadores.top();
        pilaOperadores.pop();
        for (int i = numeroHijos - 1; i >= 0; --i)
        {
            if (pilaOperandos.empty())
            {
                throw ErrorLexico("Error en el formato de la expresion", sacado.posicion);
            }
            hijos[i] = std::move(pilaOperandos.top());
            pilaOperandos.pop();
        }
        auto nodo = std::make_unique<Nodo>(sacado, std::move(hijos));
        pilaOperandos.push(std::move(nodo));
    }

    int prioridad(const Token &token)
    {
        switch (token.tipo)
        {
        case TokenType::ADD: // +
            return 1;
        case TokenType::SUB: // -
            return 1;
        case TokenType::MULT: // *
            return 2;
        case TokenType::DIVIDE: // /
            return 2;
        default:
            throw ErrorDeFormato("Hay un problema con la expresión", token.posicion);
        }
    }

    int aridad(const Token &token)
    {
        switch (token.tipo)
        {
        case TokenType::POW:
        case TokenType::NRT:
        case TokenType::LOG:
            return 2;

        case TokenType::SIN:
        case TokenType::COS:
        case TokenType::TAN:
            return 1;

        default:
            throw ErrorDeFormato("El token no es una funcion", token.posicion);
        }
    }

    bool esFuncion(const Token &token)
    {
        if (token.tipo >= TokenType::POW && token.tipo <= TokenType::LOG)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}
namespace Parser
{
    std::unique_ptr<Nodo> ShuntingYard(const std::vector<Token> &tokens)
    {
        std::stack<Token> pilaOperadores;
        std::stack<std::unique_ptr<Nodo>> pilaOperandos;
        std::stack<int> pilaComas;

        for (const auto &elemento : tokens)
        {
            if (elemento.tipo == TokenType::ABRE_PARENTESIS)
            {
                if (!pilaOperadores.empty() && esFuncion(pilaOperadores.top()))
                {
                    pilaComas.push(1);
                }
                pilaOperadores.push(elemento);
            }
            else if (elemento.tipo == TokenType::CIERRA_PARENTESIS)
            {
                while (!pilaOperadores.empty() && pilaOperadores.top().tipo != TokenType::ABRE_PARENTESIS)
                {
                    desapilarOperador(pilaOperadores, pilaOperandos, 2);
                }
                // Eliminar el paréntesis de apertura
                if (pilaOperadores.empty())
                    throw ErrorParentesis("Hay un desbalance en los paréntesis de apertura", elemento.posicion);
                pilaOperadores.pop();

                if (!pilaOperadores.empty() && esFuncion(pilaOperadores.top()))
                {
                    if (pilaComas.empty())
                    {
                        throw ErrorParentesis("Faltan argumentos en la funcion", pilaOperadores.top().posicion);
                    }
                    auto contador = pilaComas.top();
                    pilaComas.pop();
                    if (!pilaOperadores.empty() && contador != aridad(pilaOperadores.top()))
                    {
                        throw ErrorDeFormato("La funcion no cumple con los parámetros esperados", pilaOperadores.top().posicion);
                    }
                    desapilarOperador(pilaOperadores, pilaOperandos, contador);
                }
            }
            else if (elemento.tipo >= TokenType::POW && elemento.tipo <= TokenType::LOG)
            {
                pilaOperadores.push(elemento);
            }
            else if (elemento.tipo == TokenType::COMA)
            {
                if (pilaOperadores.empty())
                {
                    throw ErrorParentesis("Hay una coma que no corresponde", elemento.posicion);
                }
                while (!pilaOperadores.empty() && pilaOperadores.top().tipo != TokenType::ABRE_PARENTESIS)
                {
                    desapilarOperador(pilaOperadores, pilaOperandos, 2);
                }
                if(pilaComas.empty()){
                    throw ErrorDeFormato("Coma inesperada en la expresion", elemento.posicion);
                }
                pilaComas.top() += 1;
            }
            else if (elemento.tipo >= TokenType::ADD && elemento.tipo <= TokenType::DIVIDE) // Esto son los operadores
            {
                while (!pilaOperadores.empty() && pilaOperadores.top().tipo != TokenType::ABRE_PARENTESIS && prioridad(pilaOperadores.top()) >= prioridad(elemento))
                {
                    desapilarOperador(pilaOperadores, pilaOperandos, 2);
                }
                pilaOperadores.push(elemento);
            }
            else
            {
                auto nodo = std::make_unique<Nodo>(elemento, nullptr, nullptr);
                pilaOperandos.push(std::move(nodo));
            }
        }
        while (!pilaOperadores.empty())
        {
            if (pilaOperadores.top().tipo == TokenType::ABRE_PARENTESIS)
                throw ErrorParentesis("Hay un paréntesis sin cerrar", pilaOperadores.top().posicion);
            desapilarOperador(pilaOperadores, pilaOperandos, 2);
        }
        if (pilaOperandos.empty())
        {
            throw ErrorDeNodos("Hay operandos incorrectos");
        }
        auto ultimoElemento = std::move(pilaOperandos.top());
        pilaOperandos.pop();
        if (!pilaOperandos.empty())
        {
            throw ErrorDeNodos("Hay operandos incorrectos");
        }
        return ultimoElemento;
    }
}