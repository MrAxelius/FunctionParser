#pragma once

namespace FunctionParser::inline v1
{

    struct Punto2D
    {
        // Si y no es un punto finito (NaN / +- inf) no se evalua
        double x = 0.0;
        double y = 0.0;
    };

}