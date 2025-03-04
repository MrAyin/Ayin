#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void execute_command(char *command) {
    char *args[100];
    int i = 0;

    // �ָ�����Ͳ���
    args[i] = strtok(command, " ");
    while (args[i] != NULL) {
        i++;
        args[i] = strtok(NULL, " ");
    }

    // ������ִ��
    if (strcmp(args[0], "mycd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "mycd: missing argument\n");
        } else {
            printf("Changing directory to: %s\n", args[1]);
            if (chdir(args[1]) != 0) {
                perror("mycd failed");
		
            }
        }
    } 
	else {
        // �����ӽ�����ִ����������
        pid_t pid = fork();
        if (pid == 0) 
        {
            // �ӽ���ִ��
             if(execvp(args[0], args) == -1)
                fprintf(stderr, "Command not found: %s\n", args[0]);
             else if (strcmp(args[0], "exit") == 0) {
                exit(0);
            }
            perror("Execution failed");
            exit(1);
        } 
        else if (pid > 0) {
            wait(NULL);  // �ȴ��ӽ��̽���
        } else {
            perror("Fork failed");
        }
    }
}

int main() {
    setenv("PATH", "./:" getenv("PATH"), 1);
    char command[1024];
    printf("������myls��myls-l���������exit�˳�shell\n");

    while (1) {
        printf("shell> ");
        if (fgets(command, sizeof(command), stdin) == NULL) break;

        // �Ƴ����з�
        command[strcspn(command, "\n")] = 0;

        if (strlen(command) > 0) {
            execute_command(command);
        }
    }

    return 0;
}
