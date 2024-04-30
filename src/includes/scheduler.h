#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "operation.h"

#include <iostream>
#include <vector>

void setALAPS(std::vector<Operation *> ops, int lat);
void List_R(std::vector<Operation *> ops, int lat);

#endif
