#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>

#include "Lexer.h"

void welcome() {
    std::cout << 
    "Sample Complier\n"
    "Author: huangzhiquan\n"
    "Class: SCUT CS 2018\n"
    "Student ID: 201836580388\n\n";
}

std::string readFile(std::string &fileName) {
    std::ifstream inf;
    inf.open(fileName);
    if (!inf) {
        std::cerr << "Open file failed!\n";
        std::cerr << "File name: " << fileName << "\n";
        return std::string();
    }
    std::stringstream buf;
    buf << inf.rdbuf();
    std::string contents(buf.str());
    inf.close();
    return contents;
}

std::string getOutputFileName(std::string &fileName) {
	auto pos = fileName.find_last_of('.');
	if (pos == std::string::npos) { // no suffix
		return fileName + ".out";
	} else { // remove suffix append out
		return fileName.substr(0, pos) + ".out";
	}
}

bool handleFile(std::string &fileName, bool cmd = false) {
	auto contents = readFile(fileName);
	if (contents.empty()) return false;
	Lexer lexer(contents);
	auto outFileName = getOutputFileName(fileName);
	if (cmd) {
        lexer.outputToken(outFileName);
	    std::cout << "handle " << fileName << " ok, check " << outFileName << " for result.\n";
    } else {
        lexer.printToken();
    }
	return true;
}

int main(int argc, char *argv[]) {
    if (argc > 1) { // Command Line Mode
        // std::cerr << argc << '\n';
        for (int i = 1; i < argc; i++) {
            // std::cerr << argv[i] << '\n';
			std::string fileName(argv[i]);
			handleFile(fileName, true);
        }

        return 0;
    }
    welcome();
    while (true) { // Interactive Mode
        std::cout << "Please input test file name: ";
        std::string fileName = "test2.in";
        std::getline(std::cin, fileName);
        // std::cout << "File Content: \n";
        if (fileName.empty()) {
            std::cout << "Bye!\n";
            break;
        }
        if (!handleFile(fileName)) {
			std::cout << "Error!\n";
		}
    }
    return 0;
}
