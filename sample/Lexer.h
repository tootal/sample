#pragma once
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "Data.h"
#include "Storage.h"

#define VALUE_WORD -2
#define VALUE_NONE -1

struct token {
    unsigned type_index;
    int val_index;
    friend std::ostream &operator<<(std::ostream &out, const token &w) {
        out << "(";
        out.width(2);
        out << w.type_index;
        out << " , ";
        if (w.val_index == VALUE_WORD)
            out << "-";
        else
            out << w.val_index + 1;
        out << " )";
        return out;
    }
};

// 词法分析器
class Lexer {
    // 符号表指针
    Storage *storage;

    token getResult(const std::string &str, int index = VALUE_NONE) {
        if (str == UNDEFINED || str == "/*") {
            return token{Data::getCode(str), VALUE_NONE};
        } else if (str == INTEGER || str == STRING || str == IDENTIFIER) {
            return token{Data::getCode(str), index};
        } else {
            return token{Data::getCode(str), VALUE_WORD};
        }
    }

    bool isString(char c) { return c == '\''; }
    bool isSingleDelimiter(char c) {
        for (auto x : Data::singleDelimiter())
            if (c == x) return true;
        return false;
    }

    bool isDoubleCharDelimiter(const std::string &str) {
        for (auto &s : Data::doubleDelimiter())
            if (str == s) return true;
        return false;
    }
    bool isAnotationBegin(const std::string &str) { return str == "/*"; }
    bool isAnotationEnd(const std::string &str) { return str == "*/"; }
    bool isReserveWord(const std::string &str) {
        if (str.length() > MAX_RESERVE_WORD_LEN ||
            str.length() < MIN_RESERVE_WORD_LEN)
            return false;

        if (Data::getCode(str) >= FIRST_RESERVE_WORD &&
            Data::getCode(str) <= LAST_RESERVE_WORD)
            return true;

        return false;
    }

    token identifier(const std::string &str, size_t &i) {
        size_t j = i + 1;
        for (; j < str.length(); ++j) {
            if (!isdigit(str[j]) && !isalpha(str[j])) break;
        }

        std::string ide = str.substr(i, j - i);
        i += ide.length();
        if (isReserveWord(ide))
            return getResult(ide);
        else {
            int index = storage->addIdentifer(ide);
            return getResult(IDENTIFIER, index);
        }
    }
    token delimiter(const std::string &str, size_t &i, unsigned r) {
        if (str.length() > i + 1) {
            std::string substr = str.substr(i, 2);
            if (isDoubleCharDelimiter(substr)) {
                if (isAnotationBegin(substr)) {
                    size_t j = i + 2;
                    for (; j < str.length() - 1; ++j)
                        if (str.substr(j, 2) == "*/") {
                            i = j + 2;
                            return getResult("/*");
                        }
                    throw "第" + std::to_string(r) + "行第" +
                        std::to_string(i) + "列: 注释界符缺失。";
                } else if (isAnotationEnd(substr))
                    throw "第" + std::to_string(r) + "行第" +
                        std::to_string(i) + "列: 不应出现字符" +
                        std::string(1, str[i]);
                else {
                    i += 2;
                    return getResult(substr);
                }
            }
        }
        return getResult(str.substr(i++, 1));
    }
    token integer(const std::string &str, size_t &i, unsigned r) {
        size_t j = i + 1;
        for (; j < str.length(); ++j)
            if (!isdigit(str[j])) {
                if (isSingleDelimiter(str[j]) || isspace(str[j]))
                    break;
                else
                    throw "第" + std::to_string(r) + "行第" +
                        std::to_string(j) + "列: 不应出现字符" +
                        std::string(1, str[j]);
            }

        std::string value = str.substr(i, j - i);
        int index = storage->addIdentifer(value);
        i = j;
        return getResult(INTEGER, index);
    }
    token string(const std::string &str, size_t &i, unsigned r) {
        size_t j = i + 1;
        for (; j < str.length(); ++j) {
            if (isString(str[j])) break;
        }

        if (j == str.length())
            throw "第" + std::to_string(r) + "行第" + std::to_string(i) +
                "列: 字符串引号缺失。";

        else {
            std::string value = str.substr(i, j - i + 1);
            int index = storage->addIdentifer(value);
            i = j + 1;
            return getResult(STRING, index);
        }
    }

public:
    Lexer(Storage *storage) : storage(storage) {}
    token scan(const std::string &str, size_t &i, unsigned r) {
        auto len = str.length();
        for (; i < len; ++i) {
            if (isspace(str[i]))
                continue;
            else if (isString(str[i]))
                return string(str, i, r);
            else if (isdigit(str[i]))
                return integer(str, i, r);
            else if (isalpha(str[i]))
                return identifier(str, i);
            else if (isSingleDelimiter(str[i]))
                return delimiter(str, i, r);
            else
                throw "第" + std::to_string(r) + "行第" + std::to_string(i) +
                    "列: 出现非法字符" + std::string(1, str[i]);
        }

        return getResult(UNDEFINED);
    }
};
