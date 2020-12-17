# SAMPLE LANGUAGE DESCRIPTION
[中文](README_zh-CN.md)

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
| else     | 11  | true               | 31  | :=   | 51  |
| end      | 12  | until              | 32  | ;    | 52  |
| false    | 13  | var                | 33  | `<`  | 53  |
| for      | 14  | while              | 34  | `<=` | 54  |
| if       | 15  | write              | 35  | `<>` | 55  |
| input    | 16  | identifier         | 36  | =    | 56  |
| integer  | 17  | integer            | 37  | `>`  | 57  |
| not      | 18  | character_constant | 38  | `>=` | 58  |
| of       | 19  | (                  | 39  | `[`  | 59  |
| or       | 20  | )                  | 40  | `]`  | 60  |
