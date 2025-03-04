#define _CRT_SECURE_NO_WARNINGS 
#define _CRT_NONSTDC_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#define maxsymbolIndex 100 // 定义符号表的容量   
#define MAX_CODES 200      // Maximum number of intermediate codes
#define MAX_ERRORS 100     // 最大错误数

enum Category_symbol { variable, function }; // 标志符的类型

typedef struct Tree {
    char* data;
    int lineNo;
    char* extraInfo;
    struct Tree* firstChild;
    struct Tree* nextSibling;
} Tree;

struct Symbol {
    char name[20];
    enum Category_symbol kind;
    int address;
} symbol[maxsymbolIndex];

typedef struct Code {
    char opt[10];    // Operation code
    int operand;     // Operand
} Code;

Code codes[MAX_CODES];          // Array to store intermediate codes

int codesIndex;                 // Next empty index in codes array
int symbolIndex = 0;            // Next empty index in symbol table
int offset;
int offsetF;
int index_array[100]; // 重命名为 index_array 以避免与关键字冲突

// 定义错误结构和错误数组
typedef struct {
    int code;   // 错误代码
    int line;   // 出错的行号
} Error;

Error errorList[MAX_ERRORS];
int errorCount = 0;

// 创建一个新节点
Tree* createNode(const char* str, int lineNo, const char* extraInfo) {
    Tree* newNode = (Tree*)malloc(sizeof(Tree));
    if (!newNode) {
        perror("Failed to allocate memory for new node");
        exit(EXIT_FAILURE);
    }
    newNode->data = strdup(str); // 复制字符串
    newNode->lineNo = lineNo;
    if (extraInfo)
        newNode->extraInfo = strdup(extraInfo);
    else
        newNode->extraInfo = NULL;
    newNode->firstChild = NULL;
    newNode->nextSibling = NULL;
    return newNode;
}

// 添加非终结节点
Tree* addNonTerminal(Tree* parent, const char* str, int lineNo) {
    Tree* newSon = createNode(str, lineNo, NULL); // 使用传入的行号
    if (!parent->firstChild) {
        parent->firstChild = newSon;
    }
    else {
        Tree* current = parent->firstChild;
        while (current->nextSibling) {
            current = current->nextSibling;
        }
        current->nextSibling = newSon;
    }
    return newSon;
}

// 添加终结节点
Tree* addTerminal(Tree* parent, const char* str, const char* extraInfo) {
    Tree* newSon = createNode(str, 0, extraInfo); // lineNo 为 0 表示终结节点
    if (!parent->firstChild) {
        parent->firstChild = newSon;
    }
    else {
        Tree* current = parent->firstChild;
        while (current->nextSibling) {
            current = current->nextSibling;
        }
        current->nextSibling = newSon;
    }
    return newSon;
}

// 打印语法树
void printTree(Tree* p, int depth, FILE* outputFile) {
    if (!p) return;

    if (p->lineNo > 0 && p->firstChild) {
        for (int i = 0; i < depth; i++) {
            fprintf(outputFile, "  ");
        }
        fprintf(outputFile, "%s %d\n", p->data, p->lineNo);
    }
    else if (p->extraInfo != NULL) {
        for (int i = 0; i < depth; i++) {
            fprintf(outputFile, "  ");
        }
        if (strcmp(p->data, "ID") == 0) {
            fprintf(outputFile, "ID: %s\n", p->extraInfo);
        }
        else if (strcmp(p->data, "TYPE") == 0) {
            fprintf(outputFile, "TYPE: %s\n", p->extraInfo);
        }
        else if (strcmp(p->data, "NUM") == 0) {
            fprintf(outputFile, "NUM: %s\n", p->extraInfo);
        }
        else {
            fprintf(outputFile, "%s\n", p->data);
        }
    }
    printTree(p->firstChild, depth + 1, outputFile);
    printTree(p->nextSibling, depth, outputFile);
}

void freeTree(Tree* p) {
    if (!p) return;
    freeTree(p->firstChild);
    freeTree(p->nextSibling);
    free(p->data);
    if (p->extraInfo) free(p->extraInfo);
    free(p);
}

FILE* inputFile;
FILE* outputFile;
FILE* fpCodeText;//E:\Code\编译原理\yuyi.c
char inputAddress[100] = "E:\\Code\\编译原理\\321.txt";
char outputAddress[100] = "E:\\Code\\编译原理\\3.txt";
char Codeout[100] = "E:\\Code\\编译原理\\4.txt";
char type[20];
char value[40];
int line = 0;
Tree* root = NULL;

void getToken() {
    if (fscanf(inputFile, "%s%s%d\n", type, value, &line) != 3) {
        strcpy(type, "EOF");
        strcpy(value, "EOF");
    }
}

void logError(int errorCode, int lineNo) {
    if (errorCount < MAX_ERRORS) {
        errorList[errorCount].code = errorCode;
        errorList[errorCount].line = lineNo;
        errorCount++;
    }
    else {
        printf("\u9519\u8bef\u6570\u7ec4\u5df2\u6ee1\uff0c\u65e0\u6cd5\u8bb0\u5f55\u66f4\u591a\u9519\u8bef\u3002\n");
    }
}

// 同步函数
void skip() {
    while (strcmp(type, ";") != 0 &&
        strcmp(type, "}") != 0 &&
        strcmp(type, "EOF") != 0) {
        getToken();
    }
    if (strcmp(type, ";") == 0 || strcmp(type, "}") == 0) {
        getToken(); // 跳过同步符号
    }
}

// 声明内容可能少了一个读取
//int declaration_stat(Tree* parent); // 前向声明

int parseAlanalysis();
int program();
int fun_declaration(Tree* parent);
int main_declaration(Tree* parent);
int function_body(Tree* parent);
int declaration_list(Tree* parent);
int declaration_stat(Tree* parent);
int statement_list(Tree* parent);
int statement(Tree* parent);
int if_stat(Tree* parent);
int while_stat(Tree* parent);
int for_stat(Tree* parent);
int read_stat(Tree* parent);
int write_stat(Tree* parent);
int compound_stat(Tree* parent);
int call_stat(Tree* parent);
int expression_stat(Tree* parent);
int expression(Tree* parent);
int bool_expr(Tree* parent);
int additive_expr(Tree* parent);
int term(Tree* parent);
int factor(Tree* parent);
void getToken();
int insert_Symbol(enum Category_symbol category, char* name);
int do_while_stat(Tree* parent);

// 将当前数据存入语法树中
// 输出语法树

int insert_Symbol(enum Category_symbol category, char* name)
{
    int i, es = 0;

    if (symbolIndex >= maxsymbolIndex) return(21); // 符号表溢出 
    switch (category) {
    case function:
        for (i = symbolIndex - 1; i >= 0; i--) { // 查符号表
            if ((strcmp(symbol[i].name, name) == 0) && (symbol[i].kind == function)) {
                logError(32, line); // 缺少 '('
                break;
            }
        }
        symbol[symbolIndex].kind = function;
        symbol[symbolIndex].address = offset;
        offset++; // 分配一个单元，数据区指针加
        break;
    case variable:
        for (i = symbolIndex - 1; i >= 0; i--) { // 查符号表
            if ((strcmp(symbol[i].name, name) == 0) && (symbol[i].kind == variable)) {
               // es = 22; // 22表示变量重复定义
                logError(22, line); // 缺少 '('
                break;
            }
        }
        symbol[symbolIndex].kind = variable;
        symbol[symbolIndex].address = offset;
        offset++; // 分配一个单元，数据区指针加
        break;
    }
    if (es > 0)   return 0;
    strcpy(symbol[symbolIndex].name, name);
    symbolIndex++;
    return 0;
}

int lookup(char* name, int* pPosition)
{
    int i, es = 0;
    for (i = 0; i < symbolIndex; i++) {
        if (strcmp(symbol[i].name, name) == 0) {
            *pPosition = i;
            return 0;
        }
    }
   // logError(23, line); // 缺少 '('
    *pPosition = 999;
    return 1;
}

int statement_list(Tree* parent) {
    int es = 0;
    Tree* current = addNonTerminal(parent, "<statement_list>", line);

    while (1) {
        // 假设语句以 '}' 或 'EOF' 结束
        if (strcmp("}", type) == 0 || strcmp("EOF", type) == 0) {
            break; // 语句列表结束
        }

        es = statement(current);
        if (es > 0) {
            //logError(es, line);
           // synchronize();
            // 继续解析
            return es;
        }
    }

    return es;
}

// 语句内容
// <statement> ::= <if_stat> | <while_stat> | <for_stat> | <read_stat> | <write_stat> | <compound_stat> | <expression_stat> | <call_stat>
int statement(Tree* parent) {
    int es = 0;
    Tree* current = addNonTerminal(parent, "<statement>", line);

    if (strcmp("if", type) == 0) {
        es = if_stat(current);
    }
    else if (strcmp("while", type) == 0) {
        es = while_stat(current);
    }
    else if (strcmp("for", type) == 0) {
        es = for_stat(current);
    }
    else if (strcmp("read", type) == 0) {
        es = read_stat(current);
    }
    else if (strcmp("write", type) == 0) {
        es = write_stat(current);
    }
    else if (strcmp("{", type) == 0) {
        es = compound_stat(current);
    }
    else if (strcmp("call", type) == 0) {
        es = call_stat(current);
    }
      else if (strcmp("do", type) == 0) {   //do_while语句
        es= do_while_stat(current);
        }
    else {
        es = expression_stat(current);
    }

    if (es > 0) {
        return es;
    }

    return es;
}

// <if_stat> ::= if '(' <bool_expr> ')' <statement> [else <statement>]
int if_stat(Tree* parent) {
    int es = 0, cx1, cx2;
    Tree* current = addNonTerminal(parent, "<if_stat>", line);

    // 添加 "if" 终结符
    addTerminal(current, type, value);
    getToken(); // 读取下一个 token

    // 检查 '('
    if (strcmp("(", type) != 0) {
        logError(4, line); // 错误代码：缺少 '('
        skip();
        return 0; // 继续解析
    }
    addTerminal(current, type, value);
    getToken(); // 读取下一个 token

    // 解析 <bool_expr>
    es = bool_expr(current);
    if (es > 0) {
        return es;
    }

    if (strcmp(")", type) != 0) {
        logError(5, line); // 错误代码：缺少 ')'
        skip();
        return 0;
    }

    strcpy(codes[codesIndex].opt, "BRF");
    cx1 = codesIndex++;

    addTerminal(current, type, value);
    getToken(); // 读取下一个 token

    // 解析 <statement>
    es = statement(current);
    if (es > 0) {
        return es;
    }

    strcpy(codes[codesIndex].opt, "BR");
    cx2 = codesIndex++;
    codes[cx1].operand = codesIndex;

    // 可选的 'else' 部分
    if (strcmp("else", type) == 0) {
        addTerminal(current, type, value);
        getToken(); // 读取下一个 token
        es = statement(current);
        if (es > 0) {
            return es;
        }
    }
    codes[cx2].operand = codesIndex;
    return es;
}

// <while_stat> ::= while '(' <bool_expr> ')' <statement>
int while_stat(Tree* parent) {
    int es = 0, cx, entrance;
    entrance = codesIndex;
    Tree* current = addNonTerminal(parent, "<while_stat>", line);

    // 添加 "while" 终结符
    addTerminal(current, type, value);
    getToken(); // 读取下一个 token

    // 检查 '('
    if (strcmp("(", type) != 0) {
        logError(4, line); // 缺少 '('
        skip();
        return 0;
    }

    addTerminal(current, type, value);
    getToken(); // 读取下一个 token

    // 解析 <bool_expr>
    es = bool_expr(current);
    if (es > 0) {
        return es;
    }

    // 检查 ')'
    if (strcmp(")", type) != 0) {
        logError(5, line); // 缺少 ')'
        skip();
        return 0;
    }
    addTerminal(current, type, value);

    strcpy(codes[codesIndex].opt, "BRF");
    cx = codesIndex++;

    getToken(); // 读取下一个 token

    // 解析 <statement>
    es = statement(current);
    if (es > 0) {
        return es;
    }

    strcpy(codes[codesIndex].opt, "BR");
    codes[codesIndex].operand = entrance; // 回到判断起点
    codesIndex++;
    codes[cx].operand = codesIndex;

    return es;
}


// <do_while_stat> ::= do <statement> while '(' <bool_expr> ')' ';'
int do_while_stat(Tree* parent) {
    int es = 0, entrance, cx;
    Tree* current = addNonTerminal(parent, "<do_while_stat>", line);
 
    // 检查并添加 "do" 关键字
    if (strcmp("do", type) != 0) {
        logError(11, line); // 缺少保留字 "do"
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // 读取下一个 token

    // 记录循环开始的位置
    entrance = codesIndex;

    // 解析 <statement>
    es = statement(current);
    if (es > 0) {
        return es;
    }

    // 检查并添加 "while" 关键字
    if (strcmp("while", type) != 0) {
        logError(11, line); // 缺少保留字 "while"
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // 读取下一个 token

    // 检查 '('
    if (strcmp("(", type) != 0) {
        logError(4, line); // 缺少 '('
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // 读取下一个 token

    // 解析 <bool_expr>
    es = bool_expr(current);
    if (es > 0) {
        return es;
    }

    // 检查 ')'
    if (strcmp(")", type) != 0) {
        logError(5, line); // 缺少 ')'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // 读取下一个 token

    // 检查 ';'
    if (strcmp(";", type) != 0) {
        logError(3, line); // 缺少 ';'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // 读取下一个 token

    // 生成代码：条件判断，若为真则跳回入口
    strcpy(codes[codesIndex].opt, "BRF"); // 判断条件为假则跳出循环
    cx = codesIndex++;
    strcpy(codes[codesIndex].opt, "BR"); // 判断条件为真则跳回入口
    codes[codesIndex].operand = entrance;
    codesIndex++;

    // 填充跳出循环的地址
    codes[cx].operand = codesIndex;

    return es;
}

// <for_stat> ::= for '(' <expression> ';' <bool_expr> ';' <expression> ')' <statement>
int for_stat(Tree* parent) {
    int es = 0, cx1, cx2, entrance1, entrance2;
    Tree* current = addNonTerminal(parent, "<for_stat>", line);

    // 添加 "for" 终结符
    addTerminal(current, type, value);
    getToken(); // 读取下一个 token

    // 检查 '('
    if (strcmp("(", type) != 0) {
        logError(4, line); // 缺少 '('
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // 读取下一个 token

    // 解析 <expression>
    es = expression(current);
    if (es > 0) {
        return es;
    }

    // 检查 ';'
    if (strcmp(";", type) != 0) {
        logError(3, line); // 缺少 ';'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    entrance1 = codesIndex;
    getToken(); // 读取下一个 token

    // 解析 <bool_expr>
    es = bool_expr(current);
    if (es > 0) {
        return es;
    }

    // 检查 ';'
    if (strcmp(";", type) != 0) {
        logError(3, line); // 缺少 ';'
        skip();
        return 0;
    }
    addTerminal(current, type, value);

    strcpy(codes[codesIndex].opt, "BRF");    // 判断为假就结束 for
    cx1 = codesIndex++;                      // 暂时不知道结束位置，等待反填
    strcpy(codes[codesIndex].opt, "BR");     // 判断为真就进入循环语句
    cx2 = codesIndex++;                      // 暂时不知道语句开始位置，等待反填

    getToken(); // 读取下一个 token
    entrance2 = codesIndex;

    // 解析 <expression>
    es = expression(current);
    if (es > 0) {
        return es;
    }

    // 检查 ')'
    if (strcmp(")", type) != 0) {
        logError(5, line); // 缺少 ')'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    strcpy(codes[codesIndex].opt, "BR");    // 执行完就进入判断
    codes[codesIndex].operand = entrance1;   // 跳转的位置是判断语句的入口
    codesIndex++;
    getToken(); // 读取下一个 token

    codes[cx2].operand = codesIndex;
    // 解析 <statement>
    es = statement(current);
    if (es > 0)    return es;

    strcpy(codes[codesIndex].opt, "BR");    // 执行完就进入条件3
    codes[codesIndex].operand = entrance2;   // 跳转的位置是条件3的入口
    codesIndex++;
    codes[cx1].operand = codesIndex;

    return es;
}

// <write_stat> ::= write '(' <expression> ')' ';'
int write_stat(Tree* parent) {
    int es = 0;
    Tree* current = addNonTerminal(parent, "<write_stat>", line);

    // 添加 "write" 终结符
    addTerminal(current, type, value);
    getToken(); // 读取下一个 token

    // 解析 '('
    // 解析 <expression>
    es = expression(current);
    if (es > 0) {
        return es;
    }

    // 检查 ';'
    if (strcmp(";", type) != 0) {
        logError(3, line); // 缺少 ';'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    strcpy(codes[codesIndex].opt, "OUT");     // 输出
    codesIndex++;
    getToken(); // 读取下一个 token

    return es;
}

// <read_stat> ::= read '(' ID ')' ';'
int read_stat(Tree* parent) {
    int symbolPos, es = 0;
    Tree* current = addNonTerminal(parent, "<read_stat>", line);
    addTerminal(current, type, value);
    getToken(); // 读取下一个 token

    // 检查 ID
    if (strcmp("ID", type) != 0) {
        logError(6, line); // 缺少标识符
        skip();
        return 0;
    }
    addTerminal(current, type, value); // 添加 ID 和其值
    if(lookup(value, &symbolPos))  logError(23, line);
    if (symbol[symbolPos].kind != variable) {
        logError(35, line); // 假设 35 表示非变量类型
        skip();
        return 0;
    }

    strcpy(codes[codesIndex].opt, "IN");
    codesIndex++;
    strcpy(codes[codesIndex].opt, "STO");
    codes[codesIndex].operand = symbol[symbolPos].address;
    codesIndex++;
    getToken(); // 读取下一个 token

    // 检查 ';'
    if (strcmp(";", type) != 0) {
        logError(3, line); // 缺少 ';'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // 读取下一个 token

    return es;
}

// <compound_stat> ::= '{' <statement_list> '}'
int compound_stat(Tree* parent) {
    int es = 0;
    Tree* current = addNonTerminal(parent, "<compound_stat>", line);

    // 检查 '{'
    if (strcmp("{", type) != 0) {   // 缺少 '{'
        logError(1, line); // 错误代码：缺少 '{'
        skip();
        return 0;
    }
    addTerminal(current, type, value); // 将 '{' 添加到语法树中
    getToken(); // 读取下一个 token

    // 解析 <statement_list>
    es = statement_list(current);
    if (es > 0) {
        return es;
    }

    // 检查 '}'
    if (strcmp("}", type) != 0) {   // 缺少 '}'
        logError(2, line); // 错误代码：缺少 '}'
        skip();
        return 0;
    }
    addTerminal(current, type, value); // 将 '}' 添加到语法树中
    getToken(); // 读取下一个 token

    return es;
}

// <expression_stat> ::= <expression> ';' | ';'
int expression_stat(Tree* parent) {
    int es = 0;
    Tree* current = addNonTerminal(parent, "<expression_stat>", line);

    if (strcmp(";", type) == 0) {
        // 空表达式语句
        addTerminal(current, type, value);
        getToken(); // 读取下一个 token
        return es;
    }

    // 解析 <expression>
    es = expression(current);
    if (es > 0) {
        return es;
    }

    // 检查 ';'
    if (strcmp(";", type) != 0) {
        logError(3, line); // 错误代码：缺少 ';'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // 读取下一个 token

    return es;
}

// <call_stat> ::= call ID '(' ')' ';'
int call_stat(Tree* parent) {
    int es = 0, symbolPos;
    Tree* current = addNonTerminal(parent, "<call_stat>", line);
    addTerminal(current, type, value);
    getToken(); // 读取下一个 token

    // 检查 ID（函数名）
    if (strcmp("ID", type) != 0) {
        logError(6, line); // 缺少标识符
        skip();
        return 0;
    }
    if(lookup(value, &symbolPos))   logError(34, line);

    if (symbol[symbolPos].kind != function && !lookup(value, &symbolPos)) {
        logError(34, line); // 假设 34 表示非函数类型
        skip();
        return 0;
    }

    addTerminal(current, type, value); // 添加函数名
    getToken(); // 读取下一个 token

    // 检查 '('
    if (strcmp("(", type) != 0) {
        logError(4, line); // 缺少 '('
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // 读取下一个 token

    // 检查 ')'
    if (strcmp(")", type) != 0) {
        logError(5, line); // 缺少 ')'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // 读取下一个 token

    // 检查 ';'
    if (strcmp(";", type) != 0) {
        logError(3, line); // 缺少 ';'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    strcpy(codes[codesIndex].opt, "CAL");     // 调用函数
    codes[codesIndex].operand = symbol[symbolPos].address;     // 进入函数的开始位置
    codesIndex++;
    getToken(); // 读取下一个 token

    return es;
}

// <expression> ::= ID '=' <bool_expr> | <bool_expr>
int expression(Tree* parent) {
    int es = 0;
    Tree* current = addNonTerminal(parent, "<expression>", line);
    char type1[20];     // 临时单词类型
    char value1[40];    // 临时单词值
    long curPoint;
    int place;

    // 此处进来的数据可能是 ID
    if (strcmp("ID", type) == 0) {   //<expression> ::= ID '=' <bool_expr>
        // 保存当前文件指针和行号
        curPoint = ftell(inputFile);
        // 读取下一个 token
        strcpy(type1, type);
        strcpy(value1, value);
        getToken(); // 读取下一个 token

        // 检查是否为 '='
        if (strcmp("=", type) == 0) {    // 该表达式例如 a=xxxx
            if(lookup(value1, &place)) logError(23, line);
            addTerminal(current, type, value);
            getToken(); // 读取下一个 token

            // 解析 <bool_expr>
            es = bool_expr(current);
            if (es > 0) {
                return es;
            }

            strcpy(codes[codesIndex].opt, "STO");
            codes[codesIndex++].operand = symbol[place].address;
        }
        else {                      //<bool_expr>
            // 回退到 '=' 前的位置
            fseek(inputFile, curPoint, SEEK_SET);
            strcpy(type, type1);
            strcpy(value, value1);
            es = bool_expr(current);
            if (es > 0) {
                return es;
            }
        }
    }
    else {
        // 解析 <bool_expr>
        es = bool_expr(current);
        if (es > 0) {
            return es;
        }
    }
    return es;
}

// <bool_expr> ::= <additive_expr> { ( '>' | '<' | '>=' | '<=' | '==' | '!=' ) <additive_expr> }
int bool_expr(Tree* parent) {
    int es = 0;
    Tree* current = addNonTerminal(parent, "<bool_expr>", line);

    // 解析第一个 <additive_expr>
    es = additive_expr(current);
    if (es > 0) {
        return es;
    }

    // 解析后续的关系运算符和 <additive_expr>
    while (strcmp(">", type) == 0 || strcmp("<", type) == 0 ||
        strcmp(">=", type) == 0 || strcmp("<=", type) == 0 ||
        strcmp("==", type) == 0 || strcmp("!=", type) == 0) {
        char token2[20];
        strcpy(token2, type); // 保存运算符
        addTerminal(current, type, value); // 添加关系运算符
        getToken(); // 读取下一个 token

        es = additive_expr(current);
        if (es > 0) {
            return es;
        }

        if (strcmp(token2, ">") == 0)
            strcpy(codes[codesIndex++].opt, "GT");

        if (strcmp(token2, ">=") == 0)
            strcpy(codes[codesIndex++].opt, "GE");

        if (strcmp(token2, "<") == 0)
            strcpy(codes[codesIndex++].opt, "LES");

        if (strcmp(token2, "<=") == 0)
            strcpy(codes[codesIndex++].opt, "LE");

        if (strcmp(token2, "==") == 0)
            strcpy(codes[codesIndex++].opt, "EQ");

        if (strcmp(token2, "!=") == 0)
            strcpy(codes[codesIndex++].opt, "NOTEQ");
    }
    return es;
}

// <additive_expr> ::= <term> { ( '+' | '-' ) <term> }
int additive_expr(Tree* parent) {
    int es = 0;
    Tree* current = addNonTerminal(parent, "<additive_expr>", line);

    // 解析第一个 <term>
    es = term(current);
    if (es > 0) {
        return es;
    }

    // 解析后续的 '+' 或 '-' 和 <term>
    while (strcmp("+", type) == 0 || strcmp("-", type) == 0) {
        char token2[20];
        strcpy(token2, type);
        addTerminal(current, type, value); // 添加 '+' 或 '-'
        getToken(); // 读取下一个 token

        es = term(current);
        if (es > 0) {
            return es;
        }

        if (strcmp(token2, "+") == 0)
            strcpy(codes[codesIndex++].opt, "ADD");
        if (strcmp(token2, "-") == 0)
            strcpy(codes[codesIndex++].opt, "SUB");
    }
    return es;
}

// <term> ::= <factor> { ( '*' | '/' ) <factor> }
int term(Tree* parent) {
    int es = 0;
    Tree* current = addNonTerminal(parent, "<term>", line);

    // 解析第一个 <factor>
    es = factor(current);
    if (es > 0) {
        return es;
    }

    // 解析后续的 '*' 或 '/' 和 <factor>
    while (strcmp("*", type) == 0 || strcmp("/", type) == 0) {
        char token2[20];
        strcpy(token2, type);
        addTerminal(current, type, value); // 添加 '*' 或 '/'
        getToken(); // 读取下一个 token

        es = factor(current);
        if (es > 0) {
            return es;
        }

        if (strcmp(token2, "*") == 0)
            strcpy(codes[codesIndex++].opt, "MULT");
        if (strcmp(token2, "/") == 0)
            strcpy(codes[codesIndex++].opt, "DIV");
    }
    return es;
}

// <factor> ::= '(' <additive_expr> ')' | ID | NUM
int factor(Tree* parent) {
    int es = 0;
    int symbolPos;
    Tree* current = addNonTerminal(parent, "<factor>", line);

    if (strcmp("(", type) == 0) {
        addTerminal(current, type, value);
        getToken(); // 读取下一个 token

        // 解析 <additive_expr>
        es = additive_expr(current);
        if (es > 0) {
            return es;
        }

        if (strcmp(")", type) != 0) {
            logError(5, line); // 缺少 ')'
            skip();
            return 0;
        }
        addTerminal(current, type, value);
        getToken(); // 读取下一个 token
    }
    else if (strcmp("ID", type) == 0) {
        if(lookup(value, &symbolPos))   logError(23, line);
        strcpy(codes[codesIndex].opt, "LOAD");
        codes[codesIndex++].operand = symbol[symbolPos].address;

        addTerminal(current, type, value); // 添加 ID
        getToken(); // 读取下一个 token
    }
    else if (strcmp("NUM", type) == 0) {
        strcpy(codes[codesIndex].opt, "LOADI");
        codes[codesIndex++].operand = atoi(value);
        addTerminal(current, type, value); // 添加 NUM
        getToken(); // 读取下一个 token
    }
    else {
        logError(7, line); // 错误代码：未识别的因子
        skip();
        return 0;
    }

    return es;
}

// <declaration_stat>::= int ID ';' | int ID '=' NUM ';'
int declaration_stat(Tree* parent) {
    int es = 0;
    Tree* current = addNonTerminal(parent, "<declaration_stat>", line);
    // 当前 token 是 "int"
    addTerminal(current, type, value);

    getToken(); // 读取下一个 token
    // 读取下一个 token，应该是 ID
    if (strcmp("ID", type) != 0) {
        logError(6, line); // 缺少标识符
        skip();
        return 0;
    }
    addTerminal(current, type, value); // 添加变量名到语法树中

    es = insert_Symbol(variable, value); // 插入符号表

    getToken(); // 读取下一个 token

    // 读取下一个 token，可能是 ';' 或 '='
    if (strcmp(";", type) == 0) {
        addTerminal(current, type, value);
        getToken(); // 读取下一个 token
    }
    else if (strcmp("=", type) == 0) {
        addTerminal(current, type, value);
        getToken(); // 读取下一个 token

        // 赋值的值应该是 NUM
        if (strcmp("NUM", type) != 0) {
            logError(7, line); // 错误代码：缺少操作数
            skip();
            return 0;
        }
        addTerminal(current, type, value); // 添加 NUM 到语法树中
        getToken(); // 读取下一个 token

        // 检查 ';'
        if (strcmp(";", type) != 0) {
            logError(3, line); // 错误代码：缺少 ';'
            skip();
            return 0;
        }
        addTerminal(current, type, value);
        getToken(); // 读取下一个 token
    }
    else {
        logError(3, line); // 错误代码：缺少 ';'
        skip();
        return 0;
    }
    return es;
}

// 声明序列
int declaration_list(Tree* parent) {
    int es = 0;
    Tree* current = addNonTerminal(parent, "<declaration_list>", line);

    while (1) {
        if (strcmp("int", type) != 0) { // 不是 "int"，说明没有更多声明
            break;
        }
        es = declaration_stat(current); // 解析声明
        if (es > 0) {
            return es;
        }
    }
    return es;
}

// 函数体
// <function_body> ::= '{' <declaration_list> <statement_list> '}'
int function_body(Tree* parent) {
    int es = 0;

    // 添加 <function_body> 节点到语法树
    Tree* current = addNonTerminal(parent, "<function_body>", line);

    // 检查 '{'
    if (strcmp("{", type) != 0) {   // 缺少 '{'
        logError(1, line); // 错误代码：缺少 '{'
        skip();
        return 0;
    }
    addTerminal(current, type, value); // 将 '{' 添加到语法树中
    getToken(); // 读取下一个 token

    // 解析 <declaration_list>
    es = declaration_list(current);
    if (es > 0) {
        return es;
    }
    strcpy(codes[codesIndex].opt, "ENTER");  // 为函数调用开辟空间
    codes[codesIndex].operand = offset - offsetF + 2;
    codesIndex++;
    // 解析 <statement_list>
    es = statement_list(current);
    if (es > 0) {
        return es;
    }
    strcpy(codes[codesIndex].opt, "RETURN");  // 函数返回
    codesIndex++;
    // 检查 '}'
    if (strcmp("}", type) != 0) {   // 缺少 '}'
        logError(2, line); // 错误代码：缺少 '}'
        skip();
        return 0;
    }
    addTerminal(current, type, value); // 将 '}' 添加到语法树中
    getToken(); // 读取下一个 token

    return es;
}

// 主函数
int main_declaration(Tree* parent) { // <main_declaration>::= main '(' ')' <function_body>
    int es = 0;
    char name_main[10] = "main";
    es = insert_Symbol(function, name_main);

    // 添加 <main_declaration> 节点
    Tree* current = addNonTerminal(parent, "<main_declaration>", line);

    // 当前 token 是 "main"
    addTerminal(current, type, value); // 将 "main" 添加到语法树中
    getToken(); // 读取下一个 token

    // 检查 '('
    if (strcmp("(", type) != 0) {
        logError(4, line); // 缺少 '('
        skip();
        return 0;
    }
    addTerminal(current, type, value); // 添加 '(' 到语法树中
    getToken(); // 读取下一个 token

    // 检查 ')'
    if (strcmp(")", type) != 0) {
        logError(5, line); // 缺少 ')'
        skip();
        return 0;
    }
    addTerminal(current, type, value); // 添加 ')' 到语法树中
    codes[0].operand = codesIndex;
    offset = 2;        // 每次进入一个函数体的时候，相对地址都要重置为2
    offsetF = offset;
    symbol[symbolIndex - 1].address = codesIndex;
    getToken(); // 读取下一个 token

    // 解析函数体
    es = function_body(current);
    return es;
}

int fun_declaration(Tree* parent) { // <fun_declaration>::= function ID '(' [参数列表] ')' <function_body>
    int es = 0;
    // 添加 <fun_declaration> 节点
    Tree* current = addNonTerminal(parent, "<fun_declaration>", line);

    addTerminal(current, type, value); // 将 "function" 添加到语法树中
    getToken(); // 读取下一个 token

    // 检查 ID
    if (strcmp("ID", type) != 0) {
        logError(6, line); // 缺少标识符
        skip();
        return 0;
    }
    es = insert_Symbol(function, value); // 将函数名插入符号表
    offset = 2;        // 每次进入一个函数体的时候，相对地址都要重置为2
    offsetF = offset;
    symbol[symbolIndex - 1].address = codesIndex;
    int fPos = codesIndex; // 函数声明位置
    addTerminal(current, type, value); // 添加函数名到语法树中

    getToken(); // 读取下一个 token

    // 检查 '('
    if (strcmp("(", type) != 0) {
        logError(4, line); // 缺少 '('
        skip();
        return 0;
    }
    addTerminal(current, type, value); // 添加 '(' 到语法树中
    getToken(); // 读取下一个 token

    // 检查 ')'
    if (strcmp(")", type) != 0) {
        logError(5, line); // 缺少 ')'
        skip();
        return 0;
    }
    addTerminal(current, type, value); // 添加 ')' 到语法树中
    getToken(); // 读取下一个 token

    // 解析函数体
    es = function_body(current);
    return es;
}

int program() {
    int es = 0;
    strcpy(codes[codesIndex++].opt, "BR");
    getToken();
    while (strcmp("function", type) == 0) {
        es = fun_declaration(root);
        if (es > 0) {
            return es;
        }
    }
    codes[0].operand = codesIndex;
    if (strcmp("main", type) == 0) {
        es = main_declaration(root);
        if (es > 0) {
            return es;
        }
    }
    else {
        logError(6, line); // 缺少 main 声明
    }
    printf("     符号表\n");
    printf(" 名字\t \t类型 \t地址\n");
    for (int i = 0; i < symbolIndex; i++)
        printf("  %-8s \t%d \t%d\n", symbol[i].name, symbol[i].kind, symbol[i].address);
    return es;
}

int parseAlanalysis() {

    // 打开输入文件
    inputFile = fopen(inputAddress, "r");
    if (inputFile == NULL) {
        logError(8, line); // 无法打开输入文件
        return 8;
    }

    // 打开输出文件
    outputFile = fopen(outputAddress, "w");
    if (outputFile == NULL) {
        logError(9, line); // 无法打开输出文件
        fclose(inputFile);
        return 9;
    }

    // 初始化根节点
    root = createNode("<program>", 0, NULL);

    // 开始语法分析
    int es = program(); // 需要实现 program 函数

    return es;
}

int main() {
    int es = 0;
    es = parseAlanalysis();      // 递归下降语法分析

    // 处理结果
    printf("\n\n=============语法分析结果=====================\n\n");
    if (errorCount == 0) {
        printf("语法分析成功\n");
        printTree(root, 0, outputFile);
    }
    else {
        printf("语法分析失败，发现 %d 个错误:\n", errorCount);
        for (int i = 0; i < errorCount; i++) {
            int code = errorList[i].code;
            int lineNo = errorList[i].line;
            switch (code) { // 根据错误代码选择具体错误信息
            case 1: printf("第%d行缺少 { \n", lineNo); break;
            case 2: printf("第%d行缺少 } \n", lineNo); break;
            case 3: printf("第%d行缺少 ; \n", lineNo-1); break;
            case 4: printf("第%d行缺少 ( \n", lineNo); break;
            case 5: printf("第%d行缺少 ) \n", lineNo); break;
            case 6: printf("第%d行缺少标识符\n", lineNo); break;
            case 7: printf("第%d行缺少操作数\n", lineNo); break;
            case 8: printf("无法打开输入文件%s\n", inputAddress);  break;
            case 9: printf("无法打开输出文件%s\n", outputAddress); break;
            case 10: printf("第%d行缺少参数\n", lineNo);  break;
            case 11: printf("第%d行缺少保留字\n", lineNo);  break;
            case 21: printf("第%d行符号表溢出\n", lineNo);  break;
            case 22: printf("第%d行变量同名\n", lineNo);  break;
            case 23: printf("第%d行变量未定义\n", lineNo); break;
            case 32: printf("第%d行函数重定义\n", lineNo); break;
            case 34: printf("第%d行函数未定义\n", lineNo); break; // 添加缺失的错误代码
            case 35: printf("第%d行标识符不是变量\n", lineNo); break; // 添加缺失的错误代码
            default: printf("第%d行未知错误\n", lineNo); break;
            }
        }

        // 跳转到文件末尾并返回
        skip(); // 跳转到文件末尾
        return 0;
    }

    // 如果没有错误，继续生成中间代码
    if ((fpCodeText = fopen(Codeout, "w")) == NULL) {
        printf("\n创建%s错误!\n", Codeout);
        es = 10;
        return(es);
    }

    for (int i = 0; i < codesIndex; i++) {
        if (strcmp(codes[i].opt, "LOAD") == 0 || strcmp(codes[i].opt, "LOADI") == 0 || strcmp(codes[i].opt, "STO") == 0 ||
            strcmp(codes[i].opt, "BR") == 0 || strcmp(codes[i].opt, "BRF") == 0 || strcmp(codes[i].opt, "CAL") == 0 || strcmp(codes[i].opt, "ENTER") == 0)
            fprintf(fpCodeText, " %3d %-5s %d\n", i, codes[i].opt, codes[i].operand);
        else
            fprintf(fpCodeText, " %3d %-5s\n", i, codes[i].opt);
    }
    fclose(fpCodeText);
    // 关闭文件
    if (inputFile != NULL) fclose(inputFile);
    if (outputFile != NULL) fclose(outputFile);

    // 释放语法树内存
    freeTree(root);

    return 0;
}
