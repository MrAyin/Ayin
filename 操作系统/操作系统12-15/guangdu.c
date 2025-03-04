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

#define QUEUE_SIZE 100 // 定义队列容量

// 队列结构
typedef struct 
{
    char* data[QUEUE_SIZE]; // 队列存储动态分配的字符串
    int front;              // 队列头索引
    int rear;               // 队列尾索引
} Queue;

// 初始化队列
void initQueue(Queue* q) {
    q->front = 0;
    q->rear = 0;
    for(int i = 0; i < QUEUE_SIZE; i++) {
        q->data[i] = NULL;
    }
}

// 判断队列是否为空
bool isEmpty(Queue* q) 
{
    return q->front == q->rear;
}

// 判断队列是否已满
bool isFull(Queue* q)
{
    return ((q->rear + 1) % QUEUE_SIZE) == q->front;
}

// 入队（将字符串的副本加入队列末尾）
bool enqueue(Queue* q, const char* value) {
    if (isFull(q)) {
        printf("队列已满，无法入队: %s\n", value);
        return false;
    }
    q->data[q->rear] = strdup(value); // 分配并复制字符串
    if (q->data[q->rear] == NULL) {
        printf("内存分配失败: %s\n", value);
        return false;
    }
    q->rear = (q->rear + 1) % QUEUE_SIZE; // 更新队尾索引
    return true;
}

// 出队（移除队列头部的字符串）
bool dequeue(Queue* q, char** value) {
    if (isEmpty(q)) {
        printf("队列为空，无法出队。\n");
        return false;
    }
    *value = q->data[q->front];           // 获取队头元素
    q->data[q->front] = NULL;             // 清空队头位置（可选）
    q->front = (q->front + 1) % QUEUE_SIZE; // 更新队头索引
    return true;
}

// 获取文件大小和修改时间
static int get_file_size_time(const char *filename) {
    struct stat statbuf;
    if (stat(filename, &statbuf) == -1) { // 获取文件状态
        printf("获取 %s 状态时出错: %s\n", filename, strerror(errno));
        return -1;
    }
    if (S_ISDIR(statbuf.st_mode)) // 判断是否为目录
        return 1;
    if (S_ISREG(statbuf.st_mode)) { // 判断是否为常规文件
        printf("%s 大小: %ld 字节\t修改时间: %s", filename, statbuf.st_size, ctime(&statbuf.st_mtime));
    }
    return 0;
}

// BFS 遍历目录
void bfs_dir(const char *path1)
{
    Queue q;
    initQueue(&q);
    if (!enqueue(&q, path1)) { // 入队初始路径
        exit(EXIT_FAILURE);
    }

    DIR *dirp;
    struct dirent *direntp;
    char path[1024];

    while(!isEmpty(&q))
    {
        char* current_path = NULL;
        if (!dequeue(&q, &current_path)) { // 出队
            continue; // 出队失败，跳过当前循环
        }

        // 打开目录
        if ((dirp = opendir(current_path)) == NULL) 
        { 
            printf("打开目录 %s 时出错: %s\n", current_path, strerror(errno));
            free(current_path); // 释放出队的路径
            continue; // 继续处理队列中的下一个路径
        }

        // 读取目录内容
        while ((direntp = readdir(dirp)) != NULL) 
        { 
            if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0) 
            {
                continue; // 跳过当前目录和父目录
            }
            // 构建新的路径
            snprintf(path, sizeof(path), "%s/%s", current_path, direntp->d_name);

            int file_type = get_file_size_time(path);
            if (file_type == 1)
            {
                if (!enqueue(&q, path)) { // 如果是目录，入队
            }
        }

        closedir(dirp); // 关闭目录
        free(current_path); // 释放出队的路径
    }
}   

int main(int argc, char **argv) {
    int stats;
    if (argc != 2) { // 检查参数数量
        printf("使用方法: %s <目录路径>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 检查提供的参数是否为目录
    stats = get_file_size_time(argv[1]);
    if (stats == 0) { // 是常规文件
        printf("提供的路径是一个常规文件，程序退出。\n");
        exit(EXIT_SUCCESS);
    }
    if (stats == -1) { // stat 调用出错
        exit(EXIT_FAILURE);
    }

    bfs_dir(argv[1]); // 开始 BFS 遍历
    return EXIT_SUCCESS;
}
