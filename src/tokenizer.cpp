#include "tokenizer.hpp"

namespace {
TokenKind kindOf(std::string_view token) {
  if (token == "|") return TokenKind::Pipe;
  if (token == "&&") return TokenKind::AndAnd;
  if (token == ">" || token == "<") return TokenKind::Redirection;
  return TokenKind::Word;
}
}

Tokens tokenize(std::string_view command) {
  Tokens tokens;
  std::string current;
  bool quoted = false;

  auto addToken = [&] {
    if (current.empty()) return;
    tokens.push_back({current, kindOf(current), tokens.size()});
    current.clear();
  };

  for (char character : command) {
    if (character == '\'' || character == '"') {
      quoted = !quoted;
    } else if ((character == ' ' || character == '\t') && !quoted) {
      addToken();
    } else {
      current += character;
    }
  }
  addToken();
  return tokens;
}
