#include <map>
#include <list>
#include <tuple>
#include <vector>
#include <iomanip>
#include "Node.h"
#include "Code.h"
using namespace std;

static auto getLocal(string) -> size_t;
static auto setLocal(string) -> void;
static auto initBlock() -> void;
static auto pushBlock() -> void;
static auto popBlock() -> void;
static auto patchAddress(size_t) -> void;
static auto patchOperand(size_t, size_t) -> void;
static auto writeCode(Instruction) -> size_t;
static auto writeCode(Instruction, any) -> size_t;

static vector<Code> codeList;
static map<string, size_t> functionTable;
static map<string, any> global;
static size_t localSize;
static vector<size_t> offsetStack;
static list<map<string, size_t>> symbolStack;
static vector<vector<size_t>> continueStack;
static vector<vector<size_t>> breakStack;

auto generate(Program* program) -> tuple<vector<Code>, map<string, size_t>, map<string, any>> {
  writeCode(Instruction::GetGlobal, string("main"));
  writeCode(Instruction::Call, static_cast<size_t>(0));
  writeCode(Instruction::Exit);
  for (Variable* variable : program->variables) {
    variable->generate2();
  }
  for (Function* function : program->functions) {
    function->generate();
  }
  return { codeList, functionTable, global };
}

auto Function::generate() -> void {
  functionTable[name] = codeList.size();
  for (Statement* node : block) {
    node->generate();
  }
  writeCode(Instruction::Return);
}
auto Return::generate() -> void {}
auto Variable::generate() -> void {}
auto Variable::generate2() -> void {}
auto For::generate() -> void {}
auto Break::generate() -> void {}
auto Continue::generate() -> void {}
auto If::generate() -> void {}
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

auto Or::generate() -> void {}
auto And::generate() -> void {}
auto Relational::generate() -> void {}
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
auto Unary::generate() -> void {}
auto Call::generate() -> void {}
auto GetElement::generate() -> void {}
auto SetElement::generate() -> void {}
auto GetVariable::generate() -> void {}
auto SetVariable::generate() -> void {}
auto NullLiteral::generate() -> void {}
auto BooleanLiteral::generate() -> void {}
auto NumberLiteral::generate() -> void {
  writeCode(Instruction::PushNumber, value);
}
auto StringLiteral::generate() -> void {
  writeCode(Instruction::PushString, value);
}
auto ArrayLiteral::generate() -> void {}
auto MapLiteral::generate() -> void {}

static auto getLocal(string) -> size_t;
static auto setLocal(string) -> void;
static auto initBlock() -> void;
static auto pushBlock() -> void;
static auto popBlock() -> void;
static auto patchAddress(size_t) -> void;
static auto patchOperand(size_t, size_t) -> void;
static auto writeCode(Instruction instruction) -> size_t {
  codeList.push_back({ instruction });
  return codeList.size() - 1;
}
static auto writeCode(Instruction instruction, any operand) -> size_t {
  codeList.push_back({ instruction, operand });
  return codeList.size() - 1;
}