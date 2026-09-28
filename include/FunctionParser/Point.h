#pragma once

#include <limits>

namespace FunctionParser
{
    inline namespace v1
    {
        struct Point
        {
            // Si y no es un punto finito (NaN / +- inf) no se evalua
            double x = 0.0;
            double y = 0.0;
        };

    }
}