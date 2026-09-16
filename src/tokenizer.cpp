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
  bool token_started = false;

  auto addToken = [&] {
    if (!token_started)
      return;
    tokens.push_back({current, kindOf(current), tokens.size()});
    current.clear();
    token_started = false;
  };

  for (std::size_t i = 0; i < command.size(); ++i) {
    const char character = command[i];
    if (current_quote != '\0') {
      if (character == current_quote) {
        current_quote = '\0';
      } else if (current_quote == '"' && character == '\\' &&
                 i + 1 < command.size() &&
                 (command[i + 1] == '"' || command[i + 1] == '\\')) {
        current += command[++i];
      } else {
        current += character;
      }
      token_started = true;
      continue;
    }
    if (character == '\'' || character == '"') {
      current_quote = character;
      token_started = true;
      continue;
    }
    if (character == '\\') {
      token_started = true;
      if (i + 1 < command.size())
        current += command[++i];
      else
        current += character;
      continue;
    }

    if (character == ' ' || character == '\t') {
      addToken();
      continue;
    }
    token_started = true;
    current += character;
  }
  if (current_quote != '\0') {
    std::cerr << "Unterminated quote\n";
    return {};
  }

  addToken();
  return tokens;
}
