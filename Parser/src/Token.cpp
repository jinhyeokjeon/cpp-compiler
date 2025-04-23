#include <map>
#include <iomanip>
#include "Token.h"

using namespace std;

static map<string, Kind> stringToKind = {
  {"#Unknown",    Kind::Unknown},
  {"#EndOfTokenList", Kind::EndOfTokenList},

  {"null",        Kind::NullLiteral},
  {"true",        Kind::TrueLiteral},
  {"false",       Kind::FalseLiteral},
  {"#Number",     Kind::NumberLiteral},
  {"#String",     Kind::StringLiteral},
  {"#Identifier", Kind::Identifier},

  {"func",        Kind::Function},
  {"return",      Kind::Return},
  {"var",         Kind::Variable},
  {"for",         Kind::For},
  {"break",       Kind::Break},
  {"continue",    Kind::Continue},
  {"if",          Kind::If},
  {"elif",        Kind::Elif},
  {"else",        Kind::Else},
  {"print",       Kind::Print},
  {"printLine",   Kind::PrintLine},

  {"//",          Kind::Comment},
  {"++",          Kind::Increase},
  {"--",          Kind::Decrease},

  {"and",         Kind::LogicalAnd},
  {"or",          Kind::LogicalOr},

  {"=",           Kind::Assignment},

  {"+",           Kind::Add},
  {"-",           Kind::Subtract},
  {"*",           Kind::Multiply},
  {"/",           Kind::Divide},
  {"%",           Kind::Modulo},

  {"==",          Kind::Equal},
  {"!=",          Kind::NotEqual},
  {"<",           Kind::LessThan},
  {">",           Kind::GreaterThan},
  {"<=",          Kind::LessOrEqual},
  {">=",          Kind::GreaterOrEqual},

  {",",           Kind::Comma},
  {":",           Kind::Colon},
  {";",           Kind::Semicolon},
  {"(",           Kind::LeftParen},
  {")",           Kind::RightParen},
  {"{",           Kind::LeftBrace},
  {"}",           Kind::RightBrace},
  {"[",           Kind::LeftBracket},
  {"]",           Kind::RightBracket},
};

static map<Kind, string> kindToString = []() {
  map<Kind, string> result;
  for (auto& [key, value] : stringToKind)
    result[value] = key;
  return result;
  }();

auto toKind(string str) -> Kind {
  if (stringToKind.count(str)) {
    return stringToKind.at(str);
  }
  return Kind::Unknown;
}
auto toString(Kind type) -> string {
  if (kindToString.count(type)) {
    return kindToString.at(type);
  }
  return "";
}
auto operator<<(ostream& stream, Token& token) -> ostream& {
  return stream << setw(12) << left << toString(token.kind) << token.str;
}