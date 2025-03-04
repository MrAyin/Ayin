#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void print_help() {
    printf("Welcome to MyShell! Here are the available commands:\n");
    printf("1. myls    - List files and directories\n");
    printf("2. myecho  - Print text to console\n");
    printf("3. mycat   - Display file content\n");
    printf("4. mymkdir - Create a directory\n");
    printf("5. myrm    - Delete a file\n");
    printf("6. mycd    - Change directory\n");
    printf("7. mypwd   - Show current directory\n");
    printf("8. mywc    - Word count\n");
    printf("9. mytouch   - Create a file\n");
    printf("Type 'ctrl+c' to leave the shell.\n");
}

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
            if (execvp(args[0], args) == -1)
                fprintf(stderr, "Command not found: %s\n", args[0]);
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
    // ��� /home/rose/exp2 �͵�ǰĿ¼�� PATH ��ͷ
    char new_path[1024];
    snprintf(new_path, sizeof(new_path), "/home/rose/exp2:./:%s", getenv("PATH"));
    setenv("PATH", new_path, 1);

    char command[1024];
    printf("������myls��myls-l���������exit�˳�shell,����help�鿴��ִ�������б�\n");
    

    while (1) {
        printf("2220223952_shell> ");
        if (fgets(command, sizeof(command), stdin) == NULL) break;

        // �Ƴ����з�
        command[strcspn(command, "\n")] = 0;

        // ����Ƿ����� exit
        if (strcmp(command, "exit") == 0) {
            printf("Exiting shell. Goodbye!\n");
            exit(0); // �˳�������
        }

        if (strcmp(input, "help") == 0) {
            print_help();
            continue;
        }

        if (strlen(command) > 0) {
            execute_command(command);
        }
    }

    return 0;
}
