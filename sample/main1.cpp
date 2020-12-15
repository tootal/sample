#include <fstream>
#include <iostream>
#include <string>

#include "Lexer.h"
#include "Storage.h"

void welcome() {
    std::cout << "编译原理实验 SAMPLE语言的词法分析器\n"
                 "作者: 黄智权\n"
                 "班级: 计算机科学与技术1班\n"
                 "学号: 201836580388\n";
}

int main(int argc, char *argv[]) {
    welcome();
    while (true) {
        std::cout << "请输入测试文件名(直接回车退出程序): ";
        std::string fileName;
        std::getline(std::cin, fileName);
		if (fileName.empty()) break;
        std::ifstream in;
        in.open(fileName);
        if (!in) {
            std::cout << "无法打开文件: " << fileName << '\n';
            continue;
        }
        Storage storage;
        Lexer scanner(&storage);
        std::cout << "词法分析结果: \n";
        unsigned count = 0;
        unsigned row = 0;
        std::string str;
        while (std::getline(in, str)) {
            for (size_t i = 0; i < str.length();) {
                try {
                    token result = scanner.scan(str, i, row);
                    if (result.val_index != VALUE_NONE) {
                        ++count;
                        std::cout << result;
                        if (count % 5 == 0) std::cout << '\n';
						else std::cout << "\t";
                    }
                } catch (std::string &msg) {
                    std::cerr << msg << '\n';
                    goto out;
                }
            }
            ++row;
        }
		std::cout << std::endl;
        in.close();
        out: 0;
    }
    return 0;
}
