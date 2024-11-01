#include "Main.h"

int main() {
  string sourceCode = R""""(
    function main() {
    }
  )"""";
  vector<Token> tokenList = scan(sourceCode);
  Program* syntaxTree = parse(tokenList);
  interpret(syntaxTree);
  return 0;
}