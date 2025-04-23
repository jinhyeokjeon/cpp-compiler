#include <iostream>
#include <string>
#include <vector>
#include "Token.h"
using namespace std;

enum class CharType {
  Unknown,                // 사용할 수 없는 문자
  WhiteSpace,             // 공백, 탭, 개행
  NumberLiteral,          // 숫자 리터럴
  StringLiteral,          // 문자열 리터럴
  IdentifierAndKeyword,   // 식별자, 키워드
  OperatorAndPunctuator,  // 연산자, 구분자
};

static string::iterator current;
auto scan(string&) -> vector<Token>;

static auto scanNumberLiteral() -> Token;
static auto scanStringLiteral() -> Token;
static auto scanIdentifierAndKeyword() -> Token;
static auto scanOperatorAndPunctuator() -> Token;

static const string oper_punc = "=+-*/%!<>,:;(){}[]";
static auto getCharType(char) -> CharType;
static auto isCharType(char, CharType) -> bool;

auto scan(string& sourceCode) -> vector<Token> {
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
      Token token = scanOperatorAndPunctuator();
      if (!(token.kind == Kind::Comment || token.kind == Kind::Comments)) {
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

static auto scanNumberLiteral() -> Token {
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
static auto scanStringLiteral() -> Token {
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
static auto scanIdentifierAndKeyword() -> Token {
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
static auto scanOperatorAndPunctuator() -> Token {
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
  else if (str == "/*") {
    while (true) {
      if (*current == '\0') {
        cout << "*/ is required." << endl;
        exit(1);
      }
      if (*current == '*' && *(current + 1) == '/') {
        current += 2;
        break;
      }
      ++current;
    }
  }
  return Token{ toKind(str), str };
}

static auto getCharType(char c) -> CharType {
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
static auto isCharType(char c, CharType type) -> bool {
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