#ifndef PROCESS_HPP
#define PROCESS_HPP

#include "shell_types.hpp"

#include <string_view>

int executeCommand(std::string_view path, const Tokens &tokens);
#endif
