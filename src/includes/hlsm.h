#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "operation.h"
#include "variable.h"

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
    int currentLineIndex;

    std::vector<Variable *> variables;
    std::vector<Operation *> operations;
    std::vector<Operation *> avaliable;
    std::vector<Operation *> inIfs;

    Operation inop;
    Operation onop;
    Operation *lastIf;

    std::string currentLine;

    bool error;
    bool justExited;

    HLSM();

    void cleanPredecessors(std::vector<Operation *> ops);
    std::vector<std::string> parseLine(std::string line);
    int processFile(const std::string &fileName, const std::string &outputName, int latency);
};

#endif
