#include <FunctionParser/Expression.h>
#include <memory>
#include <iostream>

#include "Nodo.h"
#include "Parser/Parser.h"
#include "Lexer/Lexer.h"
#include "Evaluador.h"
#include "Muestreo.h"

namespace FunctionParser
{
    inline namespace v1
    {
        struct Expression::Impl
        {
            std::unique_ptr<Nodo> ast;
        };

        Expression::Expression(const std::string &input)
        {
            auto tokens = Lexer::Tokenizar(input);
            auto nodo = Parser::ShuntingYard(tokens);
            this->pImpl = std::make_unique<Impl>(std::move(nodo));
        }

        Expression::~Expression() = default;
        Expression::Expression(Expression &&) noexcept = default;
        Expression &Expression::operator=(Expression &&) noexcept = default;

        std::optional<double> Expression::eval(double x) const
        {
            auto resultado = Evaluador::evaluacionRecursiva(*pImpl->ast, x);
            // La propagación es interna, pero el resultado se filtra en la frontera
            if (!std::isfinite(resultado))
            {
                return std::nullopt;
            }
            return resultado;
        }
        std::vector<Point> Expression::evaluateFunction(const Range &rango) const
        {
            return Muestreo::muestrear(*pImpl->ast, rango);
        }
        Expression::operator bool() const noexcept
        {
            return this->pImpl != nullptr;
        }
    }
}