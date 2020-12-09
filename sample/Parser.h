#pragma once

#include "Lexer.h"

class Parser {
public:
    Parser(Lexer &lexer_) : lexer(lexer_) {

    }
private:
    Lexer &lexer;
};