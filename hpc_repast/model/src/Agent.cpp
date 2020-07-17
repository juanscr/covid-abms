/*Agent.cpp */

#include "Agent.h"

RepastHPCAgent::RepastHPCAgent(repast::AgentId id): id_(id), c(100), total(200), age(18), diseaseStage(SUSCEPTIBLE){}

RepastHPCAgent::RepastHPCAgent(repast::AgentId id, double newC, double newTotal, int newAge,DiseaseStage newDiseaseStage):
id_(id), c(newC), total(newTotal),age(newAge),diseaseStage(newDiseaseStage){}

RepastHPCAgent::~RepastHPCAgent(){ }

/*
Setters
*/

void RepastHPCAgent::set(int currentRank, double newC, double newTotal, int newAge,DiseaseStage newDiseaseStage){
    id_.currentRank(currentRank);
    c     = newC;
    total = newTotal;
    age  = newAge;
    diseaseStage = newDiseaseStage;
}

void RepastHPCAgent::setAtHome(bool new_atHome){
    atHome = new_atHome;
}

void RepastHPCAgent::setWorkShift(Shift new_workShift){
    workShift = new_workShift;
}

bool RepastHPCAgent::cooperate(){
	return repast::Random::instance()->nextDouble() < c/total;
}

void RepastHPCAgent::play(repast::SharedContext<RepastHPCAgent>* context){
    std::set<RepastHPCAgent*> agentsToPlay;
	
//    agentsToPlay.insert(this); // Prohibit playing against self
//    context->selectAgents(3, agentsToPlay, true);
	
    double cPayoff     = 0;
    double totalPayoff = 0;
    std::set<RepastHPCAgent*>::iterator agentToPlay = agentsToPlay.begin();
    while(agentToPlay != agentsToPlay.end()){
        bool iCooperated = cooperate();                          // Do I cooperate?
        double payoff = (iCooperated ?
						 ((*agentToPlay)->cooperate() ?  7 : 1) :     // If I cooperated, did my opponent?
						 ((*agentToPlay)->cooperate() ? 10 : 3));     // If I didn't cooperate, did my opponent?
        if(iCooperated) cPayoff += payoff;
        totalPayoff             += payoff;
		
        agentToPlay++;
    }
    c      += cPayoff;
    total  += totalPayoff;
	
}

void RepastHPCAgent::move(repast::SharedContinuousSpace<RepastHPCAgent, repast::StrictBorders, repast::SimpleAdder<RepastHPCAgent> >* space){

    std::vector<double> agentLoc;
    space->getLocation(id_, agentLoc);
    
    std::vector<double> agentNewLoc;
    double newXcoord;
    double newYcoord;
    //int ntries = 0;
    do{
        newXcoord = agentLoc[0] + 2.0*repast::Random::instance()->nextDouble() - 1;
        newYcoord = agentLoc[1] + 2.0*repast::Random::instance()->nextDouble() - 1;
        agentNewLoc.clear();
        agentNewLoc.push_back(newXcoord);
        agentNewLoc.push_back(newYcoord);
        //if(!space->bounds().contains(agentNewLoc)) std::cout << " INVALID: " << agentNewLoc[0] << "," << agentNewLoc[1] << std::endl;
        
    }while(!space->bounds().contains(agentNewLoc));
    
    space->moveTo(id_, agentNewLoc);
    
}


/* Serializable Agent Package Data */

RepastHPCAgentPackage::RepastHPCAgentPackage(){ }

RepastHPCAgentPackage::RepastHPCAgentPackage(int _id, int _rank, int _type, int _currentRank, double _c, double _total, int _age,
bool _atHome, bool _dayShift, double _wakeUpTime, double _returnToHomeTime,
DiseaseStage _diseaseStage, PatientType _patientType, double _timeToDeath, double _timeToImmune, double _incubationTime, double _incubationShift,
std::vector<double> _homeplace, std::vector<double> _workplace):
id(_id), rank(_rank), type(_type), currentRank(_currentRank), c(_c), total(_total), age(_age), atHome(_atHome),
dayShift(_dayShift), wakeUpTime(_wakeUpTime), returnToHomeTime(_returnToHomeTime),
diseaseStage(_diseaseStage), patientType(_patientType), timeToDeath(_timeToDeath), timeToImmune(_timeToImmune), incubationTime(_incubationTime), incubationShift(_incubationShift),
homeplace(_homeplace), workplace(_workplace) { }
