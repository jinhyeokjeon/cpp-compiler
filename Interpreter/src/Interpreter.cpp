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
extern map<string, function<any(vector<any>)>> builtinFunctionTable;
static list<list<map<string, any>>> local;
static map<string, any> global;

auto interpret(Program* program) -> void {
  functionTable.clear();
  for (Function* node : program->functions) {
    if (builtinFunctionTable.count(node->name)) {
      cout << "Function name and builtin function name are duplicated." << endl;
      exit(1);
    }
    functionTable[node->name] = node;
  }
  for (Variable* variable : program->variables) {
    if (functionTable.count(variable->name) || builtinFunctionTable.count(variable->name)) {
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
  throw ReturnException{ expression->interpret() };
}
auto Variable::interpret() -> void {
  if (functionTable.count(name) || builtinFunctionTable.count(name)) {
    cout << "Function name and local variable name are duplicated." << endl;
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
  throw BreakException();
}
auto Continue::interpret() -> void {
  throw ContinueException();
}
auto If::interpret() -> void {
  for (int i = 0; i < conditions.size(); ++i) {
    any result = conditions[i]->interpret();
    if (isFalse(result)) continue;
    local.back().emplace_front();
    for (Statement* node : blocks[i]) {
      node->interpret();
    }
    local.back().pop_front();
    return;
  }
  if (elseBlock.empty()) return;
  local.back().emplace_front();
  for (Statement* node : elseBlock) {
    node->interpret();
  }
  local.back().pop_front();
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
  any l = lhs->interpret();
  any r = rhs->interpret();
  return isTrue(l) ? true : toBoolean(r);
}
auto And::interpret() -> any {
  any l = lhs->interpret();
  any r = rhs->interpret();
  return isFalse(l) ? false : toBoolean(r);
}
auto Relational::interpret() -> any {
  any lValue = lhs->interpret();
  any rValue = rhs->interpret();
  if (isNumber(lValue) && isNumber(rValue)) {
    double l = toNumber(lValue), r = toNumber(rValue);
    switch (kind) {
    case Kind::LessThan:       return l < r;
    case Kind::LessOrEqual:    return l <= r;
    case Kind::GreaterThan:    return l > r;
    case Kind::GreaterOrEqual: return l >= r;
    case Kind::Equal:          return l == r;
    case Kind::NotEqual:       return l != r;
    default:                   break;
    }
  }
  else if (isBoolean(lValue) && isBoolean(rValue)) {
    bool l = toBoolean(lValue), r = toBoolean(rValue);
    switch (kind) {
    case Kind::Equal:    return l == r;
    case Kind::NotEqual: return l != r;
    default:             break;
    }
  }
  cout << lValue << " " << toString(kind) << " " << rValue << " is impossible" << endl;
  exit(1);
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
    default:              break;
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
    any value = sub->interpret();
    if (isNumber(value)) {
      if (kind == Kind::Add) {
        return toNumber(value) > 0 ? toNumber(value) : -toNumber(value);
      }
      else {
        return -toNumber(value);
      }
    }
    cout << toString(kind) << value << " is impossible." << endl;
    exit(1);
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

  return nullptr;
}
auto Call::interpret() -> any {
  if (functionTable.count(name)) {
    Function* func = functionTable[name];
    map<string, any> parameters;
    if (func->parameters.size() != arguments.size()) {
      cout << "The number of arguments to the function is incorrect." << endl;
      exit(1);
    }
    for (int i = 0; i < arguments.size(); ++i) {
      string name = func->parameters[i];
      parameters[name] = arguments[i]->interpret();
    }
    // local.emplace_back().emplace_front();
    local.emplace_back().push_front(parameters);
    try {
      func->interpret();
    }
    catch (ReturnException& exception) {
      local.pop_back();
      return exception.result;
    }
    local.pop_back();
    return nullptr;
  }
  if (builtinFunctionTable.count(name)) {
    vector<any> values;
    for (Expression* arg : arguments) {
      values.push_back(arg->interpret());
    }
    return builtinFunctionTable[name](values);
  }
  cout << "Function " << name << " does not exist." << endl;
  exit(1);
}
auto GetElement::interpret() -> any {
  any object = sub->interpret();
  any idx = index->interpret();

  if (isArray(object) && isNumber(idx)) {
    return getValueOfArray(object, idx);
  }
  if (isMap(object) && isString(idx)) {
    return getValueOfMap(object, idx);
  }

  cout << "Element reference error" << endl;
  exit(1);
}
auto SetElement::interpret() -> any {
  any object = sub->interpret();
  any idx = index->interpret();
  any val = value->interpret();

  if (isArray(object) && isNumber(idx)) {
    return setValueOfArray(object, idx, val);
  }
  if (isMap(object) && isString(idx)) {
    return setValueOfMap(object, idx, val);
  }

  cout << "Element reference error" << endl;
  exit(1);
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
  if (functionTable.count(name)) {
    return functionTable[name];
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
  Array* result = new Array();
  for (Expression* node : values) {
    result->values.push_back(node->interpret());
  }
  return result;
}
auto MapLiteral::interpret() -> any {
  Map* result = new Map();
  for (auto& [key, value] : values) {
    result->values[key] = value->interpret();
  }
  return result;
}
