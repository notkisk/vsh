#include "builtins.hpp"
#include "path.hpp"

#include <array>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <iostream>
#include <string_view>
#include <unistd.h>

bool runEcho(const Tokens &tokens, const std::vector<std::string> &) {
  for (std::size_t i = 1; i < tokens.size(); ++i) {
    if (i > 1)
      std::cout << ' ';
    std::cout << tokens[i].value;
  }
  std::cout << '\n';
  return true;
}

bool runCd(const Tokens &tokens, const std::vector<std::string> &) {
  if (tokens.size() > 2) {
    std::cerr << "cd: too many arguments\n";
    return true;
  }
  const char *target =
      tokens.size() == 1 ? std::getenv("HOME") : tokens[1].value.c_str();
  if (target == nullptr) {
    std::cerr << "cd: HOME not set\n";
    return true;
  }
  std::string expanded{target};
  if (expanded == "~")
    expanded = std::getenv("HOME") ?: "";
  else if (expanded.starts_with("~/"))
    expanded = std::string{std::getenv("HOME") ?: ""} + expanded.substr(1);
  if (chdir(expanded.c_str()) != 0) {
    std::cerr << "cd: " << expanded << ": ";
    perror("");
  }
  return true;
}

bool runPwd(const Tokens &tokens, const std::vector<std::string> &) {
  bool physical =
      tokens.size() > 1 && (tokens[1].value == "-P" || tokens[1].value == "-p");
  if (tokens.size() > 1 && !physical && tokens[1].value != "-L" &&
      tokens[1].value != "-l") {
    std::cerr << "pwd: invalid option: " << tokens[1].value << '\n';
    return true;
  }
  try {
    auto path =
        physical ? std::filesystem::canonical(std::filesystem::current_path())
                 : std::filesystem::absolute(std::filesystem::current_path());
    std::cout << path.string() << '\n';
  } catch (const std::filesystem::filesystem_error &error) {
    std::cerr << "pwd: " << error.what() << '\n';
  }
  return true;
}

bool runType(const Tokens &tokens, const std::vector<std::string> &paths) {
  if (tokens.size() < 2) {
    std::cout << "type: missing argument\n";
    return true;
  }
  const auto &target = tokens[1].value;
  static constexpr std::array<std::string_view, 5> builtins{
      "cd", "echo", "exit", "pwd", "type"};
  if (std::ranges::find(builtins, target) != builtins.end()) {
    std::cout << target << " is a shell builtin\n";
  } else if (auto executable = findExecutable(target, paths)) {
    std::cout << std::format("{} is {}\n", target, *executable);
  } else {
    std::cout << target << ": not found\n";
  }
  return true;
}
