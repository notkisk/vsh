#include <iostream>
#include <format>
#include <string>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  while(true){
    std::cout << "$ ";
    std::string userInput {};
    std::getline(std::cin, userInput);
    if(userInput.empty())
      continue;
    std::cerr << std::format("{}: command not found\n", userInput);
    continue;
  }
}
