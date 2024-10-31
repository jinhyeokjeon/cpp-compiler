#pragma once
#include <string>
#include <vector>
#include <iomanip>
#include <iostream>
#include "Token.h"

using namespace std;

vector<Token> scan(string);
void printTokenList(vector<Token>&);