#define _CRT_SECURE_NO_WARNINGS 
#define _CRT_NONSTDC_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#define maxsymbolIndex 100 // 瀹氫箟绗﹀彿琛ㄧ殑瀹归噺   
#define MAX_CODES 200      // Maximum number of intermediate codes
#define MAX_ERRORS 100     // 鏈€澶ч敊璇暟

enum Category_symbol { variable, function }; // 鏍囧織绗︾殑绫诲瀷

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
int index_array[100]; // 閲嶅懡鍚嶄负 index_array 浠ラ伩鍏嶄笌鍏抽敭瀛楀啿绐?

// 瀹氫箟閿欒缁撴瀯鍜岄敊璇暟缁?
typedef struct {
    int code;   // 閿欒浠ｇ爜
    int line;   // 鍑洪敊鐨勮鍙?
} Error;

Error errorList[MAX_ERRORS];
int errorCount = 0;

// 鍒涘缓涓€涓柊鑺傜偣
Tree* createNode(const char* str, int lineNo, const char* extraInfo) {
    Tree* newNode = (Tree*)malloc(sizeof(Tree));
    if (!newNode) {
        perror("Failed to allocate memory for new node");
        exit(EXIT_FAILURE);
    }
    newNode->data = strdup(str); // 澶嶅埗瀛楃涓?
    newNode->lineNo = lineNo;
    if (extraInfo)
        newNode->extraInfo = strdup(extraInfo);
    else
        newNode->extraInfo = NULL;
    newNode->firstChild = NULL;
    newNode->nextSibling = NULL;
    return newNode;
}

// 娣诲姞闈炵粓缁撹妭鐐?
Tree* addNonTerminal(Tree* parent, const char* str, int lineNo) {
    Tree* newSon = createNode(str, lineNo, NULL); // 浣跨敤浼犲叆鐨勮鍙?
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

// 娣诲姞缁堢粨鑺傜偣
Tree* addTerminal(Tree* parent, const char* str, const char* extraInfo) {
    Tree* newSon = createNode(str, 0, extraInfo); // lineNo 涓?0 琛ㄧず缁堢粨鑺傜偣
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

// 鎵撳嵃璇硶鏍?
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
FILE* fpCodeText;
char inputAddress[100] = "D:\\code\\C\\compile\\new\\321.txt";
char outputAddress[100] = "D:\\code\\C\\compile\\new\\3.txt";
char Codeout[100] = "D:\\code\\C\\compile\\new\\4.txt";
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

// 鍚屾鍑芥暟
void skip() {
    while (strcmp(type, ";") != 0 &&
        strcmp(type, "}") != 0 &&
        strcmp(type, "EOF") != 0) {
        getToken();
    }
    if (strcmp(type, ";") == 0 || strcmp(type, "}") == 0) {
        getToken(); // 璺宠繃鍚屾绗﹀彿
    }
}

// 澹版槑鍐呭鍙兘灏戜簡涓€涓鍙?
//int declaration_stat(Tree* parent); // 鍓嶅悜澹版槑

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

// 灏嗗綋鍓嶆暟鎹瓨鍏ヨ娉曟爲涓?
// 杈撳嚭璇硶鏍?

int insert_Symbol(enum Category_symbol category, char* name)
{
    int i, es = 0;

    if (symbolIndex >= maxsymbolIndex) return(21); // 绗﹀彿琛ㄦ孩鍑?
    switch (category) {
    case function:
        for (i = symbolIndex - 1; i >= 0; i--) { // 鏌ョ鍙疯〃
            if ((strcmp(symbol[i].name, name) == 0) && (symbol[i].kind == function)) {
                logError(32, line); // 缂哄皯 '('
                break;
            }
        }
        symbol[symbolIndex].kind = function;
        symbol[symbolIndex].address = offset;
        offset++; // 鍒嗛厤涓€涓崟鍏冿紝鏁版嵁鍖烘寚閽堝姞
        break;
    case variable:
        for (i = symbolIndex - 1; i >= 0; i--) { // 鏌ョ鍙疯〃
            if ((strcmp(symbol[i].name, name) == 0) && (symbol[i].kind == variable)) {
               // es = 22; // 22琛ㄧず鍙橀噺閲嶅瀹氫箟
                logError(22, line); // 缂哄皯 '('
                break;
            }
        }
        symbol[symbolIndex].kind = variable;
        symbol[symbolIndex].address = offset;
        offset++; // 鍒嗛厤涓€涓崟鍏冿紝鏁版嵁鍖烘寚閽堝姞
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
   // logError(23, line); // 缂哄皯 '('
    *pPosition = 999;
    return 1;
}

int statement_list(Tree* parent) {
    int es = 0;
    Tree* current = addNonTerminal(parent, "<statement_list>", line);

    while (1) {
        // 鍋囪璇彞浠?'}' 鎴?'EOF' 缁撴潫
        if (strcmp("}", type) == 0 || strcmp("EOF", type) == 0) {
            break; // 璇彞鍒楄〃缁撴潫
        }

        es = statement(current);
        if (es > 0) {
            //logError(es, line);
           // synchronize();
            // 缁х画瑙ｆ瀽
            return es;
        }
    }

    return es;
}

// 璇彞鍐呭
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
      else if (strcmp("do", type) == 0) {   //do_while璇彞
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

    // 娣诲姞 "if" 缁堢粨绗?
    addTerminal(current, type, value);
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 妫€鏌?'('
    if (strcmp("(", type) != 0) {
        logError(4, line); // 閿欒浠ｇ爜锛氱己灏?'('
        skip();
        return 0; // 缁х画瑙ｆ瀽
    }
    addTerminal(current, type, value);
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 瑙ｆ瀽 <bool_expr>
    es = bool_expr(current);
    if (es > 0) {
        return es;
    }

    if (strcmp(")", type) != 0) {
        logError(5, line); // 閿欒浠ｇ爜锛氱己灏?')'
        skip();
        return 0;
    }

    strcpy(codes[codesIndex].opt, "BRF");
    cx1 = codesIndex++;

    addTerminal(current, type, value);
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 瑙ｆ瀽 <statement>
    es = statement(current);
    if (es > 0) {
        return es;
    }

    strcpy(codes[codesIndex].opt, "BR");
    cx2 = codesIndex++;
    codes[cx1].operand = codesIndex;

    // 鍙€夌殑 'else' 閮ㄥ垎
    if (strcmp("else", type) == 0) {
        addTerminal(current, type, value);
        getToken(); // 璇诲彇涓嬩竴涓?token
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

    // 娣诲姞 "while" 缁堢粨绗?
    addTerminal(current, type, value);
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 妫€鏌?'('
    if (strcmp("(", type) != 0) {
        logError(4, line); // 缂哄皯 '('
        skip();
        return 0;
    }

    addTerminal(current, type, value);
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 瑙ｆ瀽 <bool_expr>
    es = bool_expr(current);
    if (es > 0) {
        return es;
    }

    // 妫€鏌?')'
    if (strcmp(")", type) != 0) {
        logError(5, line); // 缂哄皯 ')'
        skip();
        return 0;
    }
    addTerminal(current, type, value);

    strcpy(codes[codesIndex].opt, "BRF");
    cx = codesIndex++;

    getToken(); // 璇诲彇涓嬩竴涓?token

    // 瑙ｆ瀽 <statement>
    es = statement(current);
    if (es > 0) {
        return es;
    }

    strcpy(codes[codesIndex].opt, "BR");
    codes[codesIndex].operand = entrance; // 鍥炲埌鍒ゆ柇璧风偣
    codesIndex++;
    codes[cx].operand = codesIndex;

    return es;
}


// <do_while_stat> ::= do <statement> while '(' <bool_expr> ')' ';'
int do_while_stat(Tree* parent) {
    int es = 0, entrance, cx;
    Tree* current = addNonTerminal(parent, "<do_while_stat>", line);
 
    // 妫€鏌ュ苟娣诲姞 "do" 鍏抽敭瀛?
    if (strcmp("do", type) != 0) {
        logError(11, line); // 缂哄皯淇濈暀瀛?"do"
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 璁板綍寰幆寮€濮嬬殑浣嶇疆
    entrance = codesIndex;

    // 瑙ｆ瀽 <statement>
    es = statement(current);
    if (es > 0) {
        return es;
    }

    // 妫€鏌ュ苟娣诲姞 "while" 鍏抽敭瀛?
    if (strcmp("while", type) != 0) {
        logError(11, line); // 缂哄皯淇濈暀瀛?"while"
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 妫€鏌?'('
    if (strcmp("(", type) != 0) {
        logError(4, line); // 缂哄皯 '('
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 瑙ｆ瀽 <bool_expr>
    es = bool_expr(current);
    if (es > 0) {
        return es;
    }

    // 妫€鏌?')'
    if (strcmp(")", type) != 0) {
        logError(5, line); // 缂哄皯 ')'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 妫€鏌?';'
    if (strcmp(";", type) != 0) {
        logError(3, line); // 缂哄皯 ';'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 鐢熸垚浠ｇ爜锛氭潯浠跺垽鏂紝鑻ヤ负鐪熷垯璺冲洖鍏ュ彛
    strcpy(codes[codesIndex].opt, "BRF"); // 鍒ゆ柇鏉′欢涓哄亣鍒欒烦鍑哄惊鐜?
    cx = codesIndex++;
    strcpy(codes[codesIndex].opt, "BR"); // 鍒ゆ柇鏉′欢涓虹湡鍒欒烦鍥炲叆鍙?
    codes[codesIndex].operand = entrance;
    codesIndex++;

    // 濉厖璺冲嚭寰幆鐨勫湴鍧€
    codes[cx].operand = codesIndex;

    return es;
}

// <for_stat> ::= for '(' <expression> ';' <bool_expr> ';' <expression> ')' <statement>
int for_stat(Tree* parent) {
    int es = 0, cx1, cx2, entrance1, entrance2;
    Tree* current = addNonTerminal(parent, "<for_stat>", line);

    // 娣诲姞 "for" 缁堢粨绗?
    addTerminal(current, type, value);
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 妫€鏌?'('
    if (strcmp("(", type) != 0) {
        logError(4, line); // 缂哄皯 '('
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 瑙ｆ瀽 <expression>
    es = expression(current);
    if (es > 0) {
        return es;
    }

    // 妫€鏌?';'
    if (strcmp(";", type) != 0) {
        logError(3, line); // 缂哄皯 ';'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    entrance1 = codesIndex;
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 瑙ｆ瀽 <bool_expr>
    es = bool_expr(current);
    if (es > 0) {
        return es;
    }

    // 妫€鏌?';'
    if (strcmp(";", type) != 0) {
        logError(3, line); // 缂哄皯 ';'
        skip();
        return 0;
    }
    addTerminal(current, type, value);

    strcpy(codes[codesIndex].opt, "BRF");    // 鍒ゆ柇涓哄亣灏辩粨鏉?for
    cx1 = codesIndex++;                      // 鏆傛椂涓嶇煡閬撶粨鏉熶綅缃紝绛夊緟鍙嶅～
    strcpy(codes[codesIndex].opt, "BR");     // 鍒ゆ柇涓虹湡灏辫繘鍏ュ惊鐜鍙?
    cx2 = codesIndex++;                      // 鏆傛椂涓嶇煡閬撹鍙ュ紑濮嬩綅缃紝绛夊緟鍙嶅～

    getToken(); // 璇诲彇涓嬩竴涓?token
    entrance2 = codesIndex;

    // 瑙ｆ瀽 <expression>
    es = expression(current);
    if (es > 0) {
        return es;
    }

    // 妫€鏌?')'
    if (strcmp(")", type) != 0) {
        logError(5, line); // 缂哄皯 ')'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    strcpy(codes[codesIndex].opt, "BR");    // 鎵ц瀹屽氨杩涘叆鍒ゆ柇
    codes[codesIndex].operand = entrance1;   // 璺宠浆鐨勪綅缃槸鍒ゆ柇璇彞鐨勫叆鍙?
    codesIndex++;
    getToken(); // 璇诲彇涓嬩竴涓?token

    codes[cx2].operand = codesIndex;
    // 瑙ｆ瀽 <statement>
    es = statement(current);
    if (es > 0)    return es;

    strcpy(codes[codesIndex].opt, "BR");    // 鎵ц瀹屽氨杩涘叆鏉′欢3
    codes[codesIndex].operand = entrance2;   // 璺宠浆鐨勪綅缃槸鏉′欢3鐨勫叆鍙?
    codesIndex++;
    codes[cx1].operand = codesIndex;

    return es;
}

// <write_stat> ::= write '(' <expression> ')' ';'
int write_stat(Tree* parent) {
    int es = 0;
    Tree* current = addNonTerminal(parent, "<write_stat>", line);

    // 娣诲姞 "write" 缁堢粨绗?
    addTerminal(current, type, value);
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 瑙ｆ瀽 '('
    // 瑙ｆ瀽 <expression>
    es = expression(current);
    if (es > 0) {
        return es;
    }

    // 妫€鏌?';'
    if (strcmp(";", type) != 0) {
        logError(3, line); // 缂哄皯 ';'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    strcpy(codes[codesIndex].opt, "OUT");     // 杈撳嚭
    codesIndex++;
    getToken(); // 璇诲彇涓嬩竴涓?token

    return es;
}

// <read_stat> ::= read '(' ID ')' ';'
int read_stat(Tree* parent) {
    int symbolPos, es = 0;
    Tree* current = addNonTerminal(parent, "<read_stat>", line);
    addTerminal(current, type, value);
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 妫€鏌?ID
    if (strcmp("ID", type) != 0) {
        logError(6, line); // 缂哄皯鏍囪瘑绗?
        skip();
        return 0;
    }
    addTerminal(current, type, value); // 娣诲姞 ID 鍜屽叾鍊?
    if(lookup(value, &symbolPos))  logError(23, line);
    if (symbol[symbolPos].kind != variable) {
        logError(35, line); // 鍋囪 35 琛ㄧず闈炲彉閲忕被鍨?
        skip();
        return 0;
    }

    strcpy(codes[codesIndex].opt, "IN");
    codesIndex++;
    strcpy(codes[codesIndex].opt, "STO");
    codes[codesIndex].operand = symbol[symbolPos].address;
    codesIndex++;
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 妫€鏌?';'
    if (strcmp(";", type) != 0) {
        logError(3, line); // 缂哄皯 ';'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // 璇诲彇涓嬩竴涓?token

    return es;
}

// <compound_stat> ::= '{' <statement_list> '}'
int compound_stat(Tree* parent) {
    int es = 0;
    Tree* current = addNonTerminal(parent, "<compound_stat>", line);

    // 妫€鏌?'{'
    if (strcmp("{", type) != 0) {   // 缂哄皯 '{'
        logError(1, line); // 閿欒浠ｇ爜锛氱己灏?'{'
        skip();
        return 0;
    }
    addTerminal(current, type, value); // 灏?'{' 娣诲姞鍒拌娉曟爲涓?
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 瑙ｆ瀽 <statement_list>
    es = statement_list(current);
    if (es > 0) {
        return es;
    }

    // 妫€鏌?'}'
    if (strcmp("}", type) != 0) {   // 缂哄皯 '}'
        logError(2, line); // 閿欒浠ｇ爜锛氱己灏?'}'
        skip();
        return 0;
    }
    addTerminal(current, type, value); // 灏?'}' 娣诲姞鍒拌娉曟爲涓?
    getToken(); // 璇诲彇涓嬩竴涓?token

    return es;
}

// <expression_stat> ::= <expression> ';' | ';'
int expression_stat(Tree* parent) {
    int es = 0;
    Tree* current = addNonTerminal(parent, "<expression_stat>", line);

    if (strcmp(";", type) == 0) {
        // 绌鸿〃杈惧紡璇彞
        addTerminal(current, type, value);
        getToken(); // 璇诲彇涓嬩竴涓?token
        return es;
    }

    // 瑙ｆ瀽 <expression>
    es = expression(current);
    if (es > 0) {
        return es;
    }

    // 妫€鏌?';'
    if (strcmp(";", type) != 0) {
        logError(3, line); // 閿欒浠ｇ爜锛氱己灏?';'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // 璇诲彇涓嬩竴涓?token

    return es;
}

// <call_stat> ::= call ID '(' ')' ';'
int call_stat(Tree* parent) {
    int es = 0, symbolPos;
    Tree* current = addNonTerminal(parent, "<call_stat>", line);
    addTerminal(current, type, value);
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 妫€鏌?ID锛堝嚱鏁板悕锛?
    if (strcmp("ID", type) != 0) {
        logError(6, line); // 缂哄皯鏍囪瘑绗?
        skip();
        return 0;
    }
    if(lookup(value, &symbolPos))   logError(34, line);

    if (symbol[symbolPos].kind != function && !lookup(value, &symbolPos)) {
        logError(34, line); // 鍋囪 34 琛ㄧず闈炲嚱鏁扮被鍨?
        skip();
        return 0;
    }

    addTerminal(current, type, value); // 娣诲姞鍑芥暟鍚?
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 妫€鏌?'('
    if (strcmp("(", type) != 0) {
        logError(4, line); // 缂哄皯 '('
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 妫€鏌?')'
    if (strcmp(")", type) != 0) {
        logError(5, line); // 缂哄皯 ')'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 妫€鏌?';'
    if (strcmp(";", type) != 0) {
        logError(3, line); // 缂哄皯 ';'
        skip();
        return 0;
    }
    addTerminal(current, type, value);
    strcpy(codes[codesIndex].opt, "CAL");     // 璋冪敤鍑芥暟
    codes[codesIndex].operand = symbol[symbolPos].address;     // 杩涘叆鍑芥暟鐨勫紑濮嬩綅缃?
    codesIndex++;
    getToken(); // 璇诲彇涓嬩竴涓?token

    return es;
}

// <expression> ::= ID '=' <bool_expr> | <bool_expr>
int expression(Tree* parent) {
    int es = 0;
    Tree* current = addNonTerminal(parent, "<expression>", line);
    char type1[20];     // 涓存椂鍗曡瘝绫诲瀷
    char value1[40];    // 涓存椂鍗曡瘝鍊?
    long curPoint;
    int place;

    // 姝ゅ杩涙潵鐨勬暟鎹彲鑳芥槸 ID
    if (strcmp("ID", type) == 0) {   //<expression> ::= ID '=' <bool_expr>
        // 淇濆瓨褰撳墠鏂囦欢鎸囬拡鍜岃鍙?
        curPoint = ftell(inputFile);
        // 璇诲彇涓嬩竴涓?token
        strcpy(type1, type);
        strcpy(value1, value);
        getToken(); // 璇诲彇涓嬩竴涓?token

        // 妫€鏌ユ槸鍚︿负 '='
        if (strcmp("=", type) == 0) {    // 璇ヨ〃杈惧紡渚嬪 a=xxxx
            if(lookup(value1, &place)) logError(23, line);
            addTerminal(current, type, value);
            getToken(); // 璇诲彇涓嬩竴涓?token

            // 瑙ｆ瀽 <bool_expr>
            es = bool_expr(current);
            if (es > 0) {
                return es;
            }

            strcpy(codes[codesIndex].opt, "STO");
            codes[codesIndex++].operand = symbol[place].address;
        }
        else {                      //<bool_expr>
            // 鍥為€€鍒?'=' 鍓嶇殑浣嶇疆
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
        // 瑙ｆ瀽 <bool_expr>
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

    // 瑙ｆ瀽绗竴涓?<additive_expr>
    es = additive_expr(current);
    if (es > 0) {
        return es;
    }

    // 瑙ｆ瀽鍚庣画鐨勫叧绯昏繍绠楃鍜?<additive_expr>
    while (strcmp(">", type) == 0 || strcmp("<", type) == 0 ||
        strcmp(">=", type) == 0 || strcmp("<=", type) == 0 ||
        strcmp("==", type) == 0 || strcmp("!=", type) == 0) {
        char token2[20];
        strcpy(token2, type); // 淇濆瓨杩愮畻绗?
        addTerminal(current, type, value); // 娣诲姞鍏崇郴杩愮畻绗?
        getToken(); // 璇诲彇涓嬩竴涓?token

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

    // 瑙ｆ瀽绗竴涓?<term>
    es = term(current);
    if (es > 0) {
        return es;
    }

    // 瑙ｆ瀽鍚庣画鐨?'+' 鎴?'-' 鍜?<term>
    while (strcmp("+", type) == 0 || strcmp("-", type) == 0) {
        char token2[20];
        strcpy(token2, type);
        addTerminal(current, type, value); // 娣诲姞 '+' 鎴?'-'
        getToken(); // 璇诲彇涓嬩竴涓?token

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

    // 瑙ｆ瀽绗竴涓?<factor>
    es = factor(current);
    if (es > 0) {
        return es;
    }

    // 瑙ｆ瀽鍚庣画鐨?'*' 鎴?'/' 鍜?<factor>
    while (strcmp("*", type) == 0 || strcmp("/", type) == 0) {
        char token2[20];
        strcpy(token2, type);
        addTerminal(current, type, value); // 娣诲姞 '*' 鎴?'/'
        getToken(); // 璇诲彇涓嬩竴涓?token

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
        getToken(); // 璇诲彇涓嬩竴涓?token

        // 瑙ｆ瀽 <additive_expr>
        es = additive_expr(current);
        if (es > 0) {
            return es;
        }

        if (strcmp(")", type) != 0) {
            logError(5, line); // 缂哄皯 ')'
            skip();
            return 0;
        }
        addTerminal(current, type, value);
        getToken(); // 璇诲彇涓嬩竴涓?token
    }
    else if (strcmp("ID", type) == 0) {
        if(lookup(value, &symbolPos))   logError(23, line);
        strcpy(codes[codesIndex].opt, "LOAD");
        codes[codesIndex++].operand = symbol[symbolPos].address;

        addTerminal(current, type, value); // 娣诲姞 ID
        getToken(); // 璇诲彇涓嬩竴涓?token
    }
    else if (strcmp("NUM", type) == 0) {
        strcpy(codes[codesIndex].opt, "LOADI");
        codes[codesIndex++].operand = atoi(value);
        addTerminal(current, type, value); // 娣诲姞 NUM
        getToken(); // 璇诲彇涓嬩竴涓?token
    }
    else {
        logError(7, line); // 閿欒浠ｇ爜锛氭湭璇嗗埆鐨勫洜瀛?
        skip();
        return 0;
    }

    return es;
}

// <declaration_stat>::= int ID ';' | int ID '=' NUM ';'
int declaration_stat(Tree* parent) {
    int es = 0;
    Tree* current = addNonTerminal(parent, "<declaration_stat>", line);
    // 褰撳墠 token 鏄?"int"
    addTerminal(current, type, value);

    getToken(); // 璇诲彇涓嬩竴涓?token
    // 璇诲彇涓嬩竴涓?token锛屽簲璇ユ槸 ID
    if (strcmp("ID", type) != 0) {
        logError(6, line); // 缂哄皯鏍囪瘑绗?
        skip();
        return 0;
    }
    addTerminal(current, type, value); // 娣诲姞鍙橀噺鍚嶅埌璇硶鏍戜腑

    es = insert_Symbol(variable, value); // 鎻掑叆绗﹀彿琛?

    getToken(); // 璇诲彇涓嬩竴涓?token

    // 璇诲彇涓嬩竴涓?token锛屽彲鑳芥槸 ';' 鎴?'='
    if (strcmp(";", type) == 0) {
        addTerminal(current, type, value);
        getToken(); // 璇诲彇涓嬩竴涓?token
    }
    else if (strcmp("=", type) == 0) {
        addTerminal(current, type, value);
        getToken(); // 璇诲彇涓嬩竴涓?token

        // 璧嬪€肩殑鍊煎簲璇ユ槸 NUM
        if (strcmp("NUM", type) != 0) {
            logError(7, line); // 閿欒浠ｇ爜锛氱己灏戞搷浣滄暟
            skip();
            return 0;
        }
        addTerminal(current, type, value); // 娣诲姞 NUM 鍒拌娉曟爲涓?
        getToken(); // 璇诲彇涓嬩竴涓?token

        // 妫€鏌?';'
        if (strcmp(";", type) != 0) {
            logError(3, line); // 閿欒浠ｇ爜锛氱己灏?';'
            skip();
            return 0;
        }
        addTerminal(current, type, value);
        getToken(); // 璇诲彇涓嬩竴涓?token
    }
    else {
        logError(3, line); // 閿欒浠ｇ爜锛氱己灏?';'
        skip();
        return 0;
    }
    return es;
}

// 澹版槑搴忓垪
int declaration_list(Tree* parent) {
    int es = 0;
    Tree* current = addNonTerminal(parent, "<declaration_list>", line);

    while (1) {
        if (strcmp("int", type) != 0) { // 涓嶆槸 "int"锛岃鏄庢病鏈夋洿澶氬０鏄?
            break;
        }
        es = declaration_stat(current); // 瑙ｆ瀽澹版槑
        if (es > 0) {
            return es;
        }
    }
    return es;
}

// 鍑芥暟浣?
// <function_body> ::= '{' <declaration_list> <statement_list> '}'
int function_body(Tree* parent) {
    int es = 0;

    // 娣诲姞 <function_body> 鑺傜偣鍒拌娉曟爲
    Tree* current = addNonTerminal(parent, "<function_body>", line);

    // 妫€鏌?'{'
    if (strcmp("{", type) != 0) {   // 缂哄皯 '{'
        logError(1, line); // 閿欒浠ｇ爜锛氱己灏?'{'
        skip();
        return 0;
    }
    addTerminal(current, type, value); // 灏?'{' 娣诲姞鍒拌娉曟爲涓?
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 瑙ｆ瀽 <declaration_list>
    es = declaration_list(current);
    if (es > 0) {
        return es;
    }
    strcpy(codes[codesIndex].opt, "ENTER");  // 涓哄嚱鏁拌皟鐢ㄥ紑杈熺┖闂?
    codes[codesIndex].operand = offset - offsetF + 2;
    codesIndex++;
    // 瑙ｆ瀽 <statement_list>
    es = statement_list(current);
    if (es > 0) {
        return es;
    }
    strcpy(codes[codesIndex].opt, "RETURN");  // 鍑芥暟杩斿洖
    codesIndex++;
    // 妫€鏌?'}'
    if (strcmp("}", type) != 0) {   // 缂哄皯 '}'
        logError(2, line); // 閿欒浠ｇ爜锛氱己灏?'}'
        skip();
        return 0;
    }
    addTerminal(current, type, value); // 灏?'}' 娣诲姞鍒拌娉曟爲涓?
    getToken(); // 璇诲彇涓嬩竴涓?token

    return es;
}

// 涓诲嚱鏁?
int main_declaration(Tree* parent) { // <main_declaration>::= main '(' ')' <function_body>
    int es = 0;
    char name_main[10] = "main";
    es = insert_Symbol(function, name_main);

    // 娣诲姞 <main_declaration> 鑺傜偣
    Tree* current = addNonTerminal(parent, "<main_declaration>", line);

    // 褰撳墠 token 鏄?"main"
    addTerminal(current, type, value); // 灏?"main" 娣诲姞鍒拌娉曟爲涓?
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 妫€鏌?'('
    if (strcmp("(", type) != 0) {
        logError(4, line); // 缂哄皯 '('
        skip();
        return 0;
    }
    addTerminal(current, type, value); // 娣诲姞 '(' 鍒拌娉曟爲涓?
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 妫€鏌?')'
    if (strcmp(")", type) != 0) {
        logError(5, line); // 缂哄皯 ')'
        skip();
        return 0;
    }
    addTerminal(current, type, value); // 娣诲姞 ')' 鍒拌娉曟爲涓?
    codes[0].operand = codesIndex;
    offset = 2;        // 姣忔杩涘叆涓€涓嚱鏁颁綋鐨勬椂鍊欙紝鐩稿鍦板潃閮借閲嶇疆涓?
    offsetF = offset;
    symbol[symbolIndex - 1].address = codesIndex;
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 瑙ｆ瀽鍑芥暟浣?
    es = function_body(current);
    return es;
}

int fun_declaration(Tree* parent) { // <fun_declaration>::= function ID '(' [鍙傛暟鍒楄〃] ')' <function_body>
    int es = 0;
    // 娣诲姞 <fun_declaration> 鑺傜偣
    Tree* current = addNonTerminal(parent, "<fun_declaration>", line);

    addTerminal(current, type, value); // 灏?"function" 娣诲姞鍒拌娉曟爲涓?
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 妫€鏌?ID
    if (strcmp("ID", type) != 0) {
        logError(6, line); // 缂哄皯鏍囪瘑绗?
        skip();
        return 0;
    }
    es = insert_Symbol(function, value); // 灏嗗嚱鏁板悕鎻掑叆绗﹀彿琛?
    offset = 2;        // 姣忔杩涘叆涓€涓嚱鏁颁綋鐨勬椂鍊欙紝鐩稿鍦板潃閮借閲嶇疆涓?
    offsetF = offset;
    symbol[symbolIndex - 1].address = codesIndex;
    int fPos = codesIndex; // 鍑芥暟澹版槑浣嶇疆
    addTerminal(current, type, value); // 娣诲姞鍑芥暟鍚嶅埌璇硶鏍戜腑

    getToken(); // 璇诲彇涓嬩竴涓?token

    // 妫€鏌?'('
    if (strcmp("(", type) != 0) {
        logError(4, line); // 缂哄皯 '('
        skip();
        return 0;
    }
    addTerminal(current, type, value); // 娣诲姞 '(' 鍒拌娉曟爲涓?
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 妫€鏌?')'
    if (strcmp(")", type) != 0) {
        logError(5, line); // 缂哄皯 ')'
        skip();
        return 0;
    }
    addTerminal(current, type, value); // 娣诲姞 ')' 鍒拌娉曟爲涓?
    getToken(); // 璇诲彇涓嬩竴涓?token

    // 瑙ｆ瀽鍑芥暟浣?
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
        logError(6, line); // 缂哄皯 main 澹版槑
    }
    printf("     绗﹀彿琛╘n");
    printf(" 鍚嶅瓧\t \t绫诲瀷 \t鍦板潃\n");
    for (int i = 0; i < symbolIndex; i++)
        printf("  %-8s \t%d \t%d\n", symbol[i].name, symbol[i].kind, symbol[i].address);
    return es;
}

int parseAlanalysis() {

    // 鎵撳紑杈撳叆鏂囦欢
    inputFile = fopen(inputAddress, "r");
    if (inputFile == NULL) {
        logError(8, line); // 鏃犳硶鎵撳紑杈撳叆鏂囦欢
        return 8;
    }

    // 鎵撳紑杈撳嚭鏂囦欢
    outputFile = fopen(outputAddress, "w");
    if (outputFile == NULL) {
        logError(9, line); // 鏃犳硶鎵撳紑杈撳嚭鏂囦欢
        fclose(inputFile);
        return 9;
    }

    // 鍒濆鍖栨牴鑺傜偣
    root = createNode("<program>", 0, NULL);

    // 寮€濮嬭娉曞垎鏋?
    int es = program(); // 闇€瑕佸疄鐜?program 鍑芥暟

    return es;
}

int main() {
    int es = 0;
    es = parseAlanalysis();      // 閫掑綊涓嬮檷璇硶鍒嗘瀽

    // 澶勭悊缁撴灉
    printf("\n\n=============璇硶鍒嗘瀽缁撴灉=====================\n\n");
    if (errorCount == 0) {
        printf("璇硶鍒嗘瀽鎴愬姛\n");
        printTree(root, 0, outputFile);
    }
    else {
        printf("璇硶鍒嗘瀽澶辫触锛屽彂鐜?%d 涓敊璇?\n", errorCount);
        for (int i = 0; i < errorCount; i++) {
            int code = errorList[i].code;
            int lineNo = errorList[i].line;
            switch (code) { // 鏍规嵁閿欒浠ｇ爜閫夋嫨鍏蜂綋閿欒淇℃伅
            case 1: printf("绗?d琛岀己灏?{ \n", lineNo); break;
            case 2: printf("绗?d琛岀己灏?} \n", lineNo); break;
            case 3: printf("绗?d琛岀己灏?; \n", lineNo-1); break;
            case 4: printf("绗?d琛岀己灏?( \n", lineNo); break;
            case 5: printf("绗?d琛岀己灏?) \n", lineNo); break;
            case 6: printf("绗?d琛岀己灏戞爣璇嗙\n", lineNo); break;
            case 7: printf("绗?d琛岀己灏戞搷浣滄暟\n", lineNo); break;
            case 8: printf("鏃犳硶鎵撳紑杈撳叆鏂囦欢%s\n", inputAddress);  break;
            case 9: printf("鏃犳硶鎵撳紑杈撳嚭鏂囦欢%s\n", outputAddress); break;
            case 10: printf("绗?d琛岀己灏戝弬鏁癨n", lineNo);  break;
            case 11: printf("绗?d琛岀己灏戜繚鐣欏瓧\n", lineNo);  break;
            case 21: printf("绗?d琛岀鍙疯〃婧㈠嚭\n", lineNo);  break;
            case 22: printf("绗?d琛屽彉閲忓悓鍚峔n", lineNo);  break;
            case 23: printf("绗?d琛屽彉閲忔湭瀹氫箟\n", lineNo); break;
            case 32: printf("绗?d琛屽嚱鏁伴噸瀹氫箟\n", lineNo); break;
            case 34: printf("绗?d琛屽嚱鏁版湭瀹氫箟\n", lineNo); break; // 娣诲姞缂哄け鐨勯敊璇唬鐮?
            case 35: printf("绗?d琛屾爣璇嗙涓嶆槸鍙橀噺\n", lineNo); break; // 娣诲姞缂哄け鐨勯敊璇唬鐮?
            default: printf("绗?d琛屾湭鐭ラ敊璇痋n", lineNo); break;
            }
        }

        // 璺宠浆鍒版枃浠舵湯灏惧苟杩斿洖
        skip(); // 璺宠浆鍒版枃浠舵湯灏?
        return 0;
    }

    // 濡傛灉娌℃湁閿欒锛岀户缁敓鎴愪腑闂翠唬鐮?
    if ((fpCodeText = fopen(Codeout, "w")) == NULL) {
        printf("\n鍒涘缓%s閿欒!\n", Codeout);
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
    // 鍏抽棴鏂囦欢
    if (inputFile != NULL) fclose(inputFile);
    if (outputFile != NULL) fclose(outputFile);

    // 閲婃斁璇硶鏍戝唴瀛?
    freeTree(root);

    return 0;
}
