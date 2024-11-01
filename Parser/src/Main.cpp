#include "Main.h"

int main() {
  string sourceCode = R""""(
    function main() {
      global = 4;
    }
  )"""";
  vector<Token> tokenList = scan(sourceCode);
  Program* syntaxTree = parse(tokenList);
  printSyntaxTree(syntaxTree);
  return 0;
}