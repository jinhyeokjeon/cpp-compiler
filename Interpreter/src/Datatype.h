#pragma once
#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <map>
#include <any>
#include "Node.h"

using namespace std;

struct Object {
  bool isMarked = false;
  virtual ~Object() = default; //  typeid를 활용하여 파생 클래스의 타입 정보를 얻기 위해서 사용
};
struct Array : Object {
  vector<any> values;
};
struct Map : Object {
  map<string, any> values;
};

bool isNull(any& value);
bool isTrue(any value);
bool isFalse(any value);
bool isString(any& value);
string toString(any& value);
bool isBoolean(any& value);
bool toBoolean(any& value);
bool isNumber(any& value);
double toNumber(any& value);
bool isArray(any& value);
Array* toArray(any value);
bool isMap(any& value);
Map* toMap(any& value);
bool isFunction(any& value);
Function* toFunction(any& value);
bool isBuiltinFunction(any& value);
any getValueOfArray(any object, any index);
any setValueOfArray(any object, any index, any value);
any getValueOfMap(any object, any key);
any setValueOfMap(any object, any key, any value);
function<any(vector<any>)> toBuiltinFunction(any& value);
ostream& operator<<(ostream& stream, any& value);