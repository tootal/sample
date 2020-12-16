#pragma once
#include "Lexer.h"
#include "Storage.h"
#include "Util.h"

#define EMPTY -1
#define EMPTY_CHAIN 0

// 四元式 id = l op r
struct Quaternary {
    const std::string op;
    int l, r, id;
    Quaternary(const std::string &op_, int l_, int r_, int id_)
        : op(op_), l(l_), r(r_), id(id_) {}
};

class Parser {
    // 符号表指针
    Storage *storage;
    // 中间代码
    Vector<Quaternary> intermediateCode;
    // 词法单元(Token)序列
    Vector<Token> tokens;
    // 行数标记
    Vector<size_t> rowMark;
    // 获取Tokens[i]所在行数
    size_t getRow(size_t i) {
        for (size_t j = 0; i < rowMark.size(); ++j)
            if (i < rowMark[j]) return j;
        return rowMark.size();
    }
    // 生成四元式id = l op r
    size_t gen(const std::string &op, int l, int r, int id) {
        intermediateCode.emplace_back(op, l, r, id);
        return intermediateCode.size() - 1;
    }
    // 回填
    void backpatch(unsigned p, unsigned t) {
        for (int i = p; i != EMPTY;) {
            int tmp = intermediateCode[i].id;
            intermediateCode[i].id = t;
            i = tmp;
        }
    }
    unsigned merge(unsigned p1, unsigned p2) {
        if (p2 == EMPTY_CHAIN)
            return p1;
        else {
            int i;
            for (i = p2; intermediateCode[i].id != EMPTY;)
                i = intermediateCode[i].id;
            intermediateCode[i].id = p1;
            return p2;
        }
    }

    bool isType(size_t code) {
        return Data::type().contains(Data::getValue(code));
    }
    bool isBoolType(size_t code) { return code == Data::getCode("bool"); }
    bool isIntType(size_t code) { return code == Data::getCode("integer"); }
    bool isRelation(size_t code) {
        return Data::relationWord().contains(Data::getValue(code));
    }
    bool isBoolean(size_t code) {
        return Data::booleanConstant().contains(Data::getValue(code));
    }

    // 递归下降的语法分析

    bool isProgram(size_t i) {
        return i + 2 < tokens.size() && tokens[i] == "program" &&
               tokens[i + 1] == IDENTIFIER && tokens[i + 2] == ";";
    }
    bool isVariableDeclaration(size_t i) {
        return i < tokens.size() && tokens[i] == "var";
    }
    bool isVariableDefinitionEnd(size_t i) {
        return i + 2 < tokens.size() && tokens[i] == ":" &&
               isType(tokens[i + 1].type_index) && tokens[i + 2] == ";";
    }
    bool isCompoundStatement(size_t i) {
        return i < tokens.size() && tokens[i] == "begin";
    }
    bool isStatement(size_t i) {
        return i < tokens.size() &&
               (tokens[i] == IDENTIFIER || tokens[i] == "if" ||
                tokens[i] == "while" || tokens[i] == "repeat");
    }

    bool program(size_t &i) {
        gen("program", tokens[i + 1].val_index, EMPTY, EMPTY);
        i += 3;
        if (i < tokens.size()) {
            if (isVariableDeclaration(i)) {
                if (!variableDefinition(++i)) return false;
            } else
                return false;
            if (isCompoundStatement(i)) {
                if (!compoundStatement(i)) return false;
            } else
                return false;
        } else
            return false;
        if (i == tokens.size() - 1 && tokens[i] == ".") {
            gen("sys", EMPTY, EMPTY, EMPTY);
            return true;
        } else
            return false;
    }
    bool variableDefinition(size_t &i) {
        bool mark = false;
        unsigned type = 0;

        while (i < tokens.size() && tokens[i] == IDENTIFIER) {
            mark = true;
            if (!identifierTable(i, type)) return false;
        }

        return mark;
    }
    bool identifierTable(size_t &i, unsigned &type) {
        storage->setDeclare(tokens[i].val_index);
        size_t mark = i;
        if (i + 1 < tokens.size() && tokens[i + 1] == ",") {
            i += 2;
            if (i < tokens.size() && tokens[i] == IDENTIFIER) {
                if (identifierTable(i, type)) {
                    storage->setType(tokens[mark].val_index, type);
                    return true;
                } else
                    return false;
            } else
                return false;
        } else if (i + 3 < tokens.size() && tokens[i + 1] == ":" &&
                   isType(tokens[i + 2].type_index) && tokens[i + 3] == ";") {
            type = tokens[i + 2].type_index;
            storage->setType(tokens[mark].val_index, type);
            i += 4;
            return true;
        } else
            return false;
    }

    bool statement(size_t &i, unsigned &chain) {
        if (i < tokens.size()) {
            if (tokens[i] == IDENTIFIER)
                return assignmentStatement(i);
            else if (tokens[i] == "if")
                return ifStatement(i, chain);
            else if (tokens[i] == "while")
                return whileStatement(i, chain);
            else if (tokens[i] == "repeat")
                return repeatStatement(i, chain);
        }

        return false;
    }
    bool assignmentStatement(size_t &i) {
        i += 1;
        if (i < tokens.size() && tokens[i] == ":=") {
            unsigned mark = i;
            int ide = EMPTY;
            i += 1;
            if (arithmeticExpression(i, ide)) {
                if (!storage->isSameType(tokens[mark - 1].val_index, ide)) {
                    throw "第" + std::to_string(getRow(mark - 1)) +
                        "行: 变量 `" +
                        storage->getName(tokens[mark - 1].val_index) +
                        "` 类型错误。";
                } else {
                    gen(":=", ide, EMPTY, tokens[mark - 1].val_index);
                    return true;
                }
            } else {
                unsigned tl, fl;
                if (booleanExpression(i, tl, fl)) {
                }
            }
        }
        return false;
    }
    bool ifStatement(size_t &i, unsigned &chain) {
        i += 1;
        unsigned tl, fl, then_chain = EMPTY_CHAIN, else_chain = EMPTY_CHAIN;

        if (booleanExpression(i, tl, fl))

            if (i < tokens.size() && tokens[i] == "then") {
                backpatch(tl, intermediateCode.size());

                if (statement(++i, then_chain)) {
                    if (i < tokens.size() && tokens[i] == "else") {
                        unsigned mark = intermediateCode.size();
                        gen("j", EMPTY, EMPTY, EMPTY);
                        backpatch(fl, intermediateCode.size());
                        unsigned tmp_chain = merge(mark, then_chain);

                        if (statement(++i, else_chain)) {
                            chain = merge(tmp_chain, else_chain);
                            return true;
                        }
                    } else {
                        chain = merge(fl, then_chain);
                        return true;
                    }
                }
            }

        return false;
    }
    bool whileStatement(size_t &i, unsigned &chain) {
        i += 1;
        unsigned tl, fl, tmp_chain = EMPTY_CHAIN,
                         mark = intermediateCode.size();

        if (booleanExpression(i, tl, fl)) {
            if (i < tokens.size() && tokens[i] == "do") {
                backpatch(tl, intermediateCode.size());

                if (statement(++i, tmp_chain)) {
                    backpatch(tmp_chain, mark);
                    gen("j", EMPTY, EMPTY, mark);
                    chain = fl;
                    return true;
                } else
                    return false;
            } else
                return false;
        } else
            return false;
    }
    bool repeatStatement(size_t &i, unsigned &chain) {
        i += 1;
        unsigned tl, fl;
        unsigned mark = intermediateCode.size();

        if (statement(i, chain)) {
            if (i < tokens.size() && tokens[i] == "until") {
                if (booleanExpression(++i, tl, fl)) {
                    chain = tl;
                    backpatch(fl, mark);
                    return true;
                } else
                    return false;
            } else
                return false;
        } else
            return false;
    }
    bool compoundStatement(size_t &i) {
        i += 1;
        if (statementList(i)) {
            if (i < tokens.size() && tokens[i] == "end") {
                ++i;
                return true;
            } else
                return false;
        } else
            return false;
    }
    bool statementList(size_t &i) {
        i -= 1;
        unsigned chain = EMPTY_CHAIN;
        do {
            ++i;
            if (!statement(i, chain))
                return false;
            else {
                if (chain != EMPTY_CHAIN)
                    backpatch(chain, intermediateCode.size());
            }
        } while (i < tokens.size() && tokens[i] == ";");

        return true;
    }

    bool arithmeticExpression(size_t &i, int &ide) {
        if (item(i, ide)) {
            if (i < tokens.size() && (tokens[i] == "+" || tokens[i] == "-")) {
                unsigned mark = i;
                int ide_r = EMPTY;
                if (arithmeticExpression(++i, ide_r)) {
                    int index = storage->addIdentifer(TEMPORARY_VARIABLE);
                    storage->setType(index, storage->getType(ide));
                    gen(Data::getValue(tokens[mark].type_index), ide, ide_r,
                        index);
                    ide = index;
                    return true;
                } else
                    return false;
            } else
                return true;
        } else
            return false;
    }
    bool item(size_t &i, int &ide) {
        if (divisor(i, ide)) {
            if (i < tokens.size() && (tokens[i] == "*" || tokens[i] == "/")) {
                unsigned mark = i;
                int ide_r = EMPTY;

                if (item(++i, ide_r)) {
                    int index = storage->addIdentifer(TEMPORARY_VARIABLE);
                    storage->setType(index, storage->getType(ide));
                    gen(Data::getValue(tokens[mark].type_index), ide, ide_r,
                        index);
                    ide = index;
                    return true;
                } else
                    return false;
            } else
                return true;

        } else
            return false;
    }
    bool divisor(size_t &i, int &ide) {
        if (i < tokens.size() && tokens[i] == "-") {
            unsigned mark = i;

            if (divisor(++i, ide)) {
                int index = storage->addIdentifer(TEMPORARY_VARIABLE);
                storage->setType(index, storage->getType(ide));
                gen(Data::getValue(tokens[mark].type_index), ide, EMPTY, index);
                ide = index;
                return true;
            } else
                return false;
        } else
            return arithmeticUnit(i, ide);
    }
    bool arithmeticUnit(size_t &i, int &ide) {
        if (i < tokens.size()) {
            if (tokens[i] == IDENTIFIER) {
                if (storage->isDeclare(tokens[i].val_index) &&
                    isIntType(storage->getType(tokens[i].val_index))) {
                    ide = tokens[i].val_index;
                    ++i;
                    return true;
                } else
                    throw "第" + std::to_string(getRow(i)) + "行: 变量 `" +
                        storage->getName(tokens[i].val_index) + "` 未声明。";
            } else if (tokens[i] == INTEGER) {
                storage->setType(tokens[i].val_index, Data::getCode("integer"));
                ide = tokens[i].val_index;
                ++i;
                return true;
            } else if (tokens[i] == "(") {
                if (arithmeticExpression(++i, ide)) {
                    if (tokens[i] == "") {
                        ++i;
                        return true;
                    }
                }
            }
        }

        return false;
    }
    bool booleanExpression(size_t &i, unsigned &tl, unsigned &fl) {
        unsigned tl_1, fl_1, tl_2, fl_2;
        if (booleanItem(i, tl_1, fl_1)) {
            if (i < tokens.size() && tokens[i] == "or") {
                backpatch(fl_1, intermediateCode.size());
                booleanExpression(++i, tl_2, fl_2);
                fl = fl_2;
                tl = merge(tl_1, tl_2);
                return true;
            } else {
                tl = tl_1;
                fl = fl_1;
                return true;
            }
        } else
            return false;
    }
    bool booleanItem(size_t &i, unsigned &tl, unsigned &fl) {
        unsigned tl_1, fl_1, tl_2, fl_2;
        if (booleanDivisor(i, tl_1, fl_1)) {
            if (i < tokens.size() && tokens[i] == "and") {
                backpatch(tl_1, intermediateCode.size());
                booleanItem(++i, tl_2, fl_2);
                tl = tl_2;
                fl = merge(fl_1, fl_2);
                return true;
            } else {
                tl = tl_1;
                fl = fl_1;
                return true;
            }
        } else
            return false;
    }
    bool booleanDivisor(size_t &i, unsigned &tl, unsigned &fl) {
        if (i < tokens.size() && tokens[i] == "not")
            return booleanDivisor(++i, fl, tl);
        else
            return booleanUnit(i, tl, fl);
    }
    bool booleanUnit(size_t &i, unsigned &tl, unsigned &fl) {
        // <identifier> <relation_word> <identifier>
        if (i + 2 < tokens.size() && tokens[i] == IDENTIFIER &&
            isRelation(tokens[i + 1].type_index) &&
            tokens[i + 2] == IDENTIFIER) {
            if (isIntType(storage->getType(tokens[i].val_index)) &&
                storage->isSameType(tokens[i].val_index,
                                    tokens[i + 2].val_index)) {
                tl = intermediateCode.size();
                gen(std::string("j") + Data::getValue(tokens[i + 1].type_index),
                    tokens[i].val_index, tokens[i + 2].val_index, EMPTY);

                fl = intermediateCode.size();
                gen("j", EMPTY, EMPTY, EMPTY);

                i += 3;
                return true;
            } else
                throw "第" + std::to_string(getRow(i)) + "行: 变量 `" +
                    storage->getName(tokens[i].val_index) + "` 类型错误。";
        } else if (i < tokens.size()) {
            unsigned tmp = intermediateCode.size();

            int ide1, ide2;
            // <arithmetic_expression> <relation_word> <arithmetic_expression>
            if (arithmeticExpression(i, ide1)) {
                unsigned mark = i;
                if (i < tokens.size() && isRelation(tokens[i].type_index))
                    if (arithmeticExpression(++i, ide2)) {
                        tl = intermediateCode.size();
                        gen("j" + Data::getValue(tokens[mark].type_index), ide1,
                            ide2, EMPTY);
                        fl = intermediateCode.size();
                        gen("j", EMPTY, EMPTY, EMPTY);
                        return true;
                    }
            }

            while (tmp < intermediateCode.size()) intermediateCode.pop_back();
            // <identifier>
            if (tokens[i] == IDENTIFIER) {
                if (!isBoolType(storage->getType(tokens[i].val_index)) &&
                    !isIntType(storage->getType(tokens[i].val_index)))
                    throw "第" + std::to_string(getRow(i)) + "行: 变量 `" +
                        storage->getName(tokens[i].val_index) + "` 类型错误。";

                else {
                    tl = intermediateCode.size();
                    gen("jnz", tokens[i].val_index, EMPTY, EMPTY);
                    fl = intermediateCode.size();
                    gen("j", EMPTY, EMPTY, EMPTY);
                    ++i;
                    return true;
                }
            }
            // ( <boolean_expression> )
            else if (tokens[i] == "(") {
                unsigned tmp = i + 1;
                unsigned mark = intermediateCode.size();
                if (booleanExpression(++i, tl, fl))
                    if (tokens[i] == "") {
                        ++i;
                        return true;
                    } else
                        return false;
                else {
                    i = tmp;
                    while (intermediateCode.size() > mark)
                        intermediateCode.pop_back();
                }
            }
            // <boolean_constant>
            else if (isBoolean(tokens[i].type_index)) {
                tl = intermediateCode.size();
                gen("jnz", tokens[i].type_index, EMPTY, EMPTY);
                fl = intermediateCode.size();
                gen("j", EMPTY, EMPTY, EMPTY);
                ++i;
                return true;
            }
        }

        return false;
    }

public:
    Parser(Storage *storage) : storage(storage) {}
    void parse(std::ifstream &is) {
        std::string str;
        Lexer scanner(storage);
        unsigned row = 0;
        while (std::getline(is, str)) {
            size_t i = 0;
            while (i < str.length()) {
                Token token = scanner.scan(str, i, row);
                if (token.val_index != VALUE_NONE) tokens.push_back(token);
            }
            rowMark.push_back(tokens.size());
            ++row;
        }

        size_t i = 0;
        if (isProgram(i) && program(i))
            return;
        else {
            throw "第" + std::to_string(getRow(i)) + "行: 语法错误。";
        }
    }
    void printIntermediateCode(std::ostream &out) {
        unsigned i = 0;
        for (auto q : intermediateCode) {
            out << "(";
            out.width(2);
            out << i << ")"
                << "\t(" << q.op << " , ";
            if (q.l == EMPTY)
                out << "-, ";
            else if (q.op == "jnz") {
                if (isBoolean(q.l))
                    out << Data::getValue(q.l) << " , ";
                else
                    out << storage->getName(q.l) << ", ";
            } else
                out << storage->getName(q.l) << " , ";
            if (q.r == EMPTY)
                out << "- , ";
            else
                out << storage->getName(q.r) << " , ";
            if (q.op[0] == 'j')
                out << q.id;
            else if (q.id == EMPTY)
                out << "-";
            else
                out << storage->getName(q.id);
            out << ")\n";

            ++i;
        }
    }
};
