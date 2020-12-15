#pragma once
#include<exception>
#include<iostream>
#include<string>
#include<vector>
#include"Storage.h"
#define VALUE_WORD -2
#define VALUE_NONE -1


struct token {
    unsigned type_index;
    int val_index;
};

std::ostream &operator<<(std::ostream &, const token &);
bool isEqual(const token &, const std::string &);

class Lexer {

    Storage *storage;

    token getResult(const std::string &, int);

    bool isDigital(char);
    bool isString(char);
    bool isDelimiter(char);
    bool isChar(char);

    bool isEmpty(char);
    bool isDoubleCharDelimiter(const std::string &);
    bool isAnotation(const std::string &);
    bool isAnotationEnd(const std::string &);
    bool isReserveWord(const std::string &);

    token identifier(const std::string &, size_t &);
    token delimiter(const std::string &, size_t &, unsigned);
    token integer(const std::string &, size_t &, unsigned);
    token string(const std::string &, size_t &, unsigned);

public:
    Lexer(Storage *storage) :storage(storage) {}
    token scan(const std::string &, size_t &, unsigned);
};
