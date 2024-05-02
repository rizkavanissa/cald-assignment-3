#include "iov.h"

// Default constructor
Variable::Variable() : name(""), width(0), sign(false), type(VariableType::INVALID_VARIABLE) {}

// Parameterized constructor
Variable::Variable(std::vector<std::string> words, int index, Operation *inop, Operation *onop)
{
    type = varTypeParser(words.at(0));
    if (type == VariableType::INVALID_VARIABLE)
    {
        std::cerr << "Error: Invalid Variable Type" << std::endl;
        return;
    }

    sign = isSigned(words.at(1));               // Assign whether the variable is signed or unsigned integer
    width = getVariableWidth(words.at(1));      // Assign the variable bitwidth

    if (width == -1)    // Invalid bitwidth
    {
        std::cerr << "Error: Invalid bitwidth. Please use these integers: 1, 2, 8, 16, 32, 64" << std::endl;
        return;
    }

    name = words.at(index);

    if (type == INPUT)              // Check if the variable is an input edge
    {
        producers.push_back(inop);
        inop->addOutput(this);
    }
    else if (type == OUTPUT)        // Check if the variable is an output edge
    {
        consumers.push_back(onop);
        onop->addInput(this);
    }
}

// Assign the data element type
VariableType Variable::varTypeParser(std::string dataType)
{
    if (dataType.compare("input") == 0)
    {
        return VariableType::INPUT;
    }
    else if (dataType.compare("output") == 0)
    {
        return VariableType::OUTPUT;
    }
    else if (dataType.compare("variable") == 0)
    {
        return VariableType::VARIABLE;
    }
    else
    {
        return VariableType::INVALID_VARIABLE;
    }
}

// Get the datatype and bitwidth of the variable
int Variable::getVariableWidth(std::string dataType)
{
    if (dataType.compare("Int1") == 0 || dataType.compare("UInt1") == 0)
    {
        return 1;
    }
    else if (dataType.compare("Int2") == 0 || dataType.compare("UInt2") == 0)
    {
        return 2;
    }
    else if (dataType.compare("Int8") == 0 || dataType.compare("UInt8") == 0)
    {
        return 8;
    }
    else if (dataType.compare("Int16") == 0 || dataType.compare("UInt16") == 0)
    {
        return 16;
    }
    else if (dataType.compare("Int32") == 0 || dataType.compare("UInt32") == 0)
    {
        return 32;
    }
    else if (dataType.compare("Int64") == 0 || dataType.compare("UInt64") == 0)
    {
        return 64;
    }
    else
    {
        return -1;
    }
}

// Check whether the datatype is signed or unsigned
bool Variable::isSigned(std::string dataType)
{
    if (dataType.compare("Int1") == 0 || dataType.compare("Int2") == 0 ||
        dataType.compare("Int8") == 0 || dataType.compare("Int16") == 0 ||
        dataType.compare("Int32") == 0)
    {
        return true;
    }
    else if (dataType.compare("UInt1") == 0 || dataType.compare("UInt2") == 0 ||
             dataType.compare("UInt8") == 0 || dataType.compare("UInt16") == 0 ||
             dataType.compare("UInt32") == 0)
    {
        return false;
    }
    else
    {
        return false;
    }
}