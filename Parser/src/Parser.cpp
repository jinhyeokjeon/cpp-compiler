#include <iostream>
#include <set>
#include "Token.h"
#include "Node.h"

using namespace std;

static vector<Token>::iterator current;
auto parse(vector<Token>&) -> Program*;

static auto parseFunction() -> Function*;
static auto parseBlock() -> std::vector<Statement*>;
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
static auto parsePostfix(Expression* expr) -> Expression*;
static auto parseCall(Expression* expr) -> Expression*;
static auto parseElement(Expression* expr) -> Expression*;

static auto skipCurrent() -> void;
static auto skipCurrent(Kind kind) -> void;
static auto skipCurrentIf(Kind kind) -> bool;

auto parse(vector<Token>& tokens) -> Program* {
  Program* result = new Program();
  current = tokens.begin();
  while (current->kind != Kind::EndOfTokenList) {
    switch (current->kind) {
    case Kind::Function: {
      result->functions.push_back(parseFunction());
      break;
    }
    default: {
      cout << *current << " is wrong." << endl;
      exit(1);
    }
    }
  }
  return result;
}

static auto parseFunction() -> Function* {
  Function* result = new Function();
  skipCurrent(Kind::Function);
  result->name = current->str;
  skipCurrent(Kind::Identifier);
  skipCurrent(Kind::LeftParen);
  if (current->kind != Kind::RightParen) {
    do {
      skipCurrent(Kind::Variable);
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
static auto parseBlock() -> std::vector<Statement*> {
  vector<Statement*> result;
  while (current->kind != Kind::RightBrace) {
    switch (current->kind) {
    case Kind::EndOfTokenList:  cout << *current << " is wrong" << endl; exit(1);
    case Kind::Variable:        result.push_back(parseVariable()); break;
    case Kind::For:             result.push_back(parseFor()); break;
    case Kind::If:              result.push_back(parseIf());  break;
    case Kind::Print:
    case Kind::PrintLine:       result.push_back(parsePrint()); break;
    case Kind::Return:          result.push_back(parseReturn()); break;
    case Kind::Break:           result.push_back(parseBreak()); break;
    case Kind::Continue:        result.push_back(parseContinue()); break;
    default:                    result.push_back(parseExpressionStatement()); break;
    }
  }
  return result;
}
static auto parseVariable() -> Variable* {
  Variable* result = new Variable();
  skipCurrent(Kind::Variable);
  result->name = current->str;
  skipCurrent(Kind::Identifier);
  skipCurrent(Kind::Assignment);
  result->expression = parseExpression();
  skipCurrent(Kind::Semicolon);
  return result;
}
static auto parseFor() -> For* {
  For* result = new For();
  skipCurrent(Kind::For);
  skipCurrent(Kind::LeftParen);
  skipCurrent(Kind::Variable);
  result->variable = new Variable();
  result->variable->name = current->str;
  skipCurrent(Kind::Identifier);
  skipCurrent(Kind::Assignment);
  result->variable->expression = parseExpression();
  skipCurrent(Kind::Semicolon);
  result->condition = parseExpression();
  skipCurrent(Kind::Semicolon);
  result->expression = parseExpression();
  skipCurrent(Kind::RightParen);
  skipCurrent(Kind::LeftBrace);
  result->block = parseBlock();
  skipCurrent(Kind::RightBrace);
  return result;
}
static auto parseIf() -> If* {
  If* result = new If();
  skipCurrent(Kind::If);
  do {
    skipCurrent(Kind::LeftParen);
    result->conditions.push_back(parseExpression());
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
static auto parsePrint() -> Print* {
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
static auto parseReturn() -> Return* {
  Return* result = new Return();
  skipCurrent(Kind::Return);
  result->expression = parseExpression();
  skipCurrent(Kind::Semicolon);
  return result;
}
static auto parseBreak() -> Break* {
  Break* result = new Break();
  skipCurrent(Kind::Break);
  skipCurrent(Kind::Semicolon);
  return result;
}
static auto parseContinue() -> Continue* {
  Continue* result = new Continue();
  skipCurrent(Kind::Continue);
  skipCurrent(Kind::Semicolon);
  return result;
}
static auto parseExpressionStatement() -> ExpressionStatement* {
  ExpressionStatement* result = new ExpressionStatement();
  result->expression = parseExpression();
  skipCurrent(Kind::Semicolon);
  return result;
}

static auto parseExpression() -> Expression* {
  return parseAssignment();
}
static auto parseAssignment() -> Expression* {
  Expression* result = parseOr();
  if (current->kind != Kind::Assignment) {
    return result;
  }
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
  cout << "Wrong assignment Expression" << endl;
  exit(1);
}
static auto parseOr() -> Expression* {
  Expression* result = parseAnd();
  while (skipCurrentIf(Kind::LogicalOr)) {
    Or* temp = new Or();
    temp->lhs = result;
    temp->rhs = parseAnd();
    result = temp;
  }
  return result;
}
static auto parseAnd() -> Expression* {
  Expression* result = parseRelational();
  while (skipCurrentIf(Kind::LogicalAnd)) {
    And* temp = new And();
    temp->lhs = result;
    temp->rhs = parseRelational();
    result = temp;
  }
  return result;
}
static auto parseRelational() -> Expression* {
  set<Kind> operators = {
    Kind::Equal, Kind::NotEqual,
    Kind::LessThan, Kind::LessOrEqual,
    Kind::GreaterThan, Kind::GreaterOrEqual
  };
  Expression* result = parseArithmetic1();
  if (operators.count(current->kind) == 0) {
    return result;
  }
  Relational* relational = new Relational();
  relational->lhs = result;
  relational->kind = current->kind;
  skipCurrent();
  relational->rhs = parseArithmetic1();
  return relational;
}
static auto parseArithmetic1() -> Expression* {
  set<Kind> operators = { Kind::Add, Kind::Subtract };
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
static auto parseArithmetic2() -> Expression* {
  set<Kind> operators = { Kind::Multiply, Kind::Divide, Kind::Modulo };
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
static auto parseUnary() -> Expression* {
  set<Kind> operators = { Kind::Add, Kind::Subtract, Kind::Increase, Kind::Decrease };
  if (operators.count(current->kind)) {
    Unary* result = new Unary();
    result->kind = current->kind;
    skipCurrent();
    result->sub = parseOperand();
    return result;
  }
  return parseOperand();
}
static auto parseOperand() -> Expression* {
  Expression* result;
  switch (current->kind) {
  case Kind::NullLiteral:   result = parseNullLiteral();     break;
  case Kind::TrueLiteral:
  case Kind::FalseLiteral:  result = parseBooleanLiteral();  break;
  case Kind::NumberLiteral: result = parseNumberLiteral();   break;
  case Kind::StringLiteral: result = parseStringLiteral();   break;
  case Kind::LeftBracket:   result = parseListLiteral();     break;
  case Kind::LeftBrace:     result = parseMapLiteral();      break;
  case Kind::Identifier:    result = parseIdentifier();      break;
  case Kind::LeftParen:     result = parseInnerExpression(); break;
  default:                  cout << "wrong expression." << endl; exit(1);
  }
  return parsePostfix(result);
}
static auto parseNullLiteral() -> Expression* {
  skipCurrent(Kind::NullLiteral);
  return new NullLiteral();
}
static auto parseBooleanLiteral() -> Expression* {
  BooleanLiteral* result = new BooleanLiteral();
  result->value = (current->kind == Kind::TrueLiteral);
  skipCurrent();
  return result;
}
static auto parseNumberLiteral() -> Expression* {
  NumberLiteral* result = new NumberLiteral();
  result->value = stod(current->str);
  skipCurrent(Kind::NumberLiteral);
  return result;
}
static auto parseStringLiteral() -> Expression* {
  StringLiteral* result = new StringLiteral();
  result->value = current->str;
  skipCurrent(Kind::StringLiteral);
  return result;
}
static auto parseListLiteral() -> Expression* {
  ArrayLiteral* result = new ArrayLiteral();
  skipCurrent(Kind::LeftBracket);
  if (current->kind != Kind::RightBracket) {
    do result->values.push_back(parseExpression());
    while (skipCurrentIf(Kind::Comma));
  }
  skipCurrent(Kind::RightBracket);
  return result;
}
static auto parseMapLiteral() -> Expression* {
  MapLiteral* result = new MapLiteral();
  skipCurrent(Kind::LeftBrace);
  if (current->kind != Kind::RightBrace) {
    do {
      string key = current->str;
      skipCurrent(Kind::StringLiteral);
      skipCurrent(Kind::Colon);
      Expression* value = parseExpression();
      result->values[key] = value;
    } while (skipCurrentIf(Kind::Comma));
  }
  skipCurrent(Kind::RightBrace);
  return result;
}
static auto parseIdentifier() -> Expression* {
  GetVariable* result = new GetVariable();
  result->name = current->str;
  skipCurrent(Kind::Identifier);
  return result;
}
static auto parseInnerExpression() -> Expression* {
  skipCurrent(Kind::LeftParen);
  Expression* result = parseExpression();
  skipCurrent(Kind::RightParen);
  return result;
}
static auto parsePostfix(Expression* sub) -> Expression* {
  while (true) {
    switch (current->kind) {
    case Kind::LeftParen:   sub = parseCall(sub);    break;
    case Kind::LeftBracket: sub = parseElement(sub); break;
    default: return sub;
    }
  }
}
static auto parseCall(Expression* sub) -> Expression* {
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
static auto parseElement(Expression* sub) -> Expression* {
  GetElement* result = new GetElement();
  result->sub = sub;
  skipCurrent(Kind::LeftBracket);
  result->index = parseExpression();
  skipCurrent(Kind::RightBracket);
  return result;
}

static auto skipCurrent() -> void {
  ++current;
}
static auto skipCurrent(Kind kind) -> void {
  if (current->kind != kind) {
    cout << toString(kind) << " is required." << endl;
    exit(1);
  }
  ++current;
}
static auto skipCurrentIf(Kind kind) -> bool {
  if (current->kind != kind) return false;
  ++current;
  return true;
}
