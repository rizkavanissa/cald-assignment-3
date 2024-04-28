#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define INPUT "input"
#define OUTPUT "output"
#define VARIABLE "variable"
#define REGISTER "register"
#define REG "reg"
#define INVALID "invalid"
//#define ADDSUB "addsub"
#define PLUS "+"
#define MINUS "-"
#define MULT "*"
//#define LOGIC "logic"
#define LT "<"
#define GT ">"
#define ET "=="
#define SL "<<"
#define SR ">>"
#define MUX "?"
#define eINOP "INOP"
#define eONOP "ONOP"
#define IF "if ("
#define ELSE "else {"
#define SIGNED true
#define UNSIGNED false


#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include<string>
#include <vector>
#include <algorithm>

using namespace std;

enum BlockType { CONDITIONAL, FUNCTION };
//enum Resource {ADDER_SUBTRACTOR = 1, MULTIPLIER = 2, LOGICAL = 1 };

struct Resource {
	std::string name;
	std::vector<std::string> ops;
	int cnt;
	int delay;
	int inUse;
};//ADDER_SUBTRACTOR, MULTIPLIER, LOGICAL;

