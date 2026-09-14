#include "process.hpp"

#include <sys/wait.h>
#include <unistd.h>

#include <string>
#include <vector>

int executeCommand(std::string_view path, const Tokens& tokens) {
  std::vector<std::string> arguments;
  std::vector<char*> argv;
  arguments.reserve(tokens.size());
  argv.reserve(tokens.size() + 1);
  for (const auto& token : tokens) arguments.push_back(token.value);
  for (auto& argument : arguments) argv.push_back(argument.data());
  argv.push_back(nullptr);

  const pid_t pid = fork();
  if (pid == -1) return -1;
  if (pid == 0) {
    execv(std::string{path}.c_str(), argv.data());
    perror("execv");
    _exit(1);
  }
  return waitpid(pid, nullptr, 0) == -1 ? -1 : 0;
}
