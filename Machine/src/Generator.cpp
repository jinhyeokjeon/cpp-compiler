#include <map>
#include <list>
#include <tuple>
#include <vector>
#include <iomanip>
#include "Node.h"
#include "Code.h"

using namespace std;

static size_t getLocal(string);
static void setLocal(string);
static void initBlock();
static void pushBlock();
static void popBlock();
static size_t writeCode(Instruction);
static size_t writeCode(Instruction, any);
static void patchAddress(size_t);
static void patchOperand(size_t, size_t);

static vector<Code> codeList;
static map<string, size_t> functionTable;
static size_t localSize;
static vector<size_t> offsetStack;
static list<map<string, size_t>> symbolStack;
static vector<vector<size_t>> continueStack;
static vector<vector<size_t>> breakStack;

auto generate(Program* program) -> tuple<vector<Code>, map<string, size_t>> {
  codeList.clear();
  functionTable.clear();
  writeCode(Instruction::GetGlobal, string("main"));
  writeCode(Instruction::Call, static_cast<size_t>(0));
  writeCode(Instruction::Exit);
  for (auto& node : program->functions)
    node->generate();
  return { codeList, functionTable };
}
auto Function::generate() -> void {
  functionTable[name] = codeList.size();
  auto allocation = writeCode(Instruction::Alloca);
  initBlock();
  for (auto& name : parameters)
    setLocal(name);
  for (auto& node : block)
    node->generate();
  popBlock();
  patchOperand(allocation, localSize);
  writeCode(Instruction::Return);
}
auto Variable::generate() -> void {
  setLocal(name);
  expression->generate();
  // SetVariable, Variable 둘 다 SetLocal 사용하기 때문에 SetLocal은 값을 스택에 남겨두어야 한다.
  writeCode(Instruction::SetLocal, getLocal(name));
  writeCode(Instruction::PopOperand);
}
auto Return::generate() -> void {
  expression->generate();
  writeCode(Instruction::Return);
}
auto For::generate() -> void {
  breakStack.emplace_back();
  continueStack.emplace_back();
  pushBlock();
  variable->generate();
  auto jumpAddress = codeList.size();
  condition->generate();
  auto conditionJump = writeCode(Instruction::ConditionJump);
  for (auto& node : block)
    node->generate();
  auto continueAddress = codeList.size();
  expression->generate();
  writeCode(Instruction::PopOperand);
  writeCode(Instruction::Jump, jumpAddress);
  patchAddress(conditionJump);
  popBlock();
  for (auto& jump : breakStack.back())
    patchAddress(jump);
  breakStack.pop_back();
  for (auto& jump : continueStack.back())
    patchOperand(jump, continueAddress);
  continueStack.pop_back();
}
auto Break::generate() -> void {
  if (breakStack.empty()) return;
  auto jumpCode = writeCode(Instruction::Jump);
  breakStack.back().push_back(jumpCode);
}
auto Continue::generate() -> void {
  if (continueStack.empty()) return;
  auto jumpCode = writeCode(Instruction::Jump);
  continueStack.back().push_back(jumpCode);
}
auto If::generate() -> void {
  vector<size_t> jumpList;
  for (auto i = 0; i < conditions.size(); ++i) {
    conditions[i]->generate();
    auto conditionJump = writeCode(Instruction::ConditionJump);
    pushBlock();
    for (auto& node : blocks[i])
      node->generate();
    popBlock();
    jumpList.push_back(writeCode(Instruction::Jump));
    patchAddress(conditionJump);
  }
  if (!elseBlock.empty()) {
    pushBlock();
    for (auto& node : elseBlock)
      node->generate();
    popBlock();
  }
  for (auto& jump : jumpList)
    patchAddress(jump);
}
auto Print::generate() -> void {
  for (int i = arguments.size() - 1; i >= 0; --i) {
    arguments[i]->generate();
  }
  writeCode(Instruction::Print, arguments.size());
  if (lineFeed)
    writeCode(Instruction::PrintLine);
}
auto ExpressionStatement::generate() -> void {
  expression->generate();
  writeCode(Instruction::PopOperand);
}
auto Or::generate() -> void {
  lhs->generate();
  // LogicalOr = 스택에 있는 값이 참이면 값을 그대로 둔 후, 오른쪽 식의 끝으로 점프.
  // 아니라면 왼쪽 식의 값을 피연산자 스택에서 꺼내버리는 명령어.
  auto logicalOr = writeCode(Instruction::LogicalOr);
  rhs->generate();
  patchAddress(logicalOr);
}
auto And::generate() -> void {
  lhs->generate();
  // LogicalAnd = 스택에 있는 값이 거짓이면 값을 그대로 둔 후, 오른쪽 식의 끝으로 점프.
  // 아니라면 왼쪽 식의 값을 피연산자 스택에서 꺼내버리는 명령어.
  auto logicalAnd = writeCode(Instruction::LogicalAnd);
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
  sub->generate();
  writeCode(instruction[kind]);
}
auto Call::generate() -> void {
  for (int i = arguments.size() - 1; i >= 0; i--)
    arguments[i]->generate();
  sub->generate();
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
  if (getLocal(name) == SIZE_MAX)
    writeCode(Instruction::GetGlobal, name);
  else
    writeCode(Instruction::GetLocal, getLocal(name));
}
auto SetVariable::generate() -> void {
  value->generate();
  if (getLocal(name) == SIZE_MAX)
    writeCode(Instruction::SetGlobal, name);
  else
    writeCode(Instruction::SetLocal, getLocal(name));
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
  for (int i = values.size() - 1; i >= 0; --i)
    values[i]->generate();
  writeCode(Instruction::PushArray, values.size());
}
auto MapLiteral::generate() -> void {
  for (auto& [key, value] : values) {
    writeCode(Instruction::PushString, key);
    value->generate();
  }
  writeCode(Instruction::PushMap, values.size());
}

auto initBlock() -> void {
  localSize = 0;
  offsetStack.push_back(0);
  symbolStack.emplace_front();
}
auto pushBlock() -> void {
  symbolStack.emplace_front();
  offsetStack.push_back(offsetStack.back());
}
auto popBlock() -> void {
  offsetStack.pop_back();
  symbolStack.pop_front();
}
auto getLocal(string name) -> size_t {
  for (auto& symbolTable : symbolStack) {
    if (symbolTable.count(name))
      return symbolTable[name];
  }
  return SIZE_MAX;
}
auto setLocal(string name) -> void {
  symbolStack.front()[name] = offsetStack.back();
  ++offsetStack.back();
  localSize = max(localSize, offsetStack.back());
}
auto writeCode(Instruction instruction) -> size_t {
  codeList.push_back({ instruction });
  return codeList.size() - 1;
}
auto writeCode(Instruction instruction, any operand) -> size_t {
  codeList.push_back({ instruction, operand });
  return codeList.size() - 1;
}
auto patchAddress(size_t codeIndex) -> void {
  codeList[codeIndex].operand = codeList.size();
}
auto patchOperand(size_t codeIndex, size_t operand) -> void {
  codeList[codeIndex].operand = operand;
}
