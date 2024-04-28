
#include "Parser.h"

//using namespace std;

//  __  __    _    ___ _   _   _____ ___ _     _____   _   _ ____    _    ____ _____ 
// |  \/  |  / \  |_ _| \ | | |  ___|_ _| |   | ____| | | | / ___|  / \  / ___| ____|
// | |\/| | / _ \  | ||  \| | | |_   | || |   |  _|   | | | \___ \ / _ \| |  _|  _|  
// | |  | |/ ___ \ | || |\  | |  _|  | || |___| |___  | |_| |___) / ___ | |_| | |___ 
// |_|  |_/_/   \_|___|_| \_| |_|   |___|_____|_____|  \___/|____/_/   \_\____|_____|

// Parse input file to vector string for further processing in the code
void Parser::parseFile(char* inFileStr, std::vector<string>* destVector) {

	std::string line; // String to store each line from the input file
	std::ifstream inFile(inFileStr, std::ifstream::in); // Creates an file stream object and attempts to open the specified file in reading mode

	// Checks if the file can be successfully opened
	// and if the file stream is in a good state for further operations like reading
	// 
	if (inFile.is_open() && inFile.good()) {
	//	std::cout << "File Opened!" << std::endl;
	}
	else {
		int n;
		std::cout << "Unable to open the input file." << std::endl;// << "Enter a number to exit FIXME" << std::endl;
		//std::cin >> n;
		exit(1);
		//return std::vector<std::string>();
	}

	std::string str; // Store a clean line from the input file reading
	
	while (std::getline(inFile, line)) // Iteratively reads a line from the file until end of file and store as a string 
	{
		line.erase(std::remove(line.begin(), line.end(), '\t'), line.end()); // Remove tab characters from the line.

		if (line.find_first_not_of(' ') != std::string::npos) // Checks if the line is not empty or has any non-whitespace characters. 
		{
			str = trim(line); // Remove leading and trailing whitespace
			destVector->push_back(str); // Push the clean line into a string vector
		}
	}

	inFile.close(); // Close the file to free memory
}

// Remove any whitespace characters at the beginning and at the end of the string
std::string Parser::trim(string& str)
{
	std::string sTMP;
	sTMP = str;

	//sTMP.erase(std::remove(sTMP.begin(), sTMP.end(), '\t'), sTMP.end());
	size_t first = sTMP.find_first_not_of(' '); // Finds the index of the first character in str that is not a space
	size_t last = sTMP.find_last_not_of(' '); // Finds the index of the last character in str that is not a space, starting from the end of the string and working backwards.

	// 1 is added since last only includes up until the last second character
	return sTMP.substr(first, (last - first + 1)); // Returns the cleaned substring
}

// 
vector<std::string> Parser::splitByWhitespace(std::string str)
{

	string buf; // Have a buffer string
	stringstream ss(str); // Insert the string into a stream

	vector<string> tokens; // Create vector to hold our words
//	bool test;
	while (ss >> buf)
	{
		buf.erase(std::remove(buf.begin(), buf.end(), ','), buf.end());
		//std::cout << buf << "***";
		//if (buf == "b") { break; }
		
		//test = buf.find("\\");
	//	std::cout << buf << "....." << test << endl;
	
		if (buf.find("//") == string::npos)
		{
			tokens.push_back(buf);
		//	
		}
		else
		{ 
		//	std::cout << "found!....." << buf << std::endl;
			break; 
		}
	}
	return tokens;
}


std::string Parser::getModuleName(std::string filePath) {
	std::string name;
	std::string path;
	std::size_t found = filePath.find_last_of("/\\");


	//name = filePath.substr(found1 + 1).
	
	name = filePath.substr(found + 1);
	found = name.find_last_of(".");
	//std::cout << name;
	name = name.substr(0, found);


	return name;
	//std::cout << fileName << std::endl;
	//std::cout << filePath << std::endl;
}


std::string Parser::getFilePath(std::string filePath) {
	std::string name;
	std::string path;
	//string path2;
	std::size_t found = filePath.find_last_of("/\\");

	path = filePath.substr(0, found);
	//std::cout << path << endl;
	return path;
	//std::cout << fileName << std::endl;
	//std::cout << filePath << std::endl;
}