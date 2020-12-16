#pragma once
#include <string>
#include <vector>

#define TEMPORARY_VARIABLE "<T>"

// 标识符
struct Identifer {
    // 标识符名
    std::string name = "";
    // 标识符类型
    unsigned type = 0;
    // 是否声明
    bool declared = false;
    Identifer(const std::string &name_, unsigned type_, bool declared_)
        : name(name_), type(type_), declared(declared_) {}
    // 临时标识符(从1开始编号)
    static Identifer tempIdentifer() {
        static int count = 0;
        count++;
        return {"T" + std::to_string(count), 0, true};
    }
};

// 符号表
class Storage {
    Vector<Identifer> identifers;

public:
    // 通过下标访问符号表
    auto &operator[](size_t i) { return identifers[i]; }
    // 通过符号名访问符号表，不存在则自动添加
    auto &operator[](const std::string &name) { return identifers[get(name)]; }
    // 通过符号名访问符号表，不存在则自动添加，返回下标
    size_t get(const std::string &name) {
        if (name == TEMPORARY_VARIABLE) {
            identifers.push_back(Identifer::tempIdentifer());
            return identifers.size() - 1;
        }
        size_t i;
        for (i = 0; i < identifers.size(); ++i)
            if (name == identifers[i].name) break;
        if (i == identifers.size()) {
            identifers.emplace_back(name, 0, false);
            return identifers.size() - 1;
        }
        return i;
    }
    void clear() { identifers.clear(); }
};
