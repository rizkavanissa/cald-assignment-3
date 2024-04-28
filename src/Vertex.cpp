#include "Vertex.h"

std::vector<Resource*> Vertex::resources;


Vertex::Vertex() {
	used = false;
	rType = NULL;
	schedule = 0;
	visited = false;
	strNode = "a";
	ALAPtime = 999;
	//ALAPtime = CDFGraph::latency;
}


Vertex::Vertex(int n, Resource* inRType)
{
		//	ALAPtime = Vertex::latency;
		ALAPtime = 999;
		visited = false;
		ID = n;
		//sType = strType;
		rType = inRType;
}

Resource* Vertex::checkValidOp(std::string s)
{
	//std::vector<Resource*> rVec;
	//bool flag = false;
	for (std::vector<Resource*>::iterator it = resources.begin(); it != resources.end(); ++it){
		//if ((*it)->)
		for (std::vector<std::string>::iterator sIt = (*it)->ops.begin(); sIt != (*it)->ops.end(); ++sIt){
			if (*sIt == s) {
				return *it;
			}
			
		}
	}
	return NULL;

}

void Vertex::setString(std::string s)
{
	stringstream ss;

	std::vector<string> tok;
	tok = Parser::splitByWhitespace(s);

	for (std::vector<string>::iterator it = tok.begin(); it != tok.end(); ++it) {
		if (*it == "=") {
			ss << " <= ";
		}
		else {
			ss << *it;
		}
		
	}

	this->strNode = ss.str();
	ss.str("");
	ss.clear();
	return;
}

std::string Vertex::getString()
{
	return strNode;
}

void Vertex::setID(int n)
{
	ID = n;
}

int Vertex::getID()
{
	return ID;
}

void Vertex::setType(Resource * r)
{	
	int nCnt = r->cnt + 1;
	r->cnt = nCnt;
	this->ID = r->cnt;
	rType = r;
}

Resource * Vertex::getType()
{
	return rType;
}

//void Vertex::setType(string s)
//{
//	sType = s;
//}
//
//std::string Vertex::getType()
//{
//	return sType;
//}

//void Vertex::setE1E2(Edge * e1, Edge * e2)
//{
//	E1 = e1;
//	E2 = e2;
//}
//
//void Vertex::setE0(Edge * e0)
//{
//	E0 = e0;
//}
//
//Edge * Vertex::getE0()
//{
//	return E0;
//}

void Vertex::printVertex()
{
		std::string sE0, sE1, sE2, sType;
	
		for (std::vector<Edge*>::iterator it = incoming.begin(); it != incoming.end(); ++it) {
			if((*it)->sType != IF){

			}
		}
}

std::vector<Edge*> Vertex::getOutgoing()
{
	return outgoing;
}

std::vector<Edge*> Vertex::getIncoming()
{
	return incoming;
}

void Vertex::addIncoming(Edge * e)
{
	e->setOutput(this);
	incoming.push_back(e);
}

void Vertex::addOutgoing(Edge * e)
{	
	//if (e->getID() == "t") {
	//	//if (strNode == "OUTPUTS") {
	//		std::cout << "stop";
	//	//}
	//}
	e->setInput(this);

	outgoing.push_back(e);

}
void Vertex::fixOutGoing() {
	if (outgoing.front()->getOutput() == NULL) {
		outgoing.erase(outgoing.begin());
	}
	
}
//void Vertex::setVNumber(int n)
//{
//	vNumber = n;
//}
//
//int Vertex::getVNumber()
//{
//	return vNumber;
//}

void Vertex::visit()
{
	this->visited = true;
}

void Vertex::resetVisit()
{
	this->visited = false;
}

bool Vertex::checkVisited()
{
	return visited;
}

//void Vertex::setLatency(int n)
//{
//	latency = n;
//}

int Vertex::getALAPTime()
{
	return ALAPtime;
}

void Vertex::setALAPTime(int n)
{
	ALAPtime = n;
}


void Vertex::scheduleNode(int t)
{
	schedule = t;
}

int Vertex::query_Schedule()
{
	return schedule;
}

bool Vertex::operator==(const Vertex &other) const {
	  // Compare the values, and return a bool result.
	if (strNode == other.strNode) {
		if (ID == other.ID) {
			return true;
		}
	}
	return false;
}
//bool Vertex::operator!=(const Vertex &other) const {
//	// Compare the values, and return a bool result.
//	return  !(*this == other);
//}

//bool Vertex::operator < (const Vertex &other) const {
//	// Compare the values, and return a bool result.
//	return  (ALAPtime < other.ALAPtime);
//}
//
//bool Vertex::operator > (const Vertex &other) const {
//	// Compare the values, and return a bool result.
//	return  (ALAPtime > other.ALAPtime);
//}