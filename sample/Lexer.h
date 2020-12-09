#pragma once
#include<exception>
#include<iostream>
#include<string>
#include<vector>
#include"Storage.h"
#define VALUE_WORD -2
#define VALUE_NONE -1


struct token {
	unsigned type_index;
	int val_index;
};

std::ostream &operator<<(std::ostream &, const token &);
bool isEqual(const token &, const std::string &);


class anotation_error :public std::runtime_error {
public:
	const unsigned pos;
	const unsigned row;

	anotation_error(unsigned pos, unsigned row):
		std::runtime_error("注释界符缺失"),
		pos(pos), row(row) {}

	void print(std::ostream &out) {
		out << "第" << row << "行, 第" 
			<< pos << "列: " << what() << "\n";
	}
};


class string_error :public std::runtime_error {
public:
	const unsigned pos;
	const unsigned row;

	string_error(unsigned pos, unsigned row):
		std::runtime_error("字符串引号缺失"),
		pos(pos), row(row) {}

	void print(std::ostream &out) {
		out << "第" << row << "行, 第"
			<< pos << "列: " << what() << "\n";
	}
};


class character_error :public std::runtime_error {
public:
	const unsigned pos;
	const unsigned row;
	const char c;

	character_error(unsigned pos, unsigned row, 
		char c) :std::runtime_error("非法字符"), 
		pos(pos), row(row), c(c) {}

	void print(std::ostream &out) {
		out << "第" << row << "行, 第"
			<< pos << "列: " << what() 
			<< " '" << c << "'\n";
	}
};


class word_error :public std::runtime_error {
public:
	const unsigned pos;
	const unsigned row;
	const char c;

	word_error(unsigned pos, unsigned row, 
		char c) :std::runtime_error("不应出现的字符"),
		pos(pos), row(row), c(c) {}

	void print(std::ostream &out) {
		out << "第" << row << "行, 第"
			<< pos << "列: " << what()
			<< " '" << c << "'\n";
	}
};

class Lexer {

	Storage *storage;

	token getResult(const std::string &, int);

	bool isDigital(char);
	bool isString(char);
	bool isDelimiter(char);
	bool isChar(char);

	bool isEmpty(char);
	bool isDoubleCharDelimiter(const std::string &);
	bool isAnotation(const std::string &);
	bool isAnotationEnd(const std::string &);
	bool isReserveWord(const std::string &);

	token identifier(const std::string &, size_t &);
	token delimiter(const std::string &, size_t &, unsigned);
	token integer(const std::string &, size_t &, unsigned);
	token string(const std::string &, size_t &, unsigned);

public:
	Lexer(Storage *storage) :storage(storage) {}
	token scan(const std::string &, size_t &, unsigned);
};
