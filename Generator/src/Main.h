#pragma once
#include <tuple>
#include <string>
#include <fstream>
#include <iomanip>
#include <iostream>
#include "Token.h"
#include "Node.h"
#include "Code.h"
using namespace std;

vector<Token> scan(string);
Program* parse(vector<Token>&);
tuple<vector<Code>, map<string, size_t>> generate(Program*);
void printObjectCode(tuple<vector<Code>, map<string, size_t>>);