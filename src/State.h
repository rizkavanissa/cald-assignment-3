#pragma once
#ifndef STATE_H
#define STATE_H

#include "main.h"
#include "Vertex.h"

class State {

public:
	State();
	State(int t, std::string s);
	void addVertex(Vertex* v);
	//static State* combineIfElseStates(State* s1, State* s2, Vertex* vCond);
	std::vector<Vertex*> getNodes();
	int getTime();
	void addNodeVector(std::vector<Vertex*> v);
	std::vector<string> getVerilog();
	//void setNext(State* n);
	std::vector<string> getStrings();
	void setName(std::string s);
	std::string getName();
	static void combineStates(State* s1, State* conditional);
	void setNextIfTrue(State* s);
	void setNextIfFalse(State* s);
	void setNextIfTrue(std::vector<State*> s);
	void setNextIfFalse(std::vector<State*> s);
	State* getNextIfTrue();
	void clearNext();
	State* getNextIfFalse();
	std::vector<std::string> getLines();
	void addLine(std::string s);

	void printLines();

	//bool operator==(const State & other) const;

	//bool operator!=(const State & other) const;
	static void resetStateCount();
	static int getStateCount();
	void markToErase();
	bool query_is_marked_for_erase();
private:
	std::string sName;
	//State* nextState;
	static int sCount;
	State* nextIfTrue;
	State* nextIfFalse;
	int time;
	std::vector<Vertex*> nodes;
	std::vector<std::string> slines;
	bool trash;
};
#endif