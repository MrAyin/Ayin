$ cat guangdu.c
#include <unistd.h>
#include <stdio.h> 
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define QUEUE_SIZE 100 // �����������

// ���нṹ
typedef struct 
{
    char* data[QUEUE_SIZE]; // ���д洢��̬������ַ���
    int front;              // ����ͷ����
    int rear;               // ����β����
} Queue;

// ��ʼ������
void initQueue(Queue* q) {
    q->front = 0;
    q->rear = 0;
    for(int i = 0; i < QUEUE_SIZE; i++) {
        q->data[i] = NULL;
    }
}

// �ж϶����Ƿ�Ϊ��
bool isEmpty(Queue* q) 
{
    return q->front == q->rear;
}

// �ж϶����Ƿ�����
bool isFull(Queue* q)
{
    return ((q->rear + 1) % QUEUE_SIZE) == q->front;
}

// ��ӣ����ַ����ĸ����������ĩβ��
bool enqueue(Queue* q, const char* value) {
    if (isFull(q)) {
        printf("�����������޷����: %s\n", value);
        return false;
    }
    q->data[q->rear] = strdup(value); // ���䲢�����ַ���
    if (q->data[q->rear] == NULL) {
        printf("�ڴ����ʧ��: %s\n", value);
        return false;
    }
    q->rear = (q->rear + 1) % QUEUE_SIZE; // ���¶�β����
    return true;
}

// ���ӣ��Ƴ�����ͷ�����ַ�����
bool dequeue(Queue* q, char** value) {
    if (isEmpty(q)) {
        printf("����Ϊ�գ��޷����ӡ�\n");
        return false;
    }
    *value = q->data[q->front];           // ��ȡ��ͷԪ��
    q->data[q->front] = NULL;             // ��ն�ͷλ�ã���ѡ��
    q->front = (q->front + 1) % QUEUE_SIZE; // ���¶�ͷ����
    return true;
}

// ��ȡ�ļ���С���޸�ʱ��
static int get_file_size_time(const char *filename) {
    struct stat statbuf;
    if (stat(filename, &statbuf) == -1) { // ��ȡ�ļ�״̬
        printf("��ȡ %s ״̬ʱ����: %s\n", filename, strerror(errno));
        return -1;
    }
    if (S_ISDIR(statbuf.st_mode)) // �ж��Ƿ�ΪĿ¼
        return 1;
    if (S_ISREG(statbuf.st_mode)) { // �ж��Ƿ�Ϊ�����ļ�
        printf("%s ��С: %ld �ֽ�\t�޸�ʱ��: %s", filename, statbuf.st_size, ctime(&statbuf.st_mtime));
    }
    return 0;
}

// BFS ����Ŀ¼
void bfs_dir(const char *path1)
{
    Queue q;
    initQueue(&q);
    if (!enqueue(&q, path1)) { // ��ӳ�ʼ·��
        exit(EXIT_FAILURE);
    }

    DIR *dirp;
    struct dirent *direntp;
    char path[1024];

    while(!isEmpty(&q))
    {
        char* current_path = NULL;
        if (!dequeue(&q, &current_path)) { // ����
            continue; // ����ʧ�ܣ�������ǰѭ��
        }

        // ��Ŀ¼
        if ((dirp = opendir(current_path)) == NULL) 
        { 
            printf("��Ŀ¼ %s ʱ����: %s\n", current_path, strerror(errno));
            free(current_path); // �ͷų��ӵ�·��
            continue; // ������������е���һ��·��
        }

        // ��ȡĿ¼����
        while ((direntp = readdir(dirp)) != NULL) 
        { 
            if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0) 
            {
                continue; // ������ǰĿ¼�͸�Ŀ¼
            }
            // �����µ�·��
            snprintf(path, sizeof(path), "%s/%s", current_path, direntp->d_name);

            int file_type = get_file_size_time(path);
            if (file_type == 1)
            {
                if (!enqueue(&q, path)) { // �����Ŀ¼�����
            }
        }

        closedir(dirp); // �ر�Ŀ¼
        free(current_path); // �ͷų��ӵ�·��
    }
}   

int main(int argc, char **argv) {
    int stats;
    if (argc != 2) { // ����������
        printf("ʹ�÷���: %s <Ŀ¼·��>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // ����ṩ�Ĳ����Ƿ�ΪĿ¼
    stats = get_file_size_time(argv[1]);
    if (stats == 0) { // �ǳ����ļ�
        printf("�ṩ��·����һ�������ļ��������˳���\n");
        exit(EXIT_SUCCESS);
    }
    if (stats == -1) { // stat ���ó���
        exit(EXIT_FAILURE);
    }

    bfs_dir(argv[1]); // ��ʼ BFS ����
    return EXIT_SUCCESS;
}
