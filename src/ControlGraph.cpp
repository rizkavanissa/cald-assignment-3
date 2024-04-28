#include "ControlGraph.h"
ControlGraph::ControlGraph(){
	StartConditional = NULL;
	StartBlock = NULL;
}
void ControlGraph::addBlock(Block * b)
{
	if (blocks.size() == 0){
		StartBlock = b;
	}
	blocks.push_back(b);
	
}

void ControlGraph::addConditional(Conditional * c)
{
	conditionals.push_back(c);
}

Block* ControlGraph::dropLast()
{
	Block* currBlk;
	
	currBlk = blocks.back();

	while (currBlk->getNodes().size() == 0 && currBlk != NULL) {

		blocks.pop_back();
		currBlk = blocks.back();

	}
	blocks.back()->clearNext();

	return blocks.back();
}

std::vector<Block*> ControlGraph::getBlocks()
{
	return blocks;
}

std::vector<Conditional*> ControlGraph::getConditionals()
{
	return conditionals;
}
//std::vector<State*> ControlGraph::getStates(){
//
//
//}
void ControlGraph::setStart(Block* b){
	StartBlock = b;
}
void ControlGraph::setStart(Conditional* c){
	StartConditional = c;
	StartBlock = NULL;
}
//bool ControlGraph::query_isStartConditional(){
//	if (StartConditional != NULL && StartBlock == NULL){
//		return true;
//	}
//	return false;
//}

Conditional* ControlGraph::getStartConditional(){
	return StartConditional;
}
Block*  ControlGraph::getStartBlock(){
	return StartBlock;
}
std::vector<State*> ControlGraph::callGS() {
	std::vector<State*> allStates;
	State::resetStateCount();

	if (StartBlock != NULL) {

		//for(std::vector<Block*>::iterator bIt = )
		allStates = generateStates(StartBlock);
	}
	else if (StartConditional != NULL) {
		allStates = generateStates(StartConditional);
	}

	objStates = allStates;
	ControlGraph::setup_part1();
	ControlGraph::MergeStates();

	

	return objStates;
}



void  ControlGraph::setup_part1() {
	std::string s;
	State* currS;
	//State* newS = new State(objStates.size() + 1, "Wait");
	if (objStates.front()->getNodes().size() == 0) {
		objStates.front()->setName("Wait");
		Vertex* newV = new Vertex();
		newV->setString("if ( Start == 1 )");
		currS = objStates.front();
		currS->setNextIfFalse(currS);
		currS->addVertex(newV);
	}

	objStates.back()->setName("Final");
	for (std::vector<State*>::iterator sIt = objStates.begin(); sIt != objStates.end();sIt++) {

		if ((*sIt)->getNextIfTrue() == NULL) {
			(*sIt)->setNextIfTrue(currS);
		}
	
	}
	objStates.push_back(currS);

}
std::vector<State*> ControlGraph::generateStates(Block* b) {
	//State* newS;
//	Block* currB;
	std::vector<Vertex*> tmpNodes;
	//int time;
	std::vector<State*>allStates;
	std::vector<State*>tmpStates;

	State* currS;

	allStates = b->getStates();

	if (allStates.size() != 0) {
		currS = allStates.back();

		if (b->getNextConditional() != NULL) {
			tmpStates = generateStates(b->getNextConditional());
			if (tmpStates.size() == 0) {
				return allStates;
			}
			currS->setNextIfTrue(tmpStates.front());
			allStates.insert(allStates.end(), tmpStates.begin(), tmpStates.end());
			return allStates;
		}
		else if (b->getNextBlock() != NULL) {
			tmpStates = generateStates(b->getNextBlock());
			if (tmpStates.size() == 0) {
				return allStates;
			}
			currS->setNextIfTrue(tmpStates.front());
			allStates.insert(allStates.end(), tmpStates.begin(), tmpStates.end());
			return allStates;
		}
		else {
			return allStates;
		}

	}
	else {
		return std::vector<State*>();
	}
}

std::vector<State*> ControlGraph::generateStates(Conditional* c) {
	//State* newS;
//	Block* currB;
	std::vector<Vertex*> tmpNodes;
	//int time;
	std::vector<State*>allStates;
	std::vector<State*>tmpStatesTRUE;
	std::vector<State*>tmpStatesFALSE;
	State* nextS;
	State* currS = c->getConditionalState();
	allStates.push_back(currS);

	if (c->getNextBlk_True() != NULL) {
		tmpStatesTRUE = generateStates(c->getNextBlk_True());
		if (tmpStatesTRUE.size() == 0) {
			return allStates;
		}
		allStates.insert(allStates.end(), tmpStatesTRUE.begin(), tmpStatesTRUE.end());
		currS->setNextIfTrue(tmpStatesTRUE.front());
	}
	else if (c->getNextCondition_True() != NULL) {
		tmpStatesTRUE = generateStates(c->getNextCondition_True());
		if (tmpStatesTRUE.size() == 0) {
			return allStates;
		}
		allStates.insert(allStates.end(), tmpStatesTRUE.begin(), tmpStatesTRUE.end());
		currS->setNextIfTrue(tmpStatesTRUE.front());
	}

	if (c->getNextBlk_False() != NULL) {
		nextS = c->getNextBlk_False()->getTopState_if_found();
		if (nextS != NULL) {
			currS->setNextIfFalse(nextS);
		}
		else {
			tmpStatesFALSE = generateStates(c->getNextBlk_False());
			if (tmpStatesFALSE.size() == 0) {
				return allStates;
			}
			allStates.insert(allStates.end(), tmpStatesFALSE.begin(), tmpStatesFALSE.end());
			currS->setNextIfFalse(tmpStatesFALSE.front());
		}
	}
	else if (c->getNextCondition_False() != NULL) {
		if (c->getNextCondition_False()->checkFound()) {
			currS->setNextIfFalse(c->getNextCondition_False()->getConditionalState());
		}
		else {
			tmpStatesFALSE = generateStates(c->getNextCondition_False());
			if (tmpStatesFALSE.size() == 0) {
				return allStates;
			}
			allStates.insert(allStates.end(), tmpStatesFALSE.begin(), tmpStatesFALSE.end());
			currS->setNextIfFalse(tmpStatesFALSE.front());
		}
	}

	return allStates;

}

void  ControlGraph::MergeStates() {
	State *sCurr, *sNext;
	sCurr = objStates.front();


	//std::vector<State*> newVec;
	//State* newS;

	for (std::vector<State*>::iterator sIt = objStates.begin() + 1; sIt != objStates.end(); sIt++) {//this one combines conditionals in to their preceding states.
		sCurr = *sIt;
		if (sCurr->getNextIfTrue() != NULL) {//can only happen through the true branch.
			sNext = sCurr->getNextIfTrue();
			if (sCurr->getTime() == sNext->getTime()) {//can only happen if one of them is already pointing to the next.
				if (sNext->getName().at(0) == 'C') {//only if the second node is a conditional.
					State::combineStates(sCurr, sNext);
				}
			}
		}
	}


	for (std::vector<State*>::iterator sIt = objStates.begin() + 1; sIt != objStates.end(); sIt++) {
		if ((*sIt)->getNextIfTrue() != NULL) {
			if ((*sIt)->getNextIfTrue()->query_is_marked_for_erase()) {
				sCurr = (*sIt)->getNextIfTrue()->getNextIfFalse();
				(*sIt)->setNextIfTrue(sCurr);
			}
		}
		if ((*sIt)->getNextIfFalse() != NULL) {
			if ((*sIt)->getNextIfFalse()->query_is_marked_for_erase()) {
				sCurr = (*sIt)->getNextIfFalse()->getNextIfFalse();
				(*sIt)->setNextIfFalse(sCurr);
			}
		}
	}

	for (std::vector<State*>::iterator sIt = objStates.begin() + 1; sIt != objStates.end(); ) {
		if ((*sIt)->query_is_marked_for_erase()) {
			sIt = objStates.erase(sIt);
		}
		else {
			++sIt;
		}
	}
	

}