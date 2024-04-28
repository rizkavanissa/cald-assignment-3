#pragma once
#ifndef HLSM_H
#define HLSM_H

#include "main.h"
#include "State.h"

class HLSM {

public:

private:
	int latency;
	std::vector<State> states;

};
#endif