/*
#include <any>
#include <map>
#include <list>
#include <vector>
#include <functional>
#include <cmath>
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

auto execute(tuple<vector<Code>, map<string, size_t>> objectCode) -> void {
  global.clear();
  objects.clear();
  callStack.emplace_back();
  auto codeList = get<0>(objectCode);
  auto functionTable = get<1>(objectCode);
  while (true) {
    auto code = codeList[callStack.back().instructionPointer];
    switch (code.instruction) {
    case Instruction::Exit: {
      callStack.pop_back();
      return;
    }
    case Instruction::Call: {
      auto operand = popOperand();
      if (isSize(operand)) {
        StackFrame stackFrame;
        stackFrame.instructionPointer = toSize(operand);
        for (size_t i = 0; i < toSize(code.operand); ++i) {
          stackFrame.variables.push_back(callStack.back().operandStack.back());
          callStack.back().operandStack.pop_back();
        }
        callStack.push_back(stackFrame);
        continue;
      }
      if (isBuiltinFunction(operand)) {
        vector<any> arguments;
        for (auto i = 0; i < toSize(code.operand); ++i)
          arguments.push_back(popOperand());
        pushOperand(toBuiltinFunction(operand)(arguments));
        break;
      }
      cout << "함수가 존재하지 않습니다\n";
      pushOperand(nullptr);
      break;
    }
    case Instruction::Alloca: {
      //auto extraSize = toSize(code.operand);
      //auto currentSize = callStack.back().variables.size();
      //callStack.back().variables.resize(currentSize + extraSize); // 그냥 extraSize 아닌가?
      auto localSize = toSize(code.operand);
      callStack.back().variables.resize(localSize);
      break;
    }
    case Instruction::Return: {
      any result = nullptr;
      if (!callStack.back().operandStack.empty())
        result = callStack.back().operandStack.back();
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
      auto condition = popOperand();
      if (isTrue(condition))
        break;
      callStack.back().instructionPointer = toSize(code.operand);
      continue;
    }
    case Instruction::Print: {
      for (auto i = 0; i < toSize(code.operand); ++i) {
        auto value = popOperand();
        cout << value;
      }
      break;
    }
    case Instruction::PrintLine: {
      cout << endl;
      break;
    }
    case Instruction::LogicalOr: {
      auto value = popOperand();
      if (isTrue(value)) {
        pushOperand(value);
        callStack.back().instructionPointer = toSize(code.operand);
        continue;
      }
      break;
    }
    case Instruction::LogicalAnd: {
      auto value = popOperand();
      if (isFalse(value)) {
        pushOperand(value);
        callStack.back().instructionPointer = toSize(code.operand);
        continue;
      }
      break;
    }
    case Instruction::Equal: {
      auto lValue = popOperand();
      auto rValue = popOperand();
      if (isBoolean(lValue) && isBoolean(rValue))
        pushOperand(toBoolean(lValue) == toBoolean(rValue));
      else if (isNumber(lValue) && isNumber(rValue))
        pushOperand(toNumber(lValue) == toNumber(rValue));
      else if (isString(lValue) == isString(rValue))
        pushOperand(toString(lValue) == toString(rValue));
      else {
        cout << "연산 불가능\n";
        exit(1);
      }
      break;
    }
    case Instruction::NotEqual: {
      auto rValue = popOperand();
      auto lValue = popOperand();
      if (isBoolean(lValue) && isBoolean(rValue))
        pushOperand(toBoolean(lValue) != toBoolean(rValue));
      else if (isNumber(lValue) && isNumber(rValue))
        pushOperand(toNumber(lValue) != toNumber(rValue));
      else if (isString(lValue) == isString(rValue))
        pushOperand(toString(lValue) != toString(rValue));
      else {
        cout << "연산 불가능\n";
        exit(1);
      }
      break;
    }
    case Instruction::LessThan: {
      auto rValue = popOperand();
      auto lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue))
        pushOperand(toNumber(lValue) < toNumber(rValue));
      else {
        cout << "연산 불가능\n";
        exit(1);
      }
      break;
    }
    case Instruction::GreaterThan: {
      auto rValue = popOperand();
      auto lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue))
        pushOperand(toNumber(lValue) > toNumber(rValue));
      else {
        cout << "연산 불가능\n";
        exit(1);
      }
      break;
    }
    case Instruction::LessOrEqual: {
      auto rValue = popOperand();
      auto lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue))
        pushOperand(toNumber(lValue) <= toNumber(rValue));
      else {
        cout << "연산 불가능\n";
        exit(1);
      }
      break;
    }
    case Instruction::GreaterOrEqual: {
      auto rValue = popOperand();
      auto lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue))
        pushOperand(toNumber(lValue) >= toNumber(rValue));
      else {
        cout << "연산 불가능\n";
        exit(1);
      }
      break;
    }
    case Instruction::Add: {
      auto rValue = popOperand();
      auto lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue))
        pushOperand(toNumber(lValue) + toNumber(rValue));
      else if (isString(lValue) && isString(rValue))
        pushOperand(toString(lValue) + toString(rValue));
      else {
        cout << lValue << " + " << rValue << " 연산 불가능\n";
        exit(1);
      }
      break;
    }
    case Instruction::Subtract: {
      auto rValue = popOperand();
      auto lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue))
        pushOperand(toNumber(lValue) - toNumber(rValue));
      else {
        cout << lValue << " - " << rValue << " 연산 불가능\n";
        exit(1);
      }
      break;
    }
    case Instruction::Multiply: {
      auto rValue = popOperand();
      auto lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue))
        pushOperand(toNumber(lValue) * toNumber(rValue));
      else if (isString(lValue) && isNumber(rValue)) {
        string ret;
        for (int i = 0; i < toNumber(rValue); ++i)
          ret += toString(lValue)
        pushOperand(ret);
      }
      else {
        cout << lValue << " * " << rValue << " 연산 불가능\n";
        exit(1);
      }
      break;
    }
    case Instruction::Divide: {
      auto rValue = popOperand();
      auto lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue) && toNumber(rValue) != 0)
        pushOperand(toNumber(lValue) / toNumber(rValue));
      else {
        cout << lValue << " / " << rValue << " 연산 불가능\n";
        exit(1);
      }
      break;
    }
    case Instruction::Modulo: {
      auto rValue = popOperand();
      auto lValue = popOperand();
      if (isNumber(lValue) && isNumber(rValue) && toNumber(rValue) != 0)
        pushOperand(fmod(toNumber(lValue), toNumber(rValue)));
      else {
        cout << lValue << " % " << rValue << " 연산 불가능\n";
        exit(1);
      }
      break;
    }
    case Instruction::Absolute: {
      auto value = popOperand();
      if (isNumber(value))
        pushOperand(abs(toNumber(value)));
      else {
        cout << "|" << value << "|" << "연산 불가능\n";
        exit(1);
      }
      break;
    }
    case Instruction::ReverseSign: {
      auto value = popOperand();
      if (isNumber(value))
        pushOperand(toNumber(value) * -1);
      else {
        cout << "-" << value << " 연산 불가능\n";
        exit(1);
      }
      break;
    }
    case Instruction::GetElement: {
      auto index = popOperand();
      auto sub = popOperand();
      if (isArray(sub) && isNumber(index))
        pushOperand(getValueOfArray(sub, index));
      else if (isMap(sub) && isString(index))
        pushOperand(getValueOfMap(sub, index));
      else {
        cout << "참조 오류\n";
        exit(1);
      }
      break;
    }
    case Instruction::SetElement: {
      auto index = popOperand();
      auto sub = popOperand();
      if (isArray(sub) && isNumber(index))
        setValueOfArray(sub, index, peekOperand());
      else if (isMap(sub) && isString(index))
        setValueOfMap(sub, index, peekOperand());
      else {
        cout << "참조 오류\n";
        exit(1);
      }
      break;
    }
    case Instruction::GetGlobal: {
      auto name = toString(code.operand);
      if (functionTable.count(name))
        pushOperand(functionTable[name]);
      else if (builtinFunctionTable.count(name))
        pushOperand(builtinFunctionTable[name]);
      else if (global.count(name))
        pushOperand(global[name]);
      else
        pushOperand(nullptr);
      break;
    }
    case Instruction::SetGlobal: {
      auto name = toString(code.operand);
      global[name] = peekOperand();
      break;
    }
    case Instruction::GetLocal: {
      auto index = toSize(code.operand);
      pushOperand(callStack.back().variables[index]);
      break;
    }
    case Instruction::SetLocal: {
      auto index = toSize(code.operand);
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
      auto result = new Array(); // Array를 동적 할당함.
      auto size = toSize(code.operand);
      for (auto i = 0; i < size; ++i)
        result->values.push_back(popOperand());
      pushOperand(result);
      objects.push_back(result);
      break;
    }
    case Instruction::PushMap: {
      auto result = new Map(); // Map을 동적 할당함.
      for (auto i = 0; i < toSize(code.operand); ++i) {
        auto value = popOperand();
        auto key = toString(popOperand());
        result->values[key] = value;
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
  auto value = callStack.back().operandStack.back();
  callStack.back().operandStack.pop_back();
  return value;
}
static auto peekOperand() -> any {
  return callStack.back().operandStack.back();
}
static auto collectGarbage() -> void {
  for (auto& stackFrame : callStack) {
    for (auto& value : stackFrame.operandStack)
      markObject(value);
    for (auto& value : stackFrame.variables)
      markObject(value);
  }
  for (auto& [key, value] : global)
    markObject(value);
  sweepObject();
}
static auto markObject(any value) -> void {
  if (isArray(value)) {
    if (toArray(value)->isMarked) return;
    toArray(value)->isMarked = true;
    for (auto& value : toArray(value)->values)
      markObject(value);
  }
  else if (isMap(value)) {
    if (toMap(value)->isMarked) return;
    toMap(value)->isMarked = true;
    for (auto& [key, value] : toMap(value)->values)
      markObject(value);
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
*/