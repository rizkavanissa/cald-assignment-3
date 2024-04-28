#include "CDFGraph.h"

// Create a node for each resource in the CDF graph
Resource CDFGraph::ADDER_SUBTRACTOR;
Resource CDFGraph::MULTIPLIER;
Resource CDFGraph::LOGICAL;
Resource CDFGraph::IF_STATEMENT;

// Define the initial state of the Control/Data Flow Graph Object
CDFGraph::CDFGraph()
{
  
	INOP.name = "INOP";
	ONOP.name = "ONOP";

// ██████╗ ███████╗███████╗ ██████╗ ██╗   ██╗██████╗  ██████╗███████╗    ██████╗ ███████╗██╗      █████╗ ██╗   ██╗
// ██╔══██╗██╔════╝██╔════╝██╔═══██╗██║   ██║██╔══██╗██╔════╝██╔════╝    ██╔══██╗██╔════╝██║     ██╔══██╗╚██╗ ██╔╝
// ██████╔╝█████╗  ███████╗██║   ██║██║   ██║██████╔╝██║     █████╗      ██║  ██║█████╗  ██║     ███████║ ╚████╔╝ 
// ██╔══██╗██╔══╝  ╚════██║██║   ██║██║   ██║██╔══██╗██║     ██╔══╝      ██║  ██║██╔══╝  ██║     ██╔══██║  ╚██╔╝  
// ██║  ██║███████╗███████║╚██████╔╝╚██████╔╝██║  ██║╚██████╗███████╗    ██████╔╝███████╗███████╗██║  ██║   ██║   
// ╚═╝  ╚═╝╚══════╝╚══════╝ ╚═════╝  ╚═════╝ ╚═╝  ╚═╝ ╚═════╝╚══════╝    ╚═════╝ ╚══════╝╚══════╝╚═╝  ╚═╝   ╚═╝   

	// For ECE 474a, the resource for divider/modulo is not included below.

	MULTIPLIER.name = "Mult";				// Name of the resource
	MULTIPLIER.delay = 2;					// Cycle delay
	MULTIPLIER.cnt = 0;						// Resource count
	MULTIPLIER.inUse = 0;					// Number of this resource in use
	MULTIPLIER.ops.push_back("*"); 			// Push back this particular operation into the queue
	rVec.push_back(&MULTIPLIER);

	ADDER_SUBTRACTOR.name = "Add/Sub";		// Name of the resource
	ADDER_SUBTRACTOR.delay = 1;				// Cycle delay
	ADDER_SUBTRACTOR.cnt = 0;				// Resource count
	ADDER_SUBTRACTOR.inUse = 0;				// Number of this resource in use
	ADDER_SUBTRACTOR.ops.push_back("+");	// Push back an addition operation into the queue
	ADDER_SUBTRACTOR.ops.push_back("-");	// Push back an subtraction operation into the queue
	rVec.push_back(&ADDER_SUBTRACTOR);

	LOGICAL.name = "Logical";				// Name of the resource
	LOGICAL.delay = 1;						// Cycle delay
	LOGICAL.cnt = 0;						// Resource count
	LOGICAL.inUse = 0;						// Number of this resource in use
	LOGICAL.ops.push_back(GT);				// Push back a comparison operation of Greater Than into the queue
	LOGICAL.ops.push_back(LT);				// Push back a comparison operation of Less Than into the queue
	LOGICAL.ops.push_back(ET);				// Push back a comparison operation of Equal To into the queue
	LOGICAL.ops.push_back(SL);				// Push back a comparison operation of Shift Left into the queue
	LOGICAL.ops.push_back(SR);				// Push back a comparison operation of Shift Right into the queue
	LOGICAL.ops.push_back(MUX);				// Push back a comparison operation of Multiplexer into the queue
	rVec.push_back(&LOGICAL);

	IF_STATEMENT.name = "If/Else";			// Name of the resource (this is for the ternary operation)
	IF_STATEMENT.delay = 1;					// Cycle delay
	IF_STATEMENT.cnt = 0;					// Resource count
	IF_STATEMENT.ops.push_back(IF);			// Push back an IF operation into the queue
	IF_STATEMENT.ops.push_back(ELSE);		// Push back an ELSE operation into the queue
	rVec.push_back(&IF_STATEMENT);

	Vertex::resources = rVec;

	vONOP = new Vertex(0, &ONOP);			// Output no operation
	vINOP = new Vertex(0, &INOP);			// Input no operation
	vONOP->setType(&LOGICAL);
	vINOP->setType(&LOGICAL);
	vONOP->setString("OUTPUTS");
	vINOP->setString("INPUTS");
	vINOP->scheduleNode(0);
	//AddSubCnt = 0;
	//MultCnt = 0;
	//LogicCnt = 0;
	//IfCnt = 0;
	//cFlag = false;
	currBlk = new Block;
	currBlk->addVertex(vINOP);
	gControlGraph.addBlock(currBlk);
	//StartBlock = currBlk;
	_last = FUNCTION;
}

void CDFGraph::setLatency(int n)
{
	if (n <= 0) {
		std::cout << "Negative or zero latency" << std::endl;
		exit(1);
	}
	latency = n;
}

int CDFGraph::getLatency()
{
	return latency;
}

// Load the clean vector string from the input file into the CDFG graph
void CDFGraph::loadFileStrings(std::vector<string> strVec)
{
	this->FileStrings = strVec;
	//std::cout << "Raw Strings Loaded." << std::endl;
}

// Load the extracted inputs, outputs, and variables from the vector string of input file into the CDFG graph
void CDFGraph::loadIOV(std::vector<IOV> ins, std::vector<IOV> outs, std::vector<IOV> vars)
{
	this->inputs = ins;
	this->outputs = outs;
	this->variables = vars;
	//std::cout << "IOVs Loaded." << std::endl;
}

//
IOV* CDFGraph::getIOVbyName(std::string s)
{
	for (std::vector<IOV>::iterator it = inputs.begin(); it != inputs.end(); ++it) {
		if (it->getName() == s) {
			return &(*it);
		}
	}
	for (std::vector<IOV>::iterator it = outputs.begin(); it != outputs.end(); ++it) {
		if (it->getName() == s) {
			return &(*it);
		}
	}
	for (std::vector<IOV>::iterator it = variables.begin(); it != variables.end(); ++it) {
		if (it->getName() == s) {
			return &(*it);
		}
	}
	return NULL;

}

// Parse the operations within the vector string of input file to the CDFG graph 
void CDFGraph::parseOperations()
{
	Vertex* tempV;
	Conditional* newC;
	Block* newBlk;
	Block* tempF = NULL;
	//Block* lastBlock;
	bool opsFound = false;

	if (this->FileStrings.size() == 0) {
		std::cout << "No File Strings Loaded to Graph" << std::endl;
		exit(1);
	}
	bool first = true;
	//std::size_t found;
	for (std::vector<string>::iterator it = FileStrings.begin(); it != FileStrings.end(); ++it) {
		
		if (it->find(IF) != std::string::npos) {
			newC = new Conditional(*it);
			tempV = parseConditional(*it);
			Vertices.push_back(tempV);
			newC->setVCondition(tempV);

			if (_last == FUNCTION) {
				currBlk->setNext(newC);
			}
			else if (_last == CONDITIONAL) {
				currC->setNextIfTrue(newC);
			}
			else {
				std::cout << "Something went wrong (Parsing conditionals)" << std::endl;
			}
			CondVec.push_back(newC);//pushing conditional to the end of this vector because we need to know the order they occured in
			currC = newC;
			gControlGraph.addConditional(currC);
			if (first == true){
				gControlGraph.setStart(currC);
			}
			
			
			_last = CONDITIONAL;
		}
		else if (it->find(ELSE) != std::string::npos) {
		
			tempF = currBlk->convertToElse(currC);
			//currBlk->setToElse();
		}
		else if (it->find("}") != std::string::npos) {
			if (opsFound) {
				
					newBlk = new Block();
					currBlk->setNext(newBlk);
					newBlk->setPrev(currBlk);
			//		lastBlock = currBlk;
					currBlk = newBlk;
					gControlGraph.addBlock(currBlk);
				
				
				if (tempF != NULL) {
					tempF->setNext(newBlk);
					tempF = NULL;
				}
				else {
					CondVec.back()->setNextIfFalse(currBlk);
					CondVec.pop_back();
				}
				opsFound = false;
			}
			
			_last = FUNCTION;

		}
		else {
			if (_last == CONDITIONAL) {
				
					currBlk = new Block();
					currBlk->setPrev(currC);
					currBlk->setToIf();
					currC->setNextIfTrue(currBlk);
					gControlGraph.addBlock(currBlk);
					_last = FUNCTION;
				
			}
			parseOperation(*it);
			opsFound = true;
		}
		first = false;
	}
	//if (currBlk->getNodes().size() == 0) {
	//	//currBlk = lastBlock;
	//	//gControlGraph.getBlocks().erase(gControlGraph.getBlocks().end());
	//currBlk = gControlGraph.dropLast();
	//}
	//currBlk = new Block;
		
	currBlk->addVertex(vONOP);
	//currC->setNextIfFalse(currBlk);
	CDFGraph::addConditionalVertices();
	for (std::vector<Edge*>::iterator it = Edges.begin(); it != Edges.end(); ++it) {
		if ((*it)->getOutput() == NULL){
			vONOP->addIncoming(*it);
		}
	}


	//currBlk->clearNext();
}

void CDFGraph::addConditionalVertices() {
	std::vector<Conditional*> cndVec = gControlGraph.getConditionals();
	if (cndVec.size() == 0) {
		//std::cout << "Block vector Empty " << std::endl;
		return;
		//exit(1);
	}
	//int cnt = 0;

//	Block* currBlk;
//	Conditional* currCnd;
//	Edge* newE;
//	Vertex* currV;
	std::string s;
	std::vector<Edge*> eVec;
	std::vector<Block*> bVec;
	std::vector<Vertex*> vVec;
	bVec = gControlGraph.getBlocks();

	//Block* tempBlock;

	for (std::vector<Conditional*>::iterator it = cndVec.begin(); it != cndVec.end(); ++it) {

		eVec = (*it)->connectVCnd();
		
		Edges.insert(Edges.end(), eVec.begin(), eVec.end());
		
	//	cnt = 0;
	}
	if (cndVec.back()->getVCondition()->getOutgoing().front() == NULL) {
	//	cnt = 0;
	}
	//for (std::vector<Block*>::iterator bIt = bVec.begin(); bIt != bVec.end(); ++bIt) {
	//		tempBlock = (*bIt)->getNextBlock();
	//		if(tempBlock != NULL){
	//			if (tempBlock->getNodes().size() == 0) {
	//				(*bIt)->clearNext();
	//				(*bIt)->setNext(tempBlock);
	//				//Memory leaks all over the place with this sucker. over it.
	//			}
	//	}

	//}
}
//std::vector<Vertex*> CDFGraph::getNodesByOutgoingEdgeID(string s)
std::vector<Vertex*> CDFGraph::getVerticesByEdgeID(string s)
{
	//std::cout << s << std::endl;


	//gonna brute force this one because this is taking forever.
	std::vector<Vertex*> vVec;
	bool match = false;
	Vertex* vTMP;
	//std::vector<Edge*> eVec1;
	//std::vector<Edge*> eVec2;

	for (std::vector<Edge*>::iterator eIt1 = Edges.begin(); eIt1 != Edges.end(); ++eIt1) {
		
		if ((*eIt1)->getID() == s) {//get all the edges with the same name
			vTMP = (*eIt1)->getInput();
			match = false;
			if (!vVec.empty() && vTMP != NULL) {
				if (std::find(vVec.begin(), vVec.end(), vTMP) != vVec.end()) {
					match = true;
				}
				//for (std::vector<Vertex*>::iterator vIt = vVec.begin(); vIt != vVec.end(); ++vIt) {//Loop through current list of vertices which output those edges

				//	if ((*vIt)->getString() == vTMP->getString() && (*vIt)->getID() <= vTMP->getID()) {//if this vertex string doesn't match any in the vector
				//		match = true;
				//	}
				//	
				//}
			}
		/*	if (currBlk->getConverse() != NULL) {
				Block* cvBlk = currBlk->getConverse();
				if (!cvBlk->checkForVertex(vTMP)) {
					vVec.push_back(vTMP);
				}
			}*/
			if (match == false) {
				if (currBlk->getConverse() != NULL) {
					Block* cvBlk = currBlk->getConverse();
					if (!cvBlk->checkForVertex(vTMP)) {
						vVec.push_back(vTMP);
					}
				}
				else {
					vVec.push_back(vTMP);
				}
			}
		}
	}
	

	return vVec;
}

std::vector<Vertex*> CDFGraph::getVertices()
{
	return Vertices;
}

Vertex* CDFGraph::parseConditional(string s) {
	Vertex* newV;
//	int nID;
	std::vector<string> tok;
	tok = Parser::splitByWhitespace(s);
	//IfCnt++;
	//IF_STATEMENT.cnt++;
	//nID = IF_STATEMENT.cnt;
	newV = new Vertex();
	newV->setType(&IF_STATEMENT);
	std::string sName;
	sName = tok.at(0) + " " + tok.at(1) + " " + tok.at(2) + " " + tok.at(3);
	newV->setString(sName);

	CDFGraph::parseInput(tok.at(2), newV);
	CDFGraph::parseOutput(tok.at(2), newV);
	
	return newV;
}
void CDFGraph::parseOperation(string s) {

	std::vector<string> tok;

	Vertex* newV;
	//std::cout << s << std::endl;
	tok = Parser::splitByWhitespace(s);
	if (tok.size() == 5) {
	//	std::cout << tok.at(0) << '\t' << tok.at(1) << '\t' << tok.at(2) << '\t' << tok.at(3) << '\t' << tok.at(4) << std::endl;

		Resource* tp = Vertex::checkValidOp(tok.at(3));
		if (tp != NULL) {

			newV = new Vertex();
			newV->setType(tp);
			newV->setString(s);

		}

		CDFGraph::parseOutput(tok.at(0), newV);
		CDFGraph::parseInput(tok.at(2), newV);
		CDFGraph::parseInput(tok.at(4), newV);
		
	}
	else if (tok.size() == 7) {
		if (tok.at(3) == MUX) {
			newV = new Vertex();
			newV->setType(&LOGICAL);
			newV->setString(s);
			CDFGraph::parseOutput(tok.at(0), newV);
			CDFGraph::parseInput(tok.at(2), newV);
			CDFGraph::parseInput(tok.at(4), newV);
			CDFGraph::parseInput(tok.at(6), newV);
		}
	}
	
	Vertices.push_back(newV);
	currBlk->addVertex(newV);
}
void CDFGraph::parseInput(string s, Vertex* newV) {

	Edge* newEdge;
	std::vector<Vertex*> vVec;
//	Edge* eTMP;
	if (!checkInputorVariable(s)) {
		std::cout << "Invalid input to operation: " << s << " Not declared: " << newV->getString() << std::endl;
		exit(1);
	}

	if (getIOVbyName(s)->getType() == INPUT) {
		newEdge = new Edge(eINOP, s);

		vINOP->addOutgoing(newEdge);
		newV->addIncoming(newEdge);
		this->Edges.push_back(newEdge);
		this->currBlk->addEdge(newEdge);
	}
	else if (getIOVbyName(s)->getType() == VARIABLE) {

		vVec = CDFGraph::getVerticesByEdgeID(s);
		if (vVec.size() == 0) {
			newEdge = new Edge(VARIABLE, s);
			this->Edges.push_back(newEdge);
			this->currBlk->addEdge(newEdge);
		}
		else {
			for (std::vector<Vertex*>::iterator it = vVec.begin(); it != vVec.end(); ++it) {
				

					if ((*it)->getOutgoing().front()->getOutput() != NULL) {
						newEdge = new Edge(VARIABLE, s);
						(*it)->addOutgoing(newEdge);
					}
					else {
						newEdge = (*it)->getOutgoing().front();
					}

					newV->addIncoming(newEdge);
					this->Edges.push_back(newEdge);
					this->currBlk->addEdge(newEdge);
				
			}
		}
	}

}


void CDFGraph::parseOutput(string s, Vertex* newV) {

	Edge* newEdge;
//	Edge* eTMP;
	/***********************output (0)****************************/
	if (!checkOutputorVariable(s)) {
		std::cout << "Invalid output from operation: " << s << " Not declared: " << newV->getString() << std::endl;
		exit(1);
	}
	if (getIOVbyName(s)->getType() == OUTPUT) {
		newEdge = new Edge(eONOP, s);
		//newEdge->setInput(newV);
		vONOP->addIncoming(newEdge);
		newV->addOutgoing(newEdge);
		this->Edges.push_back(newEdge);
		this->currBlk->addEdge(newEdge);
	}
	if (getIOVbyName(s)->getType() == INPUT) {
		if (newV->getType() == &IF_STATEMENT) {
			newEdge = new Edge(eINOP, s);
			//vINOP->addOutgoing(newEdge);
			newV->addOutgoing(newEdge);
			this->Edges.push_back(newEdge);
			this->currBlk->addEdge(newEdge);
		}
	}
	else if (getIOVbyName(s)->getType() == VARIABLE) {
		newEdge = new Edge(VARIABLE, s);
		//newEdge->setInput(newV);
		newV->addOutgoing(newEdge);
		this->Edges.push_back(newEdge);
		this->currBlk->addEdge(newEdge);
	}

}
//void CDFGraph::parseIF(string s, Conditional *c) {
//	std::vector<string> tok;
//	Edge* newE;
//	//Vertex* newV;
//
//	tok = Parser::splitByWhitespace(s);
//	if (tok.size() == 5) {
//		std::cout << tok.at(0) << '\t' << tok.at(1) << '\t' << tok.at(2) << '\t' << tok.at(3) << '\t' << tok.at(4) << std::endl;
//		
//	}
//
//
//}
void CDFGraph::printGraph() {
	//std::cout << "Printing graph:" << std::endl;
//	std::cout << "Contents of Vertex Vector: " << std::endl << std::endl;

	//Vertex* vcurr;
	//Vertex* vnext;
	//Edge* eCurr;
	//vcurr = this->vONOP;

}

void CDFGraph::DFS(CDFGraph * g, Vertex * v)
{	
	Vertex* nxt;
	std::vector<Edge*> eVec = v->getOutgoing();
	v->visit();
	for (std::vector<Edge*>::iterator it = eVec.begin(); it != eVec.end(); ++it) {
		nxt = (*it)->getInput();
		if (nxt->checkVisited() == false) {
			DFS(g, nxt);
		}
	}
}

void CDFGraph::ALAP(CDFGraph * g, Vertex * v, int time)
{
	Vertex* nxt;

	if (time < -1) {
		std::cout << "Latency constraint insufficient to schedule all operations." << std::endl;
		exit(1);
	}
	int delay = v->getType()->delay;

	std::vector<Edge*> eVec = v->getIncoming();

	for (std::vector<Edge*>::iterator it = eVec.begin(); it != eVec.end(); ++it) {
		
		nxt = (*it)->getInput();

			if (nxt != NULL) {

				if (nxt->getALAPTime() > time) {
					nxt->setALAPTime(time);
					ALAP(g, nxt, time - delay);
				}

		}
	}

}
void CDFGraph::ALAP(CDFGraph * g, int n) {
	g->ALAP(n);
}
void CDFGraph::ALAP(int n)
{
	//Vertex::setLatency(n);
	//Vertex::latency = n + 1;
	//for (std::vector<Vertex*>::iterator it = Vertices.begin(); it != Vertices.end(); ++it) {
	//	(*it)->setALAPTime(Vertex::latency);
	//}
	ALAP(this, vONOP, n); 
}
void CDFGraph::LIST_R(CDFGraph * g, Vertex * v) {
	//g->LIST_R()
}
bool sortbyALAP(Vertex *lhs, Vertex *rhs)
{
	bool val;
	val = false;

	if (lhs->getALAPTime() < rhs->getALAPTime()) {
		val = true;
	}

		return val;

}
bool sortbySchedule(Vertex *lhs, Vertex *rhs)
{
	bool val;
	val = false;

	if (lhs->query_Schedule() < rhs->query_Schedule()) {
		val = true;
	}

	return val;

}
bool stateSorter(State *lhs, State *rhs)
{
	bool val;
	val = false;

	if (lhs->getTime() < rhs->getTime()) {
		val = true;
	}

	return val;

}
void CDFGraph::LIST_R(int n) {
	latency = n;

	vONOP->setALAPTime(n + 1);
	vONOP->scheduleNode(n + 1);
	ALAP(n);
	
	//CDFGraph::ALAP(this, n);
	//LIST_R(this, )
	int t = 1;
//	Vertex* minALAP;
//	int min = 999;
	std::vector<Vertex*> allV = Vertices;//vector full of all of the nodes in the graph, which at this point are as yet unscheduled.
	std::vector<Vertex*> vSchedule;//vector of nodes which have been scheduled?
//	bool found = false;
//	bool schFlag = false;



	do { // while (allV.size() > 0);
		std::sort(allV.begin(), allV.end(), sortbyALAP);//sorts by ALAP time small->large
		bool schFlag = false;
		CDFGraph::resetRCounts();
		//Each loop here is for a different time slot, so start by setting the count of each resource to zero

		for (std::vector<Resource*>::iterator rIt = rVec.begin(); rIt != rVec.end(); ++rIt) {
			//for each type of resource 

			for (std::vector<Vertex*>::iterator vIt1 = allV.begin(); vIt1 != allV.end();) {
				//loop through all the nodes we have
				if ((*vIt1)->getType() == *rIt) {
					//if this node is the same type as the one we're looking at
					if ((*vIt1)->getALAPTime() - t == 0) {
						//if its slack time is zero
						(*vIt1)->scheduleNode(t);
						(*rIt)->cnt++;
						vIt1 = allV.erase(vIt1);
						schFlag = true;
					}
					else if ((*rIt)->cnt < (*rIt)->inUse) {
						//if its slack time is greater than zero but we have resources still available
						//if we still have resources available
						(*vIt1)->scheduleNode(t);
						(*rIt)->cnt++;
						vIt1 = allV.erase(vIt1);
						schFlag = true;
					}
					else if ((*rIt)->cnt > (*rIt)->inUse) {
						(*rIt)->inUse = (*rIt)->cnt;
					}
				}
				if (schFlag == false) {
					++vIt1;
				}
				schFlag = false;
			}
		}
		t++;
	} while (allV.size() > 0);

	//maxTime = t;
}


void CDFGraph::resetRCounts() {
	for (std::vector<Resource*>::iterator rIt = rVec.begin(); rIt != rVec.end(); ++rIt) {
		(*rIt)->cnt = 0;
	}
}
bool CDFGraph::checkInputorVariable(std::string s) {
//	IOV* tIOV = getIOVbyName(s);
	for (std::vector<IOV>::iterator it = inputs.begin(); it != inputs.end(); ++it) {
		if (it->getName() == s) {
			return true;
		}
	}
	for (std::vector<IOV>::iterator it = variables.begin(); it != variables.end(); ++it) {
		if (it->getName() == s) {
			return true;
		}
	}
	return false;

}

bool CDFGraph::checkOutputorVariable(std::string s) {
//	IOV* tIOV = getIOVbyName(s);
	for (std::vector<IOV>::iterator it = inputs.begin(); it != inputs.end(); ++it) {
		if (it->getName() == s) {
			return true;
		}
	}
	for (std::vector<IOV>::iterator it = outputs.begin(); it != outputs.end(); ++it) {
		if (it->getName() == s) {
			return true;
		}
	}
	for (std::vector<IOV>::iterator it = variables.begin(); it != variables.end(); ++it) {
		if (it->getName() == s) {
			return true;
		}
	}
	return false;

}
int CDFGraph::calculateStates(int n) {
	
	//std::vector<int> nums;
	//int ii;
	//int min = 999;
	//int max = -1;

	/*for (std::vector<Vertex*>::iterator it = Vertices.begin(); it != Vertices.end(); ++it) {
		ii = (*it)->query_Schedule();
		if (!(std::find(nums.begin(), nums.end(), ii) != nums.end()) ) {
			nums.push_back(ii);
		}
		if (ii < min) min = ii;
		if (ii > max) max = ii;
	}*/

	int stateRegWidth = ceil(log2(n));
	//maxTime = max;
	//minTime = min;
	return stateRegWidth;
}

void CDFGraph::generateVerilogFile(char* outFileStr) {

	std::vector<string> outLines;
	std::string sLine;
	std::stringstream ss;
	
	std::sort(Vertices.begin(), Vertices.end(), sortbySchedule);
	//for (std::vector<Vertex*>::iterator it = Vertices.begin(); it != Vertices.end(); ++it) {
	//	std::cout << left << "Node: [" << (*it)->getString() << "]\t\t" << right << "ALAP Time:" << (*it)->getALAPTime() << "\tScheduled Time: " << (*it)->query_Schedule() << std::endl;
	//}

	std::ofstream outFile(outFileStr, std::ofstream::out);

	if (outFile.is_open() && outFile.good()) {
		//std::cout << "File Opened!" << std::endl;
	}
	else {
		std::cout << "Unable to open output file." << std::endl;
		//return std::vector<std::string>();
		exit(1);
	}

//	int bw = -1;

	std::string tp = "";
	std::string nm = "";
	std::string argStr;

	//bool sgn;
	//sgn = false;
	//string debugs;
	ss << "Clk, Rst, Start, Done, ";

	for (std::vector<IOV>::iterator it = inputs.begin(); it != inputs.end(); ++it) {
		tp = it->getType();
		nm = it->getName();

			ss << nm << ", ";
			//if (it + 1 != pins.end())ss << ", ";
	
	}
	for (std::vector<IOV>::iterator it = outputs.begin(); it != outputs.end(); ++it) {
		tp = it->getType();
		nm = it->getName();

			ss << nm << ", ";
			//if (it + 1 != pins.end())ss << ", ";
		
	}


	//State* currS;


	argStr = ss.str();//argstr is the list of inputs in the function signature
	ss.clear();
	ss.str("");
	argStr = argStr.substr(0, argStr.length() - 2); //get rid of extra comma
	outFile << "`timescale 1ns / 1ps" << std::endl;

	outFile << "module HLSM (" << argStr << ");" << std::endl << std::endl;

	for (std::vector<IOV>::iterator it = inputs.begin(); it != inputs.end(); ++it) {//declaring inputs etc
		outFile << it->getOutputLine() << std::endl;
	}

	outFile << "input Clk, Rst, Start;" << std::endl;
	outFile << "output Done;\n";

	for (std::vector<IOV>::iterator it = outputs.begin(); it != outputs.end(); ++it) {//declaring inputs etc
		outFile << it->getOutputLine() << std::endl;
	}

	for (std::vector<IOV>::iterator it = variables.begin(); it != variables.end(); ++it) {//declaring inputs etc
		outFile << it->getOutputLine() << std::endl;
	}
	//outFile << "input start;" << std::endl;
	//Declare state reg:
	//outFile << "output state;\n";
	stringstream ugh;
	string statestring;
	////////////////////////////////////////////////////////////////ports declared!!

	string debugLine;
	std::vector<State*> allStates;
	allStates = gControlGraph.callGS();
	//std::sort(allStates.begin(), allStates.end(), stateSorter);

	int stateRegWidth = calculateStates(allStates.size());

	ugh << "reg [" << (stateRegWidth - 1) << ":0] state, nextstate;\n";

	statestring = ugh.str();// + std::endl;
	outFile << statestring;
	ugh.clear();
	ugh.str("");

	//int time = 1;
//	int currI;
	//bool flag = true;


	stringstream params;
	std::string tabs = "\t\t\t\t\t";

	params << "parameter ";// << statestring;
	int count = 1;

	std::vector<State*>::iterator test;
	//allStates.erase(allStates.begin());
	
	allStates.pop_back();//to prevent generating a second "Wait" state. 
	for (std::vector<State*>::iterator it = allStates.begin(); it != allStates.end(); ++it) {
		
			if ((it) != allStates.end() && (it + 1) == allStates.end()) {
		
				params << (*it)->getName() <<  " = " << count << ";\n";
				count++;
			}
			else {
			//	
				params << (*it)->getName() << " = " << count << ", ";
				count++;
				
			}
	}
	outFile << params.str();
	params.str("");
	params.clear();

	outFile <<  std::endl << "always @(state) begin" << std::endl;
	//outFile << "\tbegin" << std::endl;
	//outFile << "\t\t if (rst)" << std::endl << "\t\t\tstate = 1;" << std::endl;
	//outFile << "\t\telse" << std::endl;
	outFile << "\tcase (state)" << std::endl;

	//outFile << "\t\t\tWait: begin\t\t\n\t\t\t\tif( start == 1 )\n\t\t\t\t\tnextstate <= " << allStates.front()->getName() << ";\n";
	//outFile << "\t\t\t\telse\n\t\t\t\t\tnextstate <= Wait;\n\t\t\tend\n";
	std::vector<string> vStrings;
	//State* newS = new State(allStates.size() + 1, "Wait");
	//allStates.back()->setNextIfTrue(newS);
	std::vector<State*>::iterator it1;// , it2;

	it1 = allStates.begin();
	//it2 = it1 + 1;


	do {
		
		vStrings = (*it1)->getVerilog();

		for (std::vector<string>::iterator it = vStrings.begin(); it != vStrings.end(); ++it) {
			//std::cout << *it << std::endl;
			outFile << *it;
		}
		it1++;

	} while (it1 < allStates.end());
	
	//for (std::vector<State*>::iterator it = allStates.begin(); it != allStates.end(); ++it) {

	//	if ((*it)->printed == false) {
	//		vStrings = (*it)->getVerilog();
	//		for (std::vector<string>::iterator it = vStrings.begin(); it != vStrings.end(); ++it) {
	//			//	std::cout << *it;
	//			outFile << *it;
	//		}
	//		(*it)->printed = true;
	//	}
	//}


	//ugh << "\t\t\t\t" << 1 << ": begin";
	//outFile << ugh.str() << std::endl;
	ugh.clear();
	ugh.str("");
	//currI = minTime;
	
	//outFile << "\t\t\t\tend" << std::endl;
	outFile << "\t\t\tendcase" << std::endl;
	outFile << "\t\tend\n" << std::endl;
	//outFile << "\tend" << std::endl;

	outFile << "always @(posedge Clk) begin\n";
	outFile << "\tif (Rst == 1)\n";
	outFile << "\t\tstate <= Wait;\n";
	outFile << "\telse\n";
	outFile << "\t\tstate <= nextstate;\n";
	outFile << "\tend\n";
	outFile << "endmodule" << std::endl;
	outFile.close();

	

	
}


void CDFGraph::resetVertexVisits() {
	for (std::vector<Vertex*>::iterator currV = Vertices.begin(); currV != Vertices.end(); ++currV) {
		(*currV)->resetVisit();
	}
}
