#include <catch2/catch_test_macros.hpp>
#include <numbers>

#include "Lexer/Lexer.h"
#include "Parser/Parser.h"

TEST_CASE("el lexer produce tres tokens para 2+3")
{
    auto tokens = Lexer::Tokenizar("2+3");
    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0].tipo == TokenType::NUMERO);
    REQUIRE(tokens[1].tipo == TokenType::ADD);
    REQUIRE(tokens[2].tipo == TokenType::NUMERO);
}
TEST_CASE("Prueba de tokens pi y e")
{
    auto tokens = Lexer::Tokenizar("pi + e");
    REQUIRE(tokens.size() == 3);
    CHECK(tokens[0].tipo == TokenType::NUMERO);
    CHECK(tokens[0].getValorNumerico() == std::numbers::pi);
    CHECK(tokens[1].tipo == TokenType::ADD);
    CHECK(tokens[2].tipo == TokenType::NUMERO);
    CHECK(tokens[2].getValorNumerico() == std::numbers::e);
}
TEST_CASE("String vacio")
{
    REQUIRE_THROWS(Lexer::Tokenizar(""));
}