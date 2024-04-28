#pragma once
/***
Author: Kevin Gilliam
NetID: keving
Assignment #: 3

*/
#ifndef INPUT_H
#define INPUT_H

//#include<string>
//#include <vector>
//#include<iostream>
//#include <sstream>
//#include "Parser.h"
#include "main.h"
//using namespace std;

class IOV {

public:

	IOV();
	IOV(std::string sName, std::string sDataString, std::string sBitWidth);
	//IOV(std::string s);
	std::string getName(void);
	std::string getBitWidth();
	std::string getDataType(void);
	void setType(string s);
	std::string getType();
	//unsigned int getBitWidth(void);
	void printPin();
	//std::vector<IOV> generateIOVs(std::string inputString);
	//static bool CheckType(std::string);
	static  void generateIOV(std::vector<string>* strVector, std::vector<IOV>* inputs, std::vector<IOV>* outputs, std::vector<IOV>* variables);
	std::string getOutputLine();
	//bool operator==(const IOV & other) const;

	//bool operator!=(const IOV & other) const;

private:

	//unsigned int bitWidth;
	std::string sBitWidth;
	int iBitWidth;
	std::string name;
	std::string type;
	std::string dataType;
	std::string lineOutput;
};
#endif //CONNECTION_H