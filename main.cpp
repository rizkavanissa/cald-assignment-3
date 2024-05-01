#include <string>
#include <iostream>

#include "iov.h"
#include "operation.h"
#include "converter.h"

int main(int argc, char *argv[])
{
    // Beforehand, it is worth noting the notation used to name the .cpp files:
    // - converter.cpp : process input file to be converted to a HLSM verilog file
    // - cdfg.cpp      : perform ALAP graph and List-R scheduling
    // - iov.cpp       : store the input, output, variable (iov)
    // - operation.cpp : store the operations

	// Check whether input arguments does not follow the usage statement
	if (argc != 4)
    {
		std::cerr << "Usage: " << argv[0] << " cFile latency verilogFile" << std::endl; // Users may prefer different executable file name for arg[0]
		exit(1); // Perform abnormal program termination
	}

    std::string cFile = argv[1];        // Input file (stores the path to the input file as a string)
    int latency = atoi(argv[2]);        // Latency constraint (by converting the input string representing an integer to an integer value)
    std::string verilogFile = argv[3];  // Output file stores the path to the output file as a string

    HLSM hlsm;  // Create an instance of the HLSM class

    hlsm.convertToHLSM(cFile, verilogFile, latency); // Perform the conversion and also the write to the output file

    std::cout << "Verilog file has been created." << std::endl;

    return 0;
}
