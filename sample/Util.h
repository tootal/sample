#pragma once
// 对默认C++的扩展以及一些通用函数

#include <algorithm>
#include <vector>

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