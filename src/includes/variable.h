#ifndef VARIABLE_H
#define VARIABLE_H

#include <iostream>
#include <vector>
#include <string>

#include "operation.h"

class Operation;

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
    std::string name;
    int width;
    bool sign;
    VariableType type;
    std::vector<Operation *> producers;
    std::vector<Operation *> consumers;

    Variable();
    Variable(std::vector<std::string> words, int index, Operation *inop, Operation *onop);
    VariableType varTypeParser(std::string dataType);
    int getVariableWidth(std::string dataType);
    bool isSigned(std::string dataType);
};

#endif