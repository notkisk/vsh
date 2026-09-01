#include <iostream>
#include <format>
#include <string>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::cout << "$ ";
  std::string userInput {};
  std::getline(std::cin>>std::ws, userInput);
  std::cerr << std::format("{}: command not found", userInput);
}
