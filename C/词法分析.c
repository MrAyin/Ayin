#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_WORD_LEN 100

// �����ֱ����ֵ�������
const char* keywords[] = { "break", "case", "default", "do", "else", "for", "if", "int", "read", "switch", "write", "while" };
#define NUM_KEYWORDS (sizeof(keywords) / sizeof(keywords[0]))

// ���嵥�ʽṹ�壬���� lineNumber �ֶ�
typedef struct {
    char category[MAX_WORD_LEN];
    char lexeme[MAX_WORD_LEN];
    int lineNumber; // �洢�������ڵ��к�
} Token;

// ʹ���۰���Ҽ���Ƿ�Ϊ������, ����ֵΪ 1 ��ʾ�Ǳ����֣�0 ��ʾ���Ǳ����֡�
int isKeyword(const char* word) {
    int left = 0, right = NUM_KEYWORDS - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        int cmp = strcmp(word, keywords[mid]);
        if (cmp == 0)
            return 1; // �Ǳ�����
        else if (cmp < 0)
            right = mid - 1;
        else
            left = mid + 1;
    }
    return 0; // ���Ǳ�����
}

// �ʷ�����������
void lexicalAnalysis(FILE* input, FILE* output) {
    char line[1024]; // �洢�������ļ���ȡ��һ���ַ�����󳤶�Ϊ1024����
    int lineNum = 0; // ��ǰ������кţ���1��ʼ�����ڴ���λ��
    Token tokens[1000]; // �洢ʶ����ĵ��ʣ����֧��1000�����ʡ�
    int tokenCount = 0; // ��¼��ʶ��ĵ���������
    char errors[100][1024];
    int errorCount = 0;
    int inComment = 0; // ��־��������¼�Ƿ��ڶ���ע����

    while (fgets(line, sizeof(line), input)) {
        ++lineNum;
        int i = 0;
        while (line[i] != '\0') {
            // ����ڶ���ע���У�����ע������ֱ������
            if (inComment) {
                while (line[i] != '\0' && !(line[i] == '*' && line[i + 1] == '/')) {
                    if (line[i] == '\n') ++lineNum;
                    i++;
                }
                if (line[i] == '*' && line[i + 1] == '/') {
                    inComment = 0; // ����ע�ͽ���
                    i += 2;
                }
                else {
                    // �������ĩ��δ�պϣ�������һ��
                    break;
                }
                continue;
            }

            // �����հ��ַ�
            if (isspace(line[i])) {
                i++;
                continue;
            }

            // ʶ��ע��
            if (line[i] == '/' && line[i + 1] == '*') {
                inComment = 1;
                i += 2;
                continue;
            }

            // ʶ���ʶ��������
            if (isalpha(line[i])) {
                char buffer[MAX_WORD_LEN];   // ��һ�� buffer ��ʱ�洢���ʣ�ֱ����������ĸ���ֵ��ַ���
                int idx = 0;
                while (isalnum(line[i])) { // isalnum �Ѱ��� isdigit�������ظ�
                    if (idx < MAX_WORD_LEN - 1) { // ��ֹ���������
                        buffer[idx++] = line[i++];
                    }
                    else {
                        // ���ʹ�������¼��������
                        sprintf(errors[errorCount++], "��%d��: ���ʹ���", lineNum);
                        while (isalnum(line[i])) i++;
                        break;
                    }
                }
                buffer[idx] = '\0';
                if (isKeyword(buffer)) {
                    tokens[tokenCount].lineNumber = lineNum; // ��¼�к�
                    strcpy(tokens[tokenCount].lexeme, buffer);
                    strcpy(tokens[tokenCount].category, buffer); // ������ֱ���ñ����ʾ
                    tokenCount++;
                }
                else {
                    tokens[tokenCount].lineNumber = lineNum; // ��¼�к�
                    strcpy(tokens[tokenCount].lexeme, buffer);
                    strcpy(tokens[tokenCount].category, "ID");
                    tokenCount++;
                }
                continue;
            }

            // ʶ������
            if (isdigit(line[i])) {
                char buffer[MAX_WORD_LEN];
                int idx = 0;
                while (isdigit(line[i])) {
                    if (idx < MAX_WORD_LEN - 1) { // ��ֹ���������
                        buffer[idx++] = line[i++];
                    }
                    else {
                        // ���ֹ�������¼��������
                        sprintf(errors[errorCount++], "��%d��: ���ֹ���", lineNum);
                        while (isdigit(line[i])) i++;
                        break;
                    }
                }
                buffer[idx] = '\0';
                tokens[tokenCount].lineNumber = lineNum; // ��¼�к�
                strcpy(tokens[tokenCount].lexeme, buffer);
                strcpy(tokens[tokenCount].category, "NUM");
                tokenCount++;
                continue;
            }

            // ʶ��˫�ֽ��
            if ((line[i] == '>' && line[i + 1] == '=') ||
                (line[i] == '<' && line[i + 1] == '=') ||
                (line[i] == '!' && line[i + 1] == '=') ||
                (line[i] == '=' && line[i + 1] == '=') ||
                (line[i] == '&' && line[i + 1] == '&') ||
                (line[i] == '|' && line[i + 1] == '|')) {
                char buffer[3] = { line[i], line[i + 1], '\0' };
                tokens[tokenCount].lineNumber = lineNum; // ��¼�к�
                strcpy(tokens[tokenCount].lexeme, buffer);
                strcpy(tokens[tokenCount].category, buffer); // ˫�ֽ��ֱ���ñ����ʾ
                tokenCount++;
                i += 2;
                continue;
            }

            // ʶ�𵥷ֽ��
            if (strchr("+-*/(){}[],;><!=", line[i])) {
                char buffer[2] = { line[i], '\0' };
                tokens[tokenCount].lineNumber = lineNum; // ��¼�к�
                strcpy(tokens[tokenCount].lexeme, buffer);
                strcpy(tokens[tokenCount].category, buffer); // ���ֽ��ֱ���ñ����ʾ
                tokenCount++;
                i++;
                continue;
            }

            // �Ƿ��ַ�����
            char buffer[2] = { line[i], '\0' };
            sprintf(errors[errorCount++], "��%d��: �Ƿ����� \"%s\"", lineNum, buffer);
            i++;
        }
    }

    // ���ע��δ�պϣ���¼����
    if (inComment) {
        sprintf(errors[errorCount++], "��%d��: ע��δ�պ�", lineNum);
    }

    // ���������Ϣ����׼���
    for (int j = 0; j < errorCount; ++j) {
        printf("%s\n", errors[j]);
    }

    // ����������е�����ļ��������к�
    for (int j = 0; j < tokenCount; ++j) {
        fprintf(output, "%-10s\t%-10s\t%d\n", tokens[j].category, tokens[j].lexeme, tokens[j].lineNumber);
    }
}

int main() {
    FILE* input = fopen("E://�ĵ�//����//������//����ԭ���﷨����//input.txt", "r");
    FILE* output = fopen("E://�ĵ�//����//������//����ԭ���﷨����//output.txt", "w");

    if (!input || !output) {
        perror("�ļ���ʧ��");
        return EXIT_FAILURE;
    }

    lexicalAnalysis(input, output);

    fclose(input);
    fclose(output);

    printf("�ʷ�������ɣ������д�� output.txt\n");
    return 0;
}
