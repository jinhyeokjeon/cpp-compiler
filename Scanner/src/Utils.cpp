#include <iostream>
#include <vector>
#include <iomanip>
#include "Token.h"
using namespace std;

auto printTokenList(vector<Token>& tokenList) -> void {
  cout << setw(12) << left << "KIND" << "STRING" << endl;
  cout << string(23, '-') << endl;
  for (auto& token : tokenList)
    cout << token << endl;
}
