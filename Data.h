#pragma once
#include "Util.h"

#define UNDEFINED "<undefined>"
#define IDENTIFIER "<identifier>"
#define INTEGER "<integer>"
#define STRING "<string>"
#define RESERVED_WORD "<reserved_word>"
#define SINGLE_DELIMITER "<single_delimiter>"
#define DOUBLE_DELIMITER "<double_delimiter>"
#define BOOLEAN_CONSTANT "<boolean_constant>"
#define RELATION_WORD "<relation_word>"

#define type_SET "integer", "bool", "char"
#define booleanConstant_SET "true", "false"
#define reservedWord_SET                                                      \
    "and", "array", "begin", "bool", "call", "case", "char", "constant",      \
        "dim", "do", "else", "end", "false", "for", "if", "input", "integer", \
        "not", "of", "or", "output", "procedure", "program", "read", "real",  \
        "repeat", "set", "stop", "then", "to", "true", "until", "var",        \
        "while", "write"
#define singleDelimiter_SET \
    '<', '>', ':', '/', '*', '.', '+', '-', '=', '(', ')', '[', ']', ';', ','
#define doubleDelimiter_SET "*/", "..", "/*", ":=", "<=", "<>", ">="
#define relationWord_SET "<", "<=", "<>", "=", ">", ">="
#define encodeTable_SET                                                    \
    UNDEFINED, reservedWord_SET, IDENTIFIER, INTEGER, STRING, "(", ")", "*", \
        "*/", "+", ",", "-", ".", "..", "/", "/*", ":", ":=", ";",         \
        relationWord_SET, "[", "]"

// 定义static函数访问字符集
#define SET_DEFINE(method)                 \
    static StringList& method() {          \
        static StringList t{method##_SET}; \
        return t;                          \
    }
class Data {
public:
    using CharList = const Vector<char>;
    using StringList = const Vector<std::string>;
    static CharList& singleDelimiter() {
        static CharList t{singleDelimiter_SET};
        return t;
    }
    SET_DEFINE(doubleDelimiter);
    SET_DEFINE(reservedWord);
    SET_DEFINE(relationWord);
    SET_DEFINE(booleanConstant);
    SET_DEFINE(encodeTable);
    SET_DEFINE(type);
    static unsigned getCode(const std::string& word) {
        auto pos = encodeTable().find(word);
        if (pos != encodeTable().cend())
            return pos - encodeTable().cbegin();
        else
            return 0;
    }
    static std::string getValue(unsigned code) {
        if (code >= encodeTable().size())
            return UNDEFINED;
        else
            return encodeTable()[code];
    }
};
