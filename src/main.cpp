#include "builtins.hpp"
#include "path.hpp"
#include "process.hpp"
#include "tokenizer.hpp"

#include <format>
#include <iostream>

namespace {
bool isBuiltin(std::string_view command) {
  return command == "cd" || command == "echo" || command == "exit" ||
         command == "pwd" || command == "type";
}

void runType(const Tokens& tokens, const std::vector<std::string>& paths) {
  if (tokens.size() < 2) {
    std::cout << "type: missing argument\n";
    return;
  }
  const auto& target = tokens[1].value;
  if (isBuiltin(target)) {
    std::cout << target << " is a shell builtin\n";
  } else if (auto executable = findExecutable(target, paths)) {
    std::cout << std::format("{} is {}\n", target, *executable);
  } else {
    std::cout << target << ": not found\n";
  }
}
}

int main() {
  const auto paths = retrievePath();
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  for (std::string input; std::cout << "$ ", std::getline(std::cin, input);) {
    const auto tokens = tokenize(input);
    if (tokens.empty()) continue;

    const auto& command = tokens[0].value;
    if (command == "exit") break;
    if (command == "echo") {
      runEcho(tokens);
    } else if (command == "cd") {
      runCd(tokens);
    } else if (command == "pwd") {
      runPwd(tokens);
    } else if (command == "type") {
      runType(tokens, paths);
    } else if (auto executable = findExecutable(command, paths)) {
      if (executeCommand(*executable, tokens) == -1)
        std::cerr << "failed to execute " << command << '\n';
    } else {
      std::cerr << std::format("{}: command not found\n", command);
    }
  }
}
