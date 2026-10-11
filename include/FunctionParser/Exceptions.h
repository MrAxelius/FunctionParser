#pragma once

#include <stdexcept>
#include <string>
#include <cstddef>

namespace FunctionParser
{
    inline namespace v1
    {
        // Wrapper, only 1 catch needed
        struct LibraryException : std::runtime_error
        {
            explicit LibraryException(const std::string &message)
                : std::runtime_error(message)  {}
        };
        struct RangeError : LibraryException
        {
            explicit RangeError(const std::string &message)
                : LibraryException(message) {}
        };
        struct ExpressionError : LibraryException
        {
            std::size_t position;
            explicit ExpressionError(const std::string &message, std::size_t position)
                : LibraryException(message), position(position) {}
        };
    }
}
