#include "Main.h"

int main() {
  string sourceCode = R""""(
    function main() {
      var array = [1, 2];
      array[0] = "element";
    }
  )"""";
  vector<Token> tokenList = scan(sourceCode);
  Program* syntaxTree = parse(tokenList);
  auto objectCode = generate(syntaxTree);
  printObjectCode(objectCode);
  return 0;
}

void printObjectCode(tuple<vector<Code>, map<string, size_t>> objectCode) {
  auto codeList = get<0>(objectCode);
  auto functionTable = get<1>(objectCode);
  cout << setw(11) << left << "FUNCTION" << "ADDRESS" << endl;
  cout << string(18, '-') << endl;
  for (auto& [key, value] : functionTable)
    cout << setw(11) << key << value << endl;
  cout << endl;
  cout << "ADDR" << " " << setw(15) << "INSTRUCTION" << "OPERAND" << endl;
  cout << string(36, '-') << endl;
  for (size_t i = 0; i < codeList.size(); i++)
    cout << setw(4) << right << i << " " << codeList[i] << endl;
}