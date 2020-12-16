#pragma once
#include "Lexer.h"
#include "Storage.h"
#include "Util.h"

#define EMPTY -1
#define EMPTY_CHAIN 0

// 四元式 id = l op r
struct Quaternary {
    const std::string op;
    // 保存符号表中的下标
    int l, r, id;
    Quaternary(const std::string &op_, int l_, int r_, int id_)
        : op(op_), l(l_), r(r_), id(id_) {}
};

// 语法分析器
class Parser {
    // 符号表引用
    Storage &storage;
    // 中间代码
    Vector<Quaternary> intermediateCode;
    // 词法单元(Token)序列
    Vector<Token> tokens;
    // 行数标记
    Vector<size_t> rowMark;
    // 当前正在处理的Token下标
    size_t i = 0;
    // 获取Tokens[i]所在行数
    size_t getRow() const {
        for (size_t j = 0; i < rowMark.size(); ++j)
            if (i < rowMark[j]) return j;
        return rowMark.size();
    }
    // 生成四元式id = l op r
    size_t gen(const std::string &op, int l, int r, int id) {
        intermediateCode.emplace_back(op, l, r, id);
        return intermediateCode.size() - 1;
    }
    // 生成错误信息，抛出异常
    template <typename... Args>
    void error(Args... args) const {
        throw to_string("第", getRow(), "行: ", args...);
    }

    // expect开头的函数表示一定要匹配，若不匹配则报告错误并抛出异常
    // try开头的函数表示尝试匹配，通过返回值判断是否匹配成功
    // 多个参数要求全部匹配成功，匹配成功后均会**移动下标**

    // 匹配对应的token序列
    void expect(const String &s) {
        if (i < tokens.size() && tokens[i] == s)
            i++;
        else
            error("缺少 ", s);
    }
    template <typename Head, typename... Tail>
    void expect(Head head, Tail... tail) {
        expect(head), expect(tail...);
    }
    // 尝试匹配token
    bool tryExpect(const String &s) {
        if (i < tokens.size() && tokens[i] == s)
            return i++, true;
        else
            return false;
    }
    template <typename... Args>
    bool tryExpect(Args... args) {
        int oi = i;                    // mark origin i
        if ((... && tryExpect(args))) {  // unary left fold
            return true;
        } else
            return i = oi, false;  // restore i
    }
    // 尝试匹配非终结符，匹配失败忽略异常，恢复原始状态
    template <typename Func>
    bool tryProcedure(const Func &func) {
        int oi = i;
        auto ocode = intermediateCode.size();
        bool flag = true;
        try {
            func();
        } catch (...) {  // ignore exceptions
            flag = false;
            i = oi;  // restore state
            while (intermediateCode.size() > ocode) intermediateCode.pop_back();
        }
        return flag;
    }
    // 判断是否为末尾
    void expect() const {
        if (i != tokens.size() - 1) error("文件末尾异常。");
    }
    // 判断变量是否声明
    void expectDeclaredVar() const {
        auto var = storage[tokens[i].val_index];
        if (!var.declared) error("变量 ", var.name, " 未声明。");
    }
    // 判断变量是否为整型
    void expectIntegerVar() const {
        auto var = storage[tokens[i].val_index];
        if (var.type != Data::getCode("integer"))
            error("变量 ", var.name, " 应为整型。");
    }
    // 判断变量是否为整型
    void expectBooleanVar() const {
        auto var = storage[tokens[i].val_index];
        if (var.type != Data::getCode("boolean"))
            error("变量 ", var.name, " 应为布尔型。");
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
    // <type> → integer│bool│char
    // 成功返回type_id, 失败返回0
    unsigned type() {
        auto type_id = tokens[i].type_index;
        if (Data::type().contains(Data::getValue(type_id)))
            return ++i, type_id;
        else
            return 0;
    }
    // 递归下降的语法分析

    bool isVariableDefinitionEnd(size_t i) {
        // return i + 2 < tokens.size() && tokens[i] == ":" &&
        //    isType(tokens[i + 1].type_index) && tokens[i + 2] == ";";
        return true;
    }
    // <program> → program <identifier>;
    // <variable_declaration> <compound_statement>.
    void program() {
        expect("program", IDENTIFIER, ";");
        gen("program", tokens[i - 2].val_index, EMPTY, EMPTY);
        variableDeclaration();
        compoundStatement();
        expect(".");
        gen("sys", EMPTY, EMPTY, EMPTY);
        expect();
    }
    // <variable_declaration> → var <variable_define>│ε
    void variableDeclaration() {
        if (!tryExpect("var")) return;
        i++;
        return variableDefine();
    }
    // <variable_define> → <identifier_list>:<type>;
    // <variable_define>│<identifier_list>:<type>;
    void variableDefine() {
        auto idList = identifierList();
        if (idList.empty()) error("缺少变量定义！");
        variableDefineHelper(idList);
        while (true) {
            auto idList = identifierList();
            if (idList.empty()) break;
            variableDefineHelper(idList);
        }
    }
    void variableDefineHelper(const Vector<size_t> idList) {
        expect(":");
        auto t = type();
        if (t == 0)
            error("未定义变量类型！");
        else {
            for (auto i : idList) {
                storage[i].declared = true;
                storage[i].type = t;
            }
        }
    }
    // <identifier_list> → <identifier> , <identifier_list>│<identifier>
    Vector<size_t> identifierList() {
        Vector<size_t> idList;
        expect(IDENTIFIER);
        idList.push_back(tokens[i - 1].val_index);
        while (true) {
            if (!tryExpect(",")) break;
            expect(IDENTIFIER);
            idList.push_back(tokens[i - 1].val_index);
        }
        return idList;
    }
    // <statement> →
    // <assignment_statement>│<if_statement>│<while_statement>│<repeat_statement>│<compound_statement>
    void statement(unsigned &chain) {
        if (tryExpect(IDENTIFIER))
            assignmentStatement();
        else if (tryExpect("if"))
            ifStatement(chain);
        else if (tryExpect("while"))
            whileStatement(chain);
        else if (tryExpect("repeat"))
            repeatStatement(chain);
        else if (tryExpect("begin"))
            i--, compoundStatement();
        else
            error("非法语句！");
    }
    void assignmentStatement() {
        expect(":=");
        unsigned mark = i;
        int id = EMPTY;
        i++;  // tokens[i] == ":="
        arithmeticExpression(id);
        if (storage[tokens[mark - 1].val_index].type != storage[id].type)
            error("赋值语句左右类型不一致。");
        else
            gen(":=", id, EMPTY, tokens[mark - 1].val_index);
    }
    // <if_statement>→ if <boolean_expression> then <statement>
    // │ if <boolean_expression> then <statement> else <statement>
    void ifStatement(unsigned &chain) {
        unsigned tl, fl, then_chain = EMPTY_CHAIN, else_chain = EMPTY_CHAIN;
        booleanExpression(tl, fl);
        expect("then");
        backpatch(tl, intermediateCode.size());
        statement(then_chain);
        if (tryExpect("else")) {
            auto mark = intermediateCode.size();
            gen("j", EMPTY, EMPTY, EMPTY);
            backpatch(fl, intermediateCode.size());
            auto tmp_chain = merge(mark, then_chain);
            statement(else_chain);
            chain = merge(tmp_chain, else_chain);
        } else {
            chain = merge(fl, then_chain);
        }
    }
    // <while_statement> → while <boolean_expression> do <statement>
    void whileStatement(unsigned &chain) {
        unsigned tl, fl, tmp_chain = EMPTY_CHAIN,
                         mark = intermediateCode.size();
        booleanExpression(tl, fl);
        expect("do");
        backpatch(tl, intermediateCode.size());
        statement(tmp_chain);
        backpatch(tmp_chain, mark);
        gen("j", EMPTY, EMPTY, mark);
        chain = fl;
    }
    // <repeat_statement> → repeat <statement> until <boolean_expression>
    void repeatStatement(unsigned &chain) {
        unsigned tl, fl;
        unsigned mark = intermediateCode.size();
        statement(chain);
        expect("until");
        booleanExpression(tl, fl);
        chain = tl;
        backpatch(fl, mark);
    }
    // <compound_statement> → begin <statement_list> end
    void compoundStatement() {
        expect("begin");
        statementList();
        expect("end");
    }
    // <statement_list> → <statement> ;<statement_list>│<statement>
    void statementList() {
        unsigned chain = EMPTY_CHAIN;
        statementListHelper(chain);
        while (tryExpect(";")) statementListHelper(chain);
    }
    void statementListHelper(unsigned &chain) {
        statement(chain);
        if (chain != EMPTY_CHAIN) backpatch(chain, intermediateCode.size());
    }
    // <arithmetic_expression> → <arithmetic_expression> + <arithmetic_item>
    // │<arithmetic_expression> - <arithmetic_item>
    // │<arithmetic_item>
    void arithmeticExpression(int &id) {
        arithmeticItem(id);
        if (tryExpect("+") || tryExpect("-")) {
            unsigned mark = i - 1;
            int id_r = EMPTY;
            arithmeticExpression(id_r);
            int index = storage.get(TEMPORARY_VARIABLE);
            storage[index].type = storage[id].type;
            gen(Data::getValue(tokens[mark].type_index), id, id_r, index);
            id = index;
        }
    }
    // <arithmetic_item> → <arithmetic_item> * <arithmetic_factor>
    // │<arithmetic_item> / <arithmetic_factor>
    // │<arithmetic_factor>
    void arithmeticItem(int &id) {
        arithmeticFactor(id);
        if (tryExpect("*") || tryExpect("/")) {
            unsigned mark = i - 1;
            int id_r = EMPTY;
            arithmeticItem(id_r);
            int index = storage.get(TEMPORARY_VARIABLE);
            storage[index].type = storage[id].type;
            gen(Data::getValue(tokens[mark].type_index), id, id_r, index);
            id = index;
        }
    }
    // <arithmetic_factor> → <arithmetic_variable>
    // │- <arithmetic_factor>
    void arithmeticFactor(int &id) {
        if (tryExpect("-")) {
            unsigned mark = i - 1;
            arithmeticFactor(id);
            int index = storage.get(TEMPORARY_VARIABLE);
            storage[index].type = storage[id].type;
            gen("-", id, EMPTY, index);
            id = index;
        } else
            arithmeticVariable(id);
    }
    // <arithmetic_variable> → <integer>
    // │<identifier>
    // │( <arithmetic_expression> )
    void arithmeticVariable(int &ide) {
        if (i < tokens.size()) {
            if (tokens[i] == IDENTIFIER) {
                if (storage[tokens[i].val_index].type ==
                    Data::getCode("integer")) {
                    ide = tokens[i].val_index;
                    ++i;
                } else
                    error("变量 ", storage[tokens[i].val_index].name,
                          " 未声明。");
            } else if (tokens[i] == INTEGER) {
                storage[tokens[i].val_index].type = Data::getCode("integer");
                ide = tokens[i].val_index;
                ++i;
            } else if (tokens[i] == "(") {
                i++;
                arithmeticExpression(ide);
            }
        }
    }
    // <boolean_expression> → <boolean_expression> or
    // <boolean_item>│<boolean_item>
    void booleanExpression(unsigned &tl, unsigned &fl) {
        unsigned tl_1, fl_1, tl_2, fl_2;
        booleanItem(tl_1, fl_1);
        if (tryExpect("or")) {
            backpatch(fl_1, intermediateCode.size());
            booleanExpression(tl_2, fl_2);
            fl = fl_2;
            tl = merge(tl_1, tl_2);
        } else {
            tl = tl_1;
            fl = fl_1;
        }
    }
    // <boolean_item> → <boolean_item> and <boolean_factor>│<boolean_factor>
    void booleanItem(unsigned &tl, unsigned &fl) {
        unsigned tl_1, fl_1, tl_2, fl_2;
        booleanFactor(tl_1, fl_1);
        if (tryExpect("and")) {
            backpatch(tl_1, intermediateCode.size());
            booleanItem(tl_2, fl_2);
            tl = tl_2;
            fl = merge(fl_1, fl_2);
        } else {
            tl = tl_1;
            fl = fl_1;
        }
    }
    // <boolean_factor> → <boolean_variable>│not <boolean_factor>
    void booleanFactor(unsigned &tl, unsigned &fl) {
        if (tryExpect("not"))
            return booleanFactor(fl, tl);
        else
            return booleanVariable(tl, fl);
    }
    // <boolean_variable> → <boolean_constant>
    // │ <identifier>
    // │ ( <boolean_expression> )
    // │ <identifier> <relation_word> <identifier>
    // │ <arithmetic_expression> <relation_word> <arithmetic_expression>
    void booleanVariable(unsigned &tl, unsigned &fl) {
        // <boolean_constant>
        if (tryExpect(BOOLEANCONSTANT)) {
            tl = intermediateCode.size();
            gen("jnz", tokens[i - 1].type_index, EMPTY, EMPTY);
            fl = intermediateCode.size();
            gen("j", EMPTY, EMPTY, EMPTY);
            return;
        }
        // <identifier> <relation_word> <identifier>
        if (tryExpect(IDENTIFIER, RELATIONWORD, IDENTIFIER)) {
            i -= 3, expectIntegerVar();
            i += 2, expectIntegerVar();
            tl = intermediateCode.size();
            gen(std::string("j") + Data::getValue(tokens[i - 1].type_index),
                tokens[i - 2].val_index, tokens[i].val_index, EMPTY);
            fl = intermediateCode.size();
            gen("j", EMPTY, EMPTY, EMPTY);
            return;
        }
        // ( <boolean_expression> )
        auto boolean_helper = [this, &tl, &fl]() {
            expect("(");
            booleanExpression(tl, fl);
            expect(")");
        };
        if (tryProcedure(boolean_helper)) return;
        int id1, id2, mark;
        // <arithmetic_expression> <relation_word> <arithmetic_expression>
        auto arithmetic_helper = [this, &id1, &id2, &mark]() {
            arithmeticExpression(id1);
            mark = i;
            expect(RELATIONWORD);
            arithmeticExpression(id2);
        };
        if (tryProcedure(arithmetic_helper)) {
            tl = intermediateCode.size();
            gen("j" + Data::getValue(tokens[mark].type_index), id1, id2, EMPTY);
            fl = intermediateCode.size();
            return;
        }
        // <identifier>
        expectBooleanVar();
        tl = intermediateCode.size();
        gen("jnz", tokens[i].val_index, EMPTY, EMPTY);
        fl = intermediateCode.size();
        gen("j", EMPTY, EMPTY, EMPTY);
    }

public:
    Parser(Storage &storage) : storage(storage) {}
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
        program();
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
                if (tokens[q.l] == BOOLEANCONSTANT)
                    out << Data::getValue(q.l) << " , ";
                else
                    out << storage[q.l].name << ", ";
            } else
                out << storage[q.l].name << " , ";
            if (q.r == EMPTY)
                out << "- , ";
            else
                out << storage[q.r].name << " , ";
            if (q.op[0] == 'j')
                out << q.id;
            else if (q.id == EMPTY)
                out << "-";
            else
                out << storage[q.id].name;
            out << ")\n";
            ++i;
        }
    }
};
