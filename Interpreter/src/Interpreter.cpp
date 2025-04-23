#include <iostream>
#include <functional>
#include <vector>
#include <list>
#include <map>
#include <any>
#include <set>
#include <cmath>
#include "Node.h"
#include "Datatype.h"
using namespace std;

struct ReturnException {
  any result;
};
struct BreakException {};
struct ContinueException {};

static map<string, Function*> functionTable;
static list<list<map<string, any>>> local;
static map<string, any> global;

auto interpret(Program* program) -> void {
  functionTable.clear();
  for (Function* node : program->functions) {
    functionTable[node->name] = node;
  }
  for (Variable* variable : program->variables) {
    if (functionTable.count(variable->name)) {
      cout << "Function name and global variable name are duplicated." << endl;
      exit(1);
    }
    global[variable->name] = variable->expression->interpret();
  }
  if (functionTable.count("main") == 0) {
    return;
  }
  try {
    local.emplace_back().emplace_front();
    functionTable["main"]->interpret();
  }
  catch (ReturnException e) {
    local.pop_back();
  }
}

auto Function::interpret() -> void {
  for (Statement* node : block) {
    node->interpret();
  }
}
auto Return::interpret() -> void {

}
auto Variable::interpret() -> void {
  if (local.back().front().count(name)) {
    cout << name << " already exists" << endl;
    exit(1);
  }
  local.back().front()[name] = expression->interpret();
}
auto For::interpret() -> void {
  local.back().emplace_front();
  variable->interpret();
  while (true) {
    any result = condition->interpret();
    if (isFalse(result)) break;
    try {
      for (Statement* node : block) {
        node->interpret();
      }
    }
    catch (ContinueException) {}
    catch (BreakException) { break; }
    expression->interpret();
  }
  local.back().pop_front();
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
  expression->interpret();
}

auto Or::interpret() -> any {
  return isTrue(lhs->interpret()) ? true : rhs->interpret();
}
auto And::interpret() -> any {
  return isFalse(lhs->interpret()) ? false : rhs->interpret();
}
auto Relational::interpret() -> any {
  auto lValue = lhs->interpret();
  auto rValue = rhs->interpret();
  if (isNumber(lValue) && isNumber(rValue)) {
    if (kind == Kind::LessThan) return toNumber(lValue) < toNumber(rValue);
    if (kind == Kind::LessOrEqual) return toNumber(lValue) <= toNumber(rValue);
    if (kind == Kind::GreaterThan) return toNumber(lValue) > toNumber(rValue);
    if (kind == Kind::GreaterOrEqual) return toNumber(lValue) >= toNumber(rValue);
    if (kind == Kind::Equal) return toNumber(lValue) == toNumber(rValue);
    if (kind == Kind::NotEqual) return toNumber(lValue) != toNumber(rValue);
  }
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
  set<Kind> operators1 = { Kind::Add, Kind::Subtract };
  set<Kind> operators2 = { Kind::Increase, Kind::Decrease };

  if (operators1.count(kind)) {

  }
  else {
    if (kind == Kind::Increase) {
      for (map<string, any>& variables : local.back()) {
        if (variables.count(name)) {
          return variables[name] = toNumber(variables[name]) + 1;
        }
      }
      if (global.count(name)) {
        return global[name] = toNumber(global[name]) + 1;
      }
      cout << name << " does not exist." << endl;
      exit(1);
    }
    if (kind == Kind::Decrease) {
      for (map<string, any>& variables : local.back()) {
        if (variables.count(name)) {
          return variables[name] = toNumber(variables[name]) - 1;
        }
      }
      if (global.count(name)) {
        return global[name] = toNumber(global[name]) - 1;
      }
      cout << name << " does not exist." << endl;
      exit(1);
    }
  }
}
auto Call::interpret() -> any {

}
auto GetElement::interpret() -> any {

}
auto SetElement::interpret() -> any {

}
auto GetVariable::interpret() -> any {
  for (map<string, any>& variables : local.back()) {
    if (variables.count(name)) {
      return variables[name];
    }
  }
  if (global.count(name)) {
    return global[name];
  }

  cout << name << " does not exist." << endl;
  exit(1);
}
auto SetVariable::interpret() -> any {
  for (map<string, any>& variables : local.back()) {
    if (variables.count(name)) {
      return variables[name] = value->interpret();
    }
  }
  if (global.count(name)) {
    return global[name] = value->interpret();
  }

  cout << name << " does not exist." << endl;
  exit(1);
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
