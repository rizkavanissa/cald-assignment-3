#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "operation.h"
#include "iov.h"

#include <iostream>
#include <cctype>
#include <fstream>
#include <istream>
#include <sstream>
#include <string>
#include <vector>

class HLSM
{
    public:

        int count;
        bool error;
        bool justExited;

        std::vector<Variable *> variables;
        std::vector<Operation *> operations;
        std::vector<Operation *> avaliable;
        std::vector<Operation *> inIfs;

        Operation inop;     // The first vertex or node of operation
        Operation onop;     // The last vertex or node of operation
        Operation *lastIf;

        std::string currentLine;

        HLSM();

        std::vector<std::string> parseLine(std::string line);

        void parseInput(std::ifstream& inFile, int latency);

        void cleanPredecessors(std::vector<Operation *> ops);
        
        void convertToHLSM(const std::string &fileName, const std::string &outputName, int latency);

        void writeToVerilog(std::ifstream& inFile, std::ofstream& outFile, const std::string &outputName, int latency);
};
#endif