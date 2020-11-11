#pragma once

#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <cctype>
#include <cassert>

#include "token.h"

class Lexer {
public:
	using token_pair_t = std::pair<Token, int/*id*/>;
	explicit Lexer(std::string str) {
		// std::cerr << "content: " << str << '\n';
		n = str.length();
		s = str;
		parse();
	}
	void printToken() {
		for (size_t j = 0; j < pairs.size(); j++) {
			auto p = pairs[j];
			auto tokenint = static_cast<int>(p.first);
			std::cout << "(" << tokenint << " , ";
			if (p.second == 0) std::cout << "-)";
			else std::cout << p.second << " )";
			if ((j+1) % 5 == 0) std::cout << '\n';
			else std::cout << "\t\t";
		}
	}
	void next(size_t x) {
		i += x;
		col += x;
	}
	int getId(std::string x) {
		int res;
		if (idmap.find(x) == idmap.end()) {
			// new id
			total++;
			idmap[x] = total;
			res = total;
		} else {
			// exist id
			res = idmap[x];
		}
		return res;
	}
	void error(std::string msg) {
		std::cerr << "(" << row << ", " << col << ") ";
		std::cerr << msg << '\n';
		exit(1);
	}
	void expectLiteral() {
		assert(s[i] == '\'');
		auto j = i+1;
		while (j < n && s[j] != '\n' && s[j] != '\'') j++;
		if (i == n || s[i] == '\n') error("string literal is missing quotes.");
		assert(s[j] == '\'');
		auto content = s.substr(i, j-i+1);
		pairs.emplace_back(Token::LITERAL, getId(content));
		next(j-i);
	}
	void expectComment() {
		assert(s[i] == '/');
		assert(s[i+1] == '*');
		auto j = i+2;
		while (j < n && s[j] != '\n' && s[j-1] != '*' && s[j] != '/') j++;
		if (i == n || s[i] == '\n') error("comment is missing asterisk slash.");
		assert(s[j-1] == '*' && s[j] == '/');
		// ignore comments
		next(j-i);
	}
	void expectWord() {
		assert(isalpha(s[i]));
		auto j = i + 1;
		while (j < n && (isalnum(s[j]) || s[j]=='_')) j++;
		j--;
		auto word = s.substr(i, j-i+1);
		// std::cerr << "word: " << word << '\n';
		auto token = getToken(word);
		if (token == Token::IDENTIFIER) {
			pairs.emplace_back(Token::IDENTIFIER, getId(word));
		} else {
			pairs.emplace_back(token, 0);
		}
		next(j-i);
	}
	void expectNumber() {
		assert(isdigit(s[i]));
		auto j = i + 1;
		while (j < n && (isdigit(s[j]))) j++;
		j--;
		auto number = s.substr(i, j-i+1);
		auto token = Token::NUMBER;
		pairs.emplace_back(token, getId(number));
		next(j-i);
	}
	void expectSign() {
		auto j = i + 1;
		while (j < n && s[j] != '\t' && s[j] != ' ' && s[j] != '\n' && !isalnum(s[j])) j++;
		j--;
		auto sign = s.substr(i, j-i+1);
		// std::cerr << "sign: " << sign << '\n';
		auto token = getToken(sign);
		if (token == Token::IDENTIFIER) {
			error("Unknow operator!");
		} else {
			pairs.emplace_back(token, 0);
		}
		next(j-i);
	}
	void expectToken() {
		if (isalpha(s[i])) {
			expectWord();
		} else if (isdigit(s[i])) {
			expectNumber();
		} else {
			expectSign();
		}
	}
	void parse() {
		total = 0;
		row = 1;
		col = 0;
		for (i = 0; i < n; i++) {
			col++;
			char c = s[i];
			// std::cerr << "current charint: " << static_cast<int>(c) << '\n';
			if (c == ' ' || c == '\t') continue;
			if (c == '\n') {
				row++;
				col = 0;
				continue;
			}
			if (c == '\'') {
				expectLiteral();
				continue;
			}
			if (c == '/' && i+1<n && s[i+1] == '*') {
				expectComment();
				continue;
			}
			expectToken();
		}
	}

private:
	size_t n, i, row, col;
	std::string s;
	std::vector<token_pair_t> pairs;
	int total;
	std::map<std::string, int> idmap;
};
