#include <iostream>
#include <set>
#include "Token.h"
#include "Node.h"

using namespace std;

static vector<Token>::iterator current;

static auto parseFunction() -> Function*;
static auto parseBlock() -> vector<Statement*>;
static auto parseVariable() -> Variable*;
static auto parseFor() -> For*;
static auto parseIf() -> If*;
static auto parsePrint() -> Print*;
static auto parseReturn() -> Return*;
static auto parseBreak() -> Break*;
static auto parseContinue() -> Continue*;
static auto parseExpressionStatement() -> ExpressionStatement*;
static auto parseExpression() -> Expression*;
static auto parseAssignment() -> Expression*;
static auto parseOr() -> Expression*;
static auto parseAnd() -> Expression*;
static auto parseRelational() -> Expression*;
static auto parseArithmetic1() -> Expression*;
static auto parseArithmetic2() -> Expression*;
static auto parseUnary() -> Expression*;
static auto parseOperand() -> Expression*;
static auto parseNullLiteral() -> Expression*;
static auto parseBooleanLiteral() -> Expression*;
static auto parseNumberLiteral() -> Expression*;
static auto parseStringLiteral() -> Expression*;
static auto parseListLiteral() -> Expression*;
static auto parseMapLiteral() -> Expression*;
static auto parseIdentifier() -> Expression*;
static auto parseInnerExpression() -> Expression*;
static auto parsePostfix(Expression*) -> Expression*;
static auto parseCall(Expression*) -> Expression*;
static auto parseElement(Expression*) -> Expression*;
static void skipCurrent();
static void skipCurrent(Kind);
static bool skipCurrentIf(Kind);

auto parse(vector<Token>& tokens) -> Program* {
  Program* result = new Program();
  current = tokens.begin();
  while (current->kind != Kind::EndOfTokenList) {
    switch (current->kind) {
    case Kind::Function: {
      result->functions.push_back(parseFunction());
      break;
    }
    default: { // 함수의 정의만 올 수 있다.
      cout << *current << "잘못된 구문입니다.\n";
      exit(1);
    }
    }
  }
  return result;
}
auto parseFunction() -> Function* {
  Function* result = new Function();
  skipCurrent(Kind::Function);
  result->name = current->str;
  skipCurrent(Kind::Identifier);
  skipCurrent(Kind::LeftParen);
  if (current->kind != Kind::RightParen) {
    do {
      result->parameters.push_back(current->str);
      skipCurrent(Kind::Identifier);
    } while (skipCurrentIf(Kind::Comma));
  }
  skipCurrent(Kind::RightParen);
  skipCurrent(Kind::LeftBrace);
  result->block = parseBlock();
  skipCurrent(Kind::RightBrace);
  return result;
}
auto parseBlock() -> vector<Statement*> {
  vector<Statement*> result;
  while (current->kind != Kind::RightBrace) {
    switch (current->kind) {
    case Kind::EndOfTokenList: cout << *current << " 잘못된 구문입니다.\n"; exit(1);
    case Kind::Variable:    result.push_back(parseVariable());            break;
    case Kind::For:         result.push_back(parseFor());                 break;
    case Kind::If:          result.push_back(parseIf());                  break;
    case Kind::Print:
    case Kind::PrintLine:   result.push_back(parsePrint());               break;
    case Kind::Return:      result.push_back(parseReturn());              break;
    case Kind::Break:       result.push_back(parseBreak());               break;
    case Kind::Continue:    result.push_back(parseContinue());            break;
    default:                result.push_back(parseExpressionStatement()); break;
    }
  }
  return result;
}
auto parseVariable() -> Variable* {
  Variable* result = new Variable();
  skipCurrent(Kind::Variable);
  result->name = current->str;
  skipCurrent(Kind::Identifier);
  skipCurrent(Kind::Assignment);
  result->expression = parseExpression();
  if (result->expression == nullptr) {
    cout << "변수 선언에 초기화식이 없습니다.";
    exit(1);
  }
  skipCurrent(Kind::Semicolon);
  return result;
}
auto parseFor() -> For* {
  For* result = new For();
  skipCurrent(Kind::For);
  skipCurrent(Kind::LeftParen);
  skipCurrent(Kind::Variable);
  result->variable = new Variable();
  result->variable->name = current->str;
  skipCurrent(Kind::Identifier);
  skipCurrent(Kind::Assignment);
  result->variable->expression = parseExpression();
  if (result->variable->expression == nullptr) {
    cout << "for문에 초기화식이 없습니다.\n";
    exit(1);
  }
  skipCurrent(Kind::Semicolon);
  result->condition = parseExpression();
  if (result->condition == nullptr) {
    cout << "for문에 조건식이 없습니다.\n";
    exit(1);
  }
  skipCurrent(Kind::Semicolon);
  result->expression = parseExpression();
  if (result->expression == nullptr) {
    cout << "for문에 증감식이 없습니다.\n";
    exit(1);
  }
  skipCurrent(Kind::RightParen);
  skipCurrent(Kind::LeftBrace);
  result->block = parseBlock();
  skipCurrent(Kind::RightBrace);
  return result;
}
auto parseIf() -> If* {
  If* result = new If();
  skipCurrent(Kind::If);
  do {
    skipCurrent(Kind::LeftParen);
    Expression* condition = parseExpression();
    if (condition == nullptr) {
      cout << "if문에 조건식이 없습니다.\n";
      exit(1);
    }
    result->conditions.push_back(condition);
    skipCurrent(Kind::RightParen);
    skipCurrent(Kind::LeftBrace);
    result->blocks.push_back(parseBlock());
    skipCurrent(Kind::RightBrace);
  } while (skipCurrentIf(Kind::Elif));
  if (skipCurrentIf(Kind::Else)) {
    skipCurrent(Kind::LeftBrace);
    result->elseBlock = parseBlock();
    skipCurrent(Kind::RightBrace);
  }
  return result;
}
auto parsePrint() -> Print* {
  Print* result = new Print();
  result->lineFeed = current->kind == Kind::PrintLine;
  skipCurrent();
  skipCurrent(Kind::LeftParen);
  if (current->kind != Kind::RightParen) {
    do result->arguments.push_back(parseExpression());
    while (skipCurrentIf(Kind::Comma));
  }
  skipCurrent(Kind::RightParen);
  skipCurrent(Kind::Semicolon);
  return result;
}
auto parseReturn() -> Return* {
  Return* result = new Return();
  skipCurrent(Kind::Return);
  result->expression = parseExpression();
  if (result->expression == nullptr) {
    cout << "return문에 식이 없습니다.\n";
    exit(1);
  }
  skipCurrent(Kind::Semicolon);
  return result;
}
auto parseBreak() -> Break* {
  Break* result = new Break();
  skipCurrent(Kind::Break);
  skipCurrent(Kind::Semicolon);
  return result;
}
auto parseContinue() -> Continue* {
  Continue* result = new Continue();
  skipCurrent(Kind::Continue);
  skipCurrent(Kind::Semicolon);
  return result;
}

auto parseExpressionStatement() -> ExpressionStatement* {
  ExpressionStatement* result = new ExpressionStatement();
  result->expression = parseExpression();
  skipCurrent(Kind::Semicolon);
  return result;
}
auto parseExpression() -> Expression* {
  return parseAssignment();
}
auto parseAssignment() -> Expression* {
  Expression* result = parseOr();
  if (current->kind != Kind::Assignment)
    return result;
  skipCurrent(Kind::Assignment);
  if (GetVariable* getVariable = dynamic_cast<GetVariable*>(result)) { // result가 GetVariable Pointer 형이 아니라면 nullptr, 맞다면 변환
    SetVariable* result = new SetVariable();
    result->name = getVariable->name;
    result->value = parseAssignment();
    delete getVariable;
    return result;
  }
  if (GetElement* getElement = dynamic_cast<GetElement*>(result)) {
    SetElement* result = new SetElement();
    result->sub = getElement->sub;
    result->index = getElement->index;
    result->value = parseAssignment();
    delete getElement;
    return result;
  }
  cout << "잘못된 대입 연산 식입니다.\n";
  exit(1);
}
auto parseOr() -> Expression* {
  Expression* result = parseAnd();
  while (skipCurrentIf(Kind::LogicalOr)) {
    Or* temp = new Or();
    temp->lhs = result;
    temp->rhs = parseAnd();
    result = temp;
  }
  return result;
}
auto parseAnd() -> Expression* {
  Expression* result = parseRelational();
  while (skipCurrentIf(Kind::LogicalAnd)) {
    And* temp = new And();
    temp->lhs = result;
    temp->rhs = parseRelational();
    result = temp;
  }
  return result;
}
auto parseRelational() -> Expression* {
  set<Kind> operators = {
    Kind::Equal,
    Kind::NotEqual,
    Kind::LessThan,
    Kind::GreaterThan,
    Kind::LessOrEqual,
    Kind::GreaterOrEqual,
  };
  Expression* result = parseArithmetic1();
  while (operators.count(current->kind)) {
    Relational* temp = new Relational();
    temp->kind = current->kind;
    skipCurrent();
    temp->lhs = result;
    temp->rhs = parseArithmetic1();
    result = temp;
  }
  return result;
}
auto parseArithmetic1() -> Expression* {
  set<Kind> operators = {
    Kind::Add,
    Kind::Subtract
  };
  Expression* result = parseArithmetic2();
  while (operators.count(current->kind)) {
    Arithmetic* temp = new Arithmetic();
    temp->kind = current->kind;
    skipCurrent();
    temp->lhs = result;
    temp->rhs = parseArithmetic2();
    result = temp;
  }
  return result;
}
auto parseArithmetic2() -> Expression* {
  set<Kind> operators = {
    Kind::Multiply,
    Kind::Divide,
    Kind::Modulo
  };
  Expression* result = parseUnary();
  while (operators.count(current->kind)) {
    Arithmetic* temp = new Arithmetic();
    temp->kind = current->kind;
    skipCurrent();
    temp->lhs = result;
    temp->rhs = parseUnary();
    result = temp;
  }
  return result;
}
auto parseUnary() -> Expression* {
  set<Kind> operators = {
    Kind::Add,
    Kind::Subtract
  };
  if (operators.count(current->kind)) {
    Unary* result = new Unary();
    result->kind = current->kind;
    skipCurrent();
    result->sub = parseOperand();
    return result;
  }
  return parseOperand();
}
auto parseOperand() -> Expression* {
  switch (current->kind) {
  case Kind::NullLiteral:    return parseNullLiteral();
  case Kind::TrueLiteral:
  case Kind::FalseLiteral:   return parseBooleanLiteral();
  case Kind::NumberLiteral:  return parseNumberLiteral();
  case Kind::StringLiteral:  return parseStringLiteral();
  case Kind::LeftParen:      return parseInnerExpression();
  case Kind::LeftBracket:    return parsePostfix(parseListLiteral());
  case Kind::LeftBrace:      return parsePostfix(parseMapLiteral());
  case Kind::Identifier:     return parsePostfix(parseIdentifier());
  default:                   cout << "잘못된 식입니다.\n"; exit(1);
  }
}
auto parseNullLiteral() -> Expression* {
  skipCurrent(Kind::NullLiteral);
  return new NullLiteral();
}
auto parseBooleanLiteral() -> Expression* {
  BooleanLiteral* result = new BooleanLiteral();
  result->value = current->kind == Kind::TrueLiteral;
  skipCurrent();
  return result;
}
auto parseNumberLiteral() -> Expression* {
  NumberLiteral* result = new NumberLiteral();
  result->value = stod(current->str);
  skipCurrent(Kind::NumberLiteral);
  return result;
}
auto parseStringLiteral() -> Expression* {
  StringLiteral* result = new StringLiteral();
  result->value = current->str;
  skipCurrent(Kind::StringLiteral);
  return result;
}
auto parseListLiteral() -> Expression* {
  ArrayLiteral* result = new ArrayLiteral();
  skipCurrent(Kind::LeftBracket);
  if (current->kind != Kind::RightBracket) {
    do result->values.push_back(parseExpression());
    while (skipCurrentIf(Kind::Comma));
  }
  skipCurrent(Kind::RightBracket);
  return result;
}
auto parseMapLiteral() -> Expression* {
  MapLiteral* result = new MapLiteral();
  skipCurrent(Kind::LeftBrace);
  if (current->kind != Kind::RightBrace) {
    do {
      string name = current->str;
      skipCurrent(Kind::StringLiteral);
      skipCurrent(Kind::Colon);
      Expression* value = parseExpression();
      result->values[name] = value;
    } while (skipCurrentIf(Kind::Comma));
  }
  skipCurrent(Kind::RightBrace);
  return result;
}
auto parseIdentifier() -> Expression* {
  GetVariable* result = new GetVariable();
  result->name = current->str;
  skipCurrent(Kind::Identifier);
  return result;
}
auto parseInnerExpression() -> Expression* {
  skipCurrent(Kind::LeftParen);
  Expression* result = parseExpression();
  skipCurrent(Kind::RightParen);
  return result;
}
auto parsePostfix(Expression* sub) -> Expression* {
  while (true) {
    switch (current->kind) {
    case Kind::LeftParen:    sub = parseCall(sub);     break;
    case Kind::LeftBracket:  sub = parseElement(sub);  break;
    default: return sub;
    }
  }
}
auto parseCall(Expression* sub) -> Expression* {
  Call* result = new Call();
  result->sub = sub;
  skipCurrent(Kind::LeftParen);
  if (current->kind != Kind::RightParen) {
    do result->arguments.push_back(parseExpression());
    while (skipCurrentIf(Kind::Comma));
  }
  skipCurrent(Kind::RightParen);
  return result;
}
auto parseElement(Expression* sub) -> Expression* {
  GetElement* result = new GetElement();
  result->sub = sub;
  skipCurrent(Kind::LeftBracket);
  result->index = parseExpression();
  skipCurrent(Kind::RightBracket);
  return result;
}

auto skipCurrent() -> void {
  ++current;
}
auto skipCurrent(Kind kind) -> void {
  if (current->kind != kind) {
    cout << toString(kind) << " 토큰이 필요합니다.";
    exit(1);
  }
  ++current;
}
auto skipCurrentIf(Kind kind) -> bool {
  if (current->kind != kind) return false;
  ++current;
  return true;
}