#define _CRT_SECURE_NO_WARNINGS 
#define _CRT_NONSTDC_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#define maxsymbolIndex 100 // ������ű������   
#define MAX_CODES 200      // Maximum number of intermediate codes
#define MAX_ERRORS 100     // ��������

enum Category_symbol { variable, function }; // ��־��������

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
int index_array[100]; // ������Ϊ index_array �Ա�����ؼ��ֳ�ͻ

// �������ṹ�ʹ�������
typedef struct {
    int code;   // �������
    int line;   // ������к�
} Error;

Error errorList[MAX_ERRORS];
int errorCount = 0;

// ����һ���½ڵ�
Tree* createNode(const char* str, int lineNo, const char* extraInfo) {
    Tree* newNode = (Tree*)malloc(sizeof(Tree));
    if (!newNode) {
        perror("Failed to allocate memory for new node");
        exit(EXIT_FAILURE);
    }
    newNode->data = strdup(str); // �����ַ���
    newNode->lineNo = lineNo;
    if (extraInfo)
        newNode->extraInfo = strdup(extraInfo);
    else
        newNode->extraInfo = NULL;
    newNode->firstChild = NULL;
    newNode->nextSibling = NULL;
    return newNode;
}

// ��ӷ��ս�ڵ�
Tree* addNonTerminal(Tree* parent, const char* str, int lineNo) {
    Tree* newSon = createNode(str, lineNo, NULL); // ʹ�ô�����к�
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

// ����ս�ڵ�
Tree* addTerminal(Tree* parent, const char* str, const char* extraInfo) {
    Tree* newSon = createNode(str, 0, extraInfo); // lineNo Ϊ 0 ��ʾ�ս�ڵ�
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

// ��ӡ�﷨��
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
FILE* fpCodeText;//E:\Code\����ԭ��\yuyi.c
char inputAddress[100] = "E:\\Code\\����ԭ��\\321.txt";
char outputAddress[100] = "E:\\Code\\����ԭ��\\3.txt";
char Codeout[100] = "E:\\Code\\����ԭ��\\4.txt";
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

// ͬ������
void skip() {
    while (strcmp(type, ";") != 0 &&
        strcmp(type, "}") != 0 &&
        strcmp(type, "EOF") != 0) {
        getToken();
    }
    if (strcmp(type, ";") == 0 || strcmp(type, "}") == 0) {
        getToken(); // ����ͬ������
    }
}

// �������ݿ�������һ����ȡ
//int declaration_stat(Tree* parent); // ǰ������

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

// ����ǰ���ݴ����﷨����
// ����﷨��

int insert_Symbol(enum Category_symbol category, char* name)
{
    int i, es = 0;

    if (symbolIndex >= maxsymbolIndex) return(21); // ���ű���� 
    switch (category) {
    case function:
        for (i = symbolIndex - 1; i >= 0; i--) { // ����ű�
            if ((strcmp(symbol[i].name, name) == 0) && (symbol[i].kind == function)) {
                logError(32, line); // ȱ�� '('
                break;
            }
        }
        symbol[symbolIndex].kind = function;
        symbol[symbolIndex].address = offset;
        offset++; // ����һ����Ԫ��������ָ���
        break;
    case variable:
        for (i = symbolIndex - 1; i >= 0; i--) { // ����ű�
            if ((strcmp(symbol[i].name, name) == 0) && (symbol[i].kind == variable)) {
               // es = 22; // 22��ʾ�����ظ�����
                logError(22, line); // ȱ�� '('
                break;
            }
        }
        symbol[symbolIndex].kind = variable;
        symbol[symbolIndex].address = offset;
        offset++; // ����һ����Ԫ��������ָ���
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
   // logError(23, line); // ȱ�� '('
    *pPosition = 999;
    return 1;
}

int statement_list(Tree* parent) {
    int es = 0;
    Tree* current = addNonTerminal(parent, "<statement_list>", line);

    while (1) {
        // ��������� '}' �� 'EOF' ����
        if (strcmp("}", type) == 0 || strcmp("EOF", type) == 0) {
            break; // ����б����
        }

        es = statement(current);
        if (es > 0) {
            //logError(es, line);
           // synchronize();
            // ��������
            return es;
        }
    }

    return es;
}

// �������
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
      else if (strcmp("do", type) == 0) {   //do_while���
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

    // ��� "if" �ս��
    addTerminal(current, type, value);
    getToken(); // ��ȡ��һ�� token

    // ��� '('
    if (strcmp("(", type) != 0) {
        logError(4, line); // ������룺ȱ�� '('
        skip();
        return 0; // ��������
    }
    addTerminal(current, type, value);
    getToken(); // ��ȡ��һ�� token

    // ���� <bool_expr>
    es = bool_expr(current);
    if (es > 0) {
        return es;
    }

    if (strcmp(")", type) != 0) {
        logError(5, line); // ������룺ȱ�� ')'
        skip();
        return 0;
    }

    strcpy(codes[codesIndex].opt, "BRF");
    cx1 = codesIndex++;

    addTerminal(current, type, value);
    getToken(); // ��ȡ��һ�� token

    // ���� <statement>
    es = statement(current);
    if (es > 0) {
        return es;
    }

    strcpy(codes[codesIndex].opt, "BR");
    cx2 = codesIndex++;
    codes[cx1].operand = codesIndex;

    // ��ѡ�� 'else' ����
    if (strcmp("else", type) == 0) {
        addTerminal(current, type, value);
        getToken(); // ��ȡ��һ�� token
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

    // ��� "while" �ս��
    addTerminal(current, type, value);
    getToken(); // ��ȡ��һ�� token

    // ��� '('
    if (strcmp("(", type) != 0) {
        logError(4, line); // ȱ�� '('
        skip();
        return 0;
    }

    addTerminal(current, type, value);
    getToken(); // ��ȡ��һ�� token

    // ���� <bool_expr>
    es = bool_expr(current);
    if (es > 0) {
        return es;
    }

    // ��� ')'
    if (strcmp(")", type) != 0) {
        logError(5, line); // ȱ�� ')'
        skip();
        return 0;
    }
    addTerminal(current, type, value);

    strcpy(codes[codesIndex].opt, "BRF");
    cx = codesIndex++;

    getToken(); // ��ȡ��һ�� token

    // ���� <statement>
    es = statement(current);
    if (es > 0) {
        return es;
    }

    strcpy(codes[codesIndex].opt, "BR");
    codes[codesIndex].operand = entrance; // �ص��ж����
    codesIndex++;
    codes[cx].operand = codesIndex;

    return es;
}


// <do_while_stat> ::= do <statement> while '(' <bool_expr> ')' ';'
int do_while_stat(Tree* parent) {
    int es = 0, entrance, cx;
    Tree* current = addNonTerminal(parent, "<do_while_stat>", line);
 
    // ��鲢��� "do" �ؼ���
    if (strcmp("do", type) != 0) {
        logError(11, line); // ȱ�ٱ����� "do"
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // ��ȡ��һ�� token

    // ��¼ѭ����ʼ��λ��
    entrance = codesIndex;

    // ���� <statement>
    es = statement(current);
    if (es > 0) {
        return es;
    }

    // ��鲢��� "while" �ؼ���
    if (strcmp("while", type) != 0) {
        logError(11, line); // ȱ�ٱ����� "while"
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // ��ȡ��һ�� token

    // ��� '('
    if (strcmp("(", type) != 0) {
        logError(4, line); // ȱ�� '('
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // ��ȡ��һ�� token

    // ���� <bool_expr>
    es = bool_expr(current);
    if (es > 0) {
        return es;
    }

    // ��� ')'
    if (strcmp(")", type) != 0) {
        logError(5, line); // ȱ�� ')'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // ��ȡ��һ�� token

    // ��� ';'
    if (strcmp(";", type) != 0) {
        logError(3, line); // ȱ�� ';'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // ��ȡ��һ�� token

    // ���ɴ��룺�����жϣ���Ϊ�����������
    strcpy(codes[codesIndex].opt, "BRF"); // �ж�����Ϊ��������ѭ��
    cx = codesIndex++;
    strcpy(codes[codesIndex].opt, "BR"); // �ж�����Ϊ�����������
    codes[codesIndex].operand = entrance;
    codesIndex++;

    // �������ѭ���ĵ�ַ
    codes[cx].operand = codesIndex;

    return es;
}

// <for_stat> ::= for '(' <expression> ';' <bool_expr> ';' <expression> ')' <statement>
int for_stat(Tree* parent) {
    int es = 0, cx1, cx2, entrance1, entrance2;
    Tree* current = addNonTerminal(parent, "<for_stat>", line);

    // ��� "for" �ս��
    addTerminal(current, type, value);
    getToken(); // ��ȡ��һ�� token

    // ��� '('
    if (strcmp("(", type) != 0) {
        logError(4, line); // ȱ�� '('
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // ��ȡ��һ�� token

    // ���� <expression>
    es = expression(current);
    if (es > 0) {
        return es;
    }

    // ��� ';'
    if (strcmp(";", type) != 0) {
        logError(3, line); // ȱ�� ';'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    entrance1 = codesIndex;
    getToken(); // ��ȡ��һ�� token

    // ���� <bool_expr>
    es = bool_expr(current);
    if (es > 0) {
        return es;
    }

    // ��� ';'
    if (strcmp(";", type) != 0) {
        logError(3, line); // ȱ�� ';'
        skip();
        return 0;
    }
    addTerminal(current, type, value);

    strcpy(codes[codesIndex].opt, "BRF");    // �ж�Ϊ�پͽ��� for
    cx1 = codesIndex++;                      // ��ʱ��֪������λ�ã��ȴ�����
    strcpy(codes[codesIndex].opt, "BR");     // �ж�Ϊ��ͽ���ѭ�����
    cx2 = codesIndex++;                      // ��ʱ��֪����俪ʼλ�ã��ȴ�����

    getToken(); // ��ȡ��һ�� token
    entrance2 = codesIndex;

    // ���� <expression>
    es = expression(current);
    if (es > 0) {
        return es;
    }

    // ��� ')'
    if (strcmp(")", type) != 0) {
        logError(5, line); // ȱ�� ')'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    strcpy(codes[codesIndex].opt, "BR");    // ִ����ͽ����ж�
    codes[codesIndex].operand = entrance1;   // ��ת��λ�����ж��������
    codesIndex++;
    getToken(); // ��ȡ��һ�� token

    codes[cx2].operand = codesIndex;
    // ���� <statement>
    es = statement(current);
    if (es > 0)    return es;

    strcpy(codes[codesIndex].opt, "BR");    // ִ����ͽ�������3
    codes[codesIndex].operand = entrance2;   // ��ת��λ��������3�����
    codesIndex++;
    codes[cx1].operand = codesIndex;

    return es;
}

// <write_stat> ::= write '(' <expression> ')' ';'
int write_stat(Tree* parent) {
    int es = 0;
    Tree* current = addNonTerminal(parent, "<write_stat>", line);

    // ��� "write" �ս��
    addTerminal(current, type, value);
    getToken(); // ��ȡ��һ�� token

    // ���� '('
    // ���� <expression>
    es = expression(current);
    if (es > 0) {
        return es;
    }

    // ��� ';'
    if (strcmp(";", type) != 0) {
        logError(3, line); // ȱ�� ';'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    strcpy(codes[codesIndex].opt, "OUT");     // ���
    codesIndex++;
    getToken(); // ��ȡ��һ�� token

    return es;
}

// <read_stat> ::= read '(' ID ')' ';'
int read_stat(Tree* parent) {
    int symbolPos, es = 0;
    Tree* current = addNonTerminal(parent, "<read_stat>", line);
    addTerminal(current, type, value);
    getToken(); // ��ȡ��һ�� token

    // ��� ID
    if (strcmp("ID", type) != 0) {
        logError(6, line); // ȱ�ٱ�ʶ��
        skip();
        return 0;
    }
    addTerminal(current, type, value); // ��� ID ����ֵ
    if(lookup(value, &symbolPos))  logError(23, line);
    if (symbol[symbolPos].kind != variable) {
        logError(35, line); // ���� 35 ��ʾ�Ǳ�������
        skip();
        return 0;
    }

    strcpy(codes[codesIndex].opt, "IN");
    codesIndex++;
    strcpy(codes[codesIndex].opt, "STO");
    codes[codesIndex].operand = symbol[symbolPos].address;
    codesIndex++;
    getToken(); // ��ȡ��һ�� token

    // ��� ';'
    if (strcmp(";", type) != 0) {
        logError(3, line); // ȱ�� ';'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // ��ȡ��һ�� token

    return es;
}

// <compound_stat> ::= '{' <statement_list> '}'
int compound_stat(Tree* parent) {
    int es = 0;
    Tree* current = addNonTerminal(parent, "<compound_stat>", line);

    // ��� '{'
    if (strcmp("{", type) != 0) {   // ȱ�� '{'
        logError(1, line); // ������룺ȱ�� '{'
        skip();
        return 0;
    }
    addTerminal(current, type, value); // �� '{' ��ӵ��﷨����
    getToken(); // ��ȡ��һ�� token

    // ���� <statement_list>
    es = statement_list(current);
    if (es > 0) {
        return es;
    }

    // ��� '}'
    if (strcmp("}", type) != 0) {   // ȱ�� '}'
        logError(2, line); // ������룺ȱ�� '}'
        skip();
        return 0;
    }
    addTerminal(current, type, value); // �� '}' ��ӵ��﷨����
    getToken(); // ��ȡ��һ�� token

    return es;
}

// <expression_stat> ::= <expression> ';' | ';'
int expression_stat(Tree* parent) {
    int es = 0;
    Tree* current = addNonTerminal(parent, "<expression_stat>", line);

    if (strcmp(";", type) == 0) {
        // �ձ��ʽ���
        addTerminal(current, type, value);
        getToken(); // ��ȡ��һ�� token
        return es;
    }

    // ���� <expression>
    es = expression(current);
    if (es > 0) {
        return es;
    }

    // ��� ';'
    if (strcmp(";", type) != 0) {
        logError(3, line); // ������룺ȱ�� ';'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // ��ȡ��һ�� token

    return es;
}

// <call_stat> ::= call ID '(' ')' ';'
int call_stat(Tree* parent) {
    int es = 0, symbolPos;
    Tree* current = addNonTerminal(parent, "<call_stat>", line);
    addTerminal(current, type, value);
    getToken(); // ��ȡ��һ�� token

    // ��� ID����������
    if (strcmp("ID", type) != 0) {
        logError(6, line); // ȱ�ٱ�ʶ��
        skip();
        return 0;
    }
    if(lookup(value, &symbolPos))   logError(34, line);

    if (symbol[symbolPos].kind != function && !lookup(value, &symbolPos)) {
        logError(34, line); // ���� 34 ��ʾ�Ǻ�������
        skip();
        return 0;
    }

    addTerminal(current, type, value); // ��Ӻ�����
    getToken(); // ��ȡ��һ�� token

    // ��� '('
    if (strcmp("(", type) != 0) {
        logError(4, line); // ȱ�� '('
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // ��ȡ��һ�� token

    // ��� ')'
    if (strcmp(")", type) != 0) {
        logError(5, line); // ȱ�� ')'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // ��ȡ��һ�� token

    // ��� ';'
    if (strcmp(";", type) != 0) {
        logError(3, line); // ȱ�� ';'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    strcpy(codes[codesIndex].opt, "CAL");     // ���ú���
    codes[codesIndex].operand = symbol[symbolPos].address;     // ���뺯���Ŀ�ʼλ��
    codesIndex++;
    getToken(); // ��ȡ��һ�� token

    return es;
}

// <expression> ::= ID '=' <bool_expr> | <bool_expr>
int expression(Tree* parent) {
    int es = 0;
    Tree* current = addNonTerminal(parent, "<expression>", line);
    char type1[20];     // ��ʱ��������
    char value1[40];    // ��ʱ����ֵ
    long curPoint;
    int place;

    // �˴����������ݿ����� ID
    if (strcmp("ID", type) == 0) {   //<expression> ::= ID '=' <bool_expr>
        // ���浱ǰ�ļ�ָ����к�
        curPoint = ftell(inputFile);
        // ��ȡ��һ�� token
        strcpy(type1, type);
        strcpy(value1, value);
        getToken(); // ��ȡ��һ�� token

        // ����Ƿ�Ϊ '='
        if (strcmp("=", type) == 0) {    // �ñ��ʽ���� a=xxxx
            if(lookup(value1, &place)) logError(23, line);
            addTerminal(current, type, value);
            getToken(); // ��ȡ��һ�� token

            // ���� <bool_expr>
            es = bool_expr(current);
            if (es > 0) {
                return es;
            }

            strcpy(codes[codesIndex].opt, "STO");
            codes[codesIndex++].operand = symbol[place].address;
        }
        else {                      //<bool_expr>
            // ���˵� '=' ǰ��λ��
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
        // ���� <bool_expr>
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

    // ������һ�� <additive_expr>
    es = additive_expr(current);
    if (es > 0) {
        return es;
    }

    // ���������Ĺ�ϵ������� <additive_expr>
    while (strcmp(">", type) == 0 || strcmp("<", type) == 0 ||
        strcmp(">=", type) == 0 || strcmp("<=", type) == 0 ||
        strcmp("==", type) == 0 || strcmp("!=", type) == 0) {
        char token2[20];
        strcpy(token2, type); // ���������
        addTerminal(current, type, value); // ��ӹ�ϵ�����
        getToken(); // ��ȡ��һ�� token

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

    // ������һ�� <term>
    es = term(current);
    if (es > 0) {
        return es;
    }

    // ���������� '+' �� '-' �� <term>
    while (strcmp("+", type) == 0 || strcmp("-", type) == 0) {
        char token2[20];
        strcpy(token2, type);
        addTerminal(current, type, value); // ��� '+' �� '-'
        getToken(); // ��ȡ��һ�� token

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

    // ������һ�� <factor>
    es = factor(current);
    if (es > 0) {
        return es;
    }

    // ���������� '*' �� '/' �� <factor>
    while (strcmp("*", type) == 0 || strcmp("/", type) == 0) {
        char token2[20];
        strcpy(token2, type);
        addTerminal(current, type, value); // ��� '*' �� '/'
        getToken(); // ��ȡ��һ�� token

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
        getToken(); // ��ȡ��һ�� token

        // ���� <additive_expr>
        es = additive_expr(current);
        if (es > 0) {
            return es;
        }

        if (strcmp(")", type) != 0) {
            logError(5, line); // ȱ�� ')'
            skip();
            return 0;
        }
        addTerminal(current, type, value);
        getToken(); // ��ȡ��һ�� token
    }
    else if (strcmp("ID", type) == 0) {
        if(lookup(value, &symbolPos))   logError(23, line);
        strcpy(codes[codesIndex].opt, "LOAD");
        codes[codesIndex++].operand = symbol[symbolPos].address;

        addTerminal(current, type, value); // ��� ID
        getToken(); // ��ȡ��һ�� token
    }
    else if (strcmp("NUM", type) == 0) {
        strcpy(codes[codesIndex].opt, "LOADI");
        codes[codesIndex++].operand = atoi(value);
        addTerminal(current, type, value); // ��� NUM
        getToken(); // ��ȡ��һ�� token
    }
    else {
        logError(7, line); // ������룺δʶ�������
        skip();
        return 0;
    }

    return es;
}

// <declaration_stat>::= int ID ';' | int ID '=' NUM ';'
int declaration_stat(Tree* parent) {
    int es = 0;
    Tree* current = addNonTerminal(parent, "<declaration_stat>", line);
    // ��ǰ token �� "int"
    addTerminal(current, type, value);

    getToken(); // ��ȡ��һ�� token
    // ��ȡ��һ�� token��Ӧ���� ID
    if (strcmp("ID", type) != 0) {
        logError(6, line); // ȱ�ٱ�ʶ��
        skip();
        return 0;
    }
    addTerminal(current, type, value); // ��ӱ��������﷨����

    es = insert_Symbol(variable, value); // ������ű�

    getToken(); // ��ȡ��һ�� token

    // ��ȡ��һ�� token�������� ';' �� '='
    if (strcmp(";", type) == 0) {
        addTerminal(current, type, value);
        getToken(); // ��ȡ��һ�� token
    }
    else if (strcmp("=", type) == 0) {
        addTerminal(current, type, value);
        getToken(); // ��ȡ��һ�� token

        // ��ֵ��ֵӦ���� NUM
        if (strcmp("NUM", type) != 0) {
            logError(7, line); // ������룺ȱ�ٲ�����
            skip();
            return 0;
        }
        addTerminal(current, type, value); // ��� NUM ���﷨����
        getToken(); // ��ȡ��һ�� token

        // ��� ';'
        if (strcmp(";", type) != 0) {
            logError(3, line); // ������룺ȱ�� ';'
            skip();
            return 0;
        }
        addTerminal(current, type, value);
        getToken(); // ��ȡ��һ�� token
    }
    else {
        logError(3, line); // ������룺ȱ�� ';'
        skip();
        return 0;
    }
    return es;
}

// ��������
int declaration_list(Tree* parent) {
    int es = 0;
    Tree* current = addNonTerminal(parent, "<declaration_list>", line);

    while (1) {
        if (strcmp("int", type) != 0) { // ���� "int"��˵��û�и�������
            break;
        }
        es = declaration_stat(current); // ��������
        if (es > 0) {
            return es;
        }
    }
    return es;
}

// ������
// <function_body> ::= '{' <declaration_list> <statement_list> '}'
int function_body(Tree* parent) {
    int es = 0;

    // ��� <function_body> �ڵ㵽�﷨��
    Tree* current = addNonTerminal(parent, "<function_body>", line);

    // ��� '{'
    if (strcmp("{", type) != 0) {   // ȱ�� '{'
        logError(1, line); // ������룺ȱ�� '{'
        skip();
        return 0;
    }
    addTerminal(current, type, value); // �� '{' ��ӵ��﷨����
    getToken(); // ��ȡ��һ�� token

    // ���� <declaration_list>
    es = declaration_list(current);
    if (es > 0) {
        return es;
    }
    strcpy(codes[codesIndex].opt, "ENTER");  // Ϊ�������ÿ��ٿռ�
    codes[codesIndex].operand = offset - offsetF + 2;
    codesIndex++;
    // ���� <statement_list>
    es = statement_list(current);
    if (es > 0) {
        return es;
    }
    strcpy(codes[codesIndex].opt, "RETURN");  // ��������
    codesIndex++;
    // ��� '}'
    if (strcmp("}", type) != 0) {   // ȱ�� '}'
        logError(2, line); // ������룺ȱ�� '}'
        skip();
        return 0;
    }
    addTerminal(current, type, value); // �� '}' ��ӵ��﷨����
    getToken(); // ��ȡ��һ�� token

    return es;
}

// ������
int main_declaration(Tree* parent) { // <main_declaration>::= main '(' ')' <function_body>
    int es = 0;
    char name_main[10] = "main";
    es = insert_Symbol(function, name_main);

    // ��� <main_declaration> �ڵ�
    Tree* current = addNonTerminal(parent, "<main_declaration>", line);

    // ��ǰ token �� "main"
    addTerminal(current, type, value); // �� "main" ��ӵ��﷨����
    getToken(); // ��ȡ��һ�� token

    // ��� '('
    if (strcmp("(", type) != 0) {
        logError(4, line); // ȱ�� '('
        skip();
        return 0;
    }
    addTerminal(current, type, value); // ��� '(' ���﷨����
    getToken(); // ��ȡ��һ�� token

    // ��� ')'
    if (strcmp(")", type) != 0) {
        logError(5, line); // ȱ�� ')'
        skip();
        return 0;
    }
    addTerminal(current, type, value); // ��� ')' ���﷨����
    codes[0].operand = codesIndex;
    offset = 2;        // ÿ�ν���һ���������ʱ����Ե�ַ��Ҫ����Ϊ2
    offsetF = offset;
    symbol[symbolIndex - 1].address = codesIndex;
    getToken(); // ��ȡ��һ�� token

    // ����������
    es = function_body(current);
    return es;
}

int fun_declaration(Tree* parent) { // <fun_declaration>::= function ID '(' [�����б�] ')' <function_body>
    int es = 0;
    // ��� <fun_declaration> �ڵ�
    Tree* current = addNonTerminal(parent, "<fun_declaration>", line);

    addTerminal(current, type, value); // �� "function" ��ӵ��﷨����
    getToken(); // ��ȡ��һ�� token

    // ��� ID
    if (strcmp("ID", type) != 0) {
        logError(6, line); // ȱ�ٱ�ʶ��
        skip();
        return 0;
    }
    es = insert_Symbol(function, value); // ��������������ű�
    offset = 2;        // ÿ�ν���һ���������ʱ����Ե�ַ��Ҫ����Ϊ2
    offsetF = offset;
    symbol[symbolIndex - 1].address = codesIndex;
    int fPos = codesIndex; // ��������λ��
    addTerminal(current, type, value); // ��Ӻ��������﷨����

    getToken(); // ��ȡ��һ�� token

    // ��� '('
    if (strcmp("(", type) != 0) {
        logError(4, line); // ȱ�� '('
        skip();
        return 0;
    }
    addTerminal(current, type, value); // ��� '(' ���﷨����
    getToken(); // ��ȡ��һ�� token

    // ��� ')'
    if (strcmp(")", type) != 0) {
        logError(5, line); // ȱ�� ')'
        skip();
        return 0;
    }
    addTerminal(current, type, value); // ��� ')' ���﷨����
    getToken(); // ��ȡ��һ�� token

    // ����������
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
        logError(6, line); // ȱ�� main ����
    }
    printf("     ���ű�\n");
    printf(" ����\t \t���� \t��ַ\n");
    for (int i = 0; i < symbolIndex; i++)
        printf("  %-8s \t%d \t%d\n", symbol[i].name, symbol[i].kind, symbol[i].address);
    return es;
}

int parseAlanalysis() {

    // �������ļ�
    inputFile = fopen(inputAddress, "r");
    if (inputFile == NULL) {
        logError(8, line); // �޷��������ļ�
        return 8;
    }

    // ������ļ�
    outputFile = fopen(outputAddress, "w");
    if (outputFile == NULL) {
        logError(9, line); // �޷�������ļ�
        fclose(inputFile);
        return 9;
    }

    // ��ʼ�����ڵ�
    root = createNode("<program>", 0, NULL);

    // ��ʼ�﷨����
    int es = program(); // ��Ҫʵ�� program ����

    return es;
}

int main() {
    int es = 0;
    es = parseAlanalysis();      // �ݹ��½��﷨����

    // ������
    printf("\n\n=============�﷨�������=====================\n\n");
    if (errorCount == 0) {
        printf("�﷨�����ɹ�\n");
        printTree(root, 0, outputFile);
    }
    else {
        printf("�﷨����ʧ�ܣ����� %d ������:\n", errorCount);
        for (int i = 0; i < errorCount; i++) {
            int code = errorList[i].code;
            int lineNo = errorList[i].line;
            switch (code) { // ���ݴ������ѡ����������Ϣ
            case 1: printf("��%d��ȱ�� { \n", lineNo); break;
            case 2: printf("��%d��ȱ�� } \n", lineNo); break;
            case 3: printf("��%d��ȱ�� ; \n", lineNo-1); break;
            case 4: printf("��%d��ȱ�� ( \n", lineNo); break;
            case 5: printf("��%d��ȱ�� ) \n", lineNo); break;
            case 6: printf("��%d��ȱ�ٱ�ʶ��\n", lineNo); break;
            case 7: printf("��%d��ȱ�ٲ�����\n", lineNo); break;
            case 8: printf("�޷��������ļ�%s\n", inputAddress);  break;
            case 9: printf("�޷�������ļ�%s\n", outputAddress); break;
            case 10: printf("��%d��ȱ�ٲ���\n", lineNo);  break;
            case 11: printf("��%d��ȱ�ٱ�����\n", lineNo);  break;
            case 21: printf("��%d�з��ű����\n", lineNo);  break;
            case 22: printf("��%d�б���ͬ��\n", lineNo);  break;
            case 23: printf("��%d�б���δ����\n", lineNo); break;
            case 32: printf("��%d�к����ض���\n", lineNo); break;
            case 34: printf("��%d�к���δ����\n", lineNo); break; // ���ȱʧ�Ĵ������
            case 35: printf("��%d�б�ʶ�����Ǳ���\n", lineNo); break; // ���ȱʧ�Ĵ������
            default: printf("��%d��δ֪����\n", lineNo); break;
            }
        }

        // ��ת���ļ�ĩβ������
        skip(); // ��ת���ļ�ĩβ
        return 0;
    }

    // ���û�д��󣬼��������м����
    if ((fpCodeText = fopen(Codeout, "w")) == NULL) {
        printf("\n����%s����!\n", Codeout);
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
    // �ر��ļ�
    if (inputFile != NULL) fclose(inputFile);
    if (outputFile != NULL) fclose(outputFile);

    // �ͷ��﷨���ڴ�
    freeTree(root);

    return 0;
}
