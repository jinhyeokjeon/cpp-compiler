#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Main.h"
using namespace std;

auto main() -> int {
  ifstream file("./src/source.txt");
  if (!file) { return 1; }

  stringstream buffer;
  buffer << file.rdbuf();
  string sourceCode = buffer.str();

  vector<Token> tokenList = scan(sourceCode);
  Program* syntaxTree = parse(tokenList);
  tuple<vector<Code>, map<string, size_t>, set<string>> objectCode = generate(syntaxTree);
  execute(objectCode);
}