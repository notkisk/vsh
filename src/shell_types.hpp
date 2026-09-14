#pragma once

#include <cstddef>
#include <string>
#include <vector>

enum class TokenKind { Word, Pipe, Redirection, AndAnd };

struct Token {
  std::string value;
  TokenKind kind{TokenKind::Word};
  std::size_t index{};
};

using Tokens = std::vector<Token>;
