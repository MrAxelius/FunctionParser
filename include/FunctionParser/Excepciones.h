#pragma once

#include <stdexcept>
#include <string>
#include <cstddef>

namespace FunctionParser
{
    inline namespace v1
    {
        // Envuelve todo, así solo necesita 1 catch
        struct LibraryException : std::runtime_error
        {
            explicit LibraryException(const std::string &message)
                : std::runtime_error(message) {}
        };
        struct RangeError : LibraryException
        {
            explicit RangeError(const std::string &message)
                : LibraryException(message) {}
        };
        struct ExpressionError : LibraryException
        {
            size_t position;
            explicit ExpressionError(const std::string &message, size_t position)
                : LibraryException(message), position(position) {}
        };
    }
}
