#pragma once
#include "Util.h"

#define IDENTIFIER "<identifier>"
#define INTEGER "<integer>"
#define STRING "<string>"
#define UNDEFINED "<empty>"
#define MAX_RESERVE_WORD_LEN 9
#define MIN_RESERVE_WORD_LEN 2
#define FIRST_RESERVE_WORD 1
#define LAST_RESERVE_WORD 35


class Data {
public:
    using CharList = const Vector<char>;
    using StringList = const Vector<std::string>;
    static CharList& singleDelimiter() {
        static CharList single_delimiter{'<', '>', ':', '/', '*', '.', '+', '-',
                                         '=', '(', ')', '[', ']', ';', ','};
        return single_delimiter;
    }
    static StringList& doubleDelimiter() {
        static StringList double_delimiter{
            "*/", "..", "/*", ":=", "<=", "<>", ">="};
        return double_delimiter;
    }
    static StringList& relationWord() {
        static StringList relation_word{"<", "<>", "<=", ">=", ">", "="};
        return relation_word;
    }
    static StringList& booleanConstant() {
        static StringList boolean_constant{"true", "false"};
        return boolean_constant;
    }
    static StringList& codingSchedule() {
        static StringList coding_schedule{
            UNDEFINED,  "and",   "array",    "begin",  "bool",      "call",
            "case",     "char",  "constant", "dim",    "do",        "else",
            "end",      "false", "for",      "if",     "input",     "integer",
            "not",      "of",    "or",       "output", "procedure", "program",
            "read",     "real",  "repeat",   "set",    "stop",      "then",
            "to",       "true",  "until",    "var",    "while",     "write",
            IDENTIFIER, INTEGER, STRING,     "(",      ")",         "*",
            "*/",       "+",     ",",        "-",      ".",         "..",
            "/",        "/*",    ":",        ":=",     ";",         "<",
            "<=",       "<>",    "=",        ">",      ">=",        "[",
            "]"};
        return coding_schedule;
    }
    static StringList& type() {
        static StringList types{"integer", "bool", "char"};
        return types;
    }
    static unsigned getCode(const std::string& word) {
        auto pos = codingSchedule().find(word);
        if (pos != codingSchedule().cend())
            return pos - codingSchedule().cbegin();
        else
            return 0;
    }
    static std::string getValue(unsigned code) {
        if (code >= codingSchedule().size())
            return UNDEFINED;
        else
            return codingSchedule()[code];
    }
};
