#pragma once

#include <iostream>
#include <vector>
#include <string>
#include "Token.h"
using namespace std;

auto scan(string&) -> vector<Token>;

auto printTokenList(vector<Token>&) -> void;