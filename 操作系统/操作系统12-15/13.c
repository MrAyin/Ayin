#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#define NEWFILE (O_WRONLY | O_CREAT | O_TRUNC)

int main(void) {
    char buf1[] = {"abcdefghijk"};
    char buf2[] = {"1234567890"};
    int fd;
    int length;

    // 打开文件
    if ((fd = open("test", NEWFILE, 0600)) == -1) {
        printf("ERROR, open write file error: %s\n", strerror(errno));
        exit(255);
    }

    // 写入 buf1
    length = strlen(buf1);
    if (write(fd, buf1, length) != length) {
        printf("ERROR, write file failed: %s\n", strerror(errno));
        exit(255);
    }

    // 文件偏移
    if (lseek(fd, 80, SEEK_SET) == -1) {
        printf("ERROR, lseek failed: %s\n", strerror(errno));
        exit(255);
    }

    // 写入 buf2
    length = strlen(buf2);
    if (write(fd, buf2, length) != length) {
        printf("ERROR, write file failed: %s\n", strerror(errno));
        exit(255);
    }

    close(fd);
    return 0;
}
