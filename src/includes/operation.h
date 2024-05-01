#ifndef OPERATION_H
#define OPERATION_H

#include "variable.h"
#include <vector>

class Variable;

enum OperationType
{
    REG = 0,
    ADD,
    SUB,
    MUL,
    COMP,
    MUX,
    SHR,
    SHL,
    IF,
    ELSE,
    FOR,
    DIV,
    MOD,
    NOP,
    INVALID_OPERATOR = -1
};

enum Resource
{
    ADDER = 0,
    MULTIPLIER,
    LOGIC,
    DIVIDER,
    INVALID_RESOURCE
};

class Operation
{
public:
    OperationType type;
    std::vector<Variable *> inputs;
    std::vector<Variable *> outputs;
    std::vector<Operation *> successors;
    std::vector<Operation *> predecessors;
    std::vector<Operation *> inIfs;
    int id;
    int compType;
    int edge;
    int scheduledTime;
    int scheduledState;
    bool scheduled;
    int ALAPTime;
    bool ALAPDone;
    Resource res;
    int scheTime;

    Operation();
    Operation(std::vector<std::string> words, std::vector<Variable *> available,
              std::vector<Operation *> inIfs, int count, Operation *lastIf,
              std::vector<Operation *> availableOps);

    void addOutput(Variable *addMe);
    void addInput(Variable *addMe);
    void addSuccessor(Operation *successor);
    void addPredecessor(Operation *predecessor);
    void setALAPTime(int ALAPTime);
    void setALAPDone(bool done);
    void scheduleAt(int time);
    void setScheTime(int timeNum);
    void setScheduledState(int stateTime);
    bool isALAPDone();
    Operation *getSucAt(int i);
    void removeSucAt(int i);
    void setResource();
    bool inIf(Operation *anIf);
    OperationType parseOp(std::string in);
};

#endif