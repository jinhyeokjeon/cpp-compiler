#include <iostream>
#include <ios>
#include <iomanip>
#include "Datatype.h"

bool isNull(any& value) {
  return value.type() == typeid(nullptr_t);
}
bool isTrue(any value) {
  return isBoolean(value) && toBoolean(value);
}
bool isFalse(any value) {
  return isBoolean(value) && toBoolean(value) == false;
}
bool isString(any& value) {
  return value.type() == typeid(string);
}
string toString(any& value) {
  return any_cast<string>(value);
}
bool isBoolean(any& value) {
  return value.type() == typeid(bool);
}
bool toBoolean(any& value) {
  return any_cast<bool>(value);
}
bool isNumber(any& value) {
  return value.type() == typeid(double);
}
double toNumber(any& value) {
  return any_cast<double>(value);
}
bool isArray(any& value) {
  return value.type() == typeid(Array*);
}
Array* toArray(any value) {
  return any_cast<Array*>(value);
}
bool isMap(any& value) {
  return value.type() == typeid(Map*);
}
Map* toMap(any& value) {
  return any_cast<Map*>(value);
}
bool isFunction(any& value) {
  return value.type() == typeid(Function*);
}
Function* toFunction(any& value) {
  return any_cast<Function*>(value);
}
bool isBuiltinFunction(any& value) {
  return value.type() == typeid(function<any(vector<any>)>);
}
function<any(vector<any>)> toBuiltinFunction(any& value) {
  return any_cast<function<any(vector<any>)>>(value);
}
any getValueOfArray(any object, any index) {
  auto i = static_cast<size_t>(toNumber(index));
  if (i >= 0 && i < toArray(object)->values.size())
    return toArray(object)->values[i];
  cout << "인덱스 참조 오류\n";
  exit(1);
}
any setValueOfArray(any object, any index, any value) {
  auto i = static_cast<size_t>(toNumber(index));
  if (i >= 0 && i < toArray(object)->values.size())
    return toArray(object)->values[i] = value;
  cout << "인덱스 참조 오류\n";
  exit(1);
}
any getValueOfMap(any object, any key) {
  if (toMap(object)->values.count(toString(key)))
    return toMap(object)->values[toString(key)];
  cout << "인덱스 참조 오류\n";
  exit(1);
}
any setValueOfMap(any object, any key, any value) {
  toMap(object)->values[toString(key)] = value;
  return value;
}
ostream& operator<<(ostream& stream, any& value) {
  if (isString(value))
    stream << toString(value);
  else if (isNumber(value))
    stream << toNumber(value);
  else if (isBoolean(value))
    stream << boolalpha << toBoolean(value);
  else if (isArray(value)) {
    stream << "[ ";
    for (auto& value : toArray(value)->values)
      stream << value << " ";
    stream << "]";
  }
  else if (isMap(value)) {
    stream << "{ ";
    for (auto& [key, value] : toMap(value)->values)
      stream << key << ":" << value << " ";
    stream << "}";
  }
  else if (isNull(value)) {
    stream << "null";
  }
  return stream;
}