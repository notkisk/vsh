#include <cstdlib>
#include <ios>
#include <iostream>
#include <format>
#include <string>
#include <vector>
#include <string_view>
#include <unistd.h>


namespace MAIN{


// #define DEBUG_MODE 

#ifdef DEBUG_MODE
    #define DEBUG_PRINT(msg) std::cout << "[DEBUG] " << msg << '\n';
#else
    #define DEBUG_PRINT(msg)
#endif

  struct Path_Obj {
    const std::string_view ex_path;
    bool excute_permission = false; // set to false by default
  };

  enum TokenKind{
    WORD,
    PIPE,
    REDIRECTION,
    AND_AND
  };
  struct Token{
    std::string m_token;
    TokenKind m_kind = TokenKind::WORD;
    std::size_t m_index;
  };

  bool isWhiteSpace(std::string_view character){
    return character == " "? true : false; // this might seem silly now, but later it will come in handy when deciding wheither we cosider certain characters
    // as white space like the tab! 
  }

  bool checkExcuteAccessFromPath(const char* pathName) {
    return access(pathName, X_OK) == 0;
  }

  std::vector<Path_Obj> retrievePath() {
    const char* path = std::getenv("PATH");
    const std::string_view path_view{path};
    std::size_t start{0};
    std::vector<Path_Obj> paths;

    for (std::size_t i = 0; i <= path_view.size(); ++i) {
      if (i == path_view.size() || path_view[i] == ':') {
        std::string_view path_part = path_view.substr(start, i - start);

        paths.push_back(
          Path_Obj{
            path_part
          }
        );

        start = i + 1;
      }
    }

    return paths;
  }
  /*
  * we need a command parser! it takes a sequence of characters space seperated, and it tries to determine if 
  * the command is an excutable command in PATH, if yes then it intrepret the arguments and pass them as arguments to that command!
  * and excutes that command!
  * we will start simple, take a sequence of words or characters and space seperate them, and do some preprocessing like removing the extra spaces 
  * what i will do is, decompose this problem into multiple indipendet building blocks, starting with the one that decompose the command into tokens
  * we will call it a tokenizer! because why not
  */


std::vector<Token> commandTokenizer(const std::string_view command) {
    std::vector<Token> tokens;
    std::size_t start = 0;
    std::size_t i = 0;
    std::size_t length = command.length();

    while (i <= length) {
        // A word ends if we hit a whitespace OR reach the very end of the string
        if (i == length || isWhiteSpace(command.substr(i,1))) {
            // Only push a token if it contains actual characters (ignores consecutive spaces)
            if (i > start) {
                Token token; 
                token.m_token = command.substr(start, i - start);
                token.m_kind = TokenKind::WORD; // Fixed scope resolution operator (::)
                token.m_index = 0;              // Kept as requested
                tokens.push_back(token);
            }
            start = i + 1;
        }
        ++i;
    }
  return tokens;
}

  int main() {
      std::cout << std::unitbuf;
      std::cerr << std::unitbuf;

      auto paths = retrievePath();

      while (true) {
          std::cout << "$ ";

          std::string userInput{};
          std::getline(std::cin, userInput);

          if (userInput == "exit")
              break;

          if (userInput.empty())
              continue;

          // This is just for now, later we need a more rigorous parsing,
          // a function that strips away spaces and new lines
          // for better command interpretation

          if (userInput == "echo" ||
              userInput.starts_with("echo ")) {
              if (userInput == "echo") {
                  std::cout << '\n';
              } else {
                  std::cout << userInput.substr(5) << '\n';
              }
              continue;
          }
          if (userInput.starts_with("type ")) {
              std::string command = userInput.substr(5);

              if (command == "echo" ||
                  command == "exit" ||
                  command == "type") {
                  std::cout << command << " is a shell builtin\n";
              } else {
                  bool found{false};
                  for (const auto& path : paths) {
                      std::string full_path =
                          std::format("{}/{}", path.ex_path, command);

                      if (checkExcuteAccessFromPath(full_path.c_str())) {
                          path.excute_permission = true;
                          std::cout << std::format(
                              "{} is {}\n",
                              command,
                              full_path
                          );
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

}
