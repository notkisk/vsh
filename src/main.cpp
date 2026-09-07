#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <ranges>
#include <optional>
#include <cassert>
#include <iostream>
#include <format>
#include <string>
#include <vector>
#include <string_view>
#include <unistd.h>
#include <tuple>
#include <sys/wait.h>
#include <cstring>

// #define DEBUG_MODE 

#ifdef DEBUG_MODE
    #define DEBUG_PRINT(msg) std::cout << "[DEBUG] " << msg << '\n';
#else
    #define DEBUG_PRINT(msg)
#endif

  struct Path_Obj {
    const std::string_view ex_path;
    bool execute_permission {false}; // set to false by default
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

bool isWhiteSpace(std::string_view character) {
    return character == " " || character == "\t";
}

bool checkExecuteAccessFromPath(const char* pathName) {
    return access(pathName, X_OK) == 0;
}

std::vector<Path_Obj> retrievePath() {
    const char* path = std::getenv("PATH");
    const std::string_view path_view{path};
    std::size_t start{0};
    std::vector<Path_Obj> paths;

    for (std::size_t i = 0; i <= path_view.size(); ++i) {
        if (i == path_view.size() || path_view[i] == ':') {
            std::string_view path_part =
                path_view.substr(start, i - start);

            paths.push_back(
                Path_Obj{path_part}
            );

            start = i + 1;
        }
    }

    return paths;
}

std::tuple<std::string_view, std::vector<Token>>
commandTokenizer(const std::string_view command) {
    std::vector<Token> tokens;
    std::size_t start = 0;
    std::size_t i = 0;
    std::size_t length = command.length();
    while (i <= length) {
        if (i == length || isWhiteSpace(command.substr(i, 1))) {

            if (i > start) {
                Token token;
                token.m_token = command.substr(start, i - start);

                if (token.m_token == "|") {
                    token.m_kind = TokenKind::PIPE;
                }
                else if (token.m_token == "&&") {
                    token.m_kind = TokenKind::AND_AND;
                }
                else if (token.m_token == ">" ||
                         token.m_token == "<") {
                    token.m_kind = TokenKind::REDIRECTION;
                }
                else {
                    token.m_kind = TokenKind::WORD;
                }

                token.m_index = tokens.size();

                tokens.push_back(token);
            }

            start = i + 1;
        }
        ++i;
    }
    return {command, tokens};
}

std::optional<std::string> findExcutable(
    std::string_view command,
    const std::vector<Path_Obj>& paths
) {
    for (const auto& path : paths) {

        std::string full_path =
            std::format("{}/{}", path.ex_path, command);
    #ifdef DEBUG_MODE
        std::cout << "checking: " << full_path << '\n';
    #endif
        if (checkExecuteAccessFromPath(full_path.c_str())) {
            return full_path;
        }
    }

    return std::nullopt;
}


void test_tokenizer(
    const std::string_view command,
    const std::vector<std::string_view>& expected
) {
    auto [raw_command, tokens] = commandTokenizer(command);
    DEBUG_PRINT(std::format("Raw Command: {}", raw_command));
    DEBUG_PRINT(std::format("Raw Command Size: {}", raw_command.size()));
    assert(tokens.size() == expected.size());
    std::string token_string;
    std::size_t index{};
    for (const auto& token : tokens) {
        token_string += std::format("{} ", token.m_token);
        assert(token.m_token == expected[index]);
        ++index;
    }
    DEBUG_PRINT(std::format(
        "number of tokens: {}. tokens {}",
        tokens.size(),
        token_string
    ));
}

std::vector<char*> tokens_to_argv(const std::vector<Token>& tokens){
// this one takes a series of tokens, and convert them to a c-string  vector of arguments

  assert (!tokens.empty());
  std::vector<char*> argv_vector;
  for (const auto& token : tokens){
    char* arg = new char [token.m_token.size() + 1];
    std::copy(token.m_token.begin(), token.m_token.end(), arg);
    arg[token.m_token.size()] = '\0';
    argv_vector.push_back(arg);
  }
  argv_vector.push_back(nullptr);
  return argv_vector;
} 

int excuteCommand(
    std::string_view path,
    std::vector<char*>& argv
) {
    pid_t pid = fork();
    if (pid == -1) {
        return -1;
    }
    if (pid == 0) {
        execv(path.data(), argv.data());
        perror("execv");
        _exit(1);
    }
    if (waitpid(pid, nullptr, 0) == -1){
      perror("waitpid");
      return -1;
    }
    
    for (char* arg: argv){
      if (arg != nullptr)
        delete [] arg;
  }
    return 0;
}

// pwd -L: Prints the symbolic path. 
// pwd -P: Prints the actual path.

std::int16_t cd_builtin(const char *path){
    if (path == nullptr){
        std::cerr << "cd: missing argument\n";
        return -1;
    }
    std::string_view path_view{path};

    if (path_view == "~"){
        const char* home{std::getenv("HOME")};

        if (home == nullptr){
            std::cerr << "cd: HOME not set\n";
            return -1;
        }
        if (chdir(home) == 0){
            return 0;
        }
        std::cerr << "cd: " << home << ": ";
        perror("");
        return -1;
    }
    if (path_view.starts_with("~/")){
        const char* home{std::getenv("HOME")};
        if (home == nullptr){
            std::cerr << "cd: HOME not set\n";
            return -1;
        }
        std::string expanded{
            std::format("{}{}", home, path_view.substr(1))
        };
        if (chdir(expanded.c_str()) == 0){
            return 0;
        }
        std::cerr << "cd: " << expanded << ": ";
        perror("");
        return -1;
    }
    if (chdir(path) == 0){
        return 0;
    }
    std::cerr << "cd: " << path << ": ";
    perror("");
    return -1;
}

// we will be using 
// char *getcwd(char *buf, size_t size);
std::optional<std::tuple<char*, std::int16_t>>
pwd_builtin(std::vector<char*> argv) noexcept
{
    std::int16_t status{};
    // argv[0] is "pwd"
    // argv[1] is the optional argument.
    bool isLogical{true}; // pwd defaults to logical
    if (argv.size() > 1 && argv[1] != nullptr) {
        std::string_view arg{argv[1]};
        if (arg == "-P" || arg == "-p") {
            isLogical = false;
        } else if (arg == "-L" || arg == "-l") {
            isLogical = true;
        } else {
            std::cerr << "pwd: invalid option: " << arg << '\n';
            status = -1;
            return std::nullopt;
        }
    }
    if (isLogical) {
        // TODO: eventually use the shell's logical PWD.
        std::string logical_path =
            std::filesystem::absolute(
                std::filesystem::current_path()
            ).string();

        char* buffer = static_cast<char*>(
            std::malloc(logical_path.size() + 1)
        );
        if (buffer == nullptr) {
            perror("pwd allocation failed");
            status = -1;
            return std::nullopt;
        }
        std::memcpy(
            buffer,
            logical_path.c_str(),
            logical_path.size() + 1
        );
        return std::make_tuple(buffer, status);
    }
    // Physical path.
    std::string physical_path =
        std::filesystem::canonical(
            std::filesystem::current_path()
        ).string();

    char* buffer = static_cast<char*>(
        std::malloc(physical_path.size() + 1)
    );

    if (buffer == nullptr) {
        perror("pwd allocation failed");
        status = -1;
        return std::nullopt;
    }

    std::memcpy(
        buffer,
        physical_path.c_str(),
        physical_path.size() + 1
    );

    return std::make_tuple(buffer, status);
}

// TODO: add a sophisticated token parser/intreperter, reads tokens types and content (and probably index) and decides how to excute it!


int main() {
 
    auto paths{retrievePath()};
    using namespace std::string_view_literals;

#ifdef DEBUG_MODE
    test_tokenizer(
        "echo arg1 arg2"sv,
        {"echo"sv, "arg1"sv, "arg2"sv}
    );

    test_tokenizer(
        "type  "sv,
        {"type"sv}
    );

    test_tokenizer(
        "exit       "sv,
        {"exit"sv}
    );

    test_tokenizer(
        "  echo arg1 arg2 arg3arg4  arg6"sv,
        {"echo"sv, "arg1"sv, "arg2"sv, "arg3arg4"sv, "arg6"sv}
    );
#endif

    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    while (true) {
        std::cout << "$ ";
        std::string userInput{};
        std::getline(std::cin, userInput);
        if (userInput.empty())
            continue;
        auto [raw_command, tokens] = commandTokenizer(userInput);
        if (tokens.empty())
            continue;
        const auto& command = tokens[0].m_token;
        if (command == "exit")
            break;
        if (command == "echo") {
            for (std::size_t i = 1; i < tokens.size(); ++i) {
                std::cout << tokens[i].m_token;

                if (i + 1 < tokens.size())
                    std::cout << ' ';
            }
            std::cout << '\n';
            continue;
        } else if (command == "pwd") {
            auto result = pwd_builtin(tokens_to_argv(tokens));

            if (result) {
                auto [path, status] = *result;
                std::cout << path << '\n';
                std::free(path);
            }
            continue;

        } else if (command == "cd") {
            if (tokens.size() == 1){
                const char* home{std::getenv("HOME")};
                if(home == nullptr){
                    std::cerr << "cd: HOME not set\n";
                }else{
                    cd_builtin(home);
                }
                continue;
            }
            if (tokens.size() > 2){
                std::cerr << "cd: too many arguments\n";
                continue;
            }
            cd_builtin(tokens[1].m_token.c_str());
            continue;
        }
        if (command == "type") {
            if (tokens.size() < 2) {
                std::cout << "type: missing argument\n";
                continue;
            }
            const auto& target = tokens[1].m_token;

            if (target == "echo" ||
                target == "exit" ||
                target == "type" || 
                target == "pwd" ||
                target == "cd") {

                std::cout << target
                          << " is a shell builtin\n";
            }
            else if (auto executable = findExcutable(target, paths)) {

                std::cout << std::format(
                    "{} is {}\n",
                    target,
                    *executable
                );
            }
            else {
                std::cout << target << ": not found\n";
            }
            continue;
        }
        // external command
        if (auto executable = findExcutable(command, paths)) {
            auto argv = tokens_to_argv(tokens);

            if (excuteCommand(*executable, argv) == -1) {
                std::cerr << "failed to execute " << command << '\n';
            }

            continue;
        }

        std::cerr << std::format(
            "{}: command not found\n",
            command
        );
    }
    return 0;
}
