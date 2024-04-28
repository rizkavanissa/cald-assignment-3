#pragma once

#ifndef VERTEX_H
#define VERTEX_H
#include "main.h"
#include "Edge.h"
#include "Parser.h"
class Vertex {
	friend class CDFGraph;
	friend class Block;
public:
	Vertex();
	//Vertex(int n);
	//Vertex(int n, std::string strType);
	Vertex(int n, Resource* inRType);
	static Resource* checkValidOp(std::string s);
	void setString(std::string s);
	std::string getString();
	void setID(int n);
	int getID();

	void setType(Resource *r);
	Resource* getType();
	void printVertex();
	std::vector<Edge*> getOutgoing();
	std::vector<Edge*> getIncoming();
	void addIncoming(Edge* e);
	void addOutgoing(Edge* e);
	void fixOutGoing();
	//void setVNumber(int n);
	//int getVNumber();
	void visit();
	void resetVisit();
	bool checkVisited();
	//static void setLatency(int n);
	bool used;
	int getALAPTime();
	void setALAPTime(int n);

	bool operator==(const Vertex & other) const;

	void scheduleNode(int t);
	int query_Schedule();

private:

	int schedule;
	//std::string sType;
	Resource* rType;
	std::string strNode;
	int ID;
	//int vNumber;
	std::vector<Edge*> incoming;
	std::vector<Edge*> outgoing;
	bool visited;
	int ALAPtime;
	static std::vector<Resource*> resources;


};



#endif