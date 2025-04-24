/*
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

struct ReturnException {
  any result;
};
struct BreakException {};
struct ContinueException {};

static map<string, Function*> functionTable;
static list<list<map<string, any>>> local;
static map<string, any> global;
extern map<string, function<any(vector<any>)>> builtinFunctionTable; // vector<any> 타입의 인자를 받고, any 타입을 반환하는 함수
void interpret(Program* program) {
  functionTable.clear();
  for (auto& node : program->functions)
    functionTable[node->name] = node;
  if (functionTable["main"] == nullptr)
    return;
  try {
    local.emplace_back().emplace_front();
    functionTable["main"]->interpret();
  }
  catch (ReturnException e) {
    local.pop_back();
  }
}
void Function::interpret() {
  for (auto& node : block) {
    node->interpret();
  }
}
void Variable::interpret() {
  local.back().front()[name] = expression->interpret();
}
void Return::interpret() {
  throw ReturnException{ expression->interpret() };
}
void For::interpret() {
  local.back().emplace_front();
  variable->interpret();
  while (true) {
    auto result = condition->interpret();
    if (!isTrue(result)) break;
    try {
      for (auto& node : block)
        node->interpret();
    }
    catch (ContinueException) {}
    catch (BreakException) { break; }
    expression->interpret();
  }
  local.back().pop_front();
}
void Break::interpret() {
  throw BreakException();
}
void Continue::interpret() {
  throw ContinueException();
}
void If::interpret() {
  for (int i = 0; i < conditions.size(); ++i) {
    auto result = conditions[i]->interpret();
    if (!isTrue(result)) continue;
    local.back().emplace_front();
    for (auto& node : blocks[i])
      node->interpret();
    local.back().pop_front();
    return;
  }
  if (elseBlock.empty()) return;
  local.back().emplace_front();
  for (auto& node : elseBlock)
    node->interpret();
  local.back().pop_front();
}
void Print::interpret() {
  for (auto& node : arguments) {
    auto value = node->interpret();
    cout << value;
  }
  if (lineFeed)
    cout << endl;
}
void ExpressionStatement::interpret() {
  expression->interpret();
}
any Or::interpret() {
  return isTrue(lhs->interpret()) ? true : rhs->interpret();
}
any And::interpret() {
  return isFalse(lhs->interpret()) ? false : rhs->interpret();
}
any Relational::interpret() {
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
  else if (isString(lValue) && isString(rValue)) {
    if (kind == Kind::LessThan) return toString(lValue) < toString(rValue);
    if (kind == Kind::LessOrEqual) return toString(lValue) <= toString(rValue);
    if (kind == Kind::GreaterThan) return toString(lValue) > toString(rValue);
    if (kind == Kind::GreaterOrEqual) return toString(lValue) >= toString(rValue);
    if (kind == Kind::Equal) return toString(lValue) == toString(rValue);
    if (kind == Kind::NotEqual) return toString(lValue) != toString(rValue);
  }
  else if (isBoolean(lValue) && isBoolean(rValue)) {
    if (kind == Kind::Equal) return toBoolean(lValue) == toBoolean(rValue);
    if (kind == Kind::NotEqual) return toBoolean(lValue) != toBoolean(rValue);
  }
  cout << lValue << " " << toString(kind) << " " << rValue << " 연산 불가능\n";
  exit(1);
}
any Arithmetic::interpret() {
  auto lValue = lhs->interpret();
  auto rValue = rhs->interpret();
  if (isNumber(lValue) && isNumber(rValue)) {
    if (kind == Kind::Add) return toNumber(lValue) + toNumber(rValue);
    else if (kind == Kind::Subtract) return toNumber(lValue) - toNumber(rValue);
    else if (kind == Kind::Multiply) return toNumber(lValue) * toNumber(rValue);
    else if (kind == Kind::Divide && toNumber(rValue) != 0) return toNumber(lValue) / toNumber(rValue);
    else if (kind == Kind::Modulo && toNumber(rValue) != 0) return fmod(toNumber(lValue), toNumber(rValue));
  }
  else if (isString(lValue) && isString(rValue) && kind == Kind::Add)
    return toString(lValue) + toString(rValue);
  else if (isString(lValue) && isNumber(rValue) && kind == Kind::Multiply) {
    string ret = "";
    for (int i = 0; i < (int)(toNumber(rValue)); ++i)
      ret += toString(lValue);
    return ret;
  }
  cout << lValue << " " << toString(kind) << " " << rValue << " 연산은 불가능합니다.\n";
  exit(1);
}
any Unary::interpret() {
  auto value = sub->interpret();
  if (isNumber(value)) {
    if (kind == Kind::Add) return toNumber(value);
    else return toNumber(value) * -1;
  }
  cout << toString(kind) << " " << value << " 는 연산 불가능합니다.\n";
  exit(1);
}
any Call::interpret() {
  auto value = sub->interpret();
  if (isBuiltinFunction(value)) {
    vector<any> values;
    for (int i = 0; i < arguments.size(); ++i)
      values.push_back(arguments[i]->interpret());
    return toBuiltinFunction(value)(values);
  }
  if (!isFunction(value)) {
    cout << "함수 호출 오류\n";
    exit(1);
  }
  map<string, any> parameters;
  for (auto i = 0; i < arguments.size(); ++i) {
    auto name = toFunction(value)->parameters[i];
    parameters[name] = arguments[i]->interpret();
  }
  local.emplace_back().push_front(parameters);
  try {
    toFunction(value)->interpret();
  }
  catch (ReturnException exception) {
    local.pop_back();
    return exception.result;
  }
  local.pop_back();
  return nullptr;
}
any GetElement::interpret() {
  auto object = sub->interpret();
  auto index_ = index->interpret();
  if (isArray(object) && isNumber(index_))
    return getValueOfArray(object, index_);
  if (isMap(object) && isString(index_))
    return getValueOfMap(object, index_);
  cout << "참조 오류\n";
  exit(1);
}
any SetElement::interpret() {
  auto object = sub->interpret();
  auto index_ = index->interpret();
  auto value_ = value->interpret();
  if (isArray(object) && isNumber(index_))
    return setValueOfArray(object, index_, value_);
  if (isMap(object) && isString(index_))
    return setValueOfMap(object, index_, value_);
  cout << "참조 오류\n";
  exit(1);
}
any GetVariable::interpret() {
  for (auto& variables : local.back()) {
    if (variables.count(name))
      return variables[name];
  }
  if (global.count(name)) {
    return global[name];
  }
  if (builtinFunctionTable.count(name))
    return builtinFunctionTable[name];
  if (functionTable.count(name))
    return functionTable[name];
  return nullptr;
}
any SetVariable::interpret() {
  for (auto& variables : local.back()) {
    if (variables.count(name))
      return variables[name] = value->interpret();
  }
  return global[name] = value->interpret();
}
any NullLiteral::interpret() {
  return nullptr;
}
any BooleanLiteral::interpret() {
  return value;
}
any NumberLiteral::interpret() {
  return value;
}
any StringLiteral::interpret() {
  return value;
}
any ArrayLiteral::interpret() {
  auto result = new Array();
  for (auto& node : values)
    result->values.push_back(node->interpret());
  return result;
}
any MapLiteral::interpret() {
  auto result = new Map();
  for (auto& [key, value] : values)
    result->values[key] = value->interpret();
  return result;
}
*/