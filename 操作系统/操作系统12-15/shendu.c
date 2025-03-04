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



void dfs_dir(const char* path1)
{
    DIR *dirp;
    struct dirent *direntp;
    char path[1024];

    if ((dirp = opendir(path1)) == NULL) { // ��Ŀ¼
        printf("Open Directory %s Error: %s\n", path, strerror(errno));
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
            dfs_dir(path);
        }
    }
    closedir(dirp);
}

int main(int argc, char **argv) {
    int stats;
    if (argc != 2) { // �ж������Ƿ�Ϊ��������
        printf("Usage: %s filename\n\a", argv[0]);
        exit(1);
    }
    // �� argv[1] Ϊ�ļ�������ļ���С������޸�ʱ�䣩���� stats �����˳�
    if (((stats = get_file_size_time(argv[1])) == 0) || (stats == -1))
        exit(1);

    dfs_dir(argv[1]);
    exit(1);
}
