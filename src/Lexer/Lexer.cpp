// Namespace anónimo y oculto, solo como helper
#include "Token.h"
#include "Excepciones.h"

#include <cctype>
#include <string>
#include <cstddef>
#include <charconv>
#include <vector>
#include <unordered_map>
#include <string_view>
#include <numbers>


namespace
{
    Token tokenNumero(const std::string &expresion, size_t &pos)
    {
        size_t start = pos;
        bool hayPunto = false;

        while (pos < expresion.size() && ((expresion[pos] >= '0' && expresion[pos] <= '9') || expresion[pos] == '.'))
        {
            if (expresion[pos] == '.')
            {
                if (hayPunto)
                {
                    throw ErrorLexico("Hay más de 1 punto decimal", pos); 
                }
                hayPunto = true;
            }
            pos++;
        }

        double numero;
        auto resultado = std::from_chars(expresion.data() + start, expresion.data() + pos, numero);
        if (resultado.ec != std::errc())
        {
            throw ErrorLexico("El número no es válido",start);
        }
        return Token(numero, start);
    }

    Token tokenSimbolo(const char caracter, size_t posicion)
    {
        switch (caracter)
        {
        case '+':
            return Token(TokenType::ADD, posicion);
        case '-':
            return Token(TokenType::SUB, posicion);
        case '*':
            return Token(TokenType::MULT, posicion);
        case '/':
            return Token(TokenType::DIVIDE, posicion);
        default:
            throw ErrorLexico("No es un operador", posicion);
        }
    }

    Token tokenCaracterEspecial(const char caracter, size_t posicion)
    {
        switch (caracter)
        {
        case ',':
            return Token(TokenType::COMA, posicion);
        case ';':
            return Token(TokenType::FIN_SECUENCIA, posicion);
        case '(':
            return Token(TokenType::ABRE_PARENTESIS, posicion);
        case ')':
            return Token(TokenType::CIERRA_PARENTESIS, posicion);
        default:
            throw ErrorLexico("No es un caracter especial esperado", posicion);
        }
    }

    // Mapeo de funciones
    const std::unordered_map<std::string_view, TokenType> funcionMap = {
        // first es la string
        // second es el tokentype
        {"sin", TokenType::SIN},
        {"cos", TokenType::COS},
        {"tan", TokenType::TAN},
        {"log", TokenType::LOG},
        {"pow", TokenType::POW},
        {"nrt", TokenType::NRT}};

    Token tokenIdentificador(const std::string &expresion, size_t &posicion)
    {
        size_t start = posicion;

        // Delimitar la palabra
        while (posicion < expresion.size() && (expresion[posicion] >= 'a' && expresion[posicion] <= 'z'))
        {
            posicion++;
        }

        std::string_view palabra(expresion.data() + start, posicion - start);

        // it recibe valor si palabra está en el mapa
        auto it = funcionMap.find(palabra);
        // Está en el mapa
        if (it != funcionMap.end())
        {
            // Second es el TokenType
            return Token(it->second, start);
        }
        if (palabra == "pi")
        {
            return Token(std::numbers::pi, start);
        }
        if (palabra == "e")
        {
            return Token(std::numbers::e, start);
        }
        if (palabra == "x")
        {
            return Token(palabra[0], start);
        }
        
        throw ErrorLexico("Identificador desconocido", start);
    }
}
namespace Lexer
{

    std::vector<Token> Tokenizar(const std::string &expresionInput)
    {
        
        size_t i = 0;
        std::vector<Token> tokens;
        tokens.reserve(expresionInput.size() / 3);
        while (i < expresionInput.size())
        {
            char c = expresionInput[i];
            if (std::isspace(static_cast<unsigned char>(c))) { ++i; continue; }
            if (std::isdigit(static_cast<unsigned char>(c)))
            {
                tokens.push_back(tokenNumero(expresionInput, i));
            }
            else if (std::isalpha(static_cast<unsigned char>(c)))
            {
                tokens.push_back(tokenIdentificador(expresionInput, i));
            }
            else if (c == '+' || c == '-' || c == '*' || c == '/')
            {
                tokens.push_back(tokenSimbolo(c, i));
                ++i;
            }
            else if (c == '(' || c == ')' || c == ',' || c == ';')
            {
                tokens.push_back(tokenCaracterEspecial(c, i));
                ++i;
            }
            else
            {
                throw ErrorLexico("Carácter inesperado en la expresión", i);
            }
        }
        return tokens;
    }
}
