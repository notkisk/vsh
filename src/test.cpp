#include <iostream>
#include <unistd.h>

int main() {
    std::cout << "before fork\n";

    pid_t pid = fork();

    if (pid == -1) {
        std::cerr << "fork failed\n";
        return 1;
    }

    if (pid == 0) {
        std::cout << "I am the child\n";
        const char* path = "/usr/bin/ls";

        char command[] = "ls";
        char* argv[] = {command, nullptr};

        execv(path, argv);
    } else {
        std::cout << "I am the parent\n";
    }
}
