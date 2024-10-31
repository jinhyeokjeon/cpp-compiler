#include <iostream>
#include <string>
#include <vector>
#include "Token.h"
using namespace std;

enum class CharType {
  Unknown,
  WhiteSpace,
  NumberLiteral,
  StringLiteral,
  IdentifierAndKeyword,
  OperatorAndPunctuator,
};

CharType getCharType(char c);
bool isCharType(char c, CharType type);
Token scanNumberLiteral();
Token scanStringLiteral();
Token scanIdentifierAndKeyword();
Token scanOperatorAndPunctuator();

static string::iterator current;

vector<Token> scan(string sourceCode) {
  vector<Token> result;
  sourceCode += '\0';
  current = sourceCode.begin();
  while (*current != '\0') {
    switch (getCharType(*current))
    {
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
      result.push_back(scanOperatorAndPunctuator());
      break;
    }
    default: {
      cout << *current << " 사용할 수 없는 문자입니다.";
      exit(1);
    }
    }
  }
  result.push_back({ Kind::EndOfTokenList });
  return result;
}
CharType getCharType(char c) {
  if (c == ' ' || c == '\t' || c == '\n') {
    return CharType::WhiteSpace;
  }
  if ('0' <= c && c <= '9') {
    return CharType::NumberLiteral;
  }
  if (c == '\"') {
    return CharType::StringLiteral;
  }
  if ('a' <= c && c <= 'z' || 'A' <= c && c <= 'Z') {
    return CharType::IdentifierAndKeyword;
  }
  if (c == '=' || c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
    c == '!' || c == '<' || c == '>' || c == ',' || c == ':' || c == ';' ||
    c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']') {
    return CharType::OperatorAndPunctuator;
  }
  return CharType::Unknown;
}
bool isCharType(char c, CharType type) {
  switch (type) {
  case CharType::NumberLiteral: {
    return '0' <= c && c <= '9';
  }
  case CharType::StringLiteral: {
    return 32 <= c && c <= 126 && c != '\"'; // 아스키 코드에서의 출력 가능한 문자
  }
  case CharType::IdentifierAndKeyword: {
    return
      '0' <= c && c <= '9' ||
      'a' <= c && c <= 'z' ||
      'A' <= c && c <= 'Z';
  }
  case CharType::OperatorAndPunctuator: {
    return (c == '=' || c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
      c == '!' || c == '<' || c == '>' || c == ',' || c == ':' || c == ';' ||
      c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']');
  }
  default: {
    return false;
  }
  }
}
Token scanNumberLiteral() {
  string str;
  while (isCharType(*current, CharType::NumberLiteral))
    str += *current++;
  if (*current == '.') {
    str += *current++;
    while (isCharType(*current, CharType::NumberLiteral))
      str += *current++;
  }
  return Token{ Kind::NumberLiteral, str };
}
Token scanStringLiteral() {
  string str;
  ++current;
  while (isCharType(*current, CharType::StringLiteral))
    str += *current++;
  if (*current != '\"') {
    cout << "문자열 종료 문자가 없습니다.";
    exit(1);
  }
  ++current;
  return Token{ Kind::StringLiteral, str };
}
Token scanIdentifierAndKeyword() {
  string str;
  while (isCharType(*current, CharType::IdentifierAndKeyword))
    str += *current++;
  Kind kind = toKind(str);
  if (kind == Kind::Unknown)
    kind = Kind::Identifier;
  return Token{ kind, str };
}
Token scanOperatorAndPunctuator() {
  string str;
  while (isCharType(*current, CharType::OperatorAndPunctuator))
    str += *current++;
  while (!str.empty() && toKind(str) == Kind::Unknown) {
    str.pop_back();
    --current;
  }
  if (str.empty()) {
    cout << *current << " 사용할 수 없는 문자입니다";
    exit(1);
  }
  return Token{ toKind(str), str };
}