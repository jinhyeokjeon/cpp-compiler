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
  return isTrue(lhs->interpret()) ? true : rhs->interpret();
}
auto And::interpret() -> any {
  return isFalse(lhs->interpret()) ? false : rhs->interpret();
}
auto Relational::interpret() -> any {

}
auto Arithmetic::interpret() -> any {
  any lValue = lhs->interpret();
  any rValue = rhs->interpret();
  if (isNumber(lValue) && isNumber(rValue)) {
    switch (kind) {
    case Kind::Add:       return toNumber(lValue) + toNumber(rValue);
    case Kind::Subtract:  return toNumber(lValue) - toNumber(rValue);
    case Kind::Multiply:  return toNumber(lValue) * toNumber(rValue);
    case Kind::Divide:    if (toNumber(rValue) != 0) return toNumber(lValue) / toNumber(rValue); break;
    case Kind::Modulo:    if (toNumber(rValue) != 0) return fmod(toNumber(lValue), toNumber(rValue)); break;
    }
  }
  else if (isString(lValue) && isString(rValue) && kind == Kind::Add) {
    return toString(lValue) + toString(rValue);
  }
  else if (isString(lValue) && isNumber(rValue) && kind == Kind::Multiply) {
    string result = "";
    for (int i = 0; i < (int)toNumber(rValue); ++i) {
      result += toString(lValue);
    }
    return result;
  }

  cout << lValue << " " << toString(kind) << " " << rValue << " is not possible." << endl;
  exit(1);
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
  return nullptr;
}
auto BooleanLiteral::interpret() -> any {
  return value;
}
auto NumberLiteral::interpret() -> any {
  return value;
}
auto StringLiteral::interpret() -> any {
  return value;
}
auto ArrayLiteral::interpret() -> any {

}
auto MapLiteral::interpret() -> any {

}
