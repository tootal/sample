#pragma once
#include <string>
#include <vector>
#define TEMPORARY_VARIABLE "<T>"

// 标识符
struct ide {
    std::string name = "";
    unsigned type = 0;
    bool is_declare = false;
    ide(const std::string &name_, unsigned type_, bool is_declare_) 
    : name(name_), type(type_), is_declare(is_declare_) {}
};

// 符号表
class Storage {
    std::vector<ide> Identifers;
    // 临时变量开始编号
    unsigned temp_count = 1;

public:
    // 添加标识符，返回id
    unsigned addIdentifer(const std::string &val) {
        if (val == TEMPORARY_VARIABLE) {
            Identifers.emplace_back("T" + std::to_string(temp_count), 0, true);
            ++temp_count;
            return Identifers.size() - 1;
        }

        unsigned i;
        for (i = 0; i < Identifers.size(); ++i)
            if (val == Identifers[i].name) break;

        if (i == Identifers.size()) Identifers.push_back(ide{val, 0, false});

        return i;
    }
    void setDeclare(unsigned i) {
        if (i < Identifers.size()) Identifers[i].is_declare = true;
    }
    void setType(unsigned i, unsigned type) {
        if (i < Identifers.size()) Identifers[i].type = type;
    }

    bool isDeclare(unsigned i) {
        if (i < Identifers.size())
            return Identifers[i].is_declare;
        else
            return false;
    }
    bool isSameType(unsigned i, unsigned j) {
        if (i < Identifers.size() && j < Identifers.size()) {
            if (i == j || Identifers[i].type == Identifers[j].type)
                return true;
            else
                return false;
        } else
            return false;
    }

    unsigned getType(unsigned i) {
        if (i < Identifers.size())
            return Identifers[i].type;
        else
            return 0;
    }
    std::string getName(unsigned i) {
        if (i < Identifers.size())
            return Identifers[i].name;
        else
            return std::string();
    }

    void clear() { Identifers.clear(); }
};
