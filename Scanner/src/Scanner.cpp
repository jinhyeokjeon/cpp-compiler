#include "Scanner.h"

static string::iterator current;

auto scan(string sourceCode) -> vector<Token> {
  vector<Token> result;
  sourceCode += '\0';
  current = sourceCode.begin();
  while (*current != '\0') {
    switch (getCharType(*current)) {
    case CharType::WhiteSpace: {
      ++current;
      break;
    }
    case CharType::NumberLiteral: {
      result.push_back(scanNumberLiteral());
      break;
    }
    case CharType::StringLiteral: {
      result.push_back(scanStringLiteral());
      break;
    }
    case CharType::IdentifierAndKeyword: {
      result.push_back(scanIdentifierAndKeyword());
      break;
    }
    case CharType::OperatorAndPunctuator: {
      auto token = scanOperatorAndPunctuator();
      if (token.kind != Kind::Comment) {
        result.push_back(token);
      }
      break;
    }
    default: {
      cout << *current << " is not usable" << endl;
      exit(1);
    }
    }
  }
  result.push_back({ Kind::EndOfTokenList });
  return result;
}

auto scanNumberLiteral() -> Token {
  string str;
  while (isCharType(*current, CharType::NumberLiteral)) {
    str += *current++;
  }
  if (*current == '.') {
    str += *current++;
    while (isCharType(*current, CharType::NumberLiteral)) {
      str += *current++;
    }
  }
  return Token{ Kind::NumberLiteral, str };
}
auto scanStringLiteral() -> Token {
  string str;
  ++current;
  while (isCharType(*current, CharType::StringLiteral)) {
    str += *current++;
  }
  if (*current != '"') {
    cout << "There is no string terminator" << endl;
    exit(1);
  }
  ++current;
  return Token{ Kind::StringLiteral, str };
}
auto scanIdentifierAndKeyword() -> Token {
  string str;
  while (isCharType(*current, CharType::IdentifierAndKeyword)) {
    str += *current++;
  }
  Kind kind = toKind(str);
  if (kind == Kind::Unknown) {
    return Token{ Kind::Identifier, str };
  }
  return Token{ kind, str };
}
auto scanOperatorAndPunctuator() -> Token {
  string str;
  while (isCharType(*current, CharType::OperatorAndPunctuator)) {
    str += *current++;
  }
  while (!str.empty() && toKind(str) == Kind::Unknown) {
    str.pop_back();
    --current;
  }
  if (str.empty()) {
    cout << *current << " is not usable" << endl;
    exit(1);
  }
  if (str == "//") {
    while (!(*current == '\n' || *current == '\0')) {
      ++current;
    }
  }
  return Token{ toKind(str), str };
}

static const string oper_punc = "=+-*/%!<>,:;(){}[]";
auto getCharType(char c) -> CharType {
  if (' ' == c || '\t' == c || '\r' == c || '\n' == c) {
    return CharType::WhiteSpace;
  }
  if ('0' <= c && c <= '9') {
    return CharType::NumberLiteral;
  }
  if (c == '"') {
    return CharType::StringLiteral;
  }
  if ('a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || '_' == c) {
    return CharType::IdentifierAndKeyword;
  }
  if (oper_punc.find(c) != string::npos) {
    return CharType::OperatorAndPunctuator;
  }
  return CharType::Unknown;
}
auto isCharType(char c, CharType type) -> bool {
  switch (type) {
  case CharType::NumberLiteral: {
    return '0' <= c && c <= '9';
  }
  case CharType::StringLiteral: {
    return 32 <= c && c <= 126 && c != '"'; // 아스키 코드에서의 출력 가능한 문자
  }
  case CharType::IdentifierAndKeyword: {
    return ('0' <= c && c <= '9') || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('_' == c);
  }
  case CharType::OperatorAndPunctuator: {
    return oper_punc.find(c) != string::npos;
  }
  default: {
    return false;
  }
  }
}