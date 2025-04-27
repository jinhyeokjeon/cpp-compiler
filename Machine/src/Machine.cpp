#include <any>
#include <map>
#include <list>
#include <vector>
#include <functional>
#include <cmath>
#include <set>
#include "Datatype.h"
#include "Code.h"
using namespace std;

struct StackFrame {
  vector<any> variables;
  vector<any> operandStack;
  size_t instructionPointer = 0;
};
static list<Object*> objects;
static vector<StackFrame> callStack;
static map<string, any> global;
extern map<string, function<any(vector<any>)>> builtinFunctionTable;

static auto pushOperand(any value) -> void;
static auto popOperand() -> any;
static auto peekOperand() -> any;
static auto collectGarbage() -> void;
static auto markObject(any) -> void;
static auto sweepObject() -> void;

auto execute(tuple<vector<Code>, map<string, size_t>, set<string>>& objectCode) -> void {
  callStack.emplace_back();
  vector<Code> codeList = get<0>(objectCode);
  map<string, size_t> functionTable = get<1>(objectCode);
  for (const string& name : get<2>(objectCode)) {
    global[name] = 0;
  }
  while (true) {
    Code code = codeList[callStack.back().instructionPointer];
    switch (code.instruction) {
    case Instruction::Exit: {
      callStack.pop_back();
      return;
    }
    case Instruction::Call: {
      any operand = popOperand();
      if (isSize(operand)) {
        StackFrame stackFrame;
        stackFrame.instructionPointer = toSize(operand);
        for (size_t i = 0; i < toSize(code.operand); ++i) {
          stackFrame.variables.push_back(popOperand());
        }
        callStack.push_back(stackFrame);
        continue;
      }
      else if (isString(operand) && builtinFunctionTable.count(toString(operand))) {
        vector<any> arguments;
        for (size_t i = 0; i < toSize(code.operand); ++i) {
          arguments.push_back(popOperand());
        }
        pushOperand(builtinFunctionTable[toString(operand)](arguments));
        break;
      }
      else {
        cout << "Function " << operand << " doesn't exist" << endl;
        exit(1);
      }
    }
    case Instruction::Alloca: {
      size_t localSize = toSize(code.operand);
      callStack.back().variables.resize(localSize);
      break;
    }
    case Instruction::Return: {
      any result = nullptr;
      if (!callStack.back().operandStack.empty()) {
        result = callStack.back().operandStack.back();
      }
      callStack.pop_back();
      callStack.back().operandStack.push_back(result);
      collectGarbage();
      break;
    }
    case Instruction::Jump: {
      callStack.back().instructionPointer = toSize(code.operand);
      continue;
    }
    case Instruction::ConditionJump: {
      any condition = popOperand();
      if (isTrue(condition)) {
        break;
      }
      callStack.back().instructionPointer = toSize(code.operand);
      continue;
    }
    case Instruction::Print: {
      for (size_t i = 0; i < toSize(code.operand); ++i) {
        any value = popOperand();
        cout << value;
      }
      break;
    }
    case Instruction::PrintLine: {
      cout << endl;
      break;
    }
    case Instruction::LogicalOr: {
      any value = popOperand();
      if (isTrue(value)) {
        pushOperand(value);
        callStack.back().instructionPointer = toSize(code.operand);
        continue;
      }
      break;
    }
    case Instruction::LogicalAnd: {
      any value = popOperand();
      if (isFalse(value)) {
        pushOperand(value);
        callStack.back().instructionPointer = toSize(code.operand);
        continue;
      }
      break;
    }
    case Instruction::Add: {
      any rValue = popOperand();
      any lValue = popOperand();

      if (isNumber(lValue) && isNumber(rValue)) {
        pushOperand(toNumber(lValue) + toNumber(rValue));
      }
      else if (isString(lValue) && isString(rValue)) {
        pushOperand(toString(lValue) + toString(rValue));
      }
      else {
        cout << lValue << " + " << rValue << " is wrong." << endl;
        exit(1);
      }
      break;
    }
    case Instruction::Subtract: {
      any rValue = popOperand();
      any lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue)) {
        pushOperand(toNumber(lValue) - toNumber(rValue));
      }
      else {
        cout << lValue << " - " << rValue << " is wrong" << endl;
        exit(1);
      }
      break;
    }
    case Instruction::Multiply: {
      any rValue = popOperand();
      any lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue)) {
        pushOperand(toNumber(lValue) * toNumber(rValue));
      }
      else if (isString(lValue) && isNumber(rValue)) {
        string ret = "";
        for (int i = 0; i < toNumber(rValue); ++i) {
          ret += toString(lValue);
        }
        pushOperand(ret);
      }
      else {
        cout << lValue << " * " << rValue << " is wrong." << endl;
        exit(1);
      }
      break;
    }
    case Instruction::Divide: {
      any rValue = popOperand();
      any lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue)) {
        pushOperand(toNumber(lValue) / toNumber(rValue));
      }
      else {
        cout << lValue << " / " << rValue << " is wrong." << endl;
        exit(1);
      }
      break;
    }
    case Instruction::Modulo: {
      any rValue = popOperand();
      any lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue)) {
        pushOperand(fmod(toNumber(lValue), toNumber(rValue)));
      }
      else {
        cout << lValue << " % " << rValue << " is wrong." << endl;
        exit(1);
      }
      break;
    }
    case Instruction::Equal: {
      any rValue = popOperand();
      any lValue = popOperand();
      if (isBoolean(lValue) && isBoolean(rValue)) {
        pushOperand(toBoolean(lValue) == toBoolean(rValue));
      }
      else if (isNumber(lValue) && isNumber(rValue)) {
        pushOperand(toNumber(lValue) == toNumber(rValue));
      }
      else if (isString(lValue) && isString(rValue)) {
        pushOperand(toString(lValue) == toString(rValue));
      }
      else {
        cout << lValue << " == " << rValue << " is wrong." << endl;
        exit(1);
      }
      break;
    }
    case Instruction::NotEqual: {
      any rValue = popOperand();
      any lValue = popOperand();
      if (isBoolean(lValue) && isBoolean(rValue)) {
        pushOperand(toBoolean(lValue) != toBoolean(rValue));
      }
      else if (isNumber(lValue) && isNumber(rValue)) {
        pushOperand(toNumber(lValue) != toNumber(rValue));
      }
      else if (isString(lValue) && isString(rValue)) {
        pushOperand(toString(lValue) != toString(rValue));
      }
      else {
        cout << lValue << " != " << rValue << " is wrong." << endl;
        exit(1);
      }
      break;
    }
    case Instruction::LessThan: {
      any rValue = popOperand();
      any lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue)) {
        pushOperand(toNumber(lValue) < toNumber(rValue));
      }
      else {
        cout << lValue << " < " << rValue << " is wrong." << endl;
        exit(1);
      }
      break;
    }
    case Instruction::GreaterThan: {
      any rValue = popOperand();
      any lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue)) {
        pushOperand(toNumber(lValue) > toNumber(rValue));
      }
      else {
        cout << lValue << " > " << rValue << " is wrong." << endl;
        exit(1);
      }
      break;
    }
    case Instruction::LessOrEqual: {
      any rValue = popOperand();
      any lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue)) {
        pushOperand(toNumber(lValue) <= toNumber(rValue));
      }
      else {
        cout << lValue << " <= " << rValue << " is wrong." << endl;
        exit(1);
      }
      break;
    }
    case Instruction::GreaterOrEqual: {
      any rValue = popOperand();
      any lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue)) {
        pushOperand(toNumber(lValue) >= toNumber(rValue));
      }
      else {
        cout << lValue << " >= " << rValue << " is wrong." << endl;
        exit(1);
      }
      break;
    }
    case Instruction::Absolute: {
      any value = popOperand();
      if (isNumber(value)) {
        pushOperand(fabs(toNumber(value)));
      }
      else {
        cout << "+" << value << " is wrong." << endl;
        exit(1);
      }
      break;
    }
    case Instruction::ReverseSign: {
      any value = popOperand();
      if (isNumber(value)) {
        pushOperand(-toNumber(value));
      }
      else {
        cout << "-" << value << " is wrong." << endl;
        exit(1);
      }
      break;
    }
    case Instruction::GetElement: {
      any index = popOperand();
      any object = popOperand();
      if (isArray(object) && isNumber(index)) {
        pushOperand(getValueOfArray(object, index));
      }
      else if (isMap(object) && isString(index)) {
        pushOperand(getValueOfMap(object, index));
      }
      else {
        cout << "Reference error." << endl;
        exit(1);
      }
      break;
    }
    case Instruction::SetElement: {
      any index = popOperand();
      any object = popOperand();
      if (isArray(object) && isNumber(index)) {
        pushOperand(setValueOfArray(object, index, peekOperand()));
      }
      else if (isMap(object) && isString(index)) {
        pushOperand(setValueOfMap(object, index, peekOperand()));
      }
      else {
        cout << "Reference error." << endl;
        exit(1);
      }
      break;
    }
    case Instruction::GetGlobal: {
      string name = toString(code.operand);
      if (functionTable.count(name)) {
        pushOperand(functionTable[name]);
      }
      else if (builtinFunctionTable.count(name)) {
        pushOperand(name);
      }
      else if (global.count(name)) {
        pushOperand(global[name]);
      }
      else {
        cout << name << " doesn't exist." << endl;
        exit(1);
      }
      break;
    }
    case Instruction::SetGlobal: {
      string name = toString(code.operand);
      if (global.count(name) == 0) {
        cout << name << " doesn't exist." << endl;
        exit(1);
      }
      global[name] = peekOperand();
      break;
    }
    case Instruction::GetLocal: {
      size_t index = toSize(code.operand);
      pushOperand(callStack.back().variables[index]);
      break;
    }
    case Instruction::SetLocal: {
      size_t index = toSize(code.operand);
      callStack.back().variables[index] = peekOperand();
      break;
    }
    case Instruction::PushNull: {
      pushOperand(nullptr);
      break;
    }
    case Instruction::PushBoolean: {
      pushOperand(code.operand);
      break;
    }
    case Instruction::PushNumber: {
      pushOperand(code.operand);
      break;
    }
    case Instruction::PushString: {
      pushOperand(code.operand);
      break;
    }
    case Instruction::PushArray: {
      Array* result = new Array();
      size_t size = toSize(code.operand);
      for (size_t i = 0; i < size; ++i) {
        result->values.push_back(popOperand());
      }
      pushOperand(result);
      objects.push_back(result);
      break;
    }
    case Instruction::PushMap: {
      Map* result = new Map();
      for (size_t i = 0; i < toSize(code.operand); ++i) {
        any value = popOperand();
        any key = popOperand();
        result->values[toString(key)] = value;
      }
      pushOperand(result);
      objects.push_back(result);
      break;
    }
    case Instruction::PopOperand: {
      popOperand();
      break;
    }
    }
    ++callStack.back().instructionPointer;
  }
}

static auto pushOperand(any value) -> void {
  callStack.back().operandStack.push_back(value);
}
static auto popOperand() -> any {
  any value = callStack.back().operandStack.back();
  callStack.back().operandStack.pop_back();
  return value;
}
static auto peekOperand() -> any {
  return callStack.back().operandStack.back();
}
static auto collectGarbage() -> void {
  for (StackFrame& stackFrame : callStack) {
    for (any& value : stackFrame.variables) {
      markObject(value);
    }
    for (any& value : stackFrame.operandStack) {
      markObject(value);
    }
  }
  for (auto& [key, value] : global) {
    markObject(value);
  }
  sweepObject();
}
static auto markObject(any value) -> void {
  if (isArray(value)) {
    if (toArray(value)->isMarked) return;
    toArray(value)->isMarked = true;
    for (any& value : toArray(value)->values) {
      markObject(value);
    }
  }
  else if (isMap(value)) {
    if (toMap(value)->isMarked) return;
    for (auto& [key, value] : toMap(value)->values) {
      markObject(value);
    }
  }
}
static auto sweepObject() -> void {
  objects.remove_if([](Object* object) {
    if (object->isMarked) {
      object->isMarked = false;
      return false;
    }
    delete object;
    return true;
    });
}