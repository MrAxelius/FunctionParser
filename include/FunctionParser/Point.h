#pragma once

#include <limits>

namespace FunctionParser
{
    inline namespace v1
    {
        struct Point
        {
            // y may be +-inf or NaN (if overflow or undefined), check before use with isfinite
            double x = 0.0;
            double y = 0.0;
        };

    }
}