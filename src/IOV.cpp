#include "IOV.h"
#include "Parser.h"

// IOV::IOV()
// {
// }

IOV::IOV(std::string sName, std::string sDataString, std::string strBitWidth)
{
	name = sName;
	dataType = sDataString;
	sBitWidth = strBitWidth;

	stringstream ss;
	ss.str(strBitWidth);
	ss >> iBitWidth;

	//iBitWidth = std::stoi(strBitWidth);

	return;
}

std::string IOV::getName(void)
{
	return name;
}

std::string IOV::getBitWidth()
{
	return sBitWidth;
}


std::string IOV::getDataType(void)
{
	return dataType;
}

void IOV::setType(string s)
{
	this->type = s;
	std::string s_us = " ";//signed_unsigned
	std::string sType_out = type + " ";
	
	stringstream ss;

	if (dataType == "UInt") {
		s_us = " unsigned ";
	}
	if (type == VARIABLE) {
		//sType_out = " integer";
		ss << "reg" << s_us << "[" << (iBitWidth - 1) << ":0] " << name << ";";
		lineOutput = ss.str();
		ss.clear();
		ss.str("");
	}
	else if (type == INPUT || type == OUTPUT ) {
		if (s == OUTPUT) {
			s_us = "reg" + s_us;
		}
		if (iBitWidth > 1) {
			ss << sType_out << s_us << "[" << (iBitWidth - 1) << ":0] " << name << ";";
			lineOutput = ss.str();// +std::endl;
			ss.clear();
			ss.str("");
		}
		else {
			lineOutput = sType_out  + name + s_us + ";";// +std::endl;
		}
	}
}

std::string IOV::getType()
{
	return type;
}


// void IOV::printPin()
// {
// }

// Function to extract the inputs, outputs, and variables from the vector string of the input file
void IOV::generateIOV(std::vector<string>* strVector, std::vector<IOV>* inputs, std::vector<IOV>* outputs, std::vector<IOV>* variables)
{
	std::vector<string> tok;						// Store string token
	std::vector<string> sNames;						// Store string name
	std::string sType;								// Store string type
	std::string sDataString;						// Store string datatype
	std::string bitSubStr;							// Store bitwidth substring
	string bitWidth;								// Store bitwidth
	std::vector<string>::iterator pinLinesEndHere;	// Iterator

	//std::vector<IOV> inputs;

	// Check whether the input file is successfully extracted to the vector string
	if (strVector->size() == 0)
	{
		std::cout << "Error parsing file (V_Module.cpp)" << std::endl;
		exit(1);
		//erFlag = true;
		//return false;
	}

	// The begin() method returns an iterator pointing to the first element in the vector.
	// The loop condition checks if the iterator it hasn't reached the end of the vector.
	// The end() method of the vector returns an iterator that points to one position beyond the last element.
	// A loop that will iterate through each string element in the strVector vector as long as it hasn't reached the end.
	//
	for (std::vector<string>::iterator it = strVector->begin(); it != strVector->end();)
	{
		//For each string in the vector which the file was loaded in to
		tok = Parser::splitByWhitespace(*it); //break the line up by whitespace

		if (tok.size() == 0) continue; // Check if the vector string is empty

		//sType = tok.at(0);//the first token should be input, output, wire etc
		//string dbg = tok.at(0);

		if (tok.at(0) == "input" || tok.at(0) == "output" || tok.at(0) == "variable")
		{
			//newCon = new IOV("input");

			sDataString = tok.at(1); // The second token always specify the bitwidth

			if (sDataString.substr(0, 4) == "UInt") // Check whther the datatype is a signed integer
			{
				bitSubStr = sDataString.substr(4, sDataString.size());
				//	sgn = UNSIGNED;
				sType = "UInt";
				std::istringstream iss(bitSubStr);
				if (!(iss >> bitWidth)) {
					std::cout << "Invalid bit string" << std::endl;
					exit(1);
				}
			}
			else if (sDataString.substr(0, 3) == "Int") // Check whether the datatype is an unsigned integer
			{
				sType = "Int";
				bitSubStr = sDataString.substr(3, sDataString.size());
				std::istringstream iss(bitSubStr);
				//sDataString.substr(0, 3)
				if (!(iss >> bitWidth)) std::cout << "Invalid bit string" << std::endl;
				//if (bitWidth > 1) sgn = SIGNED;
			}
			else // Otherwise the datatype is invalid
			{
				std::cout << "Error! Invalid data type:" << sDataString << std::endl;
				exit(1);
			}

			// 
			for (std::vector<string>::iterator it = tok.begin() + 2; it != tok.end(); ++it)
			{
				//std::cout << *it;
				sNames.push_back(*it);//Everything after the bitwidth string is a new pin, so we're making a vector of pin names
				//sNames.insert(sNames.begin(), *it);
			}

			// 
			for (std::vector<string>::iterator sit = sNames.begin(); sit != sNames.end(); ++sit)
			{
				//loop through the vector of pin names, and make a new V_Pin object for it
				IOV *newIOV = new IOV(*sit, sType, bitWidth);

				if (tok.at(0) == INPUT) {
					newIOV->setType(INPUT);
					if (inputs->size() == 0) inputs->push_back(*newIOV);//If the vector is empty, just put this in there.
					else inputs->push_back(*newIOV);// pins.insert(pins.begin(), newPin);//They should load in order on their own
				}
				else if (tok.at(0) == OUTPUT) {
					newIOV->setType(OUTPUT);
					if (outputs->size() == 0) outputs->push_back(*newIOV);//If the vector is empty, just put this in there.
					else outputs->push_back(*newIOV);// pins.insert(pins.begin(), newPin);//They should load in order on their own
				}
				else if (tok.at(0) == "variable") {
					newIOV->setType("variable");
					if (variables->size() == 0) variables->push_back(*newIOV);//If the vector is empty, just put this in there.
					else variables->push_back(*newIOV);// pins.insert(pins.begin(), newPin);//They should load in order on their own
				}
				//pinLinesEndHere = sit;
			}
			sNames.clear();//After making all of the pins with this type and bitwidth, empty the name vector and move to the next type
			it = strVector->erase(it);
		}
		else
		{
			++it; // 
		}

	}
	//strVector->erase(strVector->begin(), pinLinesEndHere + 1);

	//for (std::vector<string>::iterator it = strVector->begin(); it != pinLinesEndHere + 1; ++it) {
	//	strVector->erase()
	//}
	//std::vector<string>::iterator it2 = strVector->begin();
	//do {
	//	it2 = strVector->erase(it2);
	//	
	//} while (it2 <= pinLinesEndHere);


}

std::string IOV::getOutputLine()
{
	return lineOutput;
}

