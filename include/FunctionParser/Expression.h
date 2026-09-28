#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>

// Componentes públicos de la librería
#include <FunctionParser/Point.h>
#include <FunctionParser/Range.h>
#include <FunctionParser/Exceptions.h>

namespace FunctionParser
{
    inline namespace v1
    {
        class Expression
        {
        public:
            explicit Expression(const std::string &input);
            ~Expression();

            // Deshabilitar copia (el Impl contiene un unique_ptr no copiable)
            Expression(const Expression &) = delete;
            Expression &operator=(const Expression &) = delete;

            // Habilitar movimiento (eficiente)
            Expression(Expression &&) noexcept;
            Expression &operator=(Expression &&) noexcept;
            
            // pImpl not null assumed as precondition
            [[nodiscard]] std::optional<double> eval(double x) const;
            [[nodiscard]] std::vector<Punto> evaluateFunction(const Range &rango) const;

        private:
            struct Impl;
            std::unique_ptr<Impl> pImpl;
        };
    }
}