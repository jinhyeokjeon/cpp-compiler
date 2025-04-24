#include <iostream>
#include <vector>
#include <iomanip>
#include "Token.h"
#include "Node.h"
#include "Code.h"
using namespace std;

auto printTokenList(vector<Token>& tokenList) -> void {
  cout << setw(12) << left << "KIND" << "STRING" << endl;
  cout << string(23, '-') << endl;
  for (auto& token : tokenList)
    cout << token << endl;
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