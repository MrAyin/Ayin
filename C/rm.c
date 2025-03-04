#include <stdio.h>
#include <stdlib.h>

int my_rm(const char *filename) {
    // ³¢ÊÔÉ¾³ýÎÄ¼þ
    if (remove(filename) != 0) {
        perror("remove");
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_name>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *file_name = argv[1];

    if (my_rm(file_name) == -1) {
        return EXIT_FAILURE;
    }

    printf("File '%s' removed successfully.\n", file_name);
    return EXIT_SUCCESS;
}