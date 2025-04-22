#pragma once

#include <iostream>
#include <string>
using namespace std;

enum class Kind {
  Unknown, EndOfTokenList,
  NullLiteral,
  TrueLiteral, FalseLiteral,
  NumberLiteral, StringLiteral,
  Identifier,

  Function, Return,
  Variable,
  For, Break, Continue,
  If, Elif, Else,
  Print, PrintLine,

  Comment,

  LogicalAnd, LogicalOr,
  Assignment,
  Add, Subtract,
  Multiply, Divide, Modulo,
  Equal, NotEqual,
  LessThan, GreaterThan,
  LessOrEqual, GreaterOrEqual,

  Comma, Colon, Semicolon,
  LeftParen, RightParen,
  LeftBrace, RightBrace,
  LeftBracket, RightBracket,
};

struct Token {
  Kind kind = Kind::Unknown;
  string str;
};

auto toKind(string) -> Kind;
auto toString(Kind) -> string;
auto operator<<(ostream&, Token&) -> ostream&;