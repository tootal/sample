#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>

#include "lexer.h"

void welcome() {
	std::cout << "Sample Complier\n";
	std::cout << "Author: huangzhiquan\n";
	std::cout << "Class: SCUT CS 2018\n";
	std::cout << "Student ID: 201836580388\n\n";
}

std::string readFile(std::string &fileName) {
	std::ifstream inf;
	inf.open(fileName);
	if (!inf) {
		std::cerr << "Open file failed!\n";
		std::cerr << "File name: " << fileName << "\n";
		exit(1);
	}
	std::stringstream buf;
	buf << inf.rdbuf();
	std::string contents(buf.str());
	inf.close();
	return contents;
}

int main() {
	welcome();

	std::cout << "Please input test file name: ";
	std::string fileName = "test2.in";
	// std::getline(cin, fileName);
	std::cout << "File Content: \n";
	auto contents = readFile(fileName);
	Lexer lexer(contents);
	lexer.printToken();
	return 0;
}
