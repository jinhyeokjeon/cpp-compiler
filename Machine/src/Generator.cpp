#include <map>
#include <list>
#include <tuple>
#include <vector>
#include <iomanip>
#include <set>
#include "Node.h"
#include "Code.h"
using namespace std;

static auto getLocalIdx(string) -> size_t;
static auto setLocalIdx(string) -> void;
static auto initBlock() -> void;
static auto pushBlock() -> void;
static auto popBlock() -> void;
static auto patchAddress(size_t) -> void;
static auto patchOperand(size_t, size_t) -> void;
static auto writeCode(Instruction) -> size_t;
static auto writeCode(Instruction, any) -> size_t;

static vector<Code> codeList;
static map<string, size_t> functionTable;
static set<string> global;
static set<string> builtinFunctionList = { "sqrt", "length", "push", "pop", "erase", "clock" };
static size_t localSize;
static vector<size_t> offsetStack;
static list<map<string, size_t>> symbolStack;
static vector<vector<size_t>> continueAddrStack;
static vector<vector<size_t>> breakAddrStack;

auto generate(Program* program) -> tuple<vector<Code>, map<string, size_t>, set<string>> {
  // 전역 변수 초기화
  for (Variable* variable : program->variables) {
    if (builtinFunctionList.count(variable->name)) {
      cout << variable->name << " is duplicated." << endl;
      exit(1);
    }
    variable->generate2();
  }

  writeCode(Instruction::GetGlobal, string("main"));
  writeCode(Instruction::Call, static_cast<size_t>(0));
  writeCode(Instruction::Exit);

  for (Function* function : program->functions) {
    if (global.count(function->name) || builtinFunctionList.count(function->name)) {
      cout << function->name << " is duplicated." << endl;
      exit(1);
    }
    function->generate();
  }

  return { codeList, functionTable, global };
}

auto Function::generate() -> void {
  functionTable[name] = codeList.size();
  size_t temp = writeCode(Instruction::Alloca);
  initBlock();
  for (string& name : parameters) {
    setLocalIdx(name);
  }
  for (Statement* node : block) {
    node->generate();
  }
  popBlock();
  writeCode(Instruction::Return);
  patchOperand(temp, localSize);
}
auto Return::generate() -> void {
  if (expression != nullptr) {
    expression->generate();
  }
  writeCode(Instruction::Return);
}
auto Variable::generate() -> void {
  setLocalIdx(name);
  expression->generate();
  writeCode(Instruction::SetLocal, getLocalIdx(name));
  writeCode(Instruction::PopOperand);
}
auto Variable::generate2() -> void {
  global.insert(name);
  expression->generate();
  writeCode(Instruction::SetGlobal, name);
  writeCode(Instruction::PopOperand);
}
auto For::generate() -> void {
  continueAddrStack.emplace_back();
  breakAddrStack.emplace_back();
  pushBlock();
  variable->generate();
  size_t jumpAddress = codeList.size();
  condition->generate();
  size_t conditionJump = writeCode(Instruction::ConditionJump);
  for (Statement* node : block) {
    node->generate();
  }
  for (size_t jumpAddr : continueAddrStack.back()) {
    patchAddress(jumpAddr);
  }
  continueAddrStack.pop_back();
  expression->generate();
  writeCode(Instruction::PopOperand);
  writeCode(Instruction::Jump, jumpAddress);
  patchAddress(conditionJump);
  popBlock();
  for (size_t jumpAddr : breakAddrStack.back()) {
    patchAddress(jumpAddr);
  }
  breakAddrStack.pop_back();
}
auto Break::generate() -> void {
  if (breakAddrStack.empty()) return;
  size_t breakAddr = writeCode(Instruction::Jump);
  breakAddrStack.back().push_back(breakAddr);
}
auto Continue::generate() -> void {
  if (continueAddrStack.empty()) return;
  size_t jumpAddr = writeCode(Instruction::Jump);
  continueAddrStack.back().push_back(jumpAddr);
}
auto If::generate() -> void {
  vector<size_t> idxList;
  for (int i = 0; i < conditions.size(); ++i) {
    conditions[i]->generate();
    size_t conditionJump = writeCode(Instruction::ConditionJump);
    pushBlock();
    for (Statement* node : blocks[i]) {
      node->generate();
    }
    popBlock();
    idxList.push_back(writeCode(Instruction::Jump));
    patchAddress(conditionJump);
  }
  if (!elseBlock.empty()) {
    pushBlock();
    for (Statement* node : elseBlock) {
      node->generate();
    }
    popBlock();
  }
  for (size_t idx : idxList) {
    patchAddress(idx);
  }
}
auto Print::generate() -> void {
  for (int i = arguments.size() - 1; i >= 0; --i) {
    arguments[i]->generate();
  }
  writeCode(Instruction::Print, arguments.size());
  if (lineFeed) {
    writeCode(Instruction::PrintLine);
  }
}
auto ExpressionStatement::generate() -> void {
  expression->generate();
  writeCode(Instruction::PopOperand);
}

auto Or::generate() -> void {
  lhs->generate();
  size_t logicalOr = writeCode(Instruction::LogicalOr);
  rhs->generate();
  patchAddress(logicalOr);
}
auto And::generate() -> void {
  lhs->generate();
  size_t logicalAnd = writeCode(Instruction::LogicalAnd);
  rhs->generate();
  patchAddress(logicalAnd);
}
auto Relational::generate() -> void {
  map<Kind, Instruction> instructions = {
    {Kind::Equal,          Instruction::Equal},
    {Kind::NotEqual,       Instruction::NotEqual},
    {Kind::LessThan,       Instruction::LessThan},
    {Kind::GreaterThan,    Instruction::GreaterThan},
    {Kind::LessOrEqual,    Instruction::LessOrEqual},
    {Kind::GreaterOrEqual, Instruction::GreaterOrEqual}
  };
  lhs->generate();
  rhs->generate();
  writeCode(instructions[kind]);
}
auto Arithmetic::generate() -> void {
  map<Kind, Instruction> instructions = {
    {Kind::Add,      Instruction::Add},
    {Kind::Subtract, Instruction::Subtract},
    {Kind::Multiply, Instruction::Multiply},
    {Kind::Divide,   Instruction::Divide},
    {Kind::Modulo,   Instruction::Modulo},
  };
  lhs->generate();
  rhs->generate();
  writeCode(instructions[kind]);
}
auto Unary::generate() -> void {
  map<Kind, Instruction> instruction = {
    {Kind::Add,      Instruction::Absolute},
    {Kind::Subtract, Instruction::ReverseSign}
  };
  if (instruction.count(kind)) {
    sub->generate();
    writeCode(instruction[kind]);
  }
  else {
    size_t idx = getLocalIdx(name);
    if (idx == SIZE_MAX) {
      if (global.count(name)) {
        writeCode(Instruction::GetGlobal, name);
        writeCode(Instruction::PushNumber, double(1));
        if (kind == Kind::Increase) {
          writeCode(Instruction::Add);
        }
        else {
          writeCode(Instruction::Subtract);
        }
        writeCode(Instruction::SetGlobal, name);
      }
      else {
        cout << name << " doesn't exist." << endl;
        exit(1);
      }
    }
    else {
      writeCode(Instruction::GetLocal, idx);
      writeCode(Instruction::PushNumber, double(1));
      if (kind == Kind::Increase) {
        writeCode(Instruction::Add);
      }
      else {
        writeCode(Instruction::Subtract);
      }
      writeCode(Instruction::SetLocal, idx);
    }
  }
}
auto Call::generate() -> void {
  for (int i = arguments.size() - 1; i >= 0; --i) {
    arguments[i]->generate();
  }
  writeCode(Instruction::GetGlobal, name);
  writeCode(Instruction::Call, arguments.size());
}
auto GetElement::generate() -> void {
  sub->generate();
  index->generate();
  writeCode(Instruction::GetElement);
}
auto SetElement::generate() -> void {
  value->generate();
  sub->generate();
  index->generate();
  writeCode(Instruction::SetElement);
}
auto GetVariable::generate() -> void {
  size_t idx = getLocalIdx(name);
  if (idx == SIZE_MAX) {
    if (global.count(name) || functionTable.count(name) || builtinFunctionList.count(name)) {
      writeCode(Instruction::GetGlobal, name);
    }
    else {
      cout << name << " doesn't exist" << endl;
      exit(1);
    }
  }
  else {
    writeCode(Instruction::GetLocal, getLocalIdx(name));
  }
}
auto SetVariable::generate() -> void {
  value->generate();
  size_t idx = getLocalIdx(name);
  if (idx == SIZE_MAX) {
    if (global.count(name)) {
      writeCode(Instruction::SetGlobal, name);
    }
    else {
      cout << name << " doesn't exist." << endl;
      exit(1);
    }
  }
  else {
    writeCode(Instruction::SetLocal, idx);
  }
}
auto NullLiteral::generate() -> void {
  writeCode(Instruction::PushNull);
}
auto BooleanLiteral::generate() -> void {
  writeCode(Instruction::PushBoolean, value);
}
auto NumberLiteral::generate() -> void {
  writeCode(Instruction::PushNumber, value);
}
auto StringLiteral::generate() -> void {
  writeCode(Instruction::PushString, value);
}
auto ArrayLiteral::generate() -> void {
  for (int i = values.size() - 1; i >= 0; --i) {
    values[i]->generate();
  }
  writeCode(Instruction::PushArray, values.size());
}
auto MapLiteral::generate() -> void {
  for (auto& [key, value] : values) {
    writeCode(Instruction::PushString, key);
    value->generate();
  }
  writeCode(Instruction::PushMap, values.size());
}

static auto getLocalIdx(string name) -> size_t {
  for (map<string, size_t>& symbolTable : symbolStack) {
    if (symbolTable.count(name)) {
      return symbolTable[name];
    }
  }
  return SIZE_MAX;
}
static auto setLocalIdx(string name) -> void {
  symbolStack.front()[name] = offsetStack.back();
  offsetStack.back() += 1;
  localSize = max(localSize, offsetStack.back());
}
static auto initBlock() -> void {
  localSize = 0;
  offsetStack.push_back(0);
  symbolStack.emplace_front();
}
static auto pushBlock() -> void {
  offsetStack.push_back(offsetStack.back());
  symbolStack.emplace_front();
}
static auto popBlock() -> void {
  offsetStack.pop_back();
  symbolStack.pop_front();
}
static auto patchAddress(size_t codeIndex) -> void {
  codeList[codeIndex].operand = codeList.size();
}
static auto patchOperand(size_t codeIndex, size_t operand) -> void {
  codeList[codeIndex].operand = operand;
}
static auto writeCode(Instruction instruction) -> size_t {
  codeList.push_back({ instruction });
  return codeList.size() - 1;
}
static auto writeCode(Instruction instruction, any operand) -> size_t {
  codeList.push_back({ instruction, operand });
  return codeList.size() - 1;
}
