#include <stdio.h>
#include <unistd.h>

int main() {
    int pid1, pid2, i;

    // 创建第一个子进程
    while ((pid1 = fork()) == -1);

    if (pid1 == 0) {
        // 子进程1
        for (i = 0; i < 10; i++) {
            printf("child%d\n", i);
        }
    } else {
        // 父进程
        // 创建第二个子进程
        while ((pid2 = fork()) == -1);

        if (pid2 == 0) {
            // 子进程2
            for (i = 0; i < 10; i++) {
                printf("son%d\n", i);
            }
        } else {
            // 父进程继续执行
            for (i = 0; i < 10; i++) {
                printf("daughter%d\n", i);
            }
        }
    }

    return 0;
}
