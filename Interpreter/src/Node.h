#pragma once
#include <vector>
#include <string>
#include <map>
#include <any>
#include "Token.h"
using namespace std;

struct Program {
  vector<struct Function*> functions;
};
struct Statement {
  virtual void interpret() = 0;
};
struct Expression {
  virtual any interpret() = 0;
};

struct Function : Statement {
  string name;
  vector<string> parameters;
  vector<Statement*> block;
  void interpret();
};
struct Return : Statement {
  Expression* expression;
  void interpret();
};
struct Variable : Statement {
  string name;
  Expression* expression;
  void interpret();
};
struct For : Statement {
  Variable* variable;
  Expression* condition;
  Expression* expression;
  vector<Statement*> block;
  void interpret();
};
struct Break : Statement {
  void interpret();
};
struct Continue : Statement {
  void interpret();
};
struct If : Statement {
  vector<Expression*> conditions;
  vector<vector<Statement*>> blocks;
  vector<Statement*> elseBlock;
  void interpret();
};
struct Print : Statement {
  bool lineFeed = false;
  vector<Expression*> arguments;
  void interpret();
};
struct ExpressionStatement : Statement {
  Expression* expression;
  void interpret();
};
struct Or : Expression {
  Expression* lhs;
  Expression* rhs;
  any interpret();
};
struct And : Expression {
  Expression* lhs;
  Expression* rhs;
  any interpret();
};
struct Relational : Expression {
  Kind kind;
  Expression* lhs;
  Expression* rhs;
  any interpret();
};
struct Arithmetic : Expression {
  Kind kind;
  Expression* lhs;
  Expression* rhs;
  any interpret();
};
struct Unary : Expression {
  Kind kind;
  Expression* sub;
  any interpret();
};
struct Call : Expression {
  Expression* sub;
  vector<Expression*> arguments;
  any interpret();
};
struct GetElement : Expression {
  Expression* sub;
  Expression* index;
  any interpret();
};
struct SetElement : Expression {
  Expression* sub; // GetVariable*
  Expression* index;
  Expression* value;
  any interpret();
};
struct GetVariable : Expression {
  string name;
  any interpret();
};
struct SetVariable : Expression {
  string name;
  Expression* value;
  any interpret();
};
struct NullLiteral : Expression {
  any interpret();
};
struct BooleanLiteral : Expression {
  bool value = false;
  any interpret();
};
struct NumberLiteral : Expression {
  double value = 0.0;
  any interpret();
};
struct StringLiteral : Expression {
  string value;
  any interpret();
};
struct ArrayLiteral : Expression {
  vector<Expression*> values;
  any interpret();
};
struct MapLiteral : Expression {
  map<string, Expression*> values;
  any interpret();
};