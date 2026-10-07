#include "Parser.h"
#include "Token.h"
#include "Nodo.h"
#include "Excepciones.h"

#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <stack>
#include <optional>
#include <algorithm>

namespace
{
    void desapilarOperador(std::stack<Token> &pilaOperadores,
                           std::stack<std::unique_ptr<Nodo>> &pilaOperandos,
                           int numeroHijos)
    {
        std::vector<std::unique_ptr<Nodo>> hijos;
        size_t numero = static_cast<size_t>(numeroHijos);
        hijos.reserve(numero);
        auto sacado = pilaOperadores.top();
        pilaOperadores.pop();
        for (size_t i = 0; i < numero; ++i)
        {
            if (pilaOperandos.empty())
            {
                throw ErrorLexico("Incorrect formatting on the expression", sacado.posicion);
            }
            hijos.push_back(std::move(pilaOperandos.top()));
            pilaOperandos.pop();
        }
        std::ranges::reverse(hijos);
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
    enum class Estado
    {
        ESPERA_OPERANDO,
        ESPERA_OPERADOR,
        ESPERA_APERTURA
    };
    enum class Categoria
    {
        OPERANDO,
        OPERADOR,
        APERTURA
    };

    Categoria categoriaToken(const Token &token)
    {
        switch (token.tipo)
        {
        case TokenType::NUMERO:
        case TokenType::VARIABLE:
        case TokenType::SIN:
        case TokenType::COS:
        case TokenType::TAN:
        case TokenType::LOG:
        case TokenType::POW:
        case TokenType::NRT:
            return Categoria::OPERANDO;

        case TokenType::ADD:
        case TokenType::SUB:
        case TokenType::DIVIDE:
        case TokenType::MULT:
        case TokenType::COMA:
        case TokenType::CIERRA_PARENTESIS:
            return Categoria::OPERADOR;

        case TokenType::ABRE_PARENTESIS:
            return Categoria::APERTURA;

        case TokenType::NEGACION:
            // Change if NEGACION can be sent, rn is created later, so cannot exist here
            throw ErrorEnDesarrollo("This is not an expected Token");
        }
        throw ErrorEnDesarrollo("TThis is not an expected Token");
    }
    void comprobarCategoriaToken(Estado estadoEsperado, Categoria categoriaEntrada, size_t posicion)
    {
        if (estadoEsperado == Estado::ESPERA_APERTURA && categoriaEntrada != Categoria::APERTURA)
        {
            throw ErrorDeFormato("Expected an '('", posicion);
        }
        if (estadoEsperado == Estado::ESPERA_OPERADOR && categoriaEntrada != Categoria::OPERADOR)
        {
            throw ErrorDeFormato("Expected an operator", posicion);
        }
        if (estadoEsperado == Estado::ESPERA_OPERANDO && categoriaEntrada == Categoria::OPERADOR) // Must allow '('
        {
            throw ErrorDeFormato("Expected an operand", posicion);
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

        Estado estadoEsperado = Estado::ESPERA_OPERANDO;
        Categoria categoria;

        for (const auto &elemento : tokens)
        {
            categoria = categoriaToken(elemento);
            if (estadoEsperado != Estado::ESPERA_OPERANDO || elemento.tipo != TokenType::SUB)
            {
                comprobarCategoriaToken(estadoEsperado, categoria, elemento.posicion);
            }

            if (elemento.tipo == TokenType::ABRE_PARENTESIS)
            {
                estadoEsperado = Estado::ESPERA_OPERANDO;
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
                estadoEsperado = Estado::ESPERA_OPERADOR;
                // Eliminar el paréntesis de apertura
                if (pilaOperadores.empty())
                    throw ErrorParentesis("Open parenthesis has nowhere to close", elemento.posicion);
                pilaOperadores.pop();

                if (!pilaOperadores.empty() && esFuncion(pilaOperadores.top()))
                {
                    estadoEsperado = Estado::ESPERA_OPERADOR;
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
                estadoEsperado = Estado::ESPERA_APERTURA;
            }
            else if (elemento.tipo == TokenType::COMA)
            {
                estadoEsperado = Estado::ESPERA_OPERANDO;
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
            else if (elemento.tipo == TokenType::SUB && estadoEsperado == Estado::ESPERA_OPERANDO)
            {
                Token tokenSub(TokenType::NEGACION, elemento.posicion);
                pilaOperadores.push(tokenSub);
                estadoEsperado = Estado::ESPERA_OPERANDO;
            }
            else if (elemento.tipo >= TokenType::ADD && elemento.tipo <= TokenType::DIVIDE) // Esto son los operadores
            {
                estadoEsperado = Estado::ESPERA_OPERANDO;
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
                estadoEsperado = Estado::ESPERA_OPERADOR;
            }
        }
        if (estadoEsperado != Estado::ESPERA_OPERADOR)
        {
            throw ErrorDeFormato("The expression is not complete.", tokens.back().posicion);
        }
        // Empty the stack
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