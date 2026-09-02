#include <iostream>
#include <format>
#include <string>

int main() {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    while (true) {
        std::cout << "$ ";
        std::string userInput{};
        std::getline(std::cin, userInput);
        if (userInput == "exit")
            break;
        if (userInput.empty())
            continue;
        if (userInput.starts_with("echo ")) {
            std::cout << userInput.substr(5) << '\n';
            continue;
        }
        if (userInput.starts_with("type ")) {
            std::string command = userInput.substr(5);
            if (command == "echo" || command == "exit" || command == "type") {
                std::cout << command << " is a shell builtin\n";
            } else {
                std::cout << command << ": not found\n";
            }
            continue;
        }
        std::cerr << std::format("{}: command not found\n", userInput);
    }
}
