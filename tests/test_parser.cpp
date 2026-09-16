#include <catch2/catch_test_macros.hpp>
#include <numbers>

#include "Lexer/Lexer.h"
#include "Parser/Parser.h"

TEST_CASE("Prueba de los parentesis")
{
    auto tokens = Lexer::Tokenizar("pi + (e - 1)");
    auto nodo = Parser::ShuntingYard(tokens);
    CHECK(nodo->token.tipo == TokenType::ADD);
    CHECK(nodo->hijos[0]->token.getValorNumerico() == std::numbers::pi);
    CHECK(nodo->hijos[1]->token.tipo == TokenType::SUB);
    CHECK(nodo->hijos[1]->hijos[0]->token.getValorNumerico() == std::numbers::e);
    CHECK(nodo->hijos[1]->hijos[1]->token.getValorNumerico() == 1);
}

TEST_CASE("Precedencia sin parentesis")
{
    auto tokens = Lexer::Tokenizar("3 + 4 * 2");
    auto nodo = Parser::ShuntingYard(tokens);
    CHECK(nodo->token.tipo == TokenType::ADD);
    CHECK(nodo->hijos[0]->token.getValorNumerico() == 3);
    CHECK(nodo->hijos[1]->token.tipo == TokenType::MULT);
    CHECK(nodo->hijos[1]->hijos[0]->token.getValorNumerico() == 4);
    CHECK(nodo->hijos[1]->hijos[1]->token.getValorNumerico() == 2);
}

TEST_CASE("Test funciones 2 argumentos")
{
    auto tokens = Lexer::Tokenizar("pow(2, 3)");
    auto nodo = Parser::ShuntingYard(tokens);
    REQUIRE(nodo->token.tipo == TokenType::POW);
    CHECK(nodo->hijos[0]->token.getValorNumerico() == 2);
    CHECK(nodo->hijos[1]->token.getValorNumerico() == 3);
}

TEST_CASE("Test funcion 1 argumento")
{
    auto token = Lexer::Tokenizar("sin(pi)");
    auto nodo = Parser::ShuntingYard(token);
    REQUIRE(nodo->token.tipo == TokenType::SIN);
    CHECK(nodo->hijos[0]->token.getValorNumerico() == std::numbers::pi);
    CHECK(nodo->hijos.size() == 1);
}

TEST_CASE("Funciones anidadas")
{
    auto token = Lexer::Tokenizar("sin(log(e, 4))");
    auto nodo = Parser::ShuntingYard(token);
    REQUIRE(nodo->token.tipo == TokenType::SIN);
    REQUIRE(nodo->hijos.size() == 1);
    CHECK(nodo->hijos[0]->token.tipo == TokenType::LOG);
    CHECK(nodo->hijos[0]->hijos[1]->token.getValorNumerico() == 4);
    CHECK(nodo->hijos[0]->hijos[0]->token.getValorNumerico() == std::numbers::e);
}