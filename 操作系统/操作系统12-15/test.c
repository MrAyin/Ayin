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

#define QUEUE_SIZE 100 // 定义队列容量

// 队列结构
typedef struct 
{
    const char* data[100]; // 队列存储元素类型为 const char*
    int front;                    // 队列头索引
    int rear;                     // 队列尾索引
} Queue;

// 初始化队列
void initQueue(Queue* q) {
    q->front = 0;
    q->rear = 0;
}

// 判断队列是否为空
bool isEmpty(Queue* q) 
{
    return q->front == q->rear;
}


// 入队（将 const char* 元素加入队列末尾）
bool enqueue(Queue* q, const char* value) {
    q->data[q->rear] = value;           // 将字符串指针存入队列
    q->rear = (q->rear + 1) % QUEUE_SIZE; // 更新队尾索引
    return true;
}

// 出队（从队列头部移除元素，并通过参数返回值）
bool dequeue(Queue* q, const char** value) {
    if (isEmpty(q)) {
        printf("Queue is empty. Cannot dequeue.\n");
        return false;
    }
    *value = q->data[q->front];           // 取出队头元素
    q->front = (q->front + 1) % QUEUE_SIZE; // 更新队头索引
    return true;
}

static int get_file_size_time(const char *filename) {
    struct stat statbuf;
    if (stat(filename, &statbuf) == -1) { // 获取 filename 的状态
        printf("Get stat on %s Error: %s\n", filename, strerror(errno));
        return -1;
    }
    if (S_ISDIR(statbuf.st_mode)) // 判断是否是目录文件
        return 1;
    if (S_ISREG(statbuf.st_mode)) // 判断是否是普通文件
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
        dequeue(&q, &path1);//出队
        if ((dirp = opendir(path1)) == NULL) 
        { // 打开目录
        printf("Open Directory %s Error: %s\n", path1, strerror(errno));
        exit(1);
        }
    
        while ((direntp = readdir(dirp)) != NULL) 
        { // 读取目录内容
            if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0) 
            {
            continue; // 跳过本次循环
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
    if (argc != 2) { // 判断输入是否为两个参数
        printf("Usage: %s filename\n\a", argv[0]);
        exit(1);
    }
    // 若 argv[1] 为文件（输出文件大小和最后修改时间）或者 stats 错误，退出
    if (((stats = get_file_size_time(argv[1])) == 0) || (stats == -1))
        exit(1);
    
    bfs_dir(argv[1]);
    exit(1);
}