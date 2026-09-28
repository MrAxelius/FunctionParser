#pragma once
#include <cstddef>

namespace FunctionParser
{
    inline namespace v1
    {
        // 1 variable, una función
        // a step is an interval
        // return steps + 1
        struct Range
        {
            double min = -1.0;
            double max = 1.0;
            std::size_t steps = 100;
        };
    }
}