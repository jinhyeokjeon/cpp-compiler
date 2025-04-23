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

auto isNull(any& value) -> bool;
auto isTrue(any value) -> bool;
auto isFalse(any value) -> bool;
auto isString(any& value) -> bool;
auto toString(any& value) -> string;
auto isBoolean(any& value) -> bool;
auto toBoolean(any& value) -> bool;
auto isNumber(any& value) -> bool;
auto toNumber(any& value) -> double;
auto isArray(any& value) -> bool;
auto toArray(any value) -> Array*;
auto isMap(any& value) -> bool;
auto toMap(any& value) -> Map*;
auto isFunction(any& value) -> bool;
auto toFunction(any& value) -> Function*;
auto isBuiltinFunction(any& value) -> bool;
auto getValueOfArray(any object, any index) -> any;
auto setValueOfArray(any object, any index, any value) -> any;
auto getValueOfMap(any object, any key) -> any;
auto setValueOfMap(any object, any key, any value) -> any;
auto toBuiltinFunction(any& value) -> function<any(vector<any>)>;
auto operator<<(ostream& stream, any& value) -> ostream&;