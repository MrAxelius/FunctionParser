#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
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

TEST_CASE("Evaluacion logaritmica")
{
    auto tokens = Lexer::Tokenizar("log(8, 2)");
    auto nodo = Parser::ShuntingYard(tokens);
    auto valor = Evaluador::evaluacionRecursiva(*nodo, 0);
    REQUIRE(valor == Catch::Approx(3));
}
TEST_CASE("Evaluacion aritmetica con precedencia")
{
    auto tokens = Lexer::Tokenizar("2 + 3 * 4");
    auto nodo = Parser::ShuntingYard(tokens);
    auto valor = Evaluador::evaluacionRecursiva(*nodo, 0);
    REQUIRE(valor == 14);
}
TEST_CASE("Evaluacion aritmetica con variable")
{
    auto tokens = Lexer::Tokenizar("pow(x, 2)");
    auto nodo = Parser::ShuntingYard(tokens);
    auto valor = Evaluador::evaluacionRecursiva(*nodo, 5.0);
    REQUIRE(valor == 25);
}
TEST_CASE("Evaluacion fallida en la division por cero")
{
    auto tokens = Lexer::Tokenizar("1 / 0");
    auto nodo = Parser::ShuntingYard(tokens);
    auto valor = Evaluador::evaluacionRecursiva(*nodo, 5.0);
    CHECK(!valor);
}
TEST_CASE("Logaritmo con argumento negativo")
{
    auto tokens = Lexer::Tokenizar("log(0-4, 3)");
    auto nodo = Parser::ShuntingYard(tokens);
    auto valor = Evaluador::evaluacionRecursiva(*nodo, 5.0);
    CHECK(!valor);
}
TEST_CASE("Logaritmo con base 1")
{
    auto tokens = Lexer::Tokenizar("log(5, 1)");
    auto nodo = Parser::ShuntingYard(tokens);
    auto valor = Evaluador::evaluacionRecursiva(*nodo, 5.0);
    CHECK(!valor);
}
TEST_CASE("Raiz con radicando negativo")
{
    auto tokens = Lexer::Tokenizar("nrt(0-5, 2)");
    auto nodo = Parser::ShuntingYard(tokens);
    auto valor = Evaluador::evaluacionRecursiva(*nodo, 5.0);
    CHECK(!valor);
}
TEST_CASE("Propagacion de valores erroneos")
{
    auto tokens = Lexer::Tokenizar("1 + 1/0");
    auto nodo = Parser::ShuntingYard(tokens);
    auto valor = Evaluador::evaluacionRecursiva(*nodo, 5.0);
    CHECK(!valor);
}