#pragma once

#include <iostream>
#include <string>

#define dbg(...) std::cerr << "\033[32;1m[" << #__VA_ARGS__ << "]:\033[39;0m", debug(__VA_ARGS__)

std::string to_string(const std::string& s) { return '"' + s + '"'; }

void debug() { std::cerr << std::endl; }

template <typename Head, typename... Tail>
void debug(Head H, Tail... T) {
    std::cerr << " " << to_string(H);
    debug(T...);
}