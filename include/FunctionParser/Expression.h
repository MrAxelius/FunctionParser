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

            // Validity
            // --------
            // A freshly constructed Expression is always valid: the constructor either
            // succeeds or throws. Moving from an Expression leaves the source invalid.
            //
            // eval() and evaluateFunction() require a valid object. Calling either one
            // on an invalid Expression is undefined behaviour.
            //
            // The destructor, move assignment and operator bool are safe on any
            // Expression, valid or not.
            [[nodiscard]] explicit operator bool() const noexcept;
            [[nodiscard]] std::optional<double> eval(double x) const;
            // Actual steps limit is 10k
            [[nodiscard]] std::vector<Point> evaluateFunction(const Range &range) const;

        private:
            struct Impl;
            std::unique_ptr<Impl> pImpl;
        };
    }
}