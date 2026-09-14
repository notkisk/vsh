#ifndef BUILTINTS_HPP
#define BUILTINTS_HPP

#include "shell_types.hpp"

#include <string>
#include <vector>

using BuiltinHandler = bool (*)(const Tokens &,
                                const std::vector<std::string> &);

bool runEcho(const Tokens &tokens, const std::vector<std::string> &paths);
bool runCd(const Tokens &tokens, const std::vector<std::string> &paths);
bool runPwd(const Tokens &tokens, const std::vector<std::string> &paths);
bool runType(const Tokens &tokens, const std::vector<std::string> &paths);
#endif
