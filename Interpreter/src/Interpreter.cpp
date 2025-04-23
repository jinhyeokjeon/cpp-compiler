#include <iostream>
#include <functional>
#include <vector>
#include <list>
#include <map>
#include <any>
#include <cmath>
#include "Node.h"
#include "Datatype.h"
using namespace std;

static map<string, Function*> functionTable;

auto interpret(Program* program) -> void {
  for (Function* node : program->functions) {
    functionTable[node->name] = node;
  }
  if (functionTable.count("main") == 0) {
    return;
  }
  functionTable["main"]->interpret();
}

auto Function::interpret() -> void {
  for (Statement* node : block) {
    node->interpret();
  }
}
auto Return::interpret() -> void {

}
auto Variable::interpret() -> void {

}
auto For::interpret() -> void {

}
auto Break::interpret() -> void {

}
auto Continue::interpret() -> void {

}
auto If::interpret() -> void {

}
auto Print::interpret() -> void {
  for (Expression* node : arguments) {
    any value = node->interpret();
    cout << value;
  }
  if (lineFeed) {
    cout << endl;
  }
}
auto ExpressionStatement::interpret() -> void {

}

auto Or::interpret() -> any {

}
auto And::interpret() -> any {

}
auto Relational::interpret() -> any {

}
auto Arithmetic::interpret() -> any {

}
auto Unary::interpret() -> any {

}
auto Call::interpret() -> any {

}
auto GetElement::interpret() -> any {

}
auto SetElement::interpret() -> any {

}
auto GetVariable::interpret() -> any {

}
auto SetVariable::interpret() -> any {

}
auto NullLiteral::interpret() -> any {

}
auto BooleanLiteral::interpret() -> any {

}
auto NumberLiteral::interpret() -> any {

}
auto StringLiteral::interpret() -> any {
  return value;
}
auto ArrayLiteral::interpret() -> any {

}
auto MapLiteral::interpret() -> any {

}
