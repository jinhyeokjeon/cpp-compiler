#pragma once

#include <iostream>
#include <vector>
#include <string>
#include "Token.h"
#include "Node.h"
using namespace std;

auto scan(string&) -> vector<Token>;
auto parse(vector<Token>&) -> Program*;