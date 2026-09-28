#pragma once

#include "Nodo.h"
#include "Evaluador.h"

#include <vector>
#include <memory>
#include <cmath>
#include <limits>

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
            if (!std::isfinite(rango.max) || !std::isfinite(rango.min))
            {
                throw fp::RangeError("Maximum and minimum range must be finite numbers");
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

    inline std::vector<fp::Punto> muestrear(const Nodo &nodo, const fp::Range &rango)
    {
        validar::validarRango(rango);
        double paso = (rango.max - rango.min) / static_cast<double>(rango.steps);
        std::vector<fp::Punto> resultado;
        resultado.reserve(rango.steps + 1);
        
        for (std::size_t i = 0; i <= rango.steps; ++i)
        {
            double x = rango.min + static_cast<double>(i) * paso;
            auto valor = Evaluador::evaluacionRecursiva(nodo, x);
            if (valor)
            {
                resultado.push_back(fp::Punto{x, *valor});
            }
            else
            {
                resultado.push_back(fp::Punto(x, std::numeric_limits<double>::quiet_NaN())); 
            }
        }
        return resultado;
    }
}