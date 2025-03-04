#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

int main(int argc, char **argv) {
    int fd;
    char buffer[BUFFER_SIZE];

    if (argc != 2) {
        fprintf(stderr, "Usage: %s outfilename\n\a", argv[0]);
        exit(1);
    }

    if ((fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1) //ֻд��ʽ�������ļ�������ļ����ڣ�������ļ����ݣ��û����Զ�д
    {
        fprintf(stderr, "Open %s Error: %s\n\a", argv[1], strerror(errno));
        exit(1);
    }

    printf("Now, please input string\n");
    printf("(To quit use CTRL+D)\n");

    while (1) {
        fgets(buffer, BUFFER_SIZE, stdin);
        if (feof(stdin))//����Ƿ񵽴��������Ľ�����־�����û��� CTRL+D����EOF
            break;
        write(fd, buffer, strlen(buffer));
    }

    close(fd);
    exit(0);
}
