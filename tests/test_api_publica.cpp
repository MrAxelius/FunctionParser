#include <catch2/catch_test_macros.hpp>
#include <FunctionParser/FunctionParser.h>

TEST_CASE("Parentesis sin cerrar")
{
    try
    {
        auto expresion = FunctionParser::Expression("(1 + 2");
        FAIL("No ha habido ningun throw...");
    }
    catch (const FunctionParser::ExpressionError &e)
    {
        CHECK(e.position == 0);
    }
}
TEST_CASE("Identificador desconocido")
{
    REQUIRE_THROWS_AS(FunctionParser::Expression("y + 1"), FunctionParser::ExpressionError);
}
TEST_CASE("Vacio")
{
    REQUIRE_THROWS_AS(FunctionParser::Expression(""), FunctionParser::ExpressionError);
}
TEST_CASE("Camino feliz")
{
    auto expresion = FunctionParser::Expression("x*x");
    auto valor = expresion.eval(2.0);
    REQUIRE(valor.has_value());
    CHECK(valor == 4);
}
TEST_CASE("Camino feliz con rangos")
{
    auto expresion = FunctionParser::Expression("x*x");
    auto valores = expresion.evaluateFunction(FunctionParser::Range{0.0, 10.0, 5});
    CHECK(valores.size() == 6);
    CHECK(valores.at(0).y == 0);
    CHECK(valores.at(0).x == 0);
    CHECK(valores.at(5).y == 100);
    CHECK(valores.at(5).x == 10);
}
TEST_CASE("Error de rangos")
{
    auto expresion = FunctionParser::Expression("x*x");
    REQUIRE_THROWS_AS(expresion.evaluateFunction(FunctionParser::Range{0.0, 10.0, 0}), FunctionParser::RangeError);
}
TEST_CASE("Jerarquia funcional")
{
    auto expresion = FunctionParser::Expression("x*x");
    REQUIRE_THROWS_AS(FunctionParser::Expression("y + 1"), FunctionParser::LibraryException);
    REQUIRE_THROWS_AS(expresion.evaluateFunction(FunctionParser::Range{0.0, 10.0, 0}), FunctionParser::LibraryException);
}
