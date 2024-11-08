#pragma once
#include <any>
#include <map>
#include <vector>
#include <string>
#include "Token.h"

using namespace std;

struct Program {
  vector<struct Function*> functions;
};
struct Statement {
  virtual void generate() = 0;
};
struct Expression {
  virtual void generate() = 0;
};
struct Function : Statement {
  string name;
  vector<string> parameters;
  vector<Statement*> block;
  void generate();
};
struct Variable : Statement {
  string name;
  Expression* expression;
  void generate();
};
struct Return : Statement {
  Expression* expression;
  void generate();
};
struct For : Statement {
  Variable* variable;
  Expression* condition;
  Expression* expression;
  vector<Statement*> block;
  void generate();
};
struct Break : Statement {
  void generate();
};
struct Continue : Statement {
  void generate();
};
struct If : Statement {
  vector<Expression*> conditions;
  vector<vector<Statement*>> blocks;
  vector<Statement*> elseBlock;
  void generate();
};
struct Print : Statement {
  bool lineFeed = false;
  vector<Expression*> arguments;
  void generate();
};
struct ExpressionStatement : Statement {
  Expression* expression;
  void generate();
};

struct Or : Expression {
  Expression* lhs;
  Expression* rhs;
  void generate();
};
struct And : Expression {
  Expression* lhs;
  Expression* rhs;
  void generate();
};
struct Relational : Expression {
  Kind kind;
  Expression* lhs;
  Expression* rhs;
  void generate();
};
struct Arithmetic : Expression {
  Kind kind;
  Expression* lhs;
  Expression* rhs;
  void generate();
};
struct Unary : Expression {
  Kind kind;
  Expression* sub;
  void generate();
};
struct Call : Expression {
  Expression* sub;
  vector<Expression*> arguments;
  void generate();
};
struct GetElement : Expression {
  Expression* sub;
  Expression* index;
  void generate();
};
struct SetElement : Expression {
  Expression* sub;
  Expression* index;
  Expression* value;
  void generate();
};
struct GetVariable : Expression {
  string name;
  void generate();
};
struct SetVariable : Expression {
  string name;
  Expression* value;
  void generate();
};
struct NullLiteral : Expression {
  void generate();
};
struct BooleanLiteral : Expression {
  bool value = false;
  void generate();
};
struct NumberLiteral : Expression {
  double value = 0.0;
  void generate();
};
struct StringLiteral : Expression {
  string value;
  void generate();
};
struct ArrayLiteral : Expression {
  vector<Expression*> values;
  void generate();
};
struct MapLiteral : Expression {
  map<string, Expression*> values;
  void generate();
};