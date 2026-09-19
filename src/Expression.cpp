#include <FunctionParser/Expression.h>
#include <memory>

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
            // pImpl not null assumed as precondition
            return Evaluador::evaluacionRecursiva(*pImpl->ast, x);
        }
        std::vector<Punto> Expression::evaluateMesh(const Rango &rango) const
        {
            return Muestreo::muestrear(*pImpl->ast, rango);
        }

    }
}