#include "Conditional.h"

Conditional::Conditional() {
	found = false;
blkPrev = NULL;
st = NULL;
blkNextIfTrue = NULL;
blkNextIfFalse = NULL;
cndNextIfTrue = NULL;
cndNextIfFalse = NULL;
}
Conditional::Conditional(std::string Arg) {
	found = false;
	st = NULL;
	blkPrev = NULL;
	blkNextIfTrue = NULL;
	blkNextIfFalse = NULL;
	cndNextIfTrue = NULL;
	cndNextIfFalse = NULL;

	std::vector<string> tok;
	tok = Parser::splitByWhitespace(Arg);
	sArg = tok.at(2);
}
Conditional::Conditional(std::string Arg, Block *inPrev)
{
	found = false;
	blkPrev = NULL;
	st = NULL;
	blkNextIfTrue = NULL;
	blkNextIfFalse = NULL;
	cndNextIfTrue = NULL;
	cndNextIfFalse = NULL;

	sArg = Arg;
	blkPrev = inPrev;

}

void Conditional::setNextIfTrue(Block *b)
{
	if (cndNextIfTrue != NULL) {
		std::cout << "warning: Conditional already has both function and condition assigned to next." << std::endl;
	}
	b->setPrev(this);
	blkNextIfTrue = b;
}

void Conditional::setNextIfTrue(Conditional *c)
{
	if (blkNextIfTrue != NULL) {
		std::cout << "warning: Conditional already has both function and condition assigned to next." << std::endl;
	}
	c->setPrev(this);
	cndNextIfTrue = c;
}

void Conditional::setNextIfFalse(Block * b)
{
//	f->setCParent(this);
	if (cndNextIfFalse != NULL) {
		std::cout << "warning: Conditional already has both block and condition assigned to next." << std::endl;
	}
	b->setPrev(this);
	blkNextIfFalse = b;
}

void Conditional::setNextIfFalse(Conditional * c)
{
	if (blkNextIfFalse != NULL) {
		std::cout << "warning: Conditional already has both block and condition assigned to next." << std::endl;
	}
	c->setPrev(this);
	cndNextIfFalse = c;
}

Block * Conditional::getNextBlk_True()
{
	return blkNextIfTrue;
}

Block * Conditional::getNextBlk_False()
{
	return blkNextIfFalse;
}

Conditional * Conditional::getNextCondition_True()
{
	return cndNextIfTrue;
}

Conditional * Conditional::getNextCondition_False()
{
	return cndNextIfFalse;
}

void Conditional::setVCondition(Vertex * v)
{
	vCondition = v;
}



Vertex * Conditional::getVCondition()
{
	return vCondition;
}

std::string Conditional::getSCondition()
{
	return sArg;
}

std::vector<Edge*> Conditional::connectVCnd(){

	std::vector<Vertex*> vVec;
	std::vector<Edge*> eVec;
	Edge* newE;
	//for (std::vector<Vertex*>::iterator it = )
	if (blkNextIfTrue != NULL){
		vVec = blkNextIfTrue->getNodes();
		for (std::vector<Vertex*>::iterator it = vVec.begin(); it != vVec.end(); ++it) {
			newE = new Edge(VARIABLE, this->sArg);
			this->vCondition->addOutgoing(newE);
			(*it)->addIncoming(newE);
			
			eVec.push_back(newE);
		}
	}
	else if (cndNextIfTrue != NULL){
		newE = new Edge(VARIABLE, this->sArg);
		cndNextIfTrue->getVCondition()->addIncoming(newE);
		vCondition->addOutgoing(newE);
		eVec.push_back(newE);
	}
	
	/////////////////////////////////////////////
	if (blkNextIfFalse != NULL && blkNextIfFalse->query_IsElse() == true) {
		vVec = blkNextIfFalse->getNodes();
		for (std::vector<Vertex*>::iterator it = vVec.begin(); it != vVec.end(); ++it) {
			newE = new Edge(VARIABLE, this->sArg);
			this->vCondition->addOutgoing(newE);
			(*it)->addIncoming(newE);
			eVec.push_back(newE);
		}
	}
	
	return eVec;
}

State* Conditional::getConditionalState(){
	State* newS;
	int time;
	if (st == NULL) {
		time = vCondition->query_Schedule();

		newS = new State(time, "C_");
		newS->addVertex(vCondition);
		st = newS;
	}
	
	//std::cout << newS->getName() << "<<***>>" << std::endl;
	//std::cout << "\t\t" << vCondition->getString() << std::endl;
	found = true;
	return st;
}
bool Conditional::checkFound()
{
	return found;
}
void Conditional::setPrev(Block* b){
	this->blkPrev = b;
}
void Conditional::setPrev(Conditional* c){
	this->cndPrev = c;
}

