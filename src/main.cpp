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
    if (userInput == "exit")
      break;
    if(userInput.empty())
      continue;
    if (userInput.starts_with("echo ")) {
      std::cout << userInput.substr(5) << '\n';
      continue;
}
    std::cerr << std::format("{}: command not found\n", userInput);
  }
}
