#ifndef BLOCK_H
#define BLOCK_H

#include "main.h"
#include "Vertex.h"
#include "Edge.h"
//#include "CDFGraph.h"
//#include "Conditional.h"
#include "State.h"

class Conditional;
class Block {

public:

	Block();
	void addVertex(Vertex* v);
	void addEdge(Edge* e);
	void setPrev(Conditional* c);
	void setPrev(Block* f);
	void setNext(Conditional* c);
	void setNext(Block* blk);
	void clearNext();
	std::vector<Vertex*> getNodes();
	Block* convertToElse(Conditional* c);

	
	//Conditional* getNext();
	Vertex* getFirst();
	void setConverse(Block* blk);
	Block* getConverse();
	void setToElse();
	void setToIf();
	
	bool query_IsElse();
	bool query_IsIf();

	bool checkForVertex(Vertex *v);

	std::vector<State*> getStates();
	void setCParent(Conditional* c);
	Conditional* getCParent();
	Conditional* getNextConditional();
	Block* getNextBlock();
	//bool checkForVertexInConverse();
	State* getTopState_if_found();
private:

	std::vector<Vertex*> nodes;
	std::vector<Edge*> edges;
	bool found;
	bool isElse;
	bool isIf;
	//Vertex* vCond;
	Conditional* cPrev;
	Conditional* cNext;
	//Conditional* cParent;
	Block* blkPrev;
	Block* blkNext;
	Block* BlkConverse;
	std::vector<State*> states;
};

#endif

