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

    auto rango = FunctionParser::Rango{-2, 2, 4};
    auto resultado = expresion.evaluateMesh(rango);
    for (size_t i = 0; i < resultado.size(); ++i)
    {
        std::cout << resultado[i].x << std::endl;
        if (std::isnan(resultado[i].y))
        {
            std::cout << "NaN" << std::endl;
        }
        else
        {
            std::cout << resultado[i].y << std::endl;
        }
    }
}