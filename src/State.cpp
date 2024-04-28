#include "State.h"
int State::sCount;

State::State()
{
	trash = false;
	nextIfTrue = NULL;
	nextIfFalse = NULL;
	sCount++;
}
std::string tabs(int n) {
	stringstream ss;
	for (int i = 0; i < n; ++i) {
		ss << "\t";
	}
	return ss.str();
}
State::State(int t, std::string s)
{
	trash = false;
	nextIfTrue = NULL;
	nextIfFalse = NULL;

	stringstream ss;
	if (s == "wait1") {
		ss << s;
	}
	else {
		ss << s << t;
	}
	sName = ss.str();
	ss.str("");
	ss.clear();
	time = t;
	sCount++;
}

void State::addVertex(Vertex * v)
{
	nodes.push_back(v);
}

std::vector<Vertex*> State::getNodes()
{
	return nodes;
}

int State::getTime()
{
	return time;
}

void State::addNodeVector(std::vector<Vertex*> v)
{
	this->nodes.insert(this->nodes.end(), v.begin(), v.end());//combine their nodes
}

std::vector<string> State::getVerilog()
{
	std::vector<string> vLines;
	std::string s;

	s = tabs(3) + this->sName + ":\n"  +tabs(4)+  "begin\n";
	vLines.push_back(s);

	std::string sT, sF;
	string stemp;
	this->getLines();

	for (std::vector<string>::iterator it = slines.begin(); it != slines.end(); ++it) {
		
		if (!(*it).find("if")) {
			vLines.push_back(tabs(5) + *it + "\n");
		}
		else {
			vLines.push_back(tabs(5) + *it + ";\n");
		}
		
	}
	//if (sName.at(0) == 'C') {
	if (this->nextIfTrue != NULL) {
		sT = tabs(6) + "nextstate <= " + this->nextIfTrue->sName + ";\n";
		vLines.push_back(sT);
	}
	if (this->nextIfFalse != NULL) {
		stemp = this->nextIfFalse->getName();
		//if (stemp.at(0) == 'E') {
		vLines.push_back(tabs(5) + "else\n");
		sF = tabs(6) + "nextstate <= " + this->nextIfFalse->sName + ";\n";
		vLines.push_back(sF);
		//}

	}
	//}
	//else {
	//	if (this->nextIfTrue != NULL) {
	//		s = tabs(5) + "nextstate <= " + this->nextIfTrue->sName + ";\n";
	//		vLines.push_back(s);
	//	}
	//}
	s = tabs(4) + "end\n\n";
	vLines.push_back(s);

	return vLines;
}

//void State::setNext(State * n)
//{
//	nextState = n;
//}

std::vector<string> State::getStrings()
{
	std::vector<string> tmpS;

	for (std::vector<Vertex*>::iterator vIt = nodes.begin(); vIt != nodes.end(); ++vIt) {
		tmpS.push_back((*vIt)->getString() + "\n");
	}
	slines = tmpS;
	return tmpS;
}

void State::setName(std::string s){
	sName = s;
}
std::string State::getName(){
	return sName;
}
void State::setNextIfTrue(State* s){
	nextIfTrue = s;
}
void State::setNextIfFalse(State* s){
	nextIfFalse = s;
}

void State::setNextIfTrue(std::vector<State*> s){
	nextIfTrue = s.front();
}
void State::setNextIfFalse(std::vector<State*> s){
	nextIfFalse = s.front();
}

void State::combineStates(State* s1, State* s2){

	State* newS = NULL;
	State *sT, *sF;
	if (s1->getNextIfTrue() == s2) {
		s1->addNodeVector(s2->getNodes());//move the nodes over

		if (s2->getNextIfTrue() != NULL) {//move the pointers over
			sT = s2->getNextIfTrue();
			s1->setNextIfTrue(sT);
		}
		if (s2->getNextIfFalse() != NULL) {
			sF = s2->getNextIfFalse();
			s1->setNextIfFalse(sF);
		}
		//s2->setNextIfTrue(s1);
		s2->setNextIfFalse(s1);
		s2->markToErase();

	}
}
State* State::getNextIfTrue(){
	return nextIfTrue;
}
void State::clearNext() {

	this->nextIfFalse = NULL;
	this->nextIfTrue = NULL;

}
State* State::getNextIfFalse(){
	return nextIfFalse;
}

std::vector<std::string> State::getLines(){
	string s;
	for (std::vector<Vertex*>::iterator vIt = nodes.begin(); vIt != nodes.end(); ++vIt) {
		s = (*vIt)->getString();
		if (s != "INPUTS" && s != "OUTPUTS") {
			slines.push_back(s);
		}
	}
	return slines;
}
void State::addLine(std::string s){
	slines.push_back(s);
}

void State::printLines() {
	this->getLines();

	for (std::vector<string>::iterator it = slines.begin(); it != slines.end(); ++it) {
		std::cout << *it << std::endl;
	}

}

void State::resetStateCount()
{
	sCount = 0;
}

int State::getStateCount()
{
	return sCount;
}

void State::markToErase()
{
	trash = true;
}

bool State::query_is_marked_for_erase()
{
	return trash;
}

