#pragma once

#include "Nodo.h"
#include "Evaluador.h"

#include <vector>
#include <cmath>
#include <string>

#include <FunctionParser/Point.h>
#include <FunctionParser/Range.h>
#include <FunctionParser/Exceptions.h>

namespace Muestreo
{
    namespace fp = FunctionParser;
    // Validar aquí es provisional
    namespace validar
    {
        inline void validarRango(const fp::Range &rango)
        {
            constexpr std::size_t LIM_STEPS = 10000;
            if (!std::isfinite(rango.max) || !std::isfinite(rango.min))
            {
                throw fp::RangeError("Maximum and minimum range must be finite numbers");
            }
            if (rango.steps > LIM_STEPS)
            {
                throw fp::RangeError("The number of steps is too big, limit is " + std::to_string(LIM_STEPS));
            }
            if (rango.steps == 0)
            {
                throw fp::RangeError("Step cannot be 0");
            }
            if (rango.max < rango.min)
            {
                throw fp::RangeError("Maximum range is smaller than minimum range");
            }
            if (rango.max == rango.min)
            {
                throw fp::RangeError("Maximum and minimum range cannot be the same");
            }
        }
    }

    inline std::vector<fp::Point> muestrear(const Nodo &nodo, const fp::Range &rango)
    {
        validar::validarRango(rango);
        double paso = (rango.max - rango.min) / static_cast<double>(rango.steps);
        std::vector<fp::Point> resultado;
        resultado.reserve(rango.steps + 1);

        for (std::size_t i = 0; i <= rango.steps; ++i)
        {
            double x = rango.min + static_cast<double>(i) * paso;
            auto valor = Evaluador::evaluacionRecursiva(nodo, x);
            resultado.push_back(fp::Point{x, valor});
        }
        return resultado;
    }
}