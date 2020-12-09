# SAMPLE LANGUAGE DESCRIPTION
## Character Define

1．`<character_set>` → `<letter>│<digit>│<single_delimiter>`
2．`<letter>` → `A│B│…│Z│a│b│…│z`
3．`<digit>` → `0│1│2│…│9`
4．`<single_delimiter>` → `+│-│*│/│=│<│>│(│)│[│]│:│. │; │, │'`

## Word Define

5．`<word_set>` → `<reserved_word>│<double_delimiter>│<identifier>│<constant>│<single_delimiter>`
6．`<reserved_word>` → `and│array│begin│bool│call│case│char│constant│dim│do│else│end│false│for│if│input│integer│not│of│or│output│procedure│program│read│real│repeat│set│stop│then│to│true│until│var│while│write`
7．`<double_delimiter>` → `<>│<=│>=│:= │/*│*/│..`
8．`<identifier>` → `<letter>│<identifier> <digit>│<identifier> <letter>`
9．`<constant>` → `<integer>│<boolean_constant>│<character_constant>`
10．`<integer>` → `<digit>│<integer> <digit>`
11．`<boolean_constant>` → `true│false`
12．`<character_constant>` → `'any string except {'}'`

## Type Define

13．`<type>` → `integer│bool│char`

## Expression Define

14．`<expression>` → `<arithmetic_expression>│<boolean_expression>│<character_expression>`
15．`<arithmetic_expression>` → `<arithmetic_expression> + <arithmetic_item>│<arithmetic_expression> - <arithmetic_item>│<arithmetic_item>`
16．`<arithmetic_item>` → `<arithmetic_item> * <arithmetic_factor>│<arithmetic_item> / <arithmetic_factor>│<arithmetic_factor>`
17．`<arithmetic_factor>` → `<arithmetic_variable>│- <arithmetic_factor>`
18．`<arithmetic_variable>` → `<integer>│<identifier>│( <arithmetic_expression> )`
19．`<boolean_expression>` → `<boolean_expression> or <boolean_item>│<boolean_item>`
20．`<boolean_item>` → `<boolean_item> and <boolean_factor>│<boolean_factor>`
21．`<boolean_factor>` → `<boolean_variable>│not <boolean_factor>`
22．`<boolean_variable>` → `<boolean_constant>│<identifier>│( <boolean_expression> )│<identifier> <relation_word> <identifier>│<arithmetic_expression> <relation_word> <arithmetic_expression>`
23．`<relation_word>` → `<│<>│<=│>=│>│=`
24．`<character_expression>` → `<character_constant>│<identifier>`

## Statement Define

25．`<statement>` → `<assignment_statement>│<if_statement>│<while_statement>│<repeat_statement>│<compound_statement>`
26．`<assignment_statement>` → `<identifier> := <arithmetic_expression>`
27．`<if_statement>`→ `if <boolean_expression> then <statement>│if <boolean_expression> then <statement> else <statement>`
28．`<while_statement>` → `while <boolean_expression> do <statement>`
29．`<repeat_statement>` → `repeat <statement> until <boolean_expression>`
30．`<compound_statement>` → `begin <statement_list> end`
31．`<statement_list>` → `<statement> ;<statement_list>│<statement>`

## Program Define

32．`<program>` → `program <identifier> ;<variable_declaration> <compound_statement> .`
33．`<variable_declaration>` → `var <variable_define>│ε`
34．`<variable_define>` → `<identifier_list> :<type> ;<variable_define>│<identifier_list> : <type> ;`
35．`<identifier_list>` → `<identifier> , <identifier_list>│<identifier>`

## SIMPLE Word Encode


|   Word   | id  |        Word        | id  | Word | id  |
| -------- | --- | ------------------ | --- | ---- | --- |
| and      | 1   | output             | 21  | `*`  | 41  |
| array    | 2   | procedure          | 22  | `*/` | 42  |
| begin    | 3   | program            | 23  | +    | 43  |
| bool     | 4   | read               | 24  | ,    | 44  |
| call     | 5   | real               | 25  | -    | 45  |
| case     | 6   | repeat             | 26  | .    | 46  |
| char     | 7   | set                | 27  | ..   | 47  |
| constant | 8   | stop               | 28  | /    | 48  |
| dim      | 9   | then               | 29  | `/*` | 49  |
| do       | 10  | to                 | 30  | :    | 50  |
| else     | 11  | TRUE               | 31  | :=   | 51  |
| end      | 12  | until              | 32  | ;    | 52  |
| FALSE    | 13  | var                | 33  | `<`  | 53  |
| for      | 14  | while              | 34  | `<=` | 54  |
| if       | 15  | write              | 35  | `<>` | 55  |
| input    | 16  | identifier         | 36  | =    | 56  |
| integer  | 17  | integer            | 37  | `>`  | 57  |
| not      | 18  | character_constant | 38  | `>=` | 58  |
| of       | 19  | (                  | 39  | `[`  | 59  |
| or       | 20  | )                  | 40  | `]`  | 60  |


## 实验一：设计SAMPLE语言的词法分析器
检查要求：
a)启动程序后，先输出作者姓名、班级、学号（可用汉语、英语或拼音）；
b)请求输入测试程序名，键入程序名后自动开始词法分析并输出结果；
c)输出结果为单词的二元式序列（样式见样板输出1和2）；
d)要求能发现下列词法错误和指出错误性质和位置：
非法字符，即不是SAMPLE字符集的符号；
字符常数缺右边的单引号（字符常数要求左、右边用单引号界定，不能跨行）；
注释部分缺右边的界符`*/`（注释要求左右边分别用`/*`和`*/`界定，不能跨行）。

实验一测试程序与样板输出：
测试程序1： 程序名TEST1，参考文件`test1.in`.
样板输出1：（要求在屏幕上显示）注：作为自身值的内容显示做了简化，便于检查。只有标识符和常数有显示，并且通过数字来区分他们的不同。参考文件`test2.in`.

测试程序2：程序名TEST2，参考文件`test2.in`
样板输出2：（要求在屏幕上显示）参考文件`test2.ans`

## 实验二：设计SAMPLE语言的语法、语义分析器，输出四元式的中间结果。
检查要求：
a)启动程序后，先输出作者姓名、班级、学号（可用汉语、英语或拼音）。
b)请求输入测试程序名，键入程序名后自动开始编译。
c)输出四元式中间代码（样式见样板输出3和4）。
d)能发现程序的语法错误并输出出错信息。

测试样板程序与样板输出:
测试程序3：程序名TEST4，参考文件`test3.in`
样板输出3：（要求在屏幕上显示)，参考文件`test3.ans`
测试程序4：程序名TEST5，参考文件`test4.in`
样板输出4：（要求在屏幕上显示）参考文件`test4.ans`