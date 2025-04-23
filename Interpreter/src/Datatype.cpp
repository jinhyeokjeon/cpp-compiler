#include "Datatype.h"

auto isNull(any& value) -> bool;
auto isTrue(any value) -> bool;
auto isFalse(any value) -> bool;
auto isString(any& value) -> bool {
  return value.type() == typeid(string);
}
auto toString(any& value) -> string {
  return any_cast<string>(value);
}
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
auto operator<<(std::ostream& stream, any& value) -> std::ostream& {
  if (isString(value)) stream << toString(value);
  return stream;
}