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
    global[name] = any();
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
      break;
    }
    case Instruction::Jump:
    case Instruction::ConditionJump:
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
    case Instruction::LogicalOr:
    case Instruction::LogicalAnd:
    case Instruction::Add:
    case Instruction::Subtract:
    case Instruction::Multiply:
    case Instruction::Divide:
    case Instruction::Modulo:
    case Instruction::Equal:
    case Instruction::NotEqual:
    case Instruction::LessThan:
    case Instruction::GreaterThan:
    case Instruction::LessOrEqual:
    case Instruction::GreaterOrEqual:
    case Instruction::Absolute:
    case Instruction::ReverseSign:
    case Instruction::GetElement:
    case Instruction::SetElement:
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
    case Instruction::SetGlobal:
    case Instruction::GetLocal:
    case Instruction::SetLocal:
    case Instruction::PushNull:
    case Instruction::PushBoolean:
    case Instruction::PushNumber:
    case Instruction::PushString: {
      pushOperand(code.operand);
      break;
    }
    case Instruction::PushArray:
    case Instruction::PushMap:
    case Instruction::PopOperand:
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

}
static auto collectGarbage() -> void {

}
static auto markObject(any) -> void {

}
static auto sweepObject() -> void {

}