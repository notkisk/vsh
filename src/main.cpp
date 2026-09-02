#include <cstdlib>
#include <ios>
#include <iostream>
#include <format>
#include <string>
#include <cstdint>
#include <vector>
#include <string_view>
#include <unistd.h>

// #define DEBUG_MODE 

#ifdef DEBUG_MODE
  #define DEBUG_PRINT(msg) std::cout << "[DEBUG] " << msg << '\n';
#else
  #define DEBUG_PRINT(msg)
#endif

struct Path_Obj {
  const std::string_view ex_path;
  const bool ex_permission;
};

bool checkExcuteAccessFromPath(const char* pathName) {
  return access(pathName, X_OK);
}

bool checkPath(std::string_view command) {
  const char* path = std::getenv("PATH");
  const std::string_view path_view{path};
  bool does_exits{path_view.find(command) != std::string_view::npos};
  std::cout << std::boolalpha;
  DEBUG_PRINT(path);
  DEBUG_PRINT(std::format("excutable exists {}", does_exits));
  DEBUG_PRINT(path);
  
  return does_exits;
}

std::pair<std::vector<Path_Obj>, uint16_t> retrievePath() {
  const char* path = std::getenv("PATH");
  const std::string_view path_view{path};
  std::size_t start{0};
  std::vector<Path_Obj> paths;

  for (std::size_t i = 0; i <= path_view.size(); ++i) {
    if (i == path_view.size() || path_view[i] == ':') {
      std::string_view path_part = path_view.substr(start, i - start);
      paths.push_back(
        Path_Obj{
          path_part,
          checkExcuteAccessFromPath(std::string(path_part).c_str())
        }
      );
      start = i + 1;
    }
  }

  return {paths, paths.size() == 0 ? 0 : 1};
}


int main() {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;
    auto [paths, path_status] = retrievePath();
    while (true) {
        std::cout << "$ ";
        std::string userInput{};
        std::getline(std::cin, userInput);
        if (userInput == "exit")
            break;
        if (userInput.empty())
            continue;
        // This is just for now, later we need a more rigrouds parsing, a function that strips away spaces and new lines
    // for better command interpertation
        if (userInput == "echo" || userInput == "echo\n"  || userInput.starts_with("echo ")) {
            std::cout << userInput.substr(4 + (userInput.size() > 4 && userInput[4] == ' ')) << '\n';
            continue;
        }
        if (userInput.starts_with("type ")) {
            std::string command = userInput.substr(5);
            if (command == "echo" || command == "exit" || command == "type") {
                std::cout << command << " is a shell builtin\n";
            } else {
                bool found{false};
                for (const auto& path : paths) {
                    if (!path.ex_permission)
                        continue;
                    std::string full_path =
                        std::format("{}/{}", path.ex_path, command);
                    if (checkExcuteAccessFromPath(full_path.c_str()) == 0) {
                        std::cout << std::format("{} is {}\n", command, full_path);
                        found = true;
                        break;
                    }
                }
                if (!found)
                    std::cout << command << ": not found\n";
            }
            continue;
        }
        std::cerr << std::format("{}: command not found\n", userInput);
    }
}
