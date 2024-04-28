#pragma once
#ifndef EDGE_H
#define EDGE_H

#include "main.h"
//#include "CDFGraph.h"
#include "IOV.h"
class Vertex;
class Edge {
	friend class Vertex;
public:
	Edge(std::string strType, std::string strID);
	Edge(Vertex *v1, Vertex *v2);
	

	Vertex* getOutput();

	Vertex* getInput();
	void setID(string s);
	std::string getID();
	void printEdge();
protected:
	void setInput(Vertex *v);
	void setOutput(Vertex *v);
private:

	std::string sType;
	std::string ID;
	Vertex *out;
	Vertex *in;


	IOV var;
};
#endif