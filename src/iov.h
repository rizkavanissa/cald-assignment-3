#ifndef IOV_H
#define IOV_H

#include <iostream>
#include <vector>
#include <string>

#include "operation.h"

class Operation; // Provides access to Operation's class members

enum VariableType
{
    INPUT = 0,
    OUTPUT,
    VARIABLE,
    INVALID_VARIABLE
};

class Variable
{
    public:

        std::string name;                       // Store name of data element
        int width;                              // Store bidwidth of data element
        bool sign;                              // Store datatype of data element (whether signed or unsigned)
        VariableType type;                      // Store type of data element (whether input, output, or variable)
        std::vector<Operation *> producers;
        std::vector<Operation *> consumers;

        Variable();
        Variable(std::vector<std::string> words, int index, Operation *inop, Operation *onop);
        VariableType varTypeParser(std::string dataType);
        int getVariableWidth(std::string dataType);
        bool isSigned(std::string dataType);
};
#endif