#pragma once
#include "Token.h"
#include "Node.h"

vector<Token> scan(string);
Program* parse(vector<Token>&);
void printSyntaxTree(Program*);