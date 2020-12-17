#pragma once
// 对默认C++的扩展以及一些通用函数

#include <algorithm>
#include <vector>
#include <utility>
#include <tuple>

#include <cassert>

// 调试输出控制宏
#ifdef DEBUG
#define debug(...) std::cerr << to_string(__VA_ARGS__) << '\n'
#else
#define debug(...) 42
#endif

// 字符串输出宏（需要定义to_string）
#define STRING_OUT(Class)                                                \
    friend std::ostream &operator<<(std::ostream &out, const Class &x) { \
        return (out << to_string(x));                                    \
    }

template <typename T>
class Vector : public std::vector<T> {
public:
    using std::vector<T>::vector;
    using std::vector<T>::cbegin;
    using std::vector<T>::cend;
    // 在容器中查找x，调用std::find
    auto find(const T &x) const { return std::find(cbegin(), cend(), x); }
    // 检查容器中是否包含x
    auto contains(const T &x) const { return find(x) != cend(); }
};

// 字符串，主要用于调试输出
class String : public std::string {
public:
    String() : std::string() {}
    String(const char *s) : std::string(s) {}
    String(std::string s) : std::string(s) {}
};
String to_string(const char *x) { return String(x); }
String to_string(const std::string &x) { return String(x); }
String to_string(const String &x) { return x; }
template <typename T>
String to_string(T v) {
    if constexpr (std::is_integral<T>::value)
        return std::to_string(v);
    else {  // container
        bool first = true;
        String res = "{";
        for (const auto &x : v) {
            if (!first) res += ", ";
            first = false;
            res += to_string(x);
        }
        res += "}";
        return res;
    }
}
template <typename Head, typename... Tail>
String to_string(Head H, Tail... T) {
    return to_string(H) + to_string(T...);
}

using Pair = std::pair<int, int>;