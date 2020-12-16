#include <fstream>
#include <iostream>
#include <string>

#include "Parser.h"

void welcome() {
    std::cout << "编译原理实验 SAMPLE语言的语法、语义分析器\n"
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
        Parser parser(storage);
        std::cout << "\n语法分析、语义分析结果如下：\n";
        try {
            parser.parse(in);
        } catch (const std::string &msg) {
            std::cerr << msg << '\n';
            continue;
        }
        parser.printIntermediateCode(std::cout);
        std::cout << std::endl;
        in.close();
    }
    return 0;
}
