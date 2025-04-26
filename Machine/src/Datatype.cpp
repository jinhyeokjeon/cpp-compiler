#include "Datatype.h"

auto isNull(any& value) -> bool {
  return value.type() == typeid(nullptr_t);
}
auto isTrue(any value) -> bool {
  return toBoolean(value) == true;
}
auto isFalse(any value) -> bool {
  return toBoolean(value) == false;
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
  if (isBoolean(value)) {
    return any_cast<bool>(value);
  }
  else if (isNumber(value)) {
    return bool(toNumber(value));
  }
  cout << value << " cannot be converted to boolean." << endl;
  exit(1);
}
auto isNumber(any& value) -> bool {
  return value.type() == typeid(double);
}
auto toNumber(any& value) -> double {
  if (isNumber(value)) {
    return any_cast<double>(value);
  }
  cout << value << " is not a number" << endl;
  exit(1);
}
auto isSize(any& value) -> bool {
  return value.type() == typeid(size_t);
}
auto toSize(any& value) -> size_t {
  return any_cast<size_t>(value);
}
auto isArray(any& value) -> bool {
  return value.type() == typeid(Array*);
}
auto toArray(any value) -> Array* {
  return any_cast<Array*>(value);
}
auto isMap(any& value) -> bool {
  return value.type() == typeid(Map*);
}
auto toMap(any& value) -> Map* {
  return any_cast<Map*>(value);
}
auto getValueOfArray(any object, any index) -> any {
  size_t i = static_cast<size_t>(toNumber(index));
  if (i < 0 || i >= toArray(object)->values.size()) {
    cout << "Index error" << endl;
    exit(1);
  }
  return toArray(object)->values[i];
}
auto setValueOfArray(any object, any index, any value) -> any {
  size_t i = static_cast<size_t>(toNumber(index));
  if (i < 0 || i >= toArray(object)->values.size()) {
    cout << "Index error" << endl;
    exit(1);
  }
  return toArray(object)->values[i] = value;
}
auto getValueOfMap(any object, any key) -> any {
  if (toMap(object)->values.count(toString(key))) {
    return toMap(object)->values[toString(key)];
  }
  cout << "Index error." << endl;
  exit(1);
}
auto setValueOfMap(any object, any key, any value) -> any {
  toMap(object)->values[toString(key)] = value;
  return value;
}
auto operator<<(std::ostream& stream, any& value) -> std::ostream& {
  if (isString(value)) stream << toString(value);
  else if (isNumber(value)) stream << toNumber(value);
  else if (isBoolean(value)) stream << (isTrue(value) ? "true" : "false");
  else if (isNull(value)) stream << "null";
  else if (isArray(value)) {
    stream << "[ ";
    for (auto& value : toArray(value)->values)
      stream << value << " ";
    stream << "]";
  }
  else if (isMap(value)) {
    stream << "{ ";
    map<string, any>& m = toMap(value)->values;
    int i = 0;
    for (auto it = m.begin(); it != m.end(); ++it, ++i) {
      stream << it->first << ": " << it->second;
      if (i < m.size() - 1) stream << ", ";
      else stream << " ";
    }
    stream << "}";
  }
  return stream;
}
