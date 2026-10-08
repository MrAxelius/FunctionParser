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
TEST_CASE("Entradas sin tokens")
{
    REQUIRE_THROWS_AS(FunctionParser::Expression(""), FunctionParser::ExpressionError);
    REQUIRE_THROWS_AS(FunctionParser::Expression(" "), FunctionParser::ExpressionError);
    REQUIRE_THROWS_AS(FunctionParser::Expression("\t"), FunctionParser::ExpressionError);
    REQUIRE_THROWS_AS(FunctionParser::Expression("\n"), FunctionParser::ExpressionError);
}
TEST_CASE("Limites de pasos")
{
    auto expresion = FunctionParser::Expression("x*x");
    auto funcion = expresion.evaluateFunction({-1, 1, 10000});
    REQUIRE(funcion.size() == 10001);

    REQUIRE_THROWS_AS(expresion.evaluateFunction({-1, 1, 10001}), FunctionParser::RangeError);
}
TEST_CASE("Construccion y construccion por movimiento")
{
    auto expresion = FunctionParser::Expression("sin(pi)");
    CHECK(expresion);
    auto expresion2 = std::move(expresion);
    CHECK_FALSE(expresion);
    CHECK(expresion2);
}
TEST_CASE("Asignacion por movimiento en expresion preconstruida ")
{
    auto expresion = FunctionParser::Expression("sin(pi)");
    CHECK(expresion);
    auto expresion2 = FunctionParser::Expression("1 + 3");
    expresion2 = std::move(expresion);
    CHECK_FALSE(expresion);
    CHECK(expresion2);
}
TEST_CASE("Colapso del desbordamiento en la frontera")
{
    auto expresion = FunctionParser::Expression("pow(10, 400)");
    auto resultado = expresion.eval(5.0);
    CHECK_FALSE(resultado.has_value());
}
TEST_CASE("Rai negativa")
{
    auto expresion = FunctionParser::Expression("nrt(-8, 2)");
    auto resultado = expresion.eval(5.0);
    CHECK_FALSE(resultado.has_value());
}
TEST_CASE("Rai con indice 0")
{
    auto expresion = FunctionParser::Expression("nrt(0.5, 0)");
    auto resultado = expresion.eval(5.0);
    CHECK_FALSE(resultado.has_value());
}
TEST_CASE("logaritmo con 0")
{
    auto expresion = FunctionParser::Expression("log(5,0)");
    auto resultado = expresion.eval(5.0);
    CHECK_FALSE(resultado.has_value());
}
TEST_CASE("Propagacion")
{
    auto expresion = FunctionParser::Expression("1 + 2 *  sin(cos(tan(nrt(-8,1))))");
    auto resultado = expresion.eval(5.0);
    CHECK_FALSE(resultado.has_value());
    auto expresion2 = FunctionParser::Expression("1 + 2 *  sin(cos(tan(nrt(8,1))))");
    auto resultado2 = expresion2.eval(5.0);
    CHECK(resultado2.has_value());
}