#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <numbers>
#include <memory>
#include <FunctionParser/Rango.h>

#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "Evaluador.h"
#include "Muestreo.h"

TEST_CASE("Prueba de tamanno")
{
    FunctionParser::Rango rango{0, 10, 5};
    auto tokens = Lexer::Tokenizar("log(8, x)");
    auto nodo = Parser::ShuntingYard(tokens);
    auto resultado = Muestreo::muestrear(*nodo, rango);
    CHECK(resultado.size() == 6);
}
TEST_CASE("Prueba NaN")
{
    FunctionParser::Rango rango{-2.0, 2.0, 4};
    auto tokens = Lexer::Tokenizar("1 / x");
    auto nodo = Parser::ShuntingYard(tokens);
    auto resultado = Muestreo::muestrear(*nodo, rango);
    REQUIRE(resultado.size() == 5);
    CHECK(std::isfinite(resultado.at(0).y));
    CHECK(resultado.at(1).y == -1);
    CHECK(!std::isfinite(resultado.at(2).y));
    CHECK(resultado.at(3).y == 1);
    CHECK(std::isfinite(resultado.at(4).y));
}
TEST_CASE("Extremos")
{
    FunctionParser::Rango rango{0, 10, 5};
    auto tokens = Lexer::Tokenizar("x + 1 ");
    auto nodo = Parser::ShuntingYard(tokens);
    auto resultado = Muestreo::muestrear(*nodo, rango);
    REQUIRE(resultado.size() == 6);
    CHECK(resultado.at(0).x == 0);
    CHECK(resultado.at(5).x == 10);
}
TEST_CASE("pasos = 0")
{
    FunctionParser::Rango rango{0, 5, 0};
    auto tokens = Lexer::Tokenizar("x + 1");
    auto nodo = Parser::ShuntingYard(tokens);
    REQUIRE_THROWS_AS(Muestreo::muestrear(*nodo, rango), FunctionParser::RangeError);
}
TEST_CASE("min > max")
{
    FunctionParser::Rango rango{5, 2, 6};
    auto tokens = Lexer::Tokenizar("x + 1");
    auto nodo = Parser::ShuntingYard(tokens);
    REQUIRE_THROWS_AS(Muestreo::muestrear(*nodo, rango), FunctionParser::RangeError);
}
TEST_CASE("min == max ")
{
    FunctionParser::Rango rango{5, 5, 2};
    auto tokens = Lexer::Tokenizar("x + 1");
    auto nodo = Parser::ShuntingYard(tokens);
    REQUIRE_THROWS_AS(Muestreo::muestrear(*nodo, rango), FunctionParser::RangeError);
}