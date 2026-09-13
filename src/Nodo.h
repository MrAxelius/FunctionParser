#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "Token.h"

struct Nodo
{

    Token token;
    std::vector<std::unique_ptr<Nodo>> hijos;
    Nodo(const Token &token, std::unique_ptr<Nodo> hijoI, std::unique_ptr<Nodo> hijoD) 
        : token(token) {
            if(hijoI != nullptr)
                hijos.push_back(std::move(hijoI));
            if(hijoD != nullptr)
                hijos.push_back(std::move(hijoD));
        }
    Nodo(const Token &token, std::vector<std::unique_ptr<Nodo>> hijos) 
        : token(token), hijos(std::move(hijos)) {}
    // Usar move semantics porque no se pueden copiar los unique_ptr
};
