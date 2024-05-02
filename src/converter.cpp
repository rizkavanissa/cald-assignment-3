#include <cmath>
#include "converter.h"
#include "cdfg.h"

HLSM::HLSM() // Default constructor
{
    this->count = 0;
    this->error = false;
    this->justExited = false;
}

// Clean the predecessor nodes of the operations
void HLSM::cleanPredecessors(std::vector<Operation *> ops)
{
    for (Operation *op : ops)
    {
        for (int i = 0; i < op->successors.size(); i++)
        {
            Operation *suc = op->getSucAt(i);
            if (suc == op)
            {
                op->removeSucAt(i);
            }
        }
    }
}

// Tokenize a string line into a separate word strings
std::vector<std::string> HLSM::parseLine(std::string line)
{
    std::vector<std::string> out;

    std::stringstream stream;
    stream.str(line);

    while (stream)
    {
        std::string temp;       // Store the word token
        stream >> temp;         // Extract the first or next token

        if (temp.size() > 0)    // Check if the token is not empty
        {
            char last = temp[temp.length() - 1];    // Get the last character of the current word token

            if (last == ',') // Chekc if this last character is a comma character (e.g., "a," from the line "input UInt8 a, b, c")
            {
                temp = temp.substr(0, temp.size() - 1); // Extract only the alphabetic character(s)
            }
            out.push_back(temp); // Push the current word token into the "out" string vector
        }
    }

    return out; // Reutrn the string vector of the clean tokenized line
}

void HLSM::parseInput(std::ifstream& inFile, int latency)
{
    count = 0; // Store ID of operation

    // Push the memory addresses of inop and onop (although it has not been initialized yet) into the vector of operations
    operations.push_back(&inop);
    operations.push_back(&onop);

    lastIf = NULL;

    error = false;
    justExited = false;

    while (!inFile.eof()) // Iterate through each line in the input file
    {
        if (std::getline(inFile, currentLine) && !error)    // Get the next line of the input file and check if any error occurred in the previous line
        {
            if (!currentLine.empty()) // Skip over empty lines
            {
                if (currentLine.find("//") != std::string::npos)
                {
                    std::cout << "Comment found: " << currentLine << std::endl;
                    // exit(1);
                }

                std::vector<std::string> words = HLSM::parseLine(currentLine); // Tokenize each word of a line

                if (words.size() < 1)
                {
                    // Do nothing...
                }
                else if (words.at(0).compare("}") == 0) // Check whether the first word  of the line is a closing curly brace
                {
                    justExited = true;
                    lastIf = inIfs.back();
                    inIfs.pop_back();
                }
                // Check whether the first word of the line is for an else statement
                else if (words.at(0).compare("else") == 0)
                {
                    count++; // Increment count to get new ID
                    
                    if (justExited)
                    {
                        Operation *tempOp = new Operation(words, variables, inIfs, count, lastIf, operations);
                        inIfs.push_back(tempOp);
                        operations.push_back(tempOp);
                        justExited = false;
                    }
                    else
                    {
                        std::cerr << "Error: Else without corresponding If" << std::endl;
                    }
                }
                else if (words.size() < 3)
                {
                    // Do nothing...
                }
                // // Handles the data element typefrom the input file
                else if (words.size() > 2 && (words.at(0).compare("input") == 0 || 
                                            words.at(0).compare("output") == 0 || 
                                            words.at(0).compare("variable") == 0)
                        )
                {
                    for (int i = 2; i < words.size(); i++)
                    {
                        Variable *tempVar = new Variable(words, i, &inop, &onop);
                        variables.push_back(tempVar);
                        justExited = false;
                    }
                }
                // Handles the operations from the input file
                else if (words.at(1).compare("=") == 0)
                {
                    count++; // Increment count to get new ID

                    Operation *tempOp = new Operation(words, variables, inIfs, count, lastIf, operations);
                    operations.push_back(tempOp);
                    justExited = false;
                }
                // Handles the conditional if statement from the input file
                else if (words.at(0).compare("if") == 0 &&
                        words.at(1).compare("(") == 0 &&
                        words.at(3).compare(")") == 0 &&
                        words.at(4).compare("{") == 0)
                {
                    count++; // Increment count to get new ID

                    Operation *tempOp = new Operation(words, variables, inIfs, count, lastIf, operations);
                    inIfs.push_back(tempOp);
                    operations.push_back(tempOp);
                    justExited = false;
                }
            }
        }
    }

    return; // Return true if no errors occurred during parsing
}

// Write the conversion result to the specified verilog file
void HLSM::writeToVerilog(std::ifstream& inFile, std::ofstream& outFile, const std::string &outName, int latency)
{
    outFile << "`timescale 1ns / 1ps\n\n";
    outFile << "module HLSM (Clk, Rst, Start, Done"; // MODULE START
    // Specify the data elements in the module's parameter
    for (int i = 0; i < variables.size(); i++)
    {
        if (variables.at(i)->type == 0 || variables.at(i)->type == 1)
        {
            outFile << ", " << variables.at(i)->name;
        }
    }
    outFile << ");" << std::endl;

    outFile << "    input Clk, Rst, Start;" << std::endl; // INPUT START

    // Print the inputs
    for (int i = 0; i < variables.size(); i++)
    {
        if (variables.at(i)->type == 0)
        {
            outFile << "    input ";
            if (variables.at(i)->sign)
            {
                outFile << "signed ";
            }
            if (variables.at(i)->width > 1)
            {
                outFile << "[" << variables.at(i)->width - 1 << ":0] ";
            }
            outFile << variables.at(i)->name << "; " << std::endl;
        }
    }

    outFile << "    output reg Done;" << std::endl; // OUTPUT START

    // Print the outputs
    for (int i = 0; i < variables.size(); i++)
    {
        if (variables.at(i)->type == 1)
        {
            outFile << "    output reg ";
            if (variables.at(i)->sign)
            {
                outFile << "signed ";
            }
            if (variables.at(i)->width > 1)
            {
                outFile << "[" << variables.at(i)->width - 1 << ":0] ";
            }
            outFile << variables.at(i)->name << "; " << std::endl;
        }
    }

    // Print the registers/variables
    for (int i = 0; i < variables.size(); i++)
    {
        if (variables.at(i)->type == 2)
        {
            outFile << "    reg ";
            if (variables.at(i)->sign)
            {
                outFile << "signed ";
            }
            if (variables.at(i)->width > 1)
            {
                outFile << "[" << variables.at(i)->width - 1 << ":0] ";
            }
            outFile << variables.at(i)->name << "; " << std::endl;
        }
    }

    outFile << "    reg [" << ceil(log(latency) / log(2)) - 1 << ":0] State;"
                << std::endl;

    outFile << "    parameter Wait = 0, Final = 1, ";

    // Print the cycle time of the states of the operations
    for (int i = 0; i < (latency); i++)
    {
        outFile << "S" << i << " = " << i + 2;
        if (i != latency - 1)
        {
            outFile << ", ";
        }
        else
        {
            outFile << ";" << std::endl;
        }
    }

    // Print the statements for the positive clock edge
    outFile << "\n    always @(posedge Clk) begin" << std::endl;
    outFile << "        if (Rst == 1) begin" << std::endl
                << "            Done <= 0;" << std::endl
                << "            State <= Wait;" << std::endl
                << "        end" << std::endl;
    outFile << "        else begin" << std::endl;
    outFile << "            case(State)" << std::endl;
    outFile << "                Wait: begin" << std::endl;
    outFile << "                    Done <= 0;" << std::endl;
    outFile << "                    if (Start == 1) begin" << std::endl;
    outFile << "                        State <= S0;" << std::endl;
    outFile << "                    end" << std::endl;
    outFile << "                    else begin" << std::endl;
    outFile << "                        State <= Wait;" << std::endl;
    outFile << "                    end" << std::endl
                << "                end" << std::endl;

    // Print the states of the operations
    for (int i = 1; i < latency + 1; i++)
    {
        // output case
        outFile << "                S" << i - 1 << ": begin" << std::endl;
        for (int j = 0; j < operations.size(); j++)
        {
            if ((operations.at(j)->scheduledState == i) &&
                (operations.at(j)->inIfs.size() == 0))
            {
                switch (operations.at(j)->type)
                {
                case 0: // REG
                    outFile << "//REG";
                    break;
                case 1: // ADD
                    outFile << "                    "
                                << operations.at(j)->outputs.at(0)->name
                                << " <= ";
                    outFile << operations.at(j)->inputs.at(0)->name
                                << " + ";
                    outFile << operations.at(j)->inputs.at(1)->name
                                << ";";
                    break;
                case 2: // SUB
                    outFile << "                    "
                                << operations.at(j)->outputs.at(0)->name
                                << " <= ";
                    outFile << operations.at(j)->inputs.at(0)->name
                                << " - ";
                    outFile << operations.at(j)->inputs.at(1)->name
                                << ";";
                    break;
                case 3: // MUL
                    outFile << "                    "
                                << operations.at(j)->outputs.at(0)->name
                                << " <= ";
                    outFile << operations.at(j)->inputs.at(0)->name
                                << " * ";
                    outFile << operations.at(j)->inputs.at(1)->name
                                << ";";
                    break;
                case 4: // COMP
                    outFile << "                    "
                                << "if ("
                                << operations.at(j)->inputs.at(0)->name;
                    if (operations.at(j)->compType == 0)
                    {
                        outFile << " < ";
                    }
                    if (operations.at(j)->compType == 1)
                    {
                        outFile << " > ";
                    }
                    if (operations.at(j)->compType == 2)
                    {
                        outFile << " == ";
                    }
                    outFile << operations.at(j)->inputs.at(1)->name
                                << ") begin" << std::endl;
                    outFile << "                        "
                                << operations.at(j)->outputs.at(0)->name
                                << " <= 1;" << std::endl
                                << "                    end" << std::endl;
                    outFile << "                    "
                                << "else begin" << std::endl
                                << "                        "
                                << operations.at(j)->outputs.at(0)->name
                                << " <= 0;" << std::endl
                                << "                    end";
                    break;
                case 5: // MUX
                    outFile << "                    "
                                << operations.at(j)->outputs.at(0)->name
                                << " <= (";
                    outFile << operations.at(j)->inputs.at(0)->name
                                << ")?";
                    outFile << operations.at(j)->inputs.at(1)->name
                                << ":";
                    outFile << operations.at(j)->inputs.at(2)->name
                                << ";";
                    break;
                case 6: // SHR
                    outFile << "                    "
                                << operations.at(j)->outputs.at(0)->name
                                << " <= ";
                    outFile << operations.at(j)->inputs.at(0)->name
                                << " >> ";
                    outFile << operations.at(j)->inputs.at(1)->name
                                << ";";
                    break;
                case 7: // SHL
                    outFile << "                    "
                                << operations.at(j)->outputs.at(0)->name
                                << " <= ";
                    outFile << operations.at(j)->inputs.at(0)->name
                                << " << ";
                    outFile << operations.at(j)->inputs.at(1)->name
                                << ";";
                    break;
                case 8: // HANDLE THE FIRST IF STATEMENT
                    outFile << "                    "
                                << "if ("
                                << operations.at(j)->inputs.at(0)->name
                                << ") begin" << std::endl;
                    for (int k = 0; k < operations.size(); k++)
                    {
                        for (int l = 0; l < operations.at(k)->inIfs.size();
                             l++)
                        {
                            if (operations.at(k)->inIfs.at(l) ==
                                    operations.at(j) &&
                                (operations.at(k)->inIfs.size() - (l + 1)) == 0)
                            {
                                if (operations.at(k)->type != 8 &&
                                    operations.at(k)->type != 9 &&
                                    operations.at(k)->type != 4)
                                {
                                    outFile << "                        "
                                                << operations.at(k)
                                                       ->outputs
                                                       .at(0)
                                                       ->name
                                                << " <= ";
                                    if (operations.at(k)->type == 5)
                                    {
                                        outFile << "(";
                                    }
                                    outFile << operations.at(k)
                                                       ->inputs
                                                       .at(0)
                                                       ->name;
                                    if (operations.at(k)->type == 5)
                                    {
                                        outFile << ")?";
                                    }
                                    switch (operations.at(k)->type)
                                    {
                                    case 1:
                                        outFile << " + ";
                                        break;
                                    case 2:
                                        outFile << " - ";
                                        break;
                                    case 3:
                                        outFile << " * ";
                                        break;
                                    case 6:
                                        outFile << " >> ";
                                        break;
                                    case 7:
                                        outFile << " << ";
                                        break;
                                    default:
                                        outFile << "0 //";
                                        break;
                                    }
                                    outFile << operations.at(k)
                                                       ->inputs
                                                       .at(1)
                                                       ->name;
                                    if (operations.at(k)->type == 5)
                                    {
                                        outFile << ":"
                                                    << operations.at(k)
                                                           ->inputs
                                                           .at(2)
                                                           ->name;
                                    }
                                    outFile << ";" << std::endl;
                                }
                                else if (operations.at(k)->type == 4)
                                { // COMP
                                    outFile << "                    "
                                                << "if ("
                                                << operations.at(k)
                                                       ->inputs
                                                       .at(0)
                                                       ->name;
                                    if (operations.at(k)->compType == 0)
                                    {
                                        outFile << " < ";
                                    }
                                    if (operations.at(k)->compType == 1)
                                    {
                                        outFile << " > ";
                                    }
                                    if (operations.at(k)->compType == 2)
                                    {
                                        outFile << " == ";
                                    }
                                    outFile
                                        << operations.at(k)->inputs.at(1)->name
                                        << ") begin" << std::endl;
                                    outFile << operations.at(k)
                                                       ->outputs
                                                       .at(0)
                                                       ->name
                                                << " <= 1;" << std::endl
                                                << "end" << std::endl;
                                    outFile << "else begin" << std::endl
                                                << operations.at(k)
                                                       ->outputs
                                                       .at(0)
                                                       ->name
                                                << " <= 0;" << std::endl
                                                << "end" << std::endl;
                                }
                                else if (operations.at(k)->type == 8)
                                { // HANDLE FOR THE SECOND IF STATEMENT OR NESTED IF STATEMENT
                                    outFile
                                        << "                        "
                                        << "if ("
                                        << operations.at(k)->inputs.at(0)->name
                                        << ") begin" << std::endl;
                                    for (int l = 0; l < operations.size(); l++)
                                    {
                                        for (int m = 0;
                                             m < operations.at(l)->inIfs.size();
                                             m++)
                                        {
                                            if (operations.at(l)->inIfs.at(m) ==
                                                operations.at(k))
                                            {
                                                if (operations.at(l)->type != 8 &&
                                                    operations.at(l)->type != 9 &&
                                                    operations.at(l)->type != 4)
                                                {
                                                    outFile << "                            "
                                                                << operations.at(l)
                                                                       ->outputs
                                                                       .at(0)
                                                                       ->name
                                                                << " <= ";
                                                    if (operations.at(l)->type == 5)
                                                    {
                                                        outFile << "(";
                                                    }
                                                    outFile << operations.at(l)
                                                                       ->inputs
                                                                       .at(0)
                                                                       ->name;
                                                    if (operations.at(l)->type == 5)
                                                    {
                                                        outFile << ")?";
                                                    }
                                                    switch (operations.at(l)->type)
                                                    {
                                                    case 1:
                                                        outFile << " + ";
                                                        break;
                                                    case 2:
                                                        outFile << " - ";
                                                        break;
                                                    case 3:
                                                        outFile << " * ";
                                                        break;
                                                    case 5:
                                                        outFile << ":";
                                                        break;
                                                    case 6:
                                                        outFile << " >> ";
                                                        break;
                                                    case 7:
                                                        outFile << " << ";
                                                        break;
                                                    default:
                                                        outFile << "0 //";
                                                        break;
                                                    }
                                                    outFile << operations.at(l)
                                                                       ->inputs
                                                                       .at(1)
                                                                       ->name
                                                                << ";" << std::endl;
                                                }
                                                else if (operations.at(l)->type ==
                                                         4)
                                                { // COMP
                                                    outFile << "                            "
                                                                << "if ("
                                                                << operations.at(l)
                                                                       ->inputs
                                                                       .at(0)
                                                                       ->name;
                                                    if (operations.at(l)->compType ==
                                                        0)
                                                    {
                                                        outFile << " < ";
                                                    }
                                                    if (operations.at(l)->compType ==
                                                        1)
                                                    {
                                                        outFile << " > ";
                                                    }
                                                    if (operations.at(l)->compType ==
                                                        2)
                                                    {
                                                        outFile << " == ";
                                                    }
                                                    outFile << operations.at(l)
                                                                       ->inputs
                                                                       .at(1)
                                                                       ->name
                                                                << ") begin" << std::endl;
                                                    outFile << "                        "
                                                                << operations.at(l)->outputs.at(0)->name
                                                                << " <= 1;" << std::endl
                                                                << "                        "
                                                                << "end" << std::endl;
                                                    outFile << "else begin" << std::endl
                                                                << operations.at(l)
                                                                       ->outputs
                                                                       .at(0)
                                                                       ->name
                                                                << " <= 0 " << std::endl
                                                                << "                        "
                                                                << "end" << std::endl;
                                                }
                                                else if (operations.at(l)->type ==
                                                         8)
                                                { // HANDLE FOR THE THIRD NESTED IF STATEMENT
                                                    // FIXME
                                                    outFile
                                                        << "                        "
                                                        << "if ("
                                                        << operations.at(l)
                                                               ->inputs
                                                               .at(0)
                                                               ->name
                                                        << ") begin" << std::endl;
                                                    for (int m = 0; m < operations.size();
                                                         m++)
                                                    {
                                                        for (int n = 0; n < operations.at(m)
                                                                                ->inIfs
                                                                                .size();
                                                             n++)
                                                        {
                                                            if (operations.at(m)
                                                                    ->inIfs
                                                                    .at(n) ==
                                                                operations.at(l))
                                                            {
                                                                if (operations.at(m)
                                                                            ->type != 8 &&
                                                                    operations.at(m)
                                                                            ->type != 9 &&
                                                                    operations.at(m)
                                                                            ->type != 4)
                                                                {
                                                                    outFile
                                                                        << operations.at(m)
                                                                               ->outputs
                                                                               .at(0)
                                                                               ->name
                                                                        << " <= ";
                                                                    if (operations.at(m)
                                                                            ->type == 5)
                                                                    {
                                                                        outFile << "(";
                                                                    }
                                                                    outFile
                                                                        << operations.at(m)
                                                                               ->inputs
                                                                               .at(0)
                                                                               ->name;
                                                                    if (operations.at(m)
                                                                            ->type == 5)
                                                                    {
                                                                        outFile << ")?";
                                                                    }
                                                                    switch (operations.at(m)
                                                                                ->type)
                                                                    {
                                                                    case 1:
                                                                        outFile << " + ";
                                                                        break;
                                                                    case 2:
                                                                        outFile << " - ";
                                                                        break;
                                                                    case 3:
                                                                        outFile << " * ";
                                                                        break;
                                                                    case 5:
                                                                        outFile << ":";
                                                                        break;
                                                                    case 6:
                                                                        outFile << " >> ";
                                                                        break;
                                                                    case 7:
                                                                        outFile << " << ";
                                                                        break;
                                                                    default:
                                                                        outFile << "0 //";
                                                                        break;
                                                                    }
                                                                    outFile
                                                                        << operations.at(m)
                                                                               ->inputs
                                                                               .at(1)
                                                                               ->name
                                                                        << ";" << std::endl;
                                                                }
                                                                else if (operations.at(m)
                                                                             ->type ==
                                                                         4)
                                                                { // COMP
                                                                    outFile
                                                                        << "if ("
                                                                        << operations.at(m)
                                                                               ->inputs
                                                                               .at(0)
                                                                               ->name;
                                                                    if (operations.at(m)
                                                                            ->compType ==
                                                                        0)
                                                                    {
                                                                        outFile << " < ";
                                                                    }
                                                                    if (operations.at(m)
                                                                            ->compType ==
                                                                        1)
                                                                    {
                                                                        outFile << " > ";
                                                                    }
                                                                    if (operations.at(m)
                                                                            ->compType ==
                                                                        2)
                                                                    {
                                                                        outFile << " == ";
                                                                    }
                                                                    outFile
                                                                        << operations.at(m)
                                                                               ->inputs
                                                                               .at(1)
                                                                               ->name
                                                                        << ") begin" << std::endl;
                                                                    outFile
                                                                        << operations.at(m)
                                                                               ->outputs
                                                                               .at(0)
                                                                               ->name
                                                                        << " <= 1;" << std::endl
                                                                        << "end" << std::endl;
                                                                    outFile
                                                                        << "else begin" << std::endl
                                                                        << operations.at(m)
                                                                               ->outputs
                                                                               .at(0)
                                                                               ->name
                                                                        << " <= 0 " << std::endl
                                                                        << "end" << std::endl;
                                                                }
                                                                else if (operations.at(m)
                                                                             ->type ==
                                                                         8)
                                                                { // HANDLE FOR THE FOURTH NESTED IF STATEMENT
                                                                    outFile << "if (1) begin"
                                                                                << std::endl;
                                                                    // FIXME
                                                                    // FIXME
                                                                    outFile << "end" << std::endl;
                                                                }
                                                                else if (operations.at(m)
                                                                             ->type ==
                                                                         9)
                                                                { // ELSE x4
                                                                    outFile << "else begin"
                                                                                << std::endl;
                                                                    // FIXME
                                                                    // FIXME
                                                                    outFile << "end" << std::endl;
                                                                }
                                                            }
                                                        }
                                                    }
                                                    outFile << "end" << std::endl;
                                                    // FIXME
                                                }
                                                else if (operations.at(l)->type ==
                                                         9)
                                                { // ELSE x3
                                                    outFile << "else begin" << std::endl;
                                                    // FIXME
                                                    // FIXME
                                                    outFile << "end" << std::endl;
                                                }
                                            }
                                        }
                                    }

                                    outFile
                                        << "                        " << "end" << std::endl;
                                }
                                else if (operations.at(k)->type == 9)
                                { // ELSE
                                  // x2
                                    outFile
                                        << "                        " << "else begin" << std::endl;
                                    for (int l = 0; l < operations.size(); l++)
                                    {
                                        for (int m = 0;
                                             m < operations.at(l)->inIfs.size();
                                             m++)
                                        {
                                            if (operations.at(l)->inIfs.at(m) ==
                                                    operations.at(k) &&
                                                (operations.at(l)->inIfs.size() -
                                                 (m + 1)) == 0)
                                            {
                                                if (operations.at(l)->type != 8 &&
                                                    operations.at(l)->type != 9 &&
                                                    operations.at(l)->type != 4)
                                                {
                                                    outFile
                                                        << "                            " << operations.at(l)->outputs.at(0)->name
                                                        << " <= ";
                                                    if (operations.at(l)->type == 5)
                                                    {
                                                        outFile << "(";
                                                    }
                                                    outFile
                                                        << operations.at(l)
                                                               ->inputs.at(0)
                                                               ->name;
                                                    if (operations.at(l)->type == 5)
                                                    {
                                                        outFile << ")?";
                                                    }
                                                    switch (operations.at(l)->type)
                                                    {
                                                    case 1:
                                                        outFile << " + ";
                                                        break;
                                                    case 2:
                                                        outFile << " - ";
                                                        break;
                                                    case 3:
                                                        outFile << " * ";
                                                        break;
                                                    case 5:
                                                        outFile << ":";
                                                        break;
                                                    case 6:
                                                        outFile << " >> ";
                                                        break;
                                                    case 7:
                                                        outFile << " << ";
                                                        break;
                                                    default:
                                                        outFile << "0 //";
                                                        break;
                                                    }
                                                    outFile << operations.at(l)
                                                                       ->inputs
                                                                       .at(1)
                                                                       ->name
                                                                << ";" << std::endl;
                                                }
                                                else if (operations.at(l)->type ==
                                                         4)
                                                { // COMP
                                                    outFile
                                                        << "                        " << "if ("
                                                        << operations.at(l)
                                                               ->inputs
                                                               .at(0)
                                                               ->name;
                                                    if (operations.at(l)->compType ==
                                                        0)
                                                    {
                                                        outFile << " < ";
                                                    }
                                                    if (operations.at(l)->compType ==
                                                        1)
                                                    {
                                                        outFile << " > ";
                                                    }
                                                    if (operations.at(l)->compType ==
                                                        2)
                                                    {
                                                        outFile << " == ";
                                                    }
                                                    outFile << operations.at(l)
                                                                       ->inputs
                                                                       .at(1)
                                                                       ->name
                                                                << ") begin" << std::endl;
                                                    outFile << operations.at(l)
                                                                       ->outputs
                                                                       .at(0)
                                                                       ->name
                                                                << " <= 1;" << std::endl
                                                                << "end" << std::endl;
                                                    outFile << "else begin" << std::endl
                                                                << operations.at(l)
                                                                       ->outputs
                                                                       .at(0)
                                                                       ->name
                                                                << " <= 0;" << std::endl
                                                                << "end" << std::endl;
                                                }
                                                else if (operations.at(l)->type ==
                                                         8)
                                                { // IF x3
                                                    // FIXME
                                                    outFile
                                                        << "                            " << "if ("
                                                        << operations.at(l)
                                                               ->inputs
                                                               .at(0)
                                                               ->name
                                                        << ") begin" << std::endl;
                                                    for (int m = 0; m < operations.size();
                                                         m++)
                                                    {
                                                        for (int n = 0; n < operations.at(m)
                                                                                ->inIfs
                                                                                .size();
                                                             n++)
                                                        {
                                                            if (operations.at(m)
                                                                    ->inIfs
                                                                    .at(n) ==
                                                                operations.at(l))
                                                            {
                                                                if (operations.at(m)
                                                                            ->type != 8 &&
                                                                    operations.at(m)
                                                                            ->type != 9 &&
                                                                    operations.at(m)
                                                                            ->type != 4)
                                                                {
                                                                    outFile
                                                                        << "                                "
                                                                        << operations.at(m)->outputs.at(0)->name
                                                                        << " <= ";
                                                                    if (operations.at(m)
                                                                            ->type == 5)
                                                                    {
                                                                        outFile << "(";
                                                                    }
                                                                    outFile
                                                                        << operations.at(m)
                                                                               ->inputs
                                                                               .at(0)
                                                                               ->name;
                                                                    if (operations.at(m)
                                                                            ->type == 5)
                                                                    {
                                                                        outFile << ")?";
                                                                    }
                                                                    switch (operations.at(m)
                                                                                ->type)
                                                                    {
                                                                    case 1:
                                                                        outFile << " + ";
                                                                        break;
                                                                    case 2:
                                                                        outFile << " - ";
                                                                        break;
                                                                    case 3:
                                                                        outFile << " * ";
                                                                        break;
                                                                    case 5:
                                                                        outFile << ":";
                                                                        break;
                                                                    case 6:
                                                                        outFile << " >> ";
                                                                        break;
                                                                    case 7:
                                                                        outFile << " << ";
                                                                        break;
                                                                    default:
                                                                        outFile << "0 //";
                                                                        break;
                                                                    }
                                                                    outFile
                                                                        << operations.at(m)
                                                                               ->inputs
                                                                               .at(1)
                                                                               ->name
                                                                        << ";" << std::endl;
                                                                }
                                                                else if (operations.at(m)
                                                                             ->type ==
                                                                         4)
                                                                { // COMP
                                                                    outFile
                                                                        << "                                "
                                                                        << "if ("
                                                                        << operations.at(m)
                                                                               ->inputs
                                                                               .at(0)
                                                                               ->name;
                                                                    if (operations.at(m)
                                                                            ->compType ==
                                                                        0)
                                                                    {
                                                                        outFile << " < ";
                                                                    }
                                                                    if (operations.at(m)
                                                                            ->compType ==
                                                                        1)
                                                                    {
                                                                        outFile << " > ";
                                                                    }
                                                                    if (operations.at(m)
                                                                            ->compType ==
                                                                        2)
                                                                    {
                                                                        outFile << " == ";
                                                                    }
                                                                    outFile
                                                                        << operations.at(m)
                                                                               ->inputs
                                                                               .at(1)
                                                                               ->name
                                                                        << ") begin" << std::endl;
                                                                    outFile
                                                                        << operations.at(m)
                                                                               ->outputs
                                                                               .at(0)
                                                                               ->name
                                                                        << " <= 1;" << std::endl
                                                                        << "                                "
                                                                        << "end" << std::endl;
                                                                    outFile
                                                                        << "else begin" << std::endl
                                                                        << operations.at(m)
                                                                               ->outputs
                                                                               .at(0)
                                                                               ->name
                                                                        << " <= 0 " << std::endl
                                                                        << "                                "
                                                                        << "end" << std::endl;
                                                                }
                                                                else if (operations.at(m)
                                                                             ->type ==
                                                                         8)
                                                                { // IF x4
                                                                    outFile << "if (1) begin"
                                                                                << std::endl;
                                                                    // FIXME
                                                                    // FIXME
                                                                    outFile << "end" << std::endl;
                                                                }
                                                                else if (operations.at(m)
                                                                             ->type ==
                                                                         9)
                                                                { // ELSE x4
                                                                    outFile << "else begin"
                                                                                << std::endl;
                                                                    // FIXME
                                                                    // FIXME
                                                                    outFile << "end" << std::endl;
                                                                }
                                                            }
                                                        }
                                                    }
                                                    outFile << "                            " << "end" << std::endl;
                                                    // FIXME
                                                }
                                                else if (operations.at(l)->type ==
                                                         9)
                                                { // ELSE x3
                                                    outFile << "else begin" << std::endl;
                                                    // FIXME
                                                    // FIXME
                                                    outFile << "                    " << "end" << std::endl;
                                                }
                                            }
                                        }
                                    }
                                    // FIXME
                                    outFile << "                            "
                                                << "end" << std::endl;
                                }
                            }
                        }
                    }
                    outFile << "                        " << "end";
                    break;
                case 9: // ELSE x1
                    outFile << "else begin" << std::endl;
                    for (int k = 0; k < operations.size(); k++)
                    {
                        for (int l = 0; l < operations.at(k)->inIfs.size();
                             l++)
                        {
                            if (operations.at(k)->inIfs.at(l) ==
                                    operations.at(j) &&
                                (operations.at(k)->inIfs.size() - (l + 1)) == 0)
                            {
                                if (operations.at(k)->type != 8 &&
                                    operations.at(k)->type != 9 &&
                                    operations.at(k)->type != 4)
                                {
                                    outFile << operations.at(k)
                                                       ->outputs
                                                       .at(0)
                                                       ->name
                                                << " <= ";
                                    if (operations.at(k)->type == 5)
                                    {
                                        outFile << "(";
                                    }
                                    outFile << operations.at(k)
                                                       ->inputs
                                                       .at(0)
                                                       ->name;
                                    if (operations.at(k)->type == 5)
                                    {
                                        outFile << ")?";
                                    }
                                    switch (operations.at(k)->type)
                                    {
                                    case 1:
                                        outFile << " + ";
                                        break;
                                    case 2:
                                        outFile << " - ";
                                        break;
                                    case 3:
                                        outFile << " * ";
                                        break;
                                    case 5:
                                        break;
                                    case 6:
                                        outFile << " >> ";
                                        break;
                                    case 7:
                                        outFile << " << ";
                                        break;
                                    default:
                                        outFile << "0 //";
                                        break;
                                    }
                                    outFile << operations.at(k)
                                                       ->inputs
                                                       .at(1)
                                                       ->name;
                                    if (operations.at(k)->type == 5)
                                    {
                                        outFile << ":"
                                                    << operations.at(k)
                                                           ->inputs
                                                           .at(2)
                                                           ->name;
                                    }
                                    outFile << ";" << std::endl;
                                }
                                else if (operations.at(k)->type == 4)
                                { // COMP
                                    outFile << "if ("
                                                << operations.at(k)
                                                       ->inputs
                                                       .at(0)
                                                       ->name;
                                    if (operations.at(k)->compType == 0)
                                    {
                                        outFile << " < ";
                                    }
                                    if (operations.at(k)->compType == 1)
                                    {
                                        outFile << " > ";
                                    }
                                    if (operations.at(k)->compType == 2)
                                    {
                                        outFile << " == ";
                                    }
                                    outFile
                                        << operations.at(k)->inputs.at(1)->name
                                        << ") begin" << std::endl;
                                    outFile << operations.at(k)
                                                       ->outputs
                                                       .at(0)
                                                       ->name
                                                << " <= 1;" << std::endl
                                                << "end" << std::endl;
                                    outFile << "else begin" << std::endl
                                                << operations.at(k)
                                                       ->outputs
                                                       .at(0)
                                                       ->name
                                                << " <= 0;" << std::endl
                                                << "end" << std::endl;
                                }
                                else if (operations.at(k)->type == 8)
                                { // IF x2
                                    outFile
                                        << "if ("
                                        << operations.at(k)->inputs.at(0)->name
                                        << ") begin" << std::endl;
                                    for (int l = 0; l < operations.size(); l++)
                                    {
                                        for (int m = 0;
                                             m < operations.at(l)->inIfs.size();
                                             m++)
                                        {
                                            if (operations.at(l)->inIfs.at(m) ==
                                                operations.at(k))
                                            {
                                                if (operations.at(l)->type != 8 &&
                                                    operations.at(l)->type != 9 &&
                                                    operations.at(l)->type != 4)
                                                {
                                                    outFile << operations.at(l)
                                                                       ->outputs
                                                                       .at(0)
                                                                       ->name
                                                                << " <= ";
                                                    if (operations.at(l)->type == 5)
                                                    {
                                                        outFile << "(";
                                                    }
                                                    outFile << operations.at(l)
                                                                       ->inputs
                                                                       .at(0)
                                                                       ->name;
                                                    if (operations.at(l)->type == 5)
                                                    {
                                                        outFile << ")?";
                                                    }
                                                    switch (operations.at(l)->type)
                                                    {
                                                    case 1:
                                                        outFile << " + ";
                                                        break;
                                                    case 2:
                                                        outFile << " - ";
                                                        break;
                                                    case 3:
                                                        outFile << " * ";
                                                        break;
                                                    case 5:
                                                        outFile << ":";
                                                        break;
                                                    case 6:
                                                        outFile << " >> ";
                                                        break;
                                                    case 7:
                                                        outFile << " << ";
                                                        break;
                                                    default:
                                                        outFile << "0 //";
                                                        break;
                                                    }
                                                    outFile << operations.at(l)
                                                                       ->inputs
                                                                       .at(1)
                                                                       ->name
                                                                << ";" << std::endl;
                                                }
                                                else if (operations.at(l)->type ==
                                                         4)
                                                { // COMP
                                                    outFile << "if ("
                                                                << operations.at(l)
                                                                       ->inputs
                                                                       .at(0)
                                                                       ->name;
                                                    if (operations.at(l)->compType ==
                                                        0)
                                                    {
                                                        outFile << " < ";
                                                    }
                                                    if (operations.at(l)->compType ==
                                                        1)
                                                    {
                                                        outFile << " > ";
                                                    }
                                                    if (operations.at(l)->compType ==
                                                        2)
                                                    {
                                                        outFile << " == ";
                                                    }
                                                    outFile << operations.at(l)
                                                                       ->inputs
                                                                       .at(1)
                                                                       ->name
                                                                << ") begin" << std::endl;
                                                    outFile << operations.at(l)
                                                                       ->outputs
                                                                       .at(0)
                                                                       ->name
                                                                << " <= 1;" << std::endl
                                                                << "end" << std::endl;
                                                    outFile << "else begin" << std::endl
                                                                << operations.at(l)
                                                                       ->outputs
                                                                       .at(0)
                                                                       ->name
                                                                << " <= 0;" << std::endl
                                                                << "end";
                                                }
                                                else if (operations.at(l)->type ==
                                                         8)
                                                { // IF x3
                                                    outFile << "if (1) begin" << std::endl;
                                                    // FIXME
                                                    // FIXME
                                                    outFile << "end" << std::endl;
                                                }
                                                else if (operations.at(l)->type ==
                                                         9)
                                                { // ELSE x3
                                                    outFile << "else begin" << std::endl;
                                                    // FIXME
                                                    // FIXME
                                                    outFile << "end" << std::endl;
                                                }
                                            }
                                        }
                                    }
                                    outFile << "end" << std::endl;
                                }
                                else if (operations.at(k)->type == 9)
                                { // ELSE
                                  // x2
                                    outFile << "else begin" << std::endl;
                                    for (int l = 0; l < operations.size(); l++)
                                    {
                                        for (int m = 0;
                                             m < operations.at(l)->inIfs.size();
                                             m++)
                                        {
                                            if (operations.at(l)->inIfs.at(m) ==
                                                operations.at(k))
                                            {
                                                if (operations.at(l)->type != 8 &&
                                                    operations.at(l)->type != 9 &&
                                                    operations.at(l)->type != 4)
                                                {
                                                    outFile << operations.at(l)
                                                                       ->outputs
                                                                       .at(0)
                                                                       ->name
                                                                << " <= ";
                                                    if (operations.at(l)->type == 5)
                                                    {
                                                        outFile << "(";
                                                    }
                                                    outFile << operations.at(l)
                                                                       ->inputs
                                                                       .at(0)
                                                                       ->name;
                                                    if (operations.at(l)->type == 5)
                                                    {
                                                        outFile << ")?";
                                                    }
                                                    switch (operations.at(l)->type)
                                                    {
                                                    case 1:
                                                        outFile << " + ";
                                                        break;
                                                    case 2:
                                                        outFile << " - ";
                                                        break;
                                                    case 3:
                                                        outFile << " * ";
                                                        break;
                                                    case 5:
                                                        outFile << ":";
                                                        break;
                                                    case 6:
                                                        outFile << " >> ";
                                                        break;
                                                    case 7:
                                                        outFile << " << ";
                                                        break;
                                                    default:
                                                        outFile << "0 //";
                                                        break;
                                                    }
                                                    outFile << operations.at(l)
                                                                       ->inputs
                                                                       .at(1)
                                                                       ->name
                                                                << ";" << std::endl;
                                                }
                                                else if (operations.at(l)->type ==
                                                         4)
                                                { // COMP
                                                    outFile << "if ("
                                                                << operations.at(l)
                                                                       ->inputs
                                                                       .at(0)
                                                                       ->name;
                                                    if (operations.at(l)->compType ==
                                                        0)
                                                    {
                                                        outFile << " < ";
                                                    }
                                                    if (operations.at(l)->compType ==
                                                        1)
                                                    {
                                                        outFile << " > ";
                                                    }
                                                    if (operations.at(l)->compType ==
                                                        2)
                                                    {
                                                        outFile << " == ";
                                                    }
                                                    outFile << operations.at(l)
                                                                       ->inputs
                                                                       .at(1)
                                                                       ->name
                                                                << ") begin" << std::endl;
                                                    outFile << operations.at(l)
                                                                       ->outputs
                                                                       .at(0)
                                                                       ->name
                                                                << " <= 1;" << std::endl
                                                                << "end" << std::endl;
                                                    outFile << "else begin" << std::endl
                                                                << operations.at(l)
                                                                       ->outputs
                                                                       .at(0)
                                                                       ->name
                                                                << " <= 0;" << std::endl
                                                                << "end" << std::endl;
                                                }
                                                else if (operations.at(l)->type ==
                                                         8)
                                                { // IF x3
                                                    outFile << "if (1) begin" << std::endl;
                                                    // FIXME
                                                    // FIXME
                                                    outFile << "end" << std::endl;
                                                }
                                                else if (operations.at(l)->type ==
                                                         9)
                                                { // ELSE x3
                                                    outFile << "else begin" << std::endl;
                                                    // FIXME
                                                    // FIXME
                                                    outFile << "end" << std::endl;
                                                }
                                            }
                                        }
                                    }
                                    // FIXME
                                    outFile << "end" << std::endl;
                                }
                            }
                        }
                    }
                    outFile << "                    "
                                << "end";
                    break;
                case 10:
                    outFile << "//FOR ";
                    break;
                case 11:
                    outFile << "//DIV ";
                    break;
                case 12:
                    outFile << "//MOD ";
                    break;
                case 13:
                    outFile << "//NOP ";
                    break;
                }
                outFile << std::endl;
            }
        }
        if (i != latency)
        {
            outFile << "                    "
                        << "State <= S" << i << ";" << std::endl
                        << "                "
                        << "end" << std::endl;
        }
        else
        {
            outFile << "                    "
                        << "State <= Final;" << std::endl
                        << "                "
                        << "end" << std::endl;
        }
    }
    // As per the answer in Piazza question @61
    // 'Done' can be in the final or penultimate stat.
    // It functionally wouldn't make any difference (apart from the extra state) since the assumption is that an external system waits for the done signal.
    outFile << "                "
                << "Final: begin" << std::endl;
    outFile << "                    "
                << "Done <= 1;" << std::endl
                << "                    "
                << "State <= Wait;" << std::endl
                << "                "
                << "end" << std::endl
                << "            "
                << "endcase" << std::endl
                << "        "
                << "end" << std::endl
                << "    "
                << "end" << std::endl
                << "endmodule" << std::endl;

    std::cout << "\nConversion successful: " << outName << std::endl
              << std::endl;

    inFile.close();
    outFile.close();
}


void HLSM::convertToHLSM(const std::string &inName, const std::string &outName, int latency)
{
    std::ifstream inFile(inName, std::ifstream::in); // Creates an input file stream object and attempts to open the specified file in reading mode
    std::ofstream outFile(outName, std::ofstream::out); // Creates an output file stream object and attempts to open the specified file for writing.

    // Checks if the file can be successfully opened
	// and if the file stream is in a good state for further operations like reading
	// 
    if (!inFile.is_open() && !inFile.good())
    {
        // Print out the inName
		std::cerr << "Could not open cFile with the name: " << inName << std::endl;
		exit(1); // Exit the program
	}
    
	if (latency <= 0) // Check if the latency constraint is less than or equal to zero
    {
        // Print out the latency constraint value
		std::cerr << "Invalid latency constraint: " << latency << std::endl;
		exit(1); // Exit the program
	}

	// This if statement is used to test whether the outputfile extension is correct
	// The expected outputfile extension is '.v'
    // Wrong extension format or no extension specified at all will result in an error message
	//
	if (!outFile.is_open() && !outFile.good()) // Check if the output file can be opened and valid to write to
	{
		std::cerr << "Unable to open output file." << std::endl;
		//return std::vector<std::string>();
		exit(1);
	}


    // ██████╗ ██████╗ ███████╗    ██████╗ ██████╗  ██████╗  ██████╗███████╗███████╗███████╗    ██╗███╗   ██╗██████╗ ██╗   ██╗████████╗
    // ██╔══██╗██╔══██╗██╔════╝    ██╔══██╗██╔══██╗██╔═══██╗██╔════╝██╔════╝██╔════╝██╔════╝    ██║████╗  ██║██╔══██╗██║   ██║╚══██╔══╝
    // ██████╔╝██████╔╝█████╗█████╗██████╔╝██████╔╝██║   ██║██║     █████╗  ███████╗███████╗    ██║██╔██╗ ██║██████╔╝██║   ██║   ██║   
    // ██╔═══╝ ██╔══██╗██╔══╝╚════╝██╔═══╝ ██╔══██╗██║   ██║██║     ██╔══╝  ╚════██║╚════██║    ██║██║╚██╗██║██╔═══╝ ██║   ██║   ██║   
    // ██║     ██║  ██║███████╗    ██║     ██║  ██║╚██████╔╝╚██████╗███████╗███████║███████║    ██║██║ ╚████║██║     ╚██████╔╝   ██║   
    // ╚═╝     ╚═╝  ╚═╝╚══════╝    ╚═╝     ╚═╝  ╚═╝ ╚═════╝  ╚═════╝╚══════╝╚══════╝╚══════╝    ╚═╝╚═╝  ╚═══╝╚═╝      ╚═════╝    ╚═╝   

    parseInput(inFile, latency);


    // ███████╗ ██████╗██╗  ██╗███████╗██████╗ ██╗   ██╗██╗     ███████╗  
    // ██╔════╝██╔════╝██║  ██║██╔════╝██╔══██╗██║   ██║██║     ██╔════╝  
    // ███████╗██║     ███████║█████╗  ██║  ██║██║   ██║██║     █████╗   
    // ╚════██║██║     ██╔══██║██╔══╝  ██║  ██║██║   ██║██║     ██╔══╝   
    // ███████║╚██████╗██║  ██║███████╗██████╔╝╚██████╔╝███████╗███████╗ 
    // ╚══════╝ ╚═════╝╚═╝  ╚═╝╚══════╝╚═════╝  ╚═════╝ ╚══════╝╚══════╝ 

    cleanPredecessors(operations);

    setALAPS(operations, latency);  // Create the ALAP graph

    List_R(operations, latency);    // Perform List-R scheduling


    // ██╗    ██╗██████╗ ██╗████████╗███████╗    ██╗   ██╗███████╗██████╗ ██╗██╗      ██████╗  ██████╗ 
    // ██║    ██║██╔══██╗██║╚══██╔══╝██╔════╝    ██║   ██║██╔════╝██╔══██╗██║██║     ██╔═══██╗██╔════╝ 
    // ██║ █╗ ██║██████╔╝██║   ██║   █████╗      ██║   ██║█████╗  ██████╔╝██║██║     ██║   ██║██║  ███╗
    // ██║███╗██║██╔══██╗██║   ██║   ██╔══╝      ╚██╗ ██╔╝██╔══╝  ██╔══██╗██║██║     ██║   ██║██║   ██║
    // ╚███╔███╔╝██║  ██║██║   ██║   ███████╗     ╚████╔╝ ███████╗██║  ██║██║███████╗╚██████╔╝╚██████╔╝
    //  ╚══╝╚══╝ ╚═╝  ╚═╝╚═╝   ╚═╝   ╚══════╝      ╚═══╝  ╚══════╝╚═╝  ╚═╝╚═╝╚══════╝ ╚═════╝  ╚═════╝ 

    writeToVerilog(inFile, outFile, outName, latency);

    return;
}