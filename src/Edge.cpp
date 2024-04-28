#include "Edge.h"


Edge::Edge(std::string strType, std::string strID)
{
	if (strType == eONOP || strType == eINOP || strType == VARIABLE || strType == IF || strType == ELSE) {
		sType = strType;
		ID = strID;

	}
	else {
		std::cout << "Invalid edge type" << std::endl;
		exit(1);
	}
	this->in = NULL;
	this->out = NULL;
}

Edge::Edge(Vertex *v1, Vertex *v2)
{
	this->in = v1;
	this->out = v2;
}

void Edge::setOutput(Vertex * v)
{
	//if (this->sType == NOP || this->sType == VARIABLE) {
		out = v;
		
		return;
	//}
	/*else {
		std::cout << "Invalid Edge Output (NOP)." << std::endl;
	}*/
	
}



Vertex * Edge::getOutput()
{
	return out;
}

void Edge::setInput(Vertex * v)
{
	
//	if (this->sType == INOP || this->sType == VARIABLE) {
		in = v;
		return;
//	}
	//else {
//		std::cout << "Invalid Edge Output (INOP)." << std::endl;
//	}
}

Vertex * Edge::getInput()
{
	return in;
}

void Edge::setID(string s)
{
	ID = s;
}

std::string Edge::getID()
{
	return ID;
}

void Edge::printEdge()
{
}
