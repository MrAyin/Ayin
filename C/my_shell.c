#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void execute_command(char *command) {
    char *args[100];
    int i = 0;

    // 分割命令和参数
    args[i] = strtok(command, " ");
    while (args[i] != NULL) {
        i++;
        args[i] = strtok(NULL, " ");
    }

    // 检查命令并执行
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
        // 创建子进程来执行其他命令
        pid_t pid = fork();
        if (pid == 0) 
        {
            // 子进程执行
             if(execvp(args[0], args) == -1)
                fprintf(stderr, "Command not found: %s\n", args[0]);
             else if (strcmp(args[0], "exit") == 0) {
                exit(0);
            }
            perror("Execution failed");
            exit(1);
        } 
        else if (pid > 0) {
            wait(NULL);  // 等待子进程结束
        } else {
            perror("Fork failed");
        }
    }
}

int main() {
    setenv("PATH", "./:" getenv("PATH"), 1);
    char command[1024];
    printf("请输入myls，myls-l等命令，输入exit退出shell\n");

    while (1) {
        printf("shell> ");
        if (fgets(command, sizeof(command), stdin) == NULL) break;

        // 移除换行符
        command[strcspn(command, "\n")] = 0;

        if (strlen(command) > 0) {
            execute_command(command);
        }
    }

    return 0;
}
