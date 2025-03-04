#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void execute_command(const std::string& command) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        char* args[3];
        if (command == "ls") {
            args[0] = (char*)"./my_ls"; // Path to my_ls
            args[1] = nullptr;
        } else if (command.rfind("echo ", 0) == 0) {
            args[0] = (char*)"./my_echo";
            args[1] = (char*)command.substr(5).c_str();
            args[2] = nullptr;
        } else {
            std::cerr << "Command not found: " << command << std::endl;
            return;
        }
        execvp(args[0], args);
        perror("execvp failed");
        exit(1);
    } else if (pid < 0) {
        std::cerr << "Fork failed" << std::endl;
    } else {
        // Parent process
        wait(nullptr);
    }
}

int main() {
    std::string command;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);
        if (command == "exit") break;
        execute_command(command);
    }
    return 0;
}
