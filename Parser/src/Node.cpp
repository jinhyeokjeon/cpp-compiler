#include <iostream>
#include "Node.h"

using std::cout;
using std::endl;

static void indent(int depth) {
  cout << string(static_cast<size_t>(depth * 2), ' ');
}

void printSyntaxTree(Program* program) {
  for (auto& node: program->functions)
    node->print(0);
}

void Function::print(int depth) {
  indent(depth); cout << "FUNCTION " << name << ": " << endl;
  if (parameters.size()) {
    indent(depth + 1); cout << "PARAMETERS:";
    for (auto& name: parameters)
      cout << name << " ";
    cout << endl;
  }
  indent(depth + 1); cout << "BLOCK:" << endl;
  for (auto& node: block)
    node->print(depth + 2);
}

void For::print(int depth) {
  indent(depth); cout << "FOR:" << endl;
  indent(depth + 1); cout << "VARIABLE:" << endl;
  variable->print(depth + 2);
  indent(depth + 1); cout << "CONDITION:" << endl;
  condition->print(depth + 2);
  indent(depth + 1); cout << "EXPRESSION:" << endl;
  expression->print(depth + 2);
  indent(depth + 1); cout << "BLOCK:" << endl;
  for (auto& node: block)
    node->print(depth + 2);
}

void If::print(int depth) {
  for (size_t i = 0; i < conditions.size(); i++) {
    indent(depth); cout << (i == 0 ? "IF:" : "ELIF:") << endl;
    indent(depth + 1); cout << "CONDITION:" << endl;
    conditions[i]->print(depth + 2);
    indent(depth + 1); cout << "BLOCK:" << endl;
    for (auto& node: blocks[i])
      node->print(depth + 2);
  }
  if (elseBlock.size() == 0)
    return;
  indent(depth); cout << "ELSE:" << endl;
  for (auto& node: elseBlock)
    node->print(depth + 1);
}

void Variable::print(int depth) {
  indent(depth); cout << "VAR " << name << ":" << endl;
  expression->print(depth + 1);
}

void Print::print(int depth) {
  indent(depth); cout << (lineFeed ? "PRINT_LINE" : "PRINT:") << endl;
  for (auto& node: arguments)
    node->print(depth + 1);
}

void Return::print(int depth) {
  indent(depth); cout << "RETURN:" << endl;
  expression->print(depth + 1);
}

void Break::print(int depth) {
  indent(depth); cout << "BREAK" << endl;
}

void Continue::print(int depth) {
  indent(depth); cout << "CONTINUE" << endl;
}

void ExpressionStatement::print(int depth) {
  indent(depth); cout << "EXPRESSION:" << endl;
  expression->print(depth + 1);
}

void Or::print(int depth) {
  indent(depth); cout << "OR:" << endl;
  indent(depth + 1); cout << "LHS:" << endl;
  lhs->print(depth + 2);
  indent(depth + 1); cout << "RHS:" << endl;
  rhs->print(depth + 2);
}

void And::print(int depth) {
  indent(depth); cout << "AND:" << endl;
  indent(depth + 1); cout << "LHS:" << endl;
  lhs->print(depth + 2);
  indent(depth + 1); cout << "RHS:" << endl;
  rhs->print(depth + 2);
}

void Relational::print(int depth) {
  indent(depth); cout << toString(kind) << ":" << endl;
  indent(depth + 1); cout << "LHS:" << endl;
  lhs->print(depth + 2);
  indent(depth + 1); cout << "RHS:" << endl;
  rhs->print(depth + 2);
}

void Arithmetic::print(int depth) {
  indent(depth); cout << toString(kind) << ":" << endl;
  indent(depth + 1); cout << "LHS:" << endl;
  lhs->print(depth + 2);
  indent(depth + 1); cout << "RHS:" << endl;
  rhs->print(depth + 2);
}

void Unary::print(int depth) {
  indent(depth); cout << toString(kind) << endl;
  sub->print(depth + 1);
}

void GetElement::print(int depth) {
  indent(depth); cout << "GET_ELEMENT:" << endl;
  indent(depth + 1); cout << "SUB:" << endl;
  sub->print(depth + 2);
  indent(depth + 1); cout << "INDEX:" << endl;
  index->print(depth + 2);
}

void SetElement::print(int depth) {
  indent(depth); cout << "SET_ELEMENT:" << endl;
  indent(depth + 1); cout << "SUB:" << endl;
  sub->print(depth + 2);
  indent(depth + 1); cout << "INDEX:" << endl;
  index->print(depth + 2);
  indent(depth + 1); cout << "VALUE:" << endl;
  value->print(depth + 2);
}

void Call::print(int depth) {
  indent(depth); cout << "CALL:" << endl;
  indent(depth + 1); cout << "EXPRESSION:" << endl;
  sub->print(depth + 2);
  for (auto& node: arguments) {
    indent(depth + 1); cout << "ARGUMENT:" << endl;
    node->print(depth + 2);
  }
}

void NullLiteral::print(int depth) {
  indent(depth); cout << "null" << endl;
}

void BooleanLiteral::print(int depth) {
  indent(depth); cout << (value ? "true" : "false") << endl;
}

void NumberLiteral::print(int depth) {
  indent(depth); cout << value << endl;
}

void StringLiteral::print(int depth) {
  indent(depth); cout << "\"" << value << "\"" << endl;
}

void ArrayLiteral::print(int depth) {
  indent(depth); cout << "[" << endl;
  for (auto& node: values)
    node->print(depth + 1);
  indent(depth); cout << "]" << endl;
}

void MapLiteral::print(int depth) {
  indent(depth); cout << "{" << endl;
  for (auto& [key, value]: values) {
    cout << key << ": ";
    value->print(depth + 1);
  }
  indent(depth); cout << "}" << endl;
}

void GetVariable::print(int depth) {
  indent(depth); cout << "GET_VARIABLE: " << name << endl;
}

void SetVariable::print(int depth) {
  indent(depth); cout << "SET_VARIABLE: " << name << endl;
  value->print(depth + 1);
}
