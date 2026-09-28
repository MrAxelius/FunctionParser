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
                throw ErrorLexico("Incorrect formatting on the expression", sacado.posicion);
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
        case TokenType::NEGACION:
            return 3;
        default:
            throw ErrorDeFormato("There's been a problem with the expression", token.posicion);
        }
    }

    int aridad(const Token &token)
    {
        switch (token.tipo)
        {
        case TokenType::POW:
        case TokenType::NRT:
        case TokenType::LOG:
        case TokenType::ADD:
        case TokenType::SUB:
        case TokenType::MULT:
        case TokenType::DIVIDE:
            return 2;

        case TokenType::SIN:
        case TokenType::COS:
        case TokenType::TAN:
        case TokenType::NEGACION:
            return 1;

        default:
            throw ErrorDeFormato("The token is not a function", token.posicion);
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

        bool esperarOperando = true;

        for (const auto &elemento : tokens)
        {
            if (elemento.tipo == TokenType::ABRE_PARENTESIS)
            {
                esperarOperando = true;
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
                    desapilarOperador(pilaOperadores, pilaOperandos, aridad(pilaOperadores.top()));
                }
                esperarOperando = false;
                // Eliminar el paréntesis de apertura
                if (pilaOperadores.empty())
                    throw ErrorParentesis("Open parenthesis has nowhere to close", elemento.posicion);
                pilaOperadores.pop();

                if (!pilaOperadores.empty() && esFuncion(pilaOperadores.top()))
                {
                    if (pilaComas.empty())
                    {
                        throw ErrorParentesis("Too few arguments to call the function", pilaOperadores.top().posicion);
                    }
                    auto contador = pilaComas.top();
                    pilaComas.pop();
                    if (!pilaOperadores.empty() && contador != aridad(pilaOperadores.top()))
                    {
                        throw ErrorDeFormato("The function does not have the expected parameters", pilaOperadores.top().posicion);
                    }
                    desapilarOperador(pilaOperadores, pilaOperandos, contador);
                }
            }
            else if (elemento.tipo >= TokenType::POW && elemento.tipo <= TokenType::LOG)
            {
                pilaOperadores.push(elemento);
                esperarOperando = true;
            }
            else if (elemento.tipo == TokenType::COMA)
            {
                esperarOperando = true;
                if (pilaOperadores.empty())
                {
                    throw ErrorParentesis("There is a misplaced comma", elemento.posicion);
                }
                while (!pilaOperadores.empty() && pilaOperadores.top().tipo != TokenType::ABRE_PARENTESIS)
                {
                    desapilarOperador(pilaOperadores, pilaOperandos, aridad(pilaOperadores.top()));
                }
                if (pilaComas.empty())
                {
                    throw ErrorDeFormato("Unexpected comma on the expression", elemento.posicion);
                }
                pilaComas.top() += 1;
            }
            else if(elemento.tipo == TokenType::SUB && esperarOperando){
                Token tokenSub(TokenType::NEGACION, elemento.posicion);
                pilaOperadores.push(tokenSub);
                esperarOperando = true;
            }
            else if (elemento.tipo >= TokenType::ADD && elemento.tipo <= TokenType::DIVIDE) // Esto son los operadores
            {
                esperarOperando = true;
                while (!pilaOperadores.empty() && pilaOperadores.top().tipo != TokenType::ABRE_PARENTESIS && prioridad(pilaOperadores.top()) >= prioridad(elemento))
                {
                    desapilarOperador(pilaOperadores, pilaOperandos, aridad(pilaOperadores.top()));
                }
                pilaOperadores.push(elemento);
            }
            else
            {
                auto nodo = std::make_unique<Nodo>(elemento, nullptr, nullptr);
                pilaOperandos.push(std::move(nodo));
                esperarOperando = false;
            }
        }
        while (!pilaOperadores.empty())
        {
            if (pilaOperadores.top().tipo == TokenType::ABRE_PARENTESIS)
                throw ErrorParentesis("There is an open parenthesis", pilaOperadores.top().posicion);
            desapilarOperador(pilaOperadores, pilaOperandos, aridad(pilaOperadores.top()));
        }
        if (pilaOperandos.empty())
        {
            throw ErrorDeNodos("Missing operands", 0);
        }
        auto ultimoElemento = std::move(pilaOperandos.top());
        pilaOperandos.pop();
        if (!pilaOperandos.empty())
        {
            throw ErrorDeNodos("Too many operands", tokens.back().posicion);
        }
        return ultimoElemento;
    }
}