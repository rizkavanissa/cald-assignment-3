#pragma once

/***
Author: Kevin Gilliam
NetID: keving
Assignment #: 2
File: Parser.h
*/
#ifndef PARSER_H
#define PARSER_H

#include<string>
#include <sstream>
#include<vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "main.h"
//#include "V_Module.h"

//using namespace std;

class Parser {

public:

	static void parseFile(char* inFileStr, std::vector<string>* lines);
	static std::string trim(string & str);
	static vector<std::string> splitByWhitespace(std::string line);
	static std::string getModuleName(std::string filePath);
	static std::string getFilePath(std::string filePath);
};

#endif //PARSER_H