#include "Block.h"


Block::Block()
{	
	//cParent = NULL;
	//vCond = NULL;
	BlkConverse = NULL;
	cPrev = NULL;
	cNext = NULL;
	blkPrev = NULL;
	blkNext = NULL;
	isIf = false;
	isElse = false;
	found = false;
}

void Block::addVertex(Vertex * v)
{
	nodes.push_back(v);
}

void Block::addEdge(Edge * e)
{
	edges.push_back(e);
}

void Block::setPrev(Conditional * c)
{
	if (blkPrev != NULL) {
		//std::cout << "warning: Block already has both function and condition assigned to next." << std::endl;
	}
	cPrev = c;
}

void Block::setPrev(Block * f)
{
	if (cPrev != NULL) {
		std::cout << "warning: Block already has both function and condition assigned to next." << std::endl;
	}
	blkPrev = f;
}

void Block::setNext(Conditional * c)
{
	if (blkNext != NULL) {
		std::cout << "warning: Block already has both function and condition assigned to next." << std::endl;
	}
	cNext = c;
}

void Block::setNext(Block * f)
{
	if (cNext != NULL) {
		std::cout << "warning: Function already has both function and condition assigned to next." << std::endl;
	}
	blkNext = f;
}

void Block::clearNext()
{
	this->blkNext = NULL;
	this->cNext = NULL;
}

std::vector<Vertex*> Block::getNodes()
{
	return nodes;
}

Block* Block::convertToElse(Conditional* c)
{	
	isElse = true;
	Block* tmp;
	tmp = blkPrev;
	this->BlkConverse = tmp;
	tmp->setConverse(this);
	blkPrev->clearNext();
	this->blkPrev = NULL;
	this->setPrev(c);
	return tmp;
}

Vertex * Block::getFirst()
{
	return nodes.front();
}

void Block::setConverse(Block * blk)
{
	BlkConverse = blk;
}

Block * Block::getConverse()
{
	return BlkConverse;
}

void Block::setToElse()
{
	if (isElse) {
		std::cout << "ERROR THIS BLOCK IS AN ELSE AND AN IF WHAT GIVES(ELSE)" << std::endl;
	}
	isElse = true;
}

void Block::setToIf()
{
	if (isElse) {
		std::cout << "ERROR THIS BLOCK IS AN ELSE AND AN IF WHAT GIVES (IF)" << std::endl;
	}
	isIf = true;

}

bool Block::query_IsElse()
{
	
	return isElse;
}

bool Block::query_IsIf()
{
	return isIf;
}

bool Block::checkForVertex(Vertex* v) {

	
		if (std::find(nodes.begin(), nodes.end(), v) != nodes.end()) {
			return true;
		}
		return false;
}
bool sortbySchedule2(Vertex *lhs, Vertex *rhs)
{
	bool val;
	val = false;

	if (lhs->query_Schedule() < rhs->query_Schedule()) {
		val = true;
	}

	return val;

}
std::vector<State*> Block::getStates() {
	State* currS;
	State* nextS;
	//int tCurr, tLast;
	int sTime;
	stringstream ss;
	if (states.size() > 0) {
		return states;
	}

		std::vector<Vertex*> elseNodes;
//	Vertex* condV;

	std::sort(nodes.begin(), nodes.end(), sortbySchedule2);


	std::string sName;

	if (isIf){
		sName = "F_" ;
	}
	else if (isElse){
		sName = "E_";
	}
	else{
		sName = "D_";
	}
//	sName = "blk_";
	sTime = nodes.front()->query_Schedule();
	ss << sName << State::getStateCount() << "_";
	currS = new State(sTime, ss.str());
	ss.str("");
	ss.clear();
	//currS->addVertex(nodes.front());
	states.push_back(currS);
	State* cState;
	
	if (this->nodes.front()->getString() == "OUTPUTS") {
		//std::cout << "pause";
		return std::vector<State*>();
	}
	for (std::vector<Vertex*>::iterator currV = nodes.begin(); currV != nodes.end(); ++currV) {
		/*if ((*currV)->getString() == "INPUTS" || (*currV)->getString() == "OUTPUTS") {
			continue;
		}*/
		sTime = (*currV)->query_Schedule();
		
		if ((*currV)->getString() != "INPUTS" && (*currV)->getString() != "OUTPUTS" ) {
			
			if (sTime == currS->getTime()) {

				currS->addVertex(*currV);

			}
			else {

				sTime = (*currV)->query_Schedule();
				ss << sName << State::getStateCount() << "_";
				nextS = new State(sTime, ss.str());
				ss.clear();
				ss.str("");
				nextS->addVertex(*currV);
				currS->setNextIfTrue(nextS);
				currS = nextS;
				states.push_back(currS);

			}
		}

	}
	/*std::vector<State*>::iterator iPrev = states.begin();
	bool first = true;
	for (std::vector<State*>::iterator sIt = states.begin(); sIt != states.end();) {

		if ((*sIt)->getNodes().size() == 0) {
			if (!first) {
				(*iPrev)->clearNext();
				sIt = states.erase(sIt);
				(*iPrev)->setNextIfTrue(*sIt);
			}
			else {
				sIt = states.erase(sIt);
			}
		}
		else {
			first = false;
			iPrev = sIt;
			++sIt;
		}
	}*/

	//std::sort(states)
	std::vector<Vertex*> tmpN;
	found = true;
	return states;
}

void Block::setCParent(Conditional * c)
{
	//cParent = c;
}

Conditional * Block::getCParent()
{
	return cPrev;
}

Conditional* Block::getNextConditional(){
	return cNext;
}
Block* Block::getNextBlock(){
	return blkNext;
}

State * Block::getTopState_if_found()
{
	if (found) {
		return states.front();
	}
	return NULL;
}
