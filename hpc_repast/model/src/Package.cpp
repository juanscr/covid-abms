#include "Package.h"

AgentPackageProvider::AgentPackageProvider(repast::SharedContext<RepastHPCAgent>* agentPtr): agents(agentPtr){}

void AgentPackageProvider::providePackage(RepastHPCAgent * agent, std::vector<AgentPackage>& out){
    repast::AgentId id = agent->getId();
    AgentPackage package(id.id(), id.startingRank(), id.agentType(), id.currentRank(), agent->getProcessWork(), agent->getProcessHome(),
	agent->getAge(), agent->getFamily(), agent->getStratum(),
	agent->getAtHome(), agent->getWorkShit(), agent->getWakeUpTime(), agent->getReturnToHomeTime(), agent->getSleepStart(), agent->getSleepEnd(),
	agent->getDiseaseStage(), agent->getPatientType(), agent->getIncubationTime(), agent->getIncubatioShift(), agent->getTicksToInfected(), agent->getDiseaseStageEnd(), agent->getTicksToDiseaseEnd(), agent->getInfections(),
	agent->getHomePlace(), agent->getWorkPlace(), agent->getXCoord(), agent->getYCoord(), agent->getAverageWalk(), agent->getHomeZone(), agent->getWorkZone());
    out.push_back(package);
}

void AgentPackageProvider::provideContent(repast::AgentRequest req, std::vector<AgentPackage>& out){
    std::vector<repast::AgentId> ids = req.requestedAgents();
    for(size_t i = 0; i < ids.size(); i++){
        providePackage(agents->getAgent(ids[i]), out);
    }
}

AgentPackageReceiver::AgentPackageReceiver(repast::SharedContext<RepastHPCAgent>* agentPtr): agents(agentPtr){}

RepastHPCAgent * AgentPackageReceiver::createAgent(AgentPackage package){
    repast::AgentId id(package.id, package.rank, package.type, package.currentRank);
	RepastHPCAgent * agent = new RepastHPCAgent(id);
	/* Recover attributes */
	// Initialize agent attributes
	agent->setProcessWork(package.processWork);
	agent->setProcessHome(package.processHome);
	agent->setAge(package.age);
	agent->setFamily(package.family);
	agent->setStratum(package.stratum);
	agent->setDiseaseStage(package.diseaseStage);

	// Locations
	agent->setXCoord(package.xcoord);
	agent->setYCoord(package.ycoord);
	agent->setHomePlace(package.homeplace);
	agent->setHomeZone(package.homeZone);
	agent->setAverageWalk(package.averageWalk);
	agent->setWorkPlace(package.workplace);
	agent->setWorkZone(package.workZone);
	agent->setAtHome(package.atHome);

	// Times
	agent->setWorkShift(package.workShift);
	agent->setWakeUpTime(package.wakeUpTime);
	agent->setReturnToHomeTime(package.returnToHomeTime);

	// Disease stage
	agent->setIncubationShift(package.incubationShift);
	agent->setTicksToInfected(package.ticksToInfected);
	agent->setDiseaseStageEnd(package.diseaseStageEnd);
	agent->setTicksToDiseaseEnd(package.ticksToDiseaseEnd);
	agent->setInfections(package.infections);
	agent->setPatientType(package.patientType);

    return agent;
}

void AgentPackageReceiver::updateAgent(AgentPackage package){
    repast::AgentId id(package.id, package.rank, package.type);
    RepastHPCAgent * agent = agents->getAgent(id);
    agent->set(package.currentRank);

	// Recover attributes
	// Initialize agent attributes
	agent->setAge(package.age);
	agent->setDiseaseStage(package.diseaseStage);
	agent->setProcessWork(package.processWork);
	agent->setProcessHome(package.processHome);

	// Locations
	agent->setXCoord(package.xcoord);
	agent->setYCoord(package.ycoord);
	agent->setHomePlace(package.homeplace);
	agent->setWorkPlace(package.workplace);
	agent->setAtHome(package.atHome);

	// Times
	agent->setWorkShift(package.workShift);
	agent->setWakeUpTime(package.wakeUpTime);
	agent->setReturnToHomeTime(package.returnToHomeTime);

	// Disease stage
	agent->setIncubationShift(package.incubationShift);
	agent->setTicksToInfected(package.ticksToInfected);
	agent->setDiseaseStageEnd(package.diseaseStageEnd);
	agent->setTicksToDiseaseEnd(package.ticksToDiseaseEnd);
	agent->setInfections(package.infections);
	agent->setPatientType(package.patientType);
}

DataSource_AgentDiseaseStage::DataSource_AgentDiseaseStage(repast::SharedContext<RepastHPCAgent>* c, DiseaseStage ds) : context(c), diseaseStage(ds){ }
int DataSource_AgentDiseaseStage::getData(){
	int sum = 0;
	repast::SharedContext<RepastHPCAgent>::const_local_iterator iter    = context->localBegin();
	repast::SharedContext<RepastHPCAgent>::const_local_iterator iterEnd = context->localEnd();
	while( iter != iterEnd) {
		if ((*iter)->getDiseaseStage() == diseaseStage){
			sum++;
		}
		iter++;
	}
	return sum;
}