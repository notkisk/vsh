#include "builtins.hpp"
#include "path.hpp"
#include "process.hpp"
#include "tokenizer.hpp"

#include <format>
#include <iostream>
#include <unordered_map>

namespace {
const std::unordered_map<std::string_view, BuiltinHandler> builtinRegistry{
    {"cd", runCd}, {"echo", runEcho}, {"pwd", runPwd}, {"type", runType}};
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
    if (auto builtin = builtinRegistry.find(command);
        builtin != builtinRegistry.end()) {
      builtin->second(tokens, paths);
    } else if (auto executable = findExecutable(command, paths)) {
      if (executeCommand(*executable, tokens) == -1)
        std::cerr << "failed to execute " << command << '\n';
    } else {
      std::cerr << std::format("{}: command not found\n", command);
    }
  }
}
