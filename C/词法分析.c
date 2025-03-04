#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_WORD_LEN 100

// 保留字表（按字典序排序）
const char* keywords[] = { "break", "case", "default", "do", "else", "for", "if", "int", "read", "switch", "write", "while" };
#define NUM_KEYWORDS (sizeof(keywords) / sizeof(keywords[0]))

// 定义单词结构体，新增 lineNumber 字段
typedef struct {
    char category[MAX_WORD_LEN];
    char lexeme[MAX_WORD_LEN];
    int lineNumber; // 存储单词所在的行号
} Token;

// 使用折半查找检查是否为保留字, 返回值为 1 表示是保留字，0 表示不是保留字。
int isKeyword(const char* word) {
    int left = 0, right = NUM_KEYWORDS - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        int cmp = strcmp(word, keywords[mid]);
        if (cmp == 0)
            return 1; // 是保留字
        else if (cmp < 0)
            right = mid - 1;
        else
            left = mid + 1;
    }
    return 0; // 不是保留字
}

// 词法分析主函数
void lexicalAnalysis(FILE* input, FILE* output) {
    char line[1024]; // 存储从输入文件读取的一行字符（最大长度为1024）。
    int lineNum = 0; // 当前处理的行号，从1开始，用于错误定位。
    Token tokens[1000]; // 存储识别出的单词，最多支持1000个单词。
    int tokenCount = 0; // 记录已识别的单词数量。
    char errors[100][1024];
    int errorCount = 0;
    int inComment = 0; // 标志变量，记录是否在多行注释中

    while (fgets(line, sizeof(line), input)) {
        ++lineNum;
        int i = 0;
        while (line[i] != '\0') {
            // 如果在多行注释中，跳过注释内容直到结束
            if (inComment) {
                while (line[i] != '\0' && !(line[i] == '*' && line[i + 1] == '/')) {
                    if (line[i] == '\n') ++lineNum;
                    i++;
                }
                if (line[i] == '*' && line[i + 1] == '/') {
                    inComment = 0; // 多行注释结束
                    i += 2;
                }
                else {
                    // 如果到行末还未闭合，继续下一行
                    break;
                }
                continue;
            }

            // 跳过空白字符
            if (isspace(line[i])) {
                i++;
                continue;
            }

            // 识别注释
            if (line[i] == '/' && line[i + 1] == '*') {
                inComment = 1;
                i += 2;
                continue;
            }

            // 识别标识符或保留字
            if (isalpha(line[i])) {
                char buffer[MAX_WORD_LEN];   // 用一个 buffer 临时存储单词，直到遇到非字母数字的字符。
                int idx = 0;
                while (isalnum(line[i])) { // isalnum 已包含 isdigit，无需重复
                    if (idx < MAX_WORD_LEN - 1) { // 防止缓冲区溢出
                        buffer[idx++] = line[i++];
                    }
                    else {
                        // 单词过长，记录错误并跳过
                        sprintf(errors[errorCount++], "第%d行: 单词过长", lineNum);
                        while (isalnum(line[i])) i++;
                        break;
                    }
                }
                buffer[idx] = '\0';
                if (isKeyword(buffer)) {
                    tokens[tokenCount].lineNumber = lineNum; // 记录行号
                    strcpy(tokens[tokenCount].lexeme, buffer);
                    strcpy(tokens[tokenCount].category, buffer); // 保留字直接用本身表示
                    tokenCount++;
                }
                else {
                    tokens[tokenCount].lineNumber = lineNum; // 记录行号
                    strcpy(tokens[tokenCount].lexeme, buffer);
                    strcpy(tokens[tokenCount].category, "ID");
                    tokenCount++;
                }
                continue;
            }

            // 识别数字
            if (isdigit(line[i])) {
                char buffer[MAX_WORD_LEN];
                int idx = 0;
                while (isdigit(line[i])) {
                    if (idx < MAX_WORD_LEN - 1) { // 防止缓冲区溢出
                        buffer[idx++] = line[i++];
                    }
                    else {
                        // 数字过长，记录错误并跳过
                        sprintf(errors[errorCount++], "第%d行: 数字过长", lineNum);
                        while (isdigit(line[i])) i++;
                        break;
                    }
                }
                buffer[idx] = '\0';
                tokens[tokenCount].lineNumber = lineNum; // 记录行号
                strcpy(tokens[tokenCount].lexeme, buffer);
                strcpy(tokens[tokenCount].category, "NUM");
                tokenCount++;
                continue;
            }

            // 识别双分界符
            if ((line[i] == '>' && line[i + 1] == '=') ||
                (line[i] == '<' && line[i + 1] == '=') ||
                (line[i] == '!' && line[i + 1] == '=') ||
                (line[i] == '=' && line[i + 1] == '=') ||
                (line[i] == '&' && line[i + 1] == '&') ||
                (line[i] == '|' && line[i + 1] == '|')) {
                char buffer[3] = { line[i], line[i + 1], '\0' };
                tokens[tokenCount].lineNumber = lineNum; // 记录行号
                strcpy(tokens[tokenCount].lexeme, buffer);
                strcpy(tokens[tokenCount].category, buffer); // 双分界符直接用本身表示
                tokenCount++;
                i += 2;
                continue;
            }

            // 识别单分界符
            if (strchr("+-*/(){}[],;><!=", line[i])) {
                char buffer[2] = { line[i], '\0' };
                tokens[tokenCount].lineNumber = lineNum; // 记录行号
                strcpy(tokens[tokenCount].lexeme, buffer);
                strcpy(tokens[tokenCount].category, buffer); // 单分界符直接用本身表示
                tokenCount++;
                i++;
                continue;
            }

            // 非法字符处理
            char buffer[2] = { line[i], '\0' };
            sprintf(errors[errorCount++], "第%d行: 非法单词 \"%s\"", lineNum, buffer);
            i++;
        }
    }

    // 如果注释未闭合，记录错误
    if (inComment) {
        sprintf(errors[errorCount++], "第%d行: 注释未闭合", lineNum);
    }

    // 输出错误信息到标准输出
    for (int j = 0; j < errorCount; ++j) {
        printf("%s\n", errors[j]);
    }

    // 输出单词序列到输出文件，包含行号
    for (int j = 0; j < tokenCount; ++j) {
        fprintf(output, "%-10s\t%-10s\t%d\n", tokens[j].category, tokens[j].lexeme, tokens[j].lineNumber);
    }
}

int main() {
    FILE* input = fopen("E://文档//代码//大三上//编译原理语法分析//input.txt", "r");
    FILE* output = fopen("E://文档//代码//大三上//编译原理语法分析//output.txt", "w");

    if (!input || !output) {
        perror("文件打开失败");
        return EXIT_FAILURE;
    }

    lexicalAnalysis(input, output);

    fclose(input);
    fclose(output);

    printf("词法分析完成，结果已写入 output.txt\n");
    return 0;
}
