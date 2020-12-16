#pragma once
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "Data.h"
#include "Storage.h"

#define VALUE_WORD -2
#define VALUE_NONE -1

// 词法单元
struct Token {
    unsigned type_index;
    // 保存符号表中的下标
    int val_index;
    // 比较运算符
    bool operator==(const String &str) const {
        // 符号集合判断
        if (str == RELATIONWORD) {
            return Data::relationWord().contains(Data::getValue(type_index));
        } else if (str == BOOLEANCONSTANT) {
            return Data::booleanConstant().contains(Data::getValue(type_index));
        } else {
            return type_index == Data::getCode(str);
        }
    }
    bool operator!=(const String &str) const {
        return !(type_index == Data::getCode(str));
    }
    // 格式化输出
    friend std::ostream &operator<<(std::ostream &out, const Token &w) {
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
    // 符号表引用
    Storage &storage;

    Token getResult(const std::string &str, int index = VALUE_NONE) {
        if (str == UNDEFINED || str == "/*") {
            return {Data::getCode(str), VALUE_NONE};
        } else if (str == INTEGER || str == STRING || str == IDENTIFIER) {
            return {Data::getCode(str), index};
        } else {
            return {Data::getCode(str), VALUE_WORD};
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

    Token identifier(const std::string &str, size_t &i) {
        size_t j = i + 1;
        for (; j < str.length(); ++j) {
            if (!isdigit(str[j]) && !isalpha(str[j])) break;
        }

        std::string ide = str.substr(i, j - i);
        i += ide.length();
        if (isReserveWord(ide))
            return getResult(ide);
        else {
            int index = storage.get(ide);
            return getResult(IDENTIFIER, index);
        }
    }
    Token delimiter(const std::string &str, size_t &i, unsigned r) {
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
    Token integer(const std::string &str, size_t &i, unsigned r) {
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
        int index = storage.get(value);
        i = j;
        return getResult(INTEGER, index);
    }
    Token string(const std::string &str, size_t &i, unsigned r) {
        size_t j = i + 1;
        for (; j < str.length(); ++j) {
            if (isString(str[j])) break;
        }

        if (j == str.length())
            throw "第" + std::to_string(r) + "行第" + std::to_string(i) +
                "列: 字符串引号缺失。";

        else {
            std::string value = str.substr(i, j - i + 1);
            int index = storage.get(value);
            i = j + 1;
            return getResult(STRING, index);
        }
    }

public:
    Lexer(Storage &storage) : storage(storage) {}
    Token scan(const std::string &str, size_t &i, unsigned r) {
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
