#include <stdio.h>
#include <unistd.h>

int main() {
    int pid1, pid2, i;

    // ������һ���ӽ���
    while ((pid1 = fork()) == -1);

    if (pid1 == 0) {
        // �ӽ���1
        for (i = 0; i < 10; i++) {
            printf("child%d\n", i);
        }
    } else {
        // ������
        // �����ڶ����ӽ���
        while ((pid2 = fork()) == -1);

        if (pid2 == 0) {
            // �ӽ���2
            for (i = 0; i < 10; i++) {
                printf("son%d\n", i);
            }
        } else {
            // �����̼���ִ��
            for (i = 0; i < 10; i++) {
                printf("daughter%d\n", i);
            }
        }
    }

    return 0;
}
