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
    // 格式化输出
    friend String to_string(const Quaternary &q) {
        return to_string("(", q.op, ", ", q.l, ", ", q.r, ", ", q.id, ")");
    }
    STRING_OUT(Quaternary);
};

// 语法分析器
class Parser {
    // 符号表引用
    Storage &storage;
    // 中间代码
    Vector<Quaternary> inCode;
    // 词法单元(Token)序列
    Vector<Token> tokens;
    // 行数标记
    Vector<size_t> rowMark;
    // 当前正在处理的Token下标
    size_t i = 0;
    // 生成四元式id = l op r
    size_t gen(const std::string &op, int l, int r, int id) {
        inCode.emplace_back(op, l, r, id);
        debug("gen", qustr(inCode.back()));
        return inCode.size() - 1;
    }
    // 生成错误信息，抛出异常
    template <typename... Args>
    void error(Args... args) const {
        int r = std::upper_bound(rowMark.begin(), rowMark.end(), i) -
                rowMark.begin();
        throw to_string("第", r + 1, "行: ", args...);
    }

    // expect开头的函数表示一定要匹配，若不匹配则报告错误并抛出异常
    // try开头的函数表示尝试匹配，通过返回值判断是否匹配成功
    // 多个参数要求全部匹配成功，匹配成功后均会**移动下标**

    // 匹配对应的token序列
    void expect(const char *s) {
        debug("expect(", s, ")");
        if (i < tokens.size() && tokens[i] == s)
            i++;
        else
            i--, error("缺少 ", s);
    }
    template <typename Head, typename... Tail>
    void expect(Head head, Tail... tail) {
        expect(head), expect(tail...);
    }
    // 尝试匹配token
    bool tryExpect(const char *s) {
        bool flag;
        if (i < tokens.size() && tokens[i] == s)
            i++, flag = true;
        else
            flag = false;
        debug("tryExpect(", s, ")=", flag);
        return flag;
    }
    template <typename... Args>
    bool tryExpect(Args... args) {
        int oi = i;                      // mark origin i
        if ((... && tryExpect(args))) {  // unary left fold
            return true;
        } else
            return i = oi, false;  // restore i
    }
    // 尝试匹配非终结符，匹配失败忽略异常，恢复原始状态
    template <typename Func>
    bool tryProcedure(const Func &func) {
        int oi = i;
        auto ocode = inCode.size();
        bool flag = true;
        try {
            func();
        } catch (...) {  // ignore exceptions
            flag = false;
            i = oi;  // restore state
            while (inCode.size() > ocode) inCode.pop_back();
        }
        debug("tryProcedure()=", flag);
        return flag;
    }
    // 判断是否为文件末尾
    void expectEOF() const {
        debug("expectEOF()");
        if (i != tokens.size()) error("文件末尾异常。");
    }
    // 判断变量是否声明
    void expectDeclaredVar() {
        debug("expectDeclaredVar()");
        auto var = storage[tokens[i].name_id];
        if (!var.declared) error("变量 ", var.name, " 未声明。");
        i++;
    }
    // check if var is spec type
    void expectVar(const char *t) {
        debug("expectVar(", t, ")");
        expectDeclaredVar();
        i--;
        auto var = storage[tokens[i].name_id];
        if (var.type != Data::getCode(t)) error("变量 ", var.name, " 应为", t);
        i++;
    }
    // 回填
    void backpatch(unsigned p, unsigned t) {
        debug("backpatch(", p, ", ", t, ")");
        while (p != EMPTY) {
            int tmp = inCode[p].id;
            inCode[p].id = t;
            debug("backpatch(", p, ") = ", qustr(inCode[p]));
            p = tmp;
        }
    }
    int merge(int p1, int p2) {
        debug("merge(", p1, ", ", p2, ")");
        int ret;
        if (p2 == EMPTY_CHAIN)
            ret = p1;
        else {
            for (int i = p2; inCode[i].id != EMPTY;) {
                i = inCode[i].id;
                inCode[i].id = p1;
                debug(qustr(inCode[i]));
            }
            ret = p2;
        }
        debug("merge(", p1, ", ", p2, ") -> ", ret);
        return ret;
    }
    // <type> → integer│bool│char
    // 成功返回type_id, 失败error
    unsigned type() {
        auto type_id = tokens[i].type_id;
        auto type_name = Data::getValue(type_id);
        if (!Data::type().contains(type_name)) error("缺少类型");
        std::cerr << "type()=" << type_name << "\n";
        return ++i, type_id;
    }

    // <program> → program <identifier>;
    // <variable_declaration> <compound_statement>.
    void program() {
        debug("program()");
        expect("program", IDENTIFIER, ";");
        gen("program", tokens[i - 2].name_id, EMPTY, EMPTY);
        variableDeclaration();
        compoundStatement();
        expect(".");
        gen("sys", EMPTY, EMPTY, EMPTY);
        expectEOF();
    }
    // <variable_declaration> → var <variable_define>│ε
    void variableDeclaration() {
        if (!tryExpect("var")) return;
        return variableDefine();
    }
    // <variable_define> → <identifier_list>:<type>;
    // <variable_define>│<identifier_list>:<type>;
    void variableDefine() {
        debug("variableDefine()");
        auto idList = identifierList();
        if (idList.empty()) error("缺少变量定义！");
        variableDefineHelper(idList);
        while (tryExpect(IDENTIFIER)) {
            i--;
            auto idList = identifierList();
            if (idList.empty()) break;
            variableDefineHelper(idList);
        }
    }
    void variableDefineHelper(const Vector<size_t> idList) {
        expect(":");
        auto t = type();
        debug("define ", Data::getValue(t), " ", idList);
        for (auto i : idList) {
            storage[i].declared = true;
            storage[i].type = t;
        }
        expect(";");
    }
    // <identifier_list> → <identifier> , <identifier_list>│<identifier>
    Vector<size_t> identifierList() {
        debug("identifierList()");
        Vector<size_t> idList;
        expect(IDENTIFIER);
        idList.push_back(tokens[i - 1].name_id);
        while (true) {
            if (!tryExpect(",")) break;
            expect(IDENTIFIER);
            idList.push_back(tokens[i - 1].name_id);
        }
        return idList;
    }
    // <statement> →
    // <assignment_statement>│<if_statement>│<while_statement>│<repeat_statement>│<compound_statement>
    int statement() {
        debug("statement()");
        if (tryExpect(IDENTIFIER))
            return i--, assignmentStatement(), EMPTY_CHAIN;
        else if (tryExpect("if"))
            return i--, ifStatement();
        else if (tryExpect("while"))
            return i--, whileStatement();
        else if (tryExpect("repeat"))
            return i--, repeatStatement();
        else if (tryExpect("begin"))
            return i--, compoundStatement();
        else
            return error("非法语句！"), EMPTY_CHAIN;
    }
    void assignmentStatement() {
        debug("assignmentStatement()");
        expect(IDENTIFIER, ":=");
        (i -= 2), expectDeclaredVar(), (i++);
        unsigned mark = i - 1;  // tokens[mark] == ":="
        int id = arithmeticExpression();
        if (storage[tokens[mark - 1].name_id].type != storage[id].type)
            error("赋值语句左右类型不一致。");
        else
            gen(":=", id, EMPTY, tokens[mark - 1].name_id);
    }
    // <if_statement>→ if <boolean_expression> then <statement>
    // │ if <boolean_expression> then <statement> else <statement>
    int ifStatement() {
        debug("ifStatement()");
        expect("if");
        auto [tl, fl] = booleanExpression();
        debug("booleanExpression() -> (", tl, ", ", fl, ")");
        expect("then");
        backpatch(tl, inCode.size());
        auto then_chain = statement();
        debug("then_chain -> ", then_chain);
        if (tryExpect("else")) {
            auto mark = inCode.size();
            gen("j", EMPTY, EMPTY, EMPTY);
            backpatch(fl, inCode.size());
            auto tmp_chain = merge(mark, then_chain);
            auto else_chain = statement();
            debug("else_chain -> ", else_chain);
            return merge(tmp_chain, else_chain);
        } else {
            return merge(fl, then_chain);
        }
    }
    // <while_statement> → while <boolean_expression> do <statement>
    int whileStatement() {
        debug("whileStatement()");
        expect("while");
        auto mark = inCode.size();
        auto [tl, fl] = booleanExpression();
        expect("do");
        backpatch(tl, inCode.size());
        auto tmp_chain = statement();
        backpatch(tmp_chain, mark);
        gen("j", EMPTY, EMPTY, mark);
        return fl;
    }
    // <repeat_statement> → repeat <statement> until <boolean_expression>
    int repeatStatement() {
        expect("repeat");
        unsigned mark = inCode.size();
        auto chain = statement();
        expect("until");
        auto [tl, fl] = booleanExpression();
        chain = tl;
        backpatch(fl, mark);
        return chain;
    }
    // <compound_statement> → begin <statement_list> end
    int compoundStatement() {
        debug("compoundStatement()");
        expect("begin");
        auto chain = statementList();
        expect("end");
        return chain;
    }
    // <statement_list> → <statement> ;<statement_list>│<statement>
    int statementList() {
        debug("statementList()");
        auto chain = statementListHelper();
        while (tryExpect(";")) chain = statementListHelper();
        return chain;
    }
    int statementListHelper() {
        debug("statementListHelper()");
        auto chain = statement();
        debug("statementListHelper() chain -> ", chain);
        if (chain != EMPTY_CHAIN) backpatch(chain, inCode.size());
        return chain;
    }
    // <arithmetic_expression> → <arithmetic_expression> + <arithmetic_item>
    // │<arithmetic_expression> - <arithmetic_item>
    // │<arithmetic_item>
    int arithmeticExpression() {
        debug("arithmeticExpression()");
        auto id = arithmeticItem();
        if (tryExpect("+") || tryExpect("-")) {
            unsigned mark = i - 1;
            int id_r = arithmeticExpression();
            int index = storage.get(TEMPORARY_VARIABLE);
            storage[index].type = storage[id].type;
            gen(Data::getValue(tokens[mark].type_id), id, id_r, index);
            id = index;
        }
        return id;
    }
    // <arithmetic_item> → <arithmetic_item> * <arithmetic_factor>
    // │<arithmetic_item> / <arithmetic_factor>
    // │<arithmetic_factor>
    int arithmeticItem() {
        debug("arithmeticItem()");
        auto id = arithmeticFactor();
        if (tryExpect("*") || tryExpect("/")) {
            unsigned mark = i - 1;
            int id_r = arithmeticItem();
            int index = storage.get(TEMPORARY_VARIABLE);
            storage[index].type = storage[id].type;
            gen(Data::getValue(tokens[mark].type_id), id, id_r, index);
            id = index;
        }
        return id;
    }
    // <arithmetic_factor> → <arithmetic_variable>
    // │- <arithmetic_factor>
    int arithmeticFactor() {
        debug("arithmeticFactor()");
        if (tryExpect("-")) {
            unsigned mark = i - 1;
            auto id = arithmeticFactor();
            auto index = storage.get(TEMPORARY_VARIABLE);
            storage[index].type = storage[id].type;
            gen("-", id, EMPTY, index);
            return index;
        } else
            return arithmeticVariable();
    }
    // <arithmetic_variable> → <integer>
    // │<identifier>
    // │( <arithmetic_expression> )
    int arithmeticVariable() {
        debug("arithmeticVariable()");
        if (tryExpect("(")) {
            int id = arithmeticExpression();
            return expect(")"), id;
        }
        if (tryExpect(IDENTIFIER)) {
            i--, expectVar("integer");
            return tokens[i - 1].name_id;
        }
        expect(INTEGER);
        storage[tokens[i - 1].name_id].type = Data::getCode("integer");
        return tokens[i - 1].name_id;
    }
    // <boolean_expression> → <boolean_expression> or
    // <boolean_item>│<boolean_item>
    Pair booleanExpression() {
        debug("booleanExpression()");
        auto [tl_1, fl_1] = booleanItem();
        if (tryExpect("or")) {
            backpatch(fl_1, inCode.size());
            auto [tl_2, fl_2] = booleanExpression();
            return {merge(tl_1, tl_2), fl_2};
        } else
            return {tl_1, fl_1};
    }
    // <boolean_item> → <boolean_item> and <boolean_factor>
    // │<boolean_factor>
    Pair booleanItem() {
        debug("booleanItem()");
        auto [tl_1, fl_1] = booleanFactor();
        if (tryExpect("and")) {
            backpatch(tl_1, inCode.size());
            auto [tl_2, fl_2] = booleanItem();
            return {tl_2, merge(fl_1, fl_2)};
        } else
            return {tl_1, fl_1};
    }
    // <boolean_factor> → <boolean_variable>│not <boolean_factor>
    Pair booleanFactor() {
        debug("booleanFactor()");
        return tryExpect("not") ? booleanFactor() : booleanVariable();
    }
    // <boolean_variable> → <boolean_constant>
    // │ <identifier>
    // │ ( <boolean_expression> )
    // │ <identifier> <relation_word> <identifier>
    // │ <arithmetic_expression> <relation_word> <arithmetic_expression>
    Pair booleanVariable() {
        debug("booleanVariable()");
        auto gen_code = [this](const String &op, int l, int r) -> Pair {
            auto tl = inCode.size();
            gen(op, l, r, EMPTY);
            auto fl = inCode.size();
            gen("j", EMPTY, EMPTY, EMPTY);
            return {tl, fl};
        };
        // <boolean_constant>
        if (tryExpect(BOOLEAN_CONSTANT)) {
            return gen_code("jnz", tokens[i - 1].type_id, EMPTY);
        }
        // <identifier> <relation_word> <identifier>
        if (tryExpect(IDENTIFIER, RELATION_WORD, IDENTIFIER)) {
            (i -= 3), expectVar("integer");
            (i += 1), expectVar("integer");
            return gen_code(String("j") + Data::getValue(tokens[i - 2].type_id),
                            tokens[i - 3].name_id, tokens[i - 1].name_id);
        }
        // ( <boolean_expression> )
        {
            int tl, fl;
            auto booleanHelper = [this, &tl, &fl]() {
                debug("booleanHelper()");
                expect("(");
                std::tie(tl, fl) = booleanExpression();
                expect(")");
            };
            if (tryProcedure(booleanHelper)) return {tl, fl};
        }
        // <arithmetic_expression> <relation_word> <arithmetic_expression>
        {
            int id1, id2, mark;
            auto arithmeticHelper = [this, &id1, &id2, &mark]() {
                debug("arithmeticHelper()");
                id1 = arithmeticExpression();
                mark = i;
                expect(RELATION_WORD);
                id2 = arithmeticExpression();
            };
            if (tryProcedure(arithmeticHelper)) {
                return gen_code("j" + Data::getValue(tokens[mark].type_id), id1,
                                id2);
            }
        }
        // <identifier>
        expectVar("bool");
        return gen_code("jnz", tokens[i - 1].name_id, EMPTY);
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
                if (token.name_id != VALUE_NONE) tokens.push_back(token);
            }
            rowMark.push_back(tokens.size());
            ++row;
        }
        debug("rowMark: ", rowMark);
        debug("Tokens:");
        for (auto tk : tokens) debug(tk, Data::getValue(tk.type_id));
        debug("Storages:");
        for (size_t i = 0; i < storage.size(); i++) debug(storage[i]);
        program();
        debug("parse done.");
    }
    String qustr(const Quaternary &q) {
        auto s = to_string("(", q.op, " , ");
        if (q.l == EMPTY)
            s += "- , ";
        else if (q.op == "jnz") {
            if (tokens[q.l] == BOOLEAN_CONSTANT)
                s += to_string(Data::getValue(q.l), " , ");
            else
                s += to_string(storage[q.l].name, " , ");
        } else
            s += to_string(storage[q.l].name, " , ");
        if (q.r == EMPTY)
            s += "- , ";
        else
            s += to_string(storage[q.r].name, " , ");
        if (q.op[0] == 'j')
            s += to_string(q.id);
        else if (q.id == EMPTY)
            s += "-";
        else
            s += to_string(storage[q.id].name);
        s += ")";
        return s;
    }
    void printCode(std::ostream &out) {
        unsigned i = 0;
        for (auto q : inCode) {
            out << to_string("(", i, ")  ", qustr(q), "\n");
            ++i;
        }
    }
};
