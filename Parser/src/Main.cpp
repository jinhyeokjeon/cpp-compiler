#include <iostream>
#include <string>
#include "Main.h"
using namespace std;

auto main() -> int {
  string sourceCode = R"""(
    func main() {
      printLine("Hello, World!"); // print Hello, World
      printLine(1 + 2 * 3); // arithmetic calculation
      var _num_ = 3;
    }
  )""";
  vector<Token> tokenList = scan(sourceCode);
  Program* syntaxTree = parse(tokenList);

  printSyntaxTree(syntaxTree);
}