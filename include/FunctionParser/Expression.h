#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>

// Componentes públicos de la librería
#include <FunctionParser/Sample.h>
#include <FunctionParser/Rango.h>

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
            [[nodiscard]] std::optional<double> eval(double x) const;
            [[nodiscard]] std::vector<Punto> evaluateMesh(const Rango &rango) const;

        private:
            struct Impl;
            std::unique_ptr<Impl> pImpl;
        };
        [[nodiscard]] std::vector<Punto> evaluate(const std::string &input, const Rango &rango);
    }
}