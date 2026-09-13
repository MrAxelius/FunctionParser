#include <catch2/catch_test_macros.hpp>
#include <numbers>
#include <memory>

#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "Evaluador.h"

TEST_CASE("Evaluacion con potencia")
{
    auto tokens = Lexer::Tokenizar("pow(2, 3)");
    auto nodo = Parser::ShuntingYard(tokens);
    auto valor = Evaluador::evaluacionRecursiva(*nodo, 0);
    REQUIRE(valor == 8);
}

TEST_CASE("Evaluacion logarítmica")
{
    auto tokens = Lexer::Tokenizar("log(8, 2)");
    auto nodo = Parser::ShuntingYard(tokens);
    auto valor = Evaluador::evaluacionRecursiva(*nodo, 0);
    REQUIRE(valor == Catch::Approx(3));
}
TEST_CASE("Evaluación aritmética con precedencia")
{
    auto tokens = Lexer::Tokenizar("2 + 3 * 4");
    auto nodo = Parser::ShuntingYard(tokens);
    auto valor = Evaluador::evaluacionRecursiva(*nodo, 0);
    REQUIRE(valor == 14);
}
TEST_CASE("Evaluación aritmética con variable")
{
    auto tokens = Lexer::Tokenizar("pow(x, 2)");
    auto nodo = Parser::ShuntingYard(tokens);
    auto valor = Evaluador::evaluacionRecursiva(*nodo, 5.0);
    REQUIRE(valor == 25);
}
TEST_CASE("Evaluación fallida en la division por cero")
{
    auto tokens = Lexer::Tokenizar("1 / 0");
    auto nodo = Parser::ShuntingYard(tokens);
    REQUIRE_THROWS_AS(Evaluador::evaluacionRecursiva(*nodo, 0), ErrorNoEsValorEsperado);
}
