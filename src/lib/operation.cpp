#include "../includes/operation.h"

Operation::Operation() : type(NOP), scheduledTime(0), scheduled(true), ALAPTime(INT_MAX),
                         ALAPDone(false), id(0), compType(-1) {}

Operation::Operation(std::vector<std::string> words, std::vector<Variable *> availableVars,
                     std::vector<Operation *> inIfs, int count, Operation *lastIf,
                     std::vector<Operation *> availableOps)
{
    id = count;
    scheduledTime = 0;
    scheduled = false;
    ALAPTime = INT_MAX;
    ALAPDone = false;
    this->inIfs = inIfs;

    if (words.size() <= 3)
    {
        type = ELSE;
    }
    else
    {
        type = parseOp(words.at(3));
        if (type == COMP)
        {
            if (words.at(3).compare("<") == 0)
            {
                compType = 0;
            }
            if (words.at(3).compare(">") == 0)
            {
                compType = 1;
            }
            if (words.at(3).compare("==") == 0)
            {
                compType = 2;
            }
        }
    }
    if (type == OperationType::INVALID_OPERATOR)
    {
        std::cerr << "Error: Invalid Operation Type\n";
        return;
    }
    for (int i = 0; i < inIfs.size(); i++)
    {
        inIfs.at(i)->addSuccessor(this);
        this->addPredecessor(inIfs.at(i));
    }

    std::vector<std::string> inputsS;
    std::vector<std::string> outputsS;
    if (type == IF)
    {
        inputsS.push_back(words.at(2));
    }
    else if (type == ELSE)
    {
        for (int i = 0; i < lastIf->inputs.size(); i++)
        {
            inputsS.push_back(lastIf->inputs.at(i)->name);
        }
    }
    else
    {
        inputsS.push_back(words.at(2));
        inputsS.push_back(words.at(4));
        outputsS.push_back(words.at(0));
    }

    if (type == MUX)
    {
        if (words.at(5).compare(":") != 0)
        {
            std::cout << "Error: Invalid Operation" << std::endl;
            return;
        }
        inputsS.push_back(words.at(6));
    }

    bool found;
    // check that inputs provided exist
    for (int i = 0; i < inputsS.size(); i++)
    {
        found = false;
        for (int j = 0; j < availableVars.size(); j++)
        {
            if (inputsS.at(i).compare(availableVars.at(j)->name) == 0)
            {
                found = true;
                if (availableVars.at(j)->type ==
                    OUTPUT)
                {
                    std::cerr << "Output: " << availableVars.at(j)->name
                              << " used as input.\n";
                    return;
                }
                this->inputs.push_back(availableVars.at(j));
                availableVars.at(j)->consumers.push_back(this);
            }
        }
        if (!found)
        {
            std::cerr << "Input: " << inputsS.at(i) << " not declared\n";
            return;
        }
    }
    // check that outputs provided exist
    for (int i = 0; i < outputsS.size(); i++)
    {
        found = false;
        for (int j = 0; j < availableVars.size(); j++)
        {
            if (outputsS.at(i).compare(availableVars.at(j)->name) == 0)
            {
                found = true;
                if (availableVars.at(j)->type ==
                    INPUT)
                {
                    std::cerr << "Input: " << availableVars.at(j)->name
                              << " used as output.\n";
                    return;
                }
                this->outputs.push_back(availableVars.at(j));
                this->inputs.push_back(availableVars.at(j));
                availableVars.at(j)->producers.push_back(this);
                availableVars.at(j)->consumers.push_back(this);
            }
        }
        if (!found)
        {
            std::cerr << "Output: " << outputsS.at(i) << " not declared\n";
            return;
        }
    }
    for (int j = 0; j < this->inputs.size(); j++)
    {
        Variable *curInput = this->inputs.at(j);
        for (int k = 0; k < curInput->producers.size(); k++)
        {
            Operation *potentialPred = curInput->producers.at(k);
            bool inScope = true;
            if (inScope)
            {
                potentialPred->successors.push_back(this);
                this->predecessors.push_back(potentialPred);
            }
        }
    }
    this->setResource();
}

void Operation::addOutput(Variable *addMe)
{
    outputs.push_back(addMe);
}

void Operation::addInput(Variable *addMe)
{
    inputs.push_back(addMe);
}

void Operation::addSuccessor(Operation *successor)
{
    successors.push_back(successor);
}

void Operation::addPredecessor(Operation *predecessor)
{
    predecessors.push_back(predecessor);
}

void Operation::setALAPTime(int time)
{
    ALAPTime = time;
}

void Operation::setALAPDone(bool done)
{
    ALAPDone = done;
}

void Operation::scheduleAt(int time)
{
    scheduledTime = time;
    scheduled = true;
}

void Operation::setScheTime(int timeNum)
{
    scheTime = timeNum;
}

void Operation::setScheduledState(int stateTime)
{
    scheduledTime = stateTime;
}

bool Operation::isALAPDone()
{
    return this->ALAPDone;
}

Operation *Operation::getSucAt(int i)
{
    return successors.at(i);
}

void Operation::removeSucAt(int i)
{
    successors.erase(successors.begin() + i);
}

void Operation::setResource()
{
    switch (type)
    {
    case OperationType::ADD:
    case SUB:
        res = Resource::ADDER;
        break;
    case MUL:
        res = Resource::MULTIPLIER;
        break;
    case COMP:
    case MUX:
    case SHR:
    case SHL:
    case IF:
    case ELSE:
    case FOR:
        res = LOGIC;
        break;
    case DIV:
    case MOD:
        res = Resource::DIVIDER;
        break;
    default:
        res = Resource::INVALID_RESOURCE;
        break;
    }
}

bool Operation::inIf(Operation *anIf)
{
    for (Operation *IF : inIfs)
    {
        if (IF == anIf)
        {
            return true;
        }
    }
    return false;
}

OperationType Operation::parseOp(std::string in)
{
    if (in.compare("+") == 0)
    {
        return ADD;
    }
    else if (in.compare("-") == 0)
    {
        return SUB;
    }
    else if (in.compare("*") == 0)
    {
        return MUL;
    }
    else if (in.compare(">") == 0)
    {
        return COMP;
    }
    else if (in.compare("<") == 0)
    {
        return COMP;
    }
    else if (in.compare("==") == 0)
    {
        return COMP;
    }
    else if (in.compare("?") == 0)
    {
        return MUX;
    }
    else if (in.compare(">>") == 0)
    {
        return SHR;
    }
    else if (in.compare("<<") == 0)
    {
        return SHL;
    }
    else if (in.compare(")") == 0)
    {
        return IF;
    }
    else if (in.compare("") == 0)
    {
        return ELSE;
    }
    else if (in.compare("/") == 0)
    {
        return DIV;
    }
    else if (in.compare("%") == 0)
    {
        return DIV;
    }
    else
    {
        return OperationType::INVALID_OPERATOR;
    }
}
