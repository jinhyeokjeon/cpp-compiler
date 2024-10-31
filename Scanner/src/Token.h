#pragma once
#include <iostream>
#include <string>

using std::string;
using std::ostream;

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

Kind toKind(string);
string toString(Kind);

struct Token {
  Kind kind = Kind::Unknown;
  string str;
};

ostream& operator<<(ostream&, Token&);
