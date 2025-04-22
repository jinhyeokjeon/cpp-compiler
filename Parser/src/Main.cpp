#include <iostream>
#include <string>
using namespace std;

#include "Scanner.h"
#include "utils.h"

auto main() -> int {
  string sourceCode = R"""(
    func main() {
      printLine("Hello, World!"); // print Hello, World
      printLine(1 + 2 * 3); // arithmetic calculation
      var _num_ = 3;
    }
  )""";
  vector<Token> tokenList = scan(sourceCode);
  printTokenList(tokenList);
}