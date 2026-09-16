#pragma once
#include <cstddef>

namespace FunctionParser
{
    inline namespace v1
    {
        // 1 variable, una función
        // paso = intervalo, return pasos + 1 puntos
        struct Rango
        {
            double min = -1.0;
            double max = 1.0;
            std::size_t pasos = 100;
        };
    }
}