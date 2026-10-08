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

            Expression(const Expression &) = delete;
            Expression &operator=(const Expression &) = delete;

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
            [[nodiscard]] std::vector<Point> evaluateFunction(const Range &range) const;
            // Error reporting
            // ---------------
            // eval() returns an empty optional when the result is not a finite number.
            // That covers both domain errors (log of a negative number, even root of a
            // negative radicand) and overflow. The engine computes with plain IEEE 754
            // semantics and the check happens once, here, instead of at every node.
            //
            // evaluateFunction() does NOT do this. It returns the raw value for every
            // sampled point, including NaN and +/-infinity, so a caller plotting the
            // function can see where the curve breaks and which way an asymptote runs.
            // Do not assume the two functions behave the same way.
        private:
            struct Impl;
            std::unique_ptr<Impl> pImpl;
        };
    }
}