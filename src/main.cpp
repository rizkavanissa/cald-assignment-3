#include "includes/operation.h"
#include "includes/variable.h"
#include "includes/hlsm.h"

#include <string>
#include <iostream>

bool checkArguments(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " cFile latency verilogFile \n";
        return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
    if (!checkArguments(argc, argv))
    {
        return 1;
    }

    std::string cFile = argv[1];
    int latency = atoi(argv[2]);
    std::string verilogFile = argv[3];

    HLSM hlsm;

    return hlsm.processFile(cFile, verilogFile, latency);
}
