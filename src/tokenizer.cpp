#include "tokenizer.hpp"
#include "shell_types.hpp"

#include <iostream>

namespace {
TokenKind kindOf(std::string_view token) {
  if (token == "|")
    return TokenKind::Pipe;
  if (token == "&&")
    return TokenKind::AndAnd;
  if (token == ">" || token == "<")
    return TokenKind::Redirection;
  return TokenKind::Word;
}
} // namespace

Tokens tokenize(std::string_view command) {
  Tokens tokens;
  std::string current;
  char current_quote = '\0';

  auto addToken = [&] {
    if (current.empty())
      return;
    tokens.push_back({current, kindOf(current), tokens.size()});
    current.clear();
  };

  for (char character : command) {
    if (current_quote != '\0') {
      if (character == current_quote) {
        current_quote = '\0';
      } else {
        current += character;
      }
      continue;
    }
    if (character == '\'' || character == '"') {
      current_quote = character;
      continue;
    }

    if (character == ' ' || character == '\t') {
      addToken();
      continue;
    }
    current += character;
  }
  if (current_quote != '\0') {
    std::cerr << "Unterminated quote\n";
    return {};
  }

  addToken();
  return tokens;
}
