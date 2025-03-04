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

static int get_file_size_time(const char *filename) 
{
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



void dfs_dir(const char* path1)
{
    DIR *dirp;
    struct dirent *direntp;
    char path[1024];

    if ((dirp = opendir(path1)) == NULL) { // 打开目录
        printf("Open Directory %s Error: %s\n", path, strerror(errno));
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
            dfs_dir(path);
        }
    }
    closedir(dirp);
}

int main(int argc, char **argv) {
    int stats;
    if (argc != 2) { // 判断输入是否为两个参数
        printf("Usage: %s filename\n\a", argv[0]);
        exit(1);
    }
    // 若 argv[1] 为文件（输出文件大小和最后修改时间）或者 stats 错误，退出
    if (((stats = get_file_size_time(argv[1])) == 0) || (stats == -1))
        exit(1);

    dfs_dir(argv[1]);
    exit(1);
}
