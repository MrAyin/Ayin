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
    const char* data[100]; // ���д洢Ԫ������Ϊ const char*
    int front;                    // ����ͷ����
    int rear;                     // ����β����
} Queue;

// ��ʼ������
void initQueue(Queue* q) {
    q->front = 0;
    q->rear = 0;
}

// �ж϶����Ƿ�Ϊ��
bool isEmpty(Queue* q) 
{
    return q->front == q->rear;
}


// ��ӣ��� const char* Ԫ�ؼ������ĩβ��
bool enqueue(Queue* q, const char* value) {
    q->data[q->rear] = value;           // ���ַ���ָ��������
    q->rear = (q->rear + 1) % QUEUE_SIZE; // ���¶�β����
    return true;
}

// ���ӣ��Ӷ���ͷ���Ƴ�Ԫ�أ���ͨ����������ֵ��
bool dequeue(Queue* q, const char** value) {
    if (isEmpty(q)) {
        printf("Queue is empty. Cannot dequeue.\n");
        return false;
    }
    *value = q->data[q->front];           // ȡ����ͷԪ��
    q->front = (q->front + 1) % QUEUE_SIZE; // ���¶�ͷ����
    return true;
}

static int get_file_size_time(const char *filename) {
    struct stat statbuf;
    if (stat(filename, &statbuf) == -1) { // ��ȡ filename ��״̬
        printf("Get stat on %s Error: %s\n", filename, strerror(errno));
        return -1;
    }
    if (S_ISDIR(statbuf.st_mode)) // �ж��Ƿ���Ŀ¼�ļ�
        return 1;
    if (S_ISREG(statbuf.st_mode)) // �ж��Ƿ�����ͨ�ļ�
        printf("%s size: %ld bytes\tmodified at %s", filename, statbuf.st_size, ctime(&statbuf.st_mtime));
    return 0;
}


void bfs_dir(const char *path1)
{
    Queue q;
    initQueue(&q);
    enqueue(&q, path1); 
    DIR *dirp;
    struct dirent *direntp;
    char path[1024];
    while(isEmpty(&q)!=1)
    {
        dequeue(&q, &path1);//����
        if ((dirp = opendir(path1)) == NULL) 
        { // ��Ŀ¼
        printf("Open Directory %s Error: %s\n", path1, strerror(errno));
        exit(1);
        }
    
        while ((direntp = readdir(dirp)) != NULL) 
        { // ��ȡĿ¼����
            if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0) 
            {
            continue; // ��������ѭ��
            }
            memset(path, 0, 1024);
            strcpy(path, path1);
            strcat(path, "/");
            strcat(path, direntp->d_name);
            if(get_file_size_time(path) == 1)
            {
                enqueue(&q,path);
            }
        }
    }
    
}   

int main(int argc, char **argv) {
    DIR *dirp;
    struct dirent *direntp;
    int stats;
    char path[1024];
    if (argc != 2) { // �ж������Ƿ�Ϊ��������
        printf("Usage: %s filename\n\a", argv[0]);
        exit(1);
    }
    // �� argv[1] Ϊ�ļ�������ļ���С������޸�ʱ�䣩���� stats �����˳�
    if (((stats = get_file_size_time(argv[1])) == 0) || (stats == -1))
        exit(1);
    
    bfs_dir(argv[1]);
    exit(1);
}