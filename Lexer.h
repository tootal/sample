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
    unsigned type_id;
    // 保存符号表中的下标
    int name_id;
    // 比较运算符
    bool operator==(const String &str) const {
        // 符号集合判断
        if (str == RELATION_WORD) {
            return Data::relationWord().contains(Data::getValue(type_id));
        } else if (str == BOOLEAN_CONSTANT) {
            return Data::booleanConstant().contains(Data::getValue(type_id));
        } else {
            return type_id == Data::getCode(str);
        }
    }
    bool operator!=(const String &str) const {
        return !(type_id == Data::getCode(str));
    }
    // 格式化输出
    friend String to_string(const Token &w) {
        String val =
            w.name_id == VALUE_WORD ? "-" : to_string(w.name_id);
        return to_string("(", w.type_id, ", ", val, " )");
    }
    STRING_OUT(Token);
};

// 词法分析器
class Lexer {
    // 符号表引用
    Storage &storage;
    
    // 获取字符串的编码，返回Token
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
        return Data::singleDelimiter().contains(c);
    }
    bool isDoubleCharDelimiter(const String &str) {
        return Data::doubleDelimiter().contains(str);
    }
    bool isAnotationBegin(const std::string &str) { return str == "/*"; }
    bool isAnotationEnd(const std::string &str) { return str == "*/"; }
    bool isReserveWord(const std::string &str) {
        return Data::reservedWord().contains(str);
    }

    Token identifier(const std::string &str, size_t &i) {
        size_t j = i + 1;
        for (; j < str.length(); ++j) {
            if (!isdigit(str[j]) && !isalpha(str[j])) break;
        }

        String ide = str.substr(i, j - i);
        i += ide.length();
        if (isReserveWord(ide))
            return getResult(ide);
        else {
            int index = storage.get(ide);
            return getResult(IDENTIFIER, index);
        }
    }
    Token delimiter(const String &str, size_t &i, unsigned r) {
        if (str.length() > i + 1) {
            String substr = str.substr(i, 2);
            if (isDoubleCharDelimiter(substr)) {
                if (isAnotationBegin(substr)) {
                    size_t j = i + 2;
                    for (; j < str.length() - 1; ++j)
                        if (str.substr(j, 2) == "*/") {
                            i = j + 2;
                            return getResult("/*");
                        }
                    throw to_string("第", r, "行第", i, "列: 注释界符缺失。");
                } else if (isAnotationEnd(substr))
                    throw to_string("第", r, "行第", i, "列: 不应出现字符 ", str[i]);
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
                    throw to_string("第", r, "行第", j, "列: 不应出现字符 ", str[j]);
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
            throw to_string("第", r, "行第", i, "列: 字符串引号缺失。");
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
                throw to_string("第", r, "行第", i, "列: 不应出现字符 ", str[i]);
        }
        return getResult(UNDEFINED);
    }
};
