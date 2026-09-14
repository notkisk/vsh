#include "path.hpp"

#include <cstdlib>
#include <unistd.h>

std::vector<std::string> retrievePath() {
  const char *environmentPath = std::getenv("PATH");
  if (environmentPath == nullptr)
    return {};

  std::vector<std::string> paths;
  std::string path{environmentPath};
  std::size_t start = 0;
  for (std::size_t end = 0; end <= path.size(); ++end) {
    if (end == path.size() || path[end] == ':') {
      paths.push_back(path.substr(start, end - start));
      start = end + 1;
    }
  }
  return paths;
}

std::optional<std::string>
findExecutable(std::string_view command,
               const std::vector<std::string> &paths) {
  for (const auto &directory : paths) {
    std::string fullPath = directory.empty()
                               ? std::string{command}
                               : directory + "/" + std::string{command};
    if (access(fullPath.c_str(), X_OK) == 0)
      return fullPath;
  }
  return std::nullopt;
}
