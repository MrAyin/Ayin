#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory_name>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *dir_name = argv[1];

    // ���ĵ�ǰ����Ŀ¼
    if (chdir(dir_name) != 0) {
        perror("cd failed");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
