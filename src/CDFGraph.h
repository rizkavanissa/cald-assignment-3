#pragma once
#ifndef CDFGRAPH_H
#define CDFGRAPH_H

#include "main.h"
#include "Parser.h"
#include <math.h>
#include "State.h"
#include "IOV.h"
#include "Edge.h"
#include "Vertex.h"
#include "ControlGraph.h"
//class Vertex;
//class Edge;
//GITHUB or whatever
class CDFGraph {

public:
	
	CDFGraph();

	void setLatency(int n);
	int getLatency();
	void loadFileStrings(std::vector<string> strVec);
	void loadIOV(std::vector<IOV> ins, std::vector<IOV> outs, std::vector<IOV> vars);
	IOV *getIOVbyName(std::string s);
	void parseOperation(string s);
	void parseInput(string s, Vertex* newV);
	void parseOutput(string s, Vertex* newV);
	//void parseIF(string s, Conditional *c);
	void printGraph();
	void parseOperations();
	void addConditionalVertices();
	Vertex* parseConditional(string s);
//	Conditional* parseConditional(string s);
	std::vector<Vertex*> getVerticesByEdgeID(string s);
	std::vector<Vertex*> getVertices();
	void DFS(CDFGraph* g, Vertex* v);
	void ALAP(CDFGraph* g, Vertex* v, int time);
	void ALAP(CDFGraph * g, int n);
	void ALAP(int n);

	void LIST_R(int n);

	void resetRCounts();

	bool checkInputorVariable(std::string s);

	bool checkOutputorVariable(std::string s);

	int calculateStates(int n);

	void generateVerilogFile(char * outFileStr);

	void resetVertexVisits();


	void LIST_R(CDFGraph * g, Vertex * v);

private:
	std::vector<IOV> inputs;
	std::vector<IOV> outputs;
	std::vector<IOV> variables;

	Vertex* vINOP;
	Vertex* vONOP;
	Resource ONOP;
	Resource INOP;
	std::vector<Vertex*> Vertices;
	std::vector<Edge*> Edges;
	std::vector<string> FileStrings;
	std::vector<State*> vStates;
	//int AddSubCnt;
	//int MultCnt;
	//int LogicCnt;
	//int IfCnt;

	ControlGraph gControlGraph;
	Block* currBlk;
	Conditional* currC;
	std::vector<Conditional*> CondVec;
	BlockType _last;

	static Resource ADDER_SUBTRACTOR;
	static Resource MULTIPLIER;
	static Resource LOGICAL;
	static Resource IF_STATEMENT;
	std::vector<Resource*> rVec;

	int latency;
	int maxTime;
	int minTime;
};


#endif