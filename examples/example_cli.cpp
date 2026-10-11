#include <FunctionParser/FunctionParser.h>
#include <iostream>
#include <cmath>

int main()
{

    auto expresion = FunctionParser::Expression("1 / x");
    auto result = expresion.eval(3.5);
    if (result)
    {
        std::cout << *result << std::endl;
    }
    else
    {
        std::cout << "no value" << std::endl;
    }

    auto range = FunctionParser::Range{-2, 2, 4};
    auto points = expresion.evaluateFunction(range);
    for (size_t i = 0; i < points.size(); ++i)
    {
        std::cout << points[i].x << std::endl;
        if (!std::isfinite(points[i].y))
        {
            std::cout << "NaN" << std::endl;
        }
        else
        {
            std::cout << points[i].y << std::endl;
        }
    }
}