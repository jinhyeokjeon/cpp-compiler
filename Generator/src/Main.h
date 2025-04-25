#pragma once

#include <iostream>
#include <vector>
#include <string>
#include "Token.h"
#include "Node.h"
#include "Code.h"
using namespace std;

auto scan(string&) -> vector<Token>;
auto parse(vector<Token>&) -> Program*;
tuple<vector<Code>, map<string, size_t>, map<string, any>> generate(Program*);
void printObjectCode(tuple<vector<Code>, map<string, size_t>, map<string, any>>);