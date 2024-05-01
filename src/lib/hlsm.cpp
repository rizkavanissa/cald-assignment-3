#include "../includes/hlsm.h"
#include "../includes/scheduler.h"

HLSM::HLSM() {}

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

std::vector<std::string> HLSM::parseLine(std::string line)
{
    std::vector<std::string> out;

    std::stringstream stream;
    stream.str(line);

    while (stream)
    {
        std::string temp;
        stream >> temp;
        if (temp.size() > 0)
        {
            char last = temp[temp.length() - 1];
            if (last == ',')
            {
                temp = temp.substr(0, temp.size() - 1);
            }
            out.push_back(temp);
        }
    }

    return out;
}

int HLSM::processFile(const std::string &fileName, const std::string &outputName, int latency)
{
    std::ifstream inFile(fileName);
    if (!inFile)
    {
        std::cerr << "Unable to open file " << fileName << "\n";
        return 1;
    }

    std::ofstream verilogFile(outputName);
    if (!verilogFile)
    {
        std::cerr << "Unable to open file " << outputName << "\n";
        return 1;
    }

    count = 0;
    currentLineIndex = 0;

    operations.push_back(&inop);
    operations.push_back(&onop);
    lastIf = NULL;

    error = false;
    justExited = false;

    while (!inFile.eof())
    {
        if (std::getline(inFile, currentLine) && !error)
        {
            currentLineIndex++;
            std::vector<std::string> words = HLSM::parseLine(currentLine);
            if (words.size() < 1)
            {
                // Do nothing...
            }
            else if (words.at(0).compare("}") == 0)
            {
                justExited = true;
                lastIf = inIfs.back();
                inIfs.pop_back();
            }
            else if (words.at(0).compare("else") == 0)
            {
                count++;
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
            else if (words.at(0).compare(0, 2, "//") == 0)
            {
                // Do nothing...
            }
            else if (words.size() > 2 && (words.at(0).compare("input") == 0 ||
                                          words.at(0).compare("output") == 0 ||
                                          words.at(0).compare("variable") ==
                                              0))
            {
                for (int i = 2; i < words.size(); i++)
                {
                    if (words.at(i).compare(0, 2, "//") == 0)
                        break;

                    Variable *tempVar = new Variable(words, i, &inop, &onop);
                    variables.push_back(tempVar);
                    justExited = false;
                }
            }
            else if (words.at(1).compare("=") == 0)
            {
                count++;

                Operation *tempOp = new Operation(words, variables, inIfs, count, lastIf, operations);
                operations.push_back(tempOp);
                justExited = false;
            }
            else if (words.at(0).compare("if") == 0 &&
                     words.at(1).compare("(") == 0 &&
                     words.at(3).compare(")") == 0 &&
                     words.at(4).compare("{") == 0)
            {
                count++;
                Operation *tempOp = new Operation(words, variables, inIfs, count, lastIf, operations);
                inIfs.push_back(tempOp);
                operations.push_back(tempOp);
                justExited = false;
            }
        }
    }

    cleanPredecessors(operations);

    setALAPS(operations, latency);
    List_R(operations, latency);

    verilogFile << "`timescale 1ns / 1ps\n\n";
    verilogFile << "module HLSM (Clk, Rst, Start, Done"; // MODULE START
    for (int i = 0; i < variables.size(); i++)
    {
        if (variables.at(i)->type == 0 || variables.at(i)->type == 1)
        {
            verilogFile << ", " << variables.at(i)->name;
        }
    }

    verilogFile << ");" << std::endl;
    verilogFile << "    input Clk, Rst, Start;" << std::endl; // INPUT START

    for (int i = 0; i < variables.size(); i++)
    {
        if (variables.at(i)->type == 0)
        {
            verilogFile << "    input ";
            if (variables.at(i)->sign)
            {
                verilogFile << "signed ";
            }
            if (variables.at(i)->width > 1)
            {
                verilogFile << "[" << variables.at(i)->width - 1 << ":0] ";
            }
            verilogFile << variables.at(i)->name << "; " << std::endl;
        }
    }

    verilogFile << "    output reg Done;" << std::endl; // OUTPUT START
    for (int i = 0; i < variables.size(); i++)
    {
        if (variables.at(i)->type == 1)
        {
            verilogFile << "    output reg ";
            if (variables.at(i)->sign)
            {
                verilogFile << "signed ";
            }
            if (variables.at(i)->width > 1)
            {
                verilogFile << "[" << variables.at(i)->width - 1 << ":0] ";
            }
            verilogFile << variables.at(i)->name << "; " << std::endl;
        }
    }

    for (int i = 0; i < variables.size(); i++)
    {
        if (variables.at(i)->type == 2)
        {
            verilogFile << "    reg ";
            if (variables.at(i)->sign)
            {
                verilogFile << "signed ";
            }
            if (variables.at(i)->width > 1)
            {
                verilogFile << "[" << variables.at(i)->width - 1 << ":0] ";
            }
            verilogFile << variables.at(i)->name << "; " << std::endl;
        }
    }
    verilogFile << "    reg [" << ceil(log(latency) / log(2)) - 1 << ":0] State;"
                << std::endl;

    verilogFile << "    parameter Wait = 0, Final = 1, ";
    for (int i = 0; i < (latency); i++)
    {
        verilogFile << "S" << i << " = " << i + 2;
        if (i != latency - 1)
        {
            verilogFile << ", ";
        }
        else
        {
            verilogFile << ";" << std::endl;
        }
    }
    verilogFile << "\n    always @(posedge Clk) begin" << std::endl;
    verilogFile << "        if (Rst == 1) begin" << std::endl
                << "            Done <= 0;" << std::endl
                << "            State <= Wait;" << std::endl
                << "        end" << std::endl;
    verilogFile << "        else begin" << std::endl;
    verilogFile << "            case(State)" << std::endl;
    verilogFile << "                Wait: begin" << std::endl;
    verilogFile << "                    Done <= 0;" << std::endl;
    verilogFile << "                    if (Start == 1) begin" << std::endl;
    verilogFile << "                        State <= S0;" << std::endl;
    verilogFile << "                    end" << std::endl;
    verilogFile << "                    else begin" << std::endl;
    verilogFile << "                        State <= Wait;" << std::endl;
    verilogFile << "                    end" << std::endl
                << "                end" << std::endl;
    for (int i = 1; i < latency + 1; i++)
    {
        // output case
        verilogFile << "                S" << i - 1 << ": begin" << std::endl;
        for (int j = 0; j < operations.size(); j++)
        {
            if ((operations.at(j)->scheduledState == i) &&
                (operations.at(j)->inIfs.size() == 0))
            {
                switch (operations.at(j)->type)
                {
                case 0: // REG
                    verilogFile << "//REG";
                    break;
                case 1: // ADD
                    verilogFile << "                    "
                                << operations.at(j)->outputs.at(0)->name
                                << " <= ";
                    verilogFile << operations.at(j)->inputs.at(0)->name
                                << " + ";
                    verilogFile << operations.at(j)->inputs.at(1)->name
                                << ";";
                    break;
                case 2: // SUB
                    verilogFile << "                    "
                                << operations.at(j)->outputs.at(0)->name
                                << " <= ";
                    verilogFile << operations.at(j)->inputs.at(0)->name
                                << " - ";
                    verilogFile << operations.at(j)->inputs.at(1)->name
                                << ";";
                    break;
                case 3: // MUL
                    verilogFile << "                    "
                                << operations.at(j)->outputs.at(0)->name
                                << " <= ";
                    verilogFile << operations.at(j)->inputs.at(0)->name
                                << " * ";
                    verilogFile << operations.at(j)->inputs.at(1)->name
                                << ";";
                    break;
                case 4: // COMP
                    verilogFile << "                    "
                                << "if ("
                                << operations.at(j)->inputs.at(0)->name;
                    if (operations.at(j)->compType == 0)
                    {
                        verilogFile << " < ";
                    }
                    if (operations.at(j)->compType == 1)
                    {
                        verilogFile << " > ";
                    }
                    if (operations.at(j)->compType == 2)
                    {
                        verilogFile << " == ";
                    }
                    verilogFile << operations.at(j)->inputs.at(1)->name
                                << ") begin" << std::endl;
                    verilogFile << "                        "
                                << operations.at(j)->outputs.at(0)->name
                                << " <= 1;" << std::endl
                                << "                    end" << std::endl;
                    verilogFile << "                    "
                                << "else begin" << std::endl
                                << "                        "
                                << operations.at(j)->outputs.at(0)->name
                                << " <= 0;" << std::endl
                                << "                    end";
                    break;
                case 5: // MUX
                    verilogFile << "                    "
                                << operations.at(j)->outputs.at(0)->name
                                << " <= (";
                    verilogFile << operations.at(j)->inputs.at(0)->name
                                << ")?";
                    verilogFile << operations.at(j)->inputs.at(1)->name
                                << ":";
                    verilogFile << operations.at(j)->inputs.at(2)->name
                                << ";";
                    break;
                case 6: // SHR
                    verilogFile << "                    "
                                << operations.at(j)->outputs.at(0)->name
                                << " <= ";
                    verilogFile << operations.at(j)->inputs.at(0)->name
                                << " >> ";
                    verilogFile << operations.at(j)->inputs.at(1)->name
                                << ";";
                    break;
                case 7: // SHL
                    verilogFile << "                    "
                                << operations.at(j)->outputs.at(0)->name
                                << " <= ";
                    verilogFile << operations.at(j)->inputs.at(0)->name
                                << " << ";
                    verilogFile << operations.at(j)->inputs.at(1)->name
                                << ";";
                    break;
                case 8: // IF x1
                    verilogFile << "                    "
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
                                    verilogFile << "                        "
                                                << operations.at(k)
                                                       ->outputs
                                                       .at(0)
                                                       ->name
                                                << " <= ";
                                    if (operations.at(k)->type == 5)
                                    {
                                        verilogFile << "(";
                                    }
                                    verilogFile << operations.at(k)
                                                       ->inputs
                                                       .at(0)
                                                       ->name;
                                    if (operations.at(k)->type == 5)
                                    {
                                        verilogFile << ")?";
                                    }
                                    switch (operations.at(k)->type)
                                    {
                                    case 1:
                                        verilogFile << " + ";
                                        break;
                                    case 2:
                                        verilogFile << " - ";
                                        break;
                                    case 3:
                                        verilogFile << " * ";
                                        break;
                                    case 6:
                                        verilogFile << " >> ";
                                        break;
                                    case 7:
                                        verilogFile << " << ";
                                        break;
                                    default:
                                        verilogFile << "0 //";
                                        break;
                                    }
                                    verilogFile << operations.at(k)
                                                       ->inputs
                                                       .at(1)
                                                       ->name;
                                    if (operations.at(k)->type == 5)
                                    {
                                        verilogFile << ":"
                                                    << operations.at(k)
                                                           ->inputs
                                                           .at(2)
                                                           ->name;
                                    }
                                    verilogFile << ";" << std::endl;
                                }
                                else if (operations.at(k)->type == 4)
                                { // COMP
                                    verilogFile << "                    "
                                                << "if ("
                                                << operations.at(k)
                                                       ->inputs
                                                       .at(0)
                                                       ->name;
                                    if (operations.at(k)->compType == 0)
                                    {
                                        verilogFile << " < ";
                                    }
                                    if (operations.at(k)->compType == 1)
                                    {
                                        verilogFile << " > ";
                                    }
                                    if (operations.at(k)->compType == 2)
                                    {
                                        verilogFile << " == ";
                                    }
                                    verilogFile
                                        << operations.at(k)->inputs.at(1)->name
                                        << ") begin" << std::endl;
                                    verilogFile << operations.at(k)
                                                       ->outputs
                                                       .at(0)
                                                       ->name
                                                << " <= 1;" << std::endl
                                                << "end" << std::endl;
                                    verilogFile << "else begin" << std::endl
                                                << operations.at(k)
                                                       ->outputs
                                                       .at(0)
                                                       ->name
                                                << " <= 0;" << std::endl
                                                << "end" << std::endl;
                                }
                                else if (operations.at(k)->type == 8)
                                { // IF x2
                                    verilogFile
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
                                                    verilogFile << "                            "
                                                                << operations.at(l)
                                                                       ->outputs
                                                                       .at(0)
                                                                       ->name
                                                                << " <= ";
                                                    if (operations.at(l)->type == 5)
                                                    {
                                                        verilogFile << "(";
                                                    }
                                                    verilogFile << operations.at(l)
                                                                       ->inputs
                                                                       .at(0)
                                                                       ->name;
                                                    if (operations.at(l)->type == 5)
                                                    {
                                                        verilogFile << ")?";
                                                    }
                                                    switch (operations.at(l)->type)
                                                    {
                                                    case 1:
                                                        verilogFile << " + ";
                                                        break;
                                                    case 2:
                                                        verilogFile << " - ";
                                                        break;
                                                    case 3:
                                                        verilogFile << " * ";
                                                        break;
                                                    case 5:
                                                        verilogFile << ":";
                                                        break;
                                                    case 6:
                                                        verilogFile << " >> ";
                                                        break;
                                                    case 7:
                                                        verilogFile << " << ";
                                                        break;
                                                    default:
                                                        verilogFile << "0 //";
                                                        break;
                                                    }
                                                    verilogFile << operations.at(l)
                                                                       ->inputs
                                                                       .at(1)
                                                                       ->name
                                                                << ";" << std::endl;
                                                }
                                                else if (operations.at(l)->type ==
                                                         4)
                                                { // COMP
                                                    verilogFile << "                            "
                                                                << "if ("
                                                                << operations.at(l)
                                                                       ->inputs
                                                                       .at(0)
                                                                       ->name;
                                                    if (operations.at(l)->compType ==
                                                        0)
                                                    {
                                                        verilogFile << " < ";
                                                    }
                                                    if (operations.at(l)->compType ==
                                                        1)
                                                    {
                                                        verilogFile << " > ";
                                                    }
                                                    if (operations.at(l)->compType ==
                                                        2)
                                                    {
                                                        verilogFile << " == ";
                                                    }
                                                    verilogFile << operations.at(l)
                                                                       ->inputs
                                                                       .at(1)
                                                                       ->name
                                                                << ") begin" << std::endl;
                                                    verilogFile << "                        "
                                                                << operations.at(l)->outputs.at(0)->name
                                                                << " <= 1;" << std::endl
                                                                << "                        "
                                                                << "end" << std::endl;
                                                    verilogFile << "else begin" << std::endl
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
                                                { // IF x3
                                                    // FIXME
                                                    verilogFile
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
                                                                    verilogFile
                                                                        << operations.at(m)
                                                                               ->outputs
                                                                               .at(0)
                                                                               ->name
                                                                        << " <= ";
                                                                    if (operations.at(m)
                                                                            ->type == 5)
                                                                    {
                                                                        verilogFile << "(";
                                                                    }
                                                                    verilogFile
                                                                        << operations.at(m)
                                                                               ->inputs
                                                                               .at(0)
                                                                               ->name;
                                                                    if (operations.at(m)
                                                                            ->type == 5)
                                                                    {
                                                                        verilogFile << ")?";
                                                                    }
                                                                    switch (operations.at(m)
                                                                                ->type)
                                                                    {
                                                                    case 1:
                                                                        verilogFile << " + ";
                                                                        break;
                                                                    case 2:
                                                                        verilogFile << " - ";
                                                                        break;
                                                                    case 3:
                                                                        verilogFile << " * ";
                                                                        break;
                                                                    case 5:
                                                                        verilogFile << ":";
                                                                        break;
                                                                    case 6:
                                                                        verilogFile << " >> ";
                                                                        break;
                                                                    case 7:
                                                                        verilogFile << " << ";
                                                                        break;
                                                                    default:
                                                                        verilogFile << "0 //";
                                                                        break;
                                                                    }
                                                                    verilogFile
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
                                                                    verilogFile
                                                                        << "if ("
                                                                        << operations.at(m)
                                                                               ->inputs
                                                                               .at(0)
                                                                               ->name;
                                                                    if (operations.at(m)
                                                                            ->compType ==
                                                                        0)
                                                                    {
                                                                        verilogFile << " < ";
                                                                    }
                                                                    if (operations.at(m)
                                                                            ->compType ==
                                                                        1)
                                                                    {
                                                                        verilogFile << " > ";
                                                                    }
                                                                    if (operations.at(m)
                                                                            ->compType ==
                                                                        2)
                                                                    {
                                                                        verilogFile << " == ";
                                                                    }
                                                                    verilogFile
                                                                        << operations.at(m)
                                                                               ->inputs
                                                                               .at(1)
                                                                               ->name
                                                                        << ") begin" << std::endl;
                                                                    verilogFile
                                                                        << operations.at(m)
                                                                               ->outputs
                                                                               .at(0)
                                                                               ->name
                                                                        << " <= 1;" << std::endl
                                                                        << "end" << std::endl;
                                                                    verilogFile
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
                                                                { // IF x4
                                                                    verilogFile << "if (1) begin"
                                                                                << std::endl;
                                                                    // FIXME
                                                                    // FIXME
                                                                    verilogFile << "end" << std::endl;
                                                                }
                                                                else if (operations.at(m)
                                                                             ->type ==
                                                                         9)
                                                                { // ELSE x4
                                                                    verilogFile << "else begin"
                                                                                << std::endl;
                                                                    // FIXME
                                                                    // FIXME
                                                                    verilogFile << "end" << std::endl;
                                                                }
                                                            }
                                                        }
                                                    }
                                                    verilogFile << "end" << std::endl;
                                                    // FIXME
                                                }
                                                else if (operations.at(l)->type ==
                                                         9)
                                                { // ELSE x3
                                                    verilogFile << "else begin" << std::endl;
                                                    // FIXME
                                                    // FIXME
                                                    verilogFile << "end" << std::endl;
                                                }
                                            }
                                        }
                                    }

                                    verilogFile
                                        << "                        " << "end" << std::endl;
                                }
                                else if (operations.at(k)->type == 9)
                                { // ELSE
                                  // x2
                                    verilogFile
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
                                                    verilogFile
                                                        << "                            " << operations.at(l)->outputs.at(0)->name
                                                        << " <= ";
                                                    if (operations.at(l)->type == 5)
                                                    {
                                                        verilogFile << "(";
                                                    }
                                                    verilogFile
                                                        << operations.at(l)
                                                               ->inputs.at(0)
                                                               ->name;
                                                    if (operations.at(l)->type == 5)
                                                    {
                                                        verilogFile << ")?";
                                                    }
                                                    switch (operations.at(l)->type)
                                                    {
                                                    case 1:
                                                        verilogFile << " + ";
                                                        break;
                                                    case 2:
                                                        verilogFile << " - ";
                                                        break;
                                                    case 3:
                                                        verilogFile << " * ";
                                                        break;
                                                    case 5:
                                                        verilogFile << ":";
                                                        break;
                                                    case 6:
                                                        verilogFile << " >> ";
                                                        break;
                                                    case 7:
                                                        verilogFile << " << ";
                                                        break;
                                                    default:
                                                        verilogFile << "0 //";
                                                        break;
                                                    }
                                                    verilogFile << operations.at(l)
                                                                       ->inputs
                                                                       .at(1)
                                                                       ->name
                                                                << ";" << std::endl;
                                                }
                                                else if (operations.at(l)->type ==
                                                         4)
                                                { // COMP
                                                    verilogFile
                                                        << "                        " << "if ("
                                                        << operations.at(l)
                                                               ->inputs
                                                               .at(0)
                                                               ->name;
                                                    if (operations.at(l)->compType ==
                                                        0)
                                                    {
                                                        verilogFile << " < ";
                                                    }
                                                    if (operations.at(l)->compType ==
                                                        1)
                                                    {
                                                        verilogFile << " > ";
                                                    }
                                                    if (operations.at(l)->compType ==
                                                        2)
                                                    {
                                                        verilogFile << " == ";
                                                    }
                                                    verilogFile << operations.at(l)
                                                                       ->inputs
                                                                       .at(1)
                                                                       ->name
                                                                << ") begin" << std::endl;
                                                    verilogFile << operations.at(l)
                                                                       ->outputs
                                                                       .at(0)
                                                                       ->name
                                                                << " <= 1;" << std::endl
                                                                << "end" << std::endl;
                                                    verilogFile << "else begin" << std::endl
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
                                                    verilogFile
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
                                                                    verilogFile
                                                                        << "                                "
                                                                        << operations.at(m)->outputs.at(0)->name
                                                                        << " <= ";
                                                                    if (operations.at(m)
                                                                            ->type == 5)
                                                                    {
                                                                        verilogFile << "(";
                                                                    }
                                                                    verilogFile
                                                                        << operations.at(m)
                                                                               ->inputs
                                                                               .at(0)
                                                                               ->name;
                                                                    if (operations.at(m)
                                                                            ->type == 5)
                                                                    {
                                                                        verilogFile << ")?";
                                                                    }
                                                                    switch (operations.at(m)
                                                                                ->type)
                                                                    {
                                                                    case 1:
                                                                        verilogFile << " + ";
                                                                        break;
                                                                    case 2:
                                                                        verilogFile << " - ";
                                                                        break;
                                                                    case 3:
                                                                        verilogFile << " * ";
                                                                        break;
                                                                    case 5:
                                                                        verilogFile << ":";
                                                                        break;
                                                                    case 6:
                                                                        verilogFile << " >> ";
                                                                        break;
                                                                    case 7:
                                                                        verilogFile << " << ";
                                                                        break;
                                                                    default:
                                                                        verilogFile << "0 //";
                                                                        break;
                                                                    }
                                                                    verilogFile
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
                                                                    verilogFile
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
                                                                        verilogFile << " < ";
                                                                    }
                                                                    if (operations.at(m)
                                                                            ->compType ==
                                                                        1)
                                                                    {
                                                                        verilogFile << " > ";
                                                                    }
                                                                    if (operations.at(m)
                                                                            ->compType ==
                                                                        2)
                                                                    {
                                                                        verilogFile << " == ";
                                                                    }
                                                                    verilogFile
                                                                        << operations.at(m)
                                                                               ->inputs
                                                                               .at(1)
                                                                               ->name
                                                                        << ") begin" << std::endl;
                                                                    verilogFile
                                                                        << operations.at(m)
                                                                               ->outputs
                                                                               .at(0)
                                                                               ->name
                                                                        << " <= 1;" << std::endl
                                                                        << "                                "
                                                                        << "end" << std::endl;
                                                                    verilogFile
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
                                                                    verilogFile << "if (1) begin"
                                                                                << std::endl;
                                                                    // FIXME
                                                                    // FIXME
                                                                    verilogFile << "end" << std::endl;
                                                                }
                                                                else if (operations.at(m)
                                                                             ->type ==
                                                                         9)
                                                                { // ELSE x4
                                                                    verilogFile << "else begin"
                                                                                << std::endl;
                                                                    // FIXME
                                                                    // FIXME
                                                                    verilogFile << "end" << std::endl;
                                                                }
                                                            }
                                                        }
                                                    }
                                                    verilogFile << "                            " << "end" << std::endl;
                                                    // FIXME
                                                }
                                                else if (operations.at(l)->type ==
                                                         9)
                                                { // ELSE x3
                                                    verilogFile << "else begin" << std::endl;
                                                    // FIXME
                                                    // FIXME
                                                    verilogFile << "                    " << "end" << std::endl;
                                                }
                                            }
                                        }
                                    }
                                    // FIXME
                                    verilogFile << "                            "
                                                << "end" << std::endl;
                                }
                            }
                        }
                    }
                    verilogFile << "                        " << "end";
                    break;
                case 9: // ELSE x1
                    verilogFile << "else begin" << std::endl;
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
                                    verilogFile << operations.at(k)
                                                       ->outputs
                                                       .at(0)
                                                       ->name
                                                << " <= ";
                                    if (operations.at(k)->type == 5)
                                    {
                                        verilogFile << "(";
                                    }
                                    verilogFile << operations.at(k)
                                                       ->inputs
                                                       .at(0)
                                                       ->name;
                                    if (operations.at(k)->type == 5)
                                    {
                                        verilogFile << ")?";
                                    }
                                    switch (operations.at(k)->type)
                                    {
                                    case 1:
                                        verilogFile << " + ";
                                        break;
                                    case 2:
                                        verilogFile << " - ";
                                        break;
                                    case 3:
                                        verilogFile << " * ";
                                        break;
                                    case 5:
                                        break;
                                    case 6:
                                        verilogFile << " >> ";
                                        break;
                                    case 7:
                                        verilogFile << " << ";
                                        break;
                                    default:
                                        verilogFile << "0 //";
                                        break;
                                    }
                                    verilogFile << operations.at(k)
                                                       ->inputs
                                                       .at(1)
                                                       ->name;
                                    if (operations.at(k)->type == 5)
                                    {
                                        verilogFile << ":"
                                                    << operations.at(k)
                                                           ->inputs
                                                           .at(2)
                                                           ->name;
                                    }
                                    verilogFile << ";" << std::endl;
                                }
                                else if (operations.at(k)->type == 4)
                                { // COMP
                                    verilogFile << "if ("
                                                << operations.at(k)
                                                       ->inputs
                                                       .at(0)
                                                       ->name;
                                    if (operations.at(k)->compType == 0)
                                    {
                                        verilogFile << " < ";
                                    }
                                    if (operations.at(k)->compType == 1)
                                    {
                                        verilogFile << " > ";
                                    }
                                    if (operations.at(k)->compType == 2)
                                    {
                                        verilogFile << " == ";
                                    }
                                    verilogFile
                                        << operations.at(k)->inputs.at(1)->name
                                        << ") begin" << std::endl;
                                    verilogFile << operations.at(k)
                                                       ->outputs
                                                       .at(0)
                                                       ->name
                                                << " <= 1;" << std::endl
                                                << "end" << std::endl;
                                    verilogFile << "else begin" << std::endl
                                                << operations.at(k)
                                                       ->outputs
                                                       .at(0)
                                                       ->name
                                                << " <= 0;" << std::endl
                                                << "end" << std::endl;
                                }
                                else if (operations.at(k)->type == 8)
                                { // IF x2
                                    verilogFile
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
                                                    verilogFile << operations.at(l)
                                                                       ->outputs
                                                                       .at(0)
                                                                       ->name
                                                                << " <= ";
                                                    if (operations.at(l)->type == 5)
                                                    {
                                                        verilogFile << "(";
                                                    }
                                                    verilogFile << operations.at(l)
                                                                       ->inputs
                                                                       .at(0)
                                                                       ->name;
                                                    if (operations.at(l)->type == 5)
                                                    {
                                                        verilogFile << ")?";
                                                    }
                                                    switch (operations.at(l)->type)
                                                    {
                                                    case 1:
                                                        verilogFile << " + ";
                                                        break;
                                                    case 2:
                                                        verilogFile << " - ";
                                                        break;
                                                    case 3:
                                                        verilogFile << " * ";
                                                        break;
                                                    case 5:
                                                        verilogFile << ":";
                                                        break;
                                                    case 6:
                                                        verilogFile << " >> ";
                                                        break;
                                                    case 7:
                                                        verilogFile << " << ";
                                                        break;
                                                    default:
                                                        verilogFile << "0 //";
                                                        break;
                                                    }
                                                    verilogFile << operations.at(l)
                                                                       ->inputs
                                                                       .at(1)
                                                                       ->name
                                                                << ";" << std::endl;
                                                }
                                                else if (operations.at(l)->type ==
                                                         4)
                                                { // COMP
                                                    verilogFile << "if ("
                                                                << operations.at(l)
                                                                       ->inputs
                                                                       .at(0)
                                                                       ->name;
                                                    if (operations.at(l)->compType ==
                                                        0)
                                                    {
                                                        verilogFile << " < ";
                                                    }
                                                    if (operations.at(l)->compType ==
                                                        1)
                                                    {
                                                        verilogFile << " > ";
                                                    }
                                                    if (operations.at(l)->compType ==
                                                        2)
                                                    {
                                                        verilogFile << " == ";
                                                    }
                                                    verilogFile << operations.at(l)
                                                                       ->inputs
                                                                       .at(1)
                                                                       ->name
                                                                << ") begin" << std::endl;
                                                    verilogFile << operations.at(l)
                                                                       ->outputs
                                                                       .at(0)
                                                                       ->name
                                                                << " <= 1;" << std::endl
                                                                << "end" << std::endl;
                                                    verilogFile << "else begin" << std::endl
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
                                                    verilogFile << "if (1) begin" << std::endl;
                                                    // FIXME
                                                    // FIXME
                                                    verilogFile << "end" << std::endl;
                                                }
                                                else if (operations.at(l)->type ==
                                                         9)
                                                { // ELSE x3
                                                    verilogFile << "else begin" << std::endl;
                                                    // FIXME
                                                    // FIXME
                                                    verilogFile << "end" << std::endl;
                                                }
                                            }
                                        }
                                    }
                                    verilogFile << "end" << std::endl;
                                }
                                else if (operations.at(k)->type == 9)
                                { // ELSE
                                  // x2
                                    verilogFile << "else begin" << std::endl;
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
                                                    verilogFile << operations.at(l)
                                                                       ->outputs
                                                                       .at(0)
                                                                       ->name
                                                                << " <= ";
                                                    if (operations.at(l)->type == 5)
                                                    {
                                                        verilogFile << "(";
                                                    }
                                                    verilogFile << operations.at(l)
                                                                       ->inputs
                                                                       .at(0)
                                                                       ->name;
                                                    if (operations.at(l)->type == 5)
                                                    {
                                                        verilogFile << ")?";
                                                    }
                                                    switch (operations.at(l)->type)
                                                    {
                                                    case 1:
                                                        verilogFile << " + ";
                                                        break;
                                                    case 2:
                                                        verilogFile << " - ";
                                                        break;
                                                    case 3:
                                                        verilogFile << " * ";
                                                        break;
                                                    case 5:
                                                        verilogFile << ":";
                                                        break;
                                                    case 6:
                                                        verilogFile << " >> ";
                                                        break;
                                                    case 7:
                                                        verilogFile << " << ";
                                                        break;
                                                    default:
                                                        verilogFile << "0 //";
                                                        break;
                                                    }
                                                    verilogFile << operations.at(l)
                                                                       ->inputs
                                                                       .at(1)
                                                                       ->name
                                                                << ";" << std::endl;
                                                }
                                                else if (operations.at(l)->type ==
                                                         4)
                                                { // COMP
                                                    verilogFile << "if ("
                                                                << operations.at(l)
                                                                       ->inputs
                                                                       .at(0)
                                                                       ->name;
                                                    if (operations.at(l)->compType ==
                                                        0)
                                                    {
                                                        verilogFile << " < ";
                                                    }
                                                    if (operations.at(l)->compType ==
                                                        1)
                                                    {
                                                        verilogFile << " > ";
                                                    }
                                                    if (operations.at(l)->compType ==
                                                        2)
                                                    {
                                                        verilogFile << " == ";
                                                    }
                                                    verilogFile << operations.at(l)
                                                                       ->inputs
                                                                       .at(1)
                                                                       ->name
                                                                << ") begin" << std::endl;
                                                    verilogFile << operations.at(l)
                                                                       ->outputs
                                                                       .at(0)
                                                                       ->name
                                                                << " <= 1;" << std::endl
                                                                << "end" << std::endl;
                                                    verilogFile << "else begin" << std::endl
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
                                                    verilogFile << "if (1) begin" << std::endl;
                                                    // FIXME
                                                    // FIXME
                                                    verilogFile << "end" << std::endl;
                                                }
                                                else if (operations.at(l)->type ==
                                                         9)
                                                { // ELSE x3
                                                    verilogFile << "else begin" << std::endl;
                                                    // FIXME
                                                    // FIXME
                                                    verilogFile << "end" << std::endl;
                                                }
                                            }
                                        }
                                    }
                                    // FIXME
                                    verilogFile << "end" << std::endl;
                                }
                            }
                        }
                    }
                    verilogFile << "                    "
                                << "end";
                    break;
                case 10:
                    verilogFile << "//FOR ";
                    break;
                case 11:
                    verilogFile << "//DIV ";
                    break;
                case 12:
                    verilogFile << "//MOD ";
                    break;
                case 13:
                    verilogFile << "//NOP ";
                    break;
                }
                verilogFile << std::endl;
            }
        }
        if (i != latency)
        {
            verilogFile << "                    "
                        << "State <= S" << i << ";" << std::endl
                        << "                "
                        << "end" << std::endl;
        }
        else
        {
            verilogFile << "                    "
                        << "State <= Final;" << std::endl
                        << "                "
                        << "end" << std::endl;
        }
    }
    verilogFile << "                "
                << "Final: begin" << std::endl;
    verilogFile << "                    "
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

    std::cout << "DONE:" << outputName << std::endl
              << std::endl;

    inFile.close();
    verilogFile.close();

    return 0;
}