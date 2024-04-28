//#include <iostream>
//#include <string>
//#include <sstream>
//#include <cstdlib>
//#include <fstream>
#include "Parser.h"
#include "IOV.h"
#include "CDFGraph.h"
#include "Parser.h"


//THIS MAIN FUNCTION IS FOR ECE3... THIS IS ECE3 FRIENDLY...

int main (int argc, char* argv[])
{
	
	// Check whether input arguments does not follow the usage statement
	if (argc != 4) {
		std::cout << "Usage: hlsyn cFile latency verilogFile" << std::endl;
		exit(1);
	}

	// Vector string to hold the input C-like file 
	std::vector<string> rawFileStrings;

	// Create an object of the Control/Data Flow Graph to create a graph of the C-like algorithms
	CDFGraph *g1 = new CDFGraph;

	// Create an vector to store IOV objects for inputs, outputs, and variables for the verilog code
	// IOV here stands for Input/Output/Variable
	// 
	std::vector<IOV> inputs;
	std::vector<IOV> outputs;
	std::vector<IOV> variables;

	// Parse every line in input file to a vector string
	Parser::parseFile(argv[1], &rawFileStrings);

	// Extract the inputs, outputs, and variables from the vector string of cleaned input file
	IOV::generateIOV(&rawFileStrings, &inputs, &outputs, &variables);

	// Load the vector string of the cleaned input file as well as the Inputs/Outputs/Variables into the CFDG graph
	g1->loadFileStrings(rawFileStrings);
	g1->loadIOV(inputs, outputs, variables);

	// Parse the operations within the vector string of input file to the CDFG graph 
	g1->parseOperations();

	int latency; // Store the latency constraint for the scheduling
	stringstream ss; // Create a stringstream object to handle string manipulation

	ss << argv[2]; // Push the second argument into the stringstream
	ss >> latency; // Extract the value from the stringstream and store it in 'latency'
	g1->LIST_R(latency); // Set the latency constraint to the List-R scheduling

	g1->generateVerilogFile(argv[3]); // Generate the Verilog file with the given output name

	return 0; // Exit the program
}

//THIS MAIN FUNCTION IS FOR FIGURING OUT WTF IS GOING ON WITHOUT HAVING TO MESS WITH THE COMMANDLINE.
//
//int main(int argc, char* argv[]) {
//
//	//string path = "E:\\Kevin\\Documents\\GitHub\\keving_lsepulveda_ssoumbeyalley_hlsyn\\testfiles";
//	//string path = "E:\\Kevin\\Documents\\GitHub\\keving_lsepulveda_ssoumbeyalley_hlsyn\\testfiles\\standard tests\\";
//	string path = "E:\\Kevin\\Documents\\GitHub\\keving_lsepulveda_ssoumbeyalley_hlsyn\\testfiles\\";
//	string inFileString = "\\if_tests\\test_if4.c";
//	char* inCStr = new char[(path + inFileString).length() - 1];
//	strcpy(inCStr, (path + inFileString).c_str());
//
//
//	std::string outFileString = "\\output\\out3.v";
//
//	char* outCStr = new char[(path + outFileString).length() - 1];
//	strcpy(outCStr, (path + outFileString).c_str());
//
//	int lat = 10;
//
//	if (argc != 4) {
//		std::cout << "Usage: hlsyn cFile latency verilogFile" << std::endl;
//		//exit(1);
//		//return 0;
//	}
//
//	//std::cout << argv[0] << std::endl;
//
//	std::vector<string> rawFileStrings;
//	CDFGraph *g1 = new CDFGraph;
//
//	std::vector<IOV> inputs;
//	std::vector<IOV> outputs;
//	std::vector<IOV> variables;
//	//Parser::parseFile(argv[1], rawFileStrings);
//	Parser::parseFile(inCStr, &rawFileStrings);
//
//	/*for (std::vector<string>::iterator it = rawFileStrings.begin(); it != rawFileStrings.end(); ++it) {
//	std::cout << *it << std::endl;
//	}*/
//	//	std::cout << "......................................" << std::endl << std::endl;
//
//
//	IOV::generateIOV(&rawFileStrings, &inputs, &outputs, &variables);
//	//Note: This function removes the declaration lines, so only procedural statements remain in rawFileStrings after this. 
//
//	//string dName = Parser::getModuleName(argv[1]);
//	/*std::cout << "Inputs:" << endl;
//	for (std::vector<IOV>::iterator it = inputs.begin(); it != inputs.end(); ++it) {
//	std::cout << it->getName() << " : " << it->getDataType() << " : " << it->getBitWidth() << std::endl;
//	}
//	std::cout << "Outputs:" << endl;
//	for (std::vector<IOV>::iterator it = outputs.begin(); it != outputs.end(); ++it) {
//	std::cout << it->getName() << " : " << it->getDataType() << " : " << it->getBitWidth() << std::endl;
//	}
//	std::cout << "Variables:" << endl;
//	for (std::vector<IOV>::iterator it = variables.begin(); it != variables.end(); ++it) {
//	std::cout << it->getName() << " : " << it->getDataType() << " : " << it->getBitWidth() << std::endl;
//	}
//	std::cout << "......................................" << std::endl;
//	for (std::vector<string>::iterator it = rawFileStrings.begin(); it != rawFileStrings.end(); ++it) {
//	std::cout << *it << std::endl;
//	}
//	std::cout << "......................................" << std::endl<<std::endl;*/
//	////////////////////////////////////////////////////////////
//	g1->loadFileStrings(rawFileStrings);
//	g1->loadIOV(inputs, outputs, variables);
//
//	//	std::cout << "......................................" << std::endl << std::endl;
//	g1->parseOperations();
//	//g1->printGraph();
//	//Vertex::latency = 10;
//	//std::cout << "......................................" << std::endl << std::endl;
//	//g1->ALAP(lat);
//	g1->LIST_R(lat);
//	//std::vector<Vertex*> Verts = g1->getVertices();
//	g1->generateVerilogFile(outCStr);
//	/*for (std::vector<Vertex*>::iterator it = Verts.begin(); it != Verts.end(); ++it) {
//	std::cout << left << "Node: [" << (*it)->getString() << "]\t\t" << right << "ALAP Time:" << (*it)->getALAPTime() << std::endl;
//	}*/
//	//std::cout << "......................................" << std::endl << std::endl;
//	return 0;
//}
//