#pragma once

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

auto scan(string) -> vector<Token>;

auto scanNumberLiteral() -> Token;
auto scanStringLiteral() -> Token;
auto scanIdentifierAndKeyword() -> Token;
auto scanOperatorAndPunctuator() -> Token;

auto getCharType(char) -> CharType;
auto isCharType(char, CharType) -> bool;