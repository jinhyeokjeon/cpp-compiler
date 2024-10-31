#include "Main.h"

int main(int argc, char** argv) {
  string sourceCode = R""""(
    function main() {
      print("Hello, World!");
      for(var i=0; i<3; i=i+1) {
        print("Hello, World!");
      }
    }
  )"""";
  vector<Token> tokenList = scan(sourceCode);
  printTokenList(tokenList);
  return 0;
}

void printTokenList(vector<Token>& tokenList) {
  cout << setw(15) << left << "KIND" << "STRING" << endl;
  cout << string(23, '-') << endl;
  for (auto& token : tokenList)
    cout << token << endl;
}