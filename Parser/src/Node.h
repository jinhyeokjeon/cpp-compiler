#pragma once
#include <vector>
#include <string>
#include <map>
#include "Token.h"
using std::vector;
using std::string;
using std::map;

struct Program {
    vector<struct Function*> functions;
    void print(int);
};

struct Statement {
    virtual void print(int) = 0;
};
struct Expression {
    virtual void print(int) = 0;
};

struct Function: Statement {
    string name;
    vector<string> parameters;
    vector<Statement*> block;
    void print(int);
};

struct Return: Statement {
    Expression* expression;
    void print(int);
};

struct Variable: Statement {
    string name;
    Expression* expression;
    void print(int);
};

struct For: Statement {
    Variable* variable;
    Expression* condition;
    Expression* expression;
    vector<Statement*> block;
    void print(int);
};

struct Break: Statement {
    void print(int);
};
struct Continue: Statement {
    void print(int);
};

struct If: Statement {
    vector<Expression*> conditions;
    vector<vector<Statement*>> blocks;
    vector<Statement*> elseBlock;
    void print(int);
};

struct Print: Statement {
    bool lineFeed = false;
    vector<Expression*> arguments;
    void print(int);
};

struct ExpressionStatement: Statement {
    Expression* expression;
    void print(int);
};

struct Or: Expression {
    Expression* lhs;
    Expression* rhs;
    void print(int);
};
struct And: Expression {
    Expression* lhs;
    Expression* rhs;
    void print(int);
};

struct Relational: Expression {
    Kind kind;
    Expression* lhs;
    Expression* rhs;
    void print(int);
};

struct Arithmetic: Expression {
    Kind kind;
    Expression* lhs;
    Expression* rhs;
    void print(int);
};

struct Unary: Expression {
    Kind kind;
    Expression* sub;
    void print(int);
};

struct Call: Expression {
    Expression* sub;
    vector<Expression*> arguments;
    void print(int);
};

struct GetElement: Expression {
    Expression* sub;
    Expression* index;
    void print(int);
};

struct SetElement: Expression {
    Expression* sub;
    Expression* index;
    Expression* value;
    void print(int);
};

struct GetVariable: Expression {
    string name;
    void print(int);
};

struct SetVariable: Expression {
    string name;
    Expression* value;
    void print(int);
};

struct NullLiteral: Expression {
    void print(int);
};
struct BooleanLiteral: Expression {
    bool value = false;
    void print(int);
};
struct NumberLiteral: Expression {
    double value = 0.0;
    void print(int);
};
struct StringLiteral: Expression {
    string value;
    void print(int);
};

struct ArrayLiteral: Expression {
    vector<Expression*> values;
    void print(int);
};

struct MapLiteral: Expression {
    map<string, Expression*> values;
    void print(int);
};
