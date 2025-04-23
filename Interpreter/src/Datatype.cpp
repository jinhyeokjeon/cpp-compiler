#include "Datatype.h"

auto isNull(any& value) -> bool {
  return value.type() == typeid(nullptr_t);
}
auto isTrue(any value) -> bool {
  return isBoolean(value) && toBoolean(value);
}
auto isFalse(any value) -> bool {
  return isBoolean(value) && !toBoolean(value);
}
auto isString(any& value) -> bool {
  return value.type() == typeid(string);
}
auto toString(any& value) -> string {
  return any_cast<string>(value);
}
auto isBoolean(any& value) -> bool {
  return value.type() == typeid(bool);
}
auto toBoolean(any& value) -> bool {
  return any_cast<bool>(value);
}
auto isNumber(any& value) -> bool {
  return value.type() == typeid(double);
}
auto toNumber(any& value) -> double {
  return any_cast<double>(value);
}
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
auto operator<<(std::ostream& stream, any& value) -> std::ostream& {
  if (isString(value)) stream << toString(value);
  else if (isNumber(value)) stream << toNumber(value);
  else if (isBoolean(value)) stream << (isTrue(value) ? "true" : "false");
  return stream;
}
