/* Demo_03_Model.cpp */
#include <stdio.h>
#include <vector>
#include <boost/mpi.hpp>
#include "repast_hpc/AgentId.h"
#include "repast_hpc/RepastProcess.h"
#include "repast_hpc/Utilities.h"
#include "repast_hpc/Properties.h"
#include "repast_hpc/initialize_random.h"
#include "repast_hpc/SVDataSetBuilder.h"
#include "repast_hpc/Point.h"

#include "Model.h"
#include "Probabilities.h"
#include "PolicyEnforcer.h"

RepastHPCAgentPackageProvider::RepastHPCAgentPackageProvider(repast::SharedContext<RepastHPCAgent>* agentPtr): agents(agentPtr){}

void RepastHPCAgentPackageProvider::providePackage(RepastHPCAgent * agent, std::vector<RepastHPCAgentPackage>& out){
    repast::AgentId id = agent->getId();
    RepastHPCAgentPackage package(id.id(), id.startingRank(), id.agentType(), id.currentRank(), agent->getProcessWork(), agent->getProcessHome(),
	agent->getAge(),
	agent->getAtHome(), agent->getWorkShit(), agent->getWakeUpTime(), agent->getReturnToHomeTime(),
	agent->getDiseaseStage(), agent->getPatientType(), agent->getIncubationTime(), agent->getIncubatioShift(), agent->getTicksToInfected(), agent->getDiseaseStageEnd(), agent->getTicksToDiseaseEnd(), agent->getInfections(),
	agent->getHomePlace(), agent->getWorkPlace(), agent->getXCoord(), agent->getYCoord());
    out.push_back(package);
}

void RepastHPCAgentPackageProvider::provideContent(repast::AgentRequest req, std::vector<RepastHPCAgentPackage>& out){
    std::vector<repast::AgentId> ids = req.requestedAgents();
    for(size_t i = 0; i < ids.size(); i++){
        providePackage(agents->getAgent(ids[i]), out);
    }
}

RepastHPCAgentPackageReceiver::RepastHPCAgentPackageReceiver(repast::SharedContext<RepastHPCAgent>* agentPtr): agents(agentPtr){}

RepastHPCAgent * RepastHPCAgentPackageReceiver::createAgent(RepastHPCAgentPackage package){
    repast::AgentId id(package.id, package.rank, package.type, package.currentRank);
	RepastHPCAgent * agent = new RepastHPCAgent(id);
	/* Recover attributes */
	// Initialize agent attributes
	agent->setProcessWork(package.processWork);
	agent->setProcessHome(package.processHome);
	agent->setAge(package.age);
	agent->setDiseaseStage(package.diseaseStage);

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

    return agent;
}

void RepastHPCAgentPackageReceiver::updateAgent(RepastHPCAgentPackage package){

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

RepastHPCModel::RepastHPCModel(std::string propsFile, int argc, char** argv, boost::mpi::communicator* comm): context(comm){
	props = new repast::Properties(propsFile, argc, argv, comm);
	stopAt = repast::strToInt(props->getProperty("stop.at"));

	// Current rank
	int crank = repast::RepastProcess::instance()->rank();

    // Number of agents
	countOfAgents = repast::strToInt(props->getProperty("count.of.agents"));

	bufferSize = repast::strToDouble(props->getProperty("buffer.size"));
	infectionRadius = repast::strToDouble(props->getProperty("infection.radius"));

	// Number of processors
	procsX = repast::strToInt(props->getProperty("procs.x"));
	procsY = repast::strToInt(props->getProperty("procs.y"));

	// Read the sit zone path
	szp = props->getProperty("sit.zone.path");

	// Read borders and polygons
	// Gen first polygon
	poly.clear();

	Geography::getExternalBorders(crank, true, szp, procsX, procsY, &poly, &ext_borders, &originX, &extentX, &originY, &extentY, zones);
	npoints = boost::geometry::num_points(poly);
	points = poly.outer();
	maxX = originX + extentX;
	maxY = originY + extentY;

	// Distributions
	initializeRandom(*props, comm);

	if(repast::RepastProcess::instance()->rank() == 0){
		props->writeToSVFile("./output/record.csv");
	};

	seed = repast::Random::instance()->createUniIntGenerator(1, procsX*procsY).next();

	provider = new RepastHPCAgentPackageProvider(&context);
	receiver = new RepastHPCAgentPackageReceiver(&context);

    repast::Point<double> origin(originX, originY);
    repast::Point<double> extent(extentX, extentY);
    repast::GridDimensions gd(origin, extent);

    std::vector<int> processDims;
    processDims.push_back(procsX);
    processDims.push_back(procsY);

    continuousSpace = new repast::SharedContinuousSpace<RepastHPCAgent, repast::StrictBorders, repast::SimpleAdder<RepastHPCAgent> >("AgentContinuousSpace", gd, processDims, bufferSize, comm);
   	context.addProjection(continuousSpace);

	// Data collection
	// Create the data set builder
	std::string fileOutputName("./output/agent_total_data.csv");
	repast::SVDataSetBuilder builder(fileOutputName.c_str(), ",", repast::RepastProcess::instance()->getScheduleRunner().schedule());

	// Create the individual data sets to be added to the builder
	DataSource_AgentDiseaseStage* agentSusceptible = new DataSource_AgentDiseaseStage(& context, SUSCEPTIBLE);
	builder.addDataSource(createSVDataSource("Susceptible", agentSusceptible, std::plus<int>()));

	DataSource_AgentDiseaseStage* agentExposed = new DataSource_AgentDiseaseStage(& context, EXPOSED);
	builder.addDataSource(createSVDataSource("Exposed", agentExposed, std::plus<int>()));

	DataSource_AgentDiseaseStage* agentInfected = new DataSource_AgentDiseaseStage(& context, INFECTED);
	builder.addDataSource(createSVDataSource("Infected", agentInfected, std::plus<int>()));

	DataSource_AgentDiseaseStage* agentImmune = new DataSource_AgentDiseaseStage(& context, IMMUNE);
	builder.addDataSource(createSVDataSource("Immune", agentImmune, std::plus<int>()));

	DataSource_AgentDiseaseStage* agentDead = new DataSource_AgentDiseaseStage(& context, DEAD);
	builder.addDataSource(createSVDataSource("Dead", agentDead, std::plus<int>()));

	// Use the builder to create the data set
	agentValues = builder.createDataSet();

	// Define csv file to export agent states
	csvFile = "./output/agent_states.csv";

	//Uncomment to save agent states in a CSV File
	// if(repast::RepastProcess::instance()->rank() == 0){
	// 	if(!fileExists(csvFile))
    //     writeCsvFile(csvFile, "Tick", "ID", "X", "Y", "DiseaseStage");
	// };
}

RepastHPCModel::~RepastHPCModel(){
	delete props;
	delete provider;
	delete receiver;
	delete agentValues;
}

void RepastHPCModel::init(repast::ScheduleRunner& runner){
	// Initialize agents

	// Check if agent location is feasible
	std::vector<double> agentWorkLoc;

	// Current rank
	int crank = repast::RepastProcess::instance()->rank();

	// Lower x and y coordinates
	double init_x = continuousSpace->dimensions().origin().getX();
	double init_y = continuousSpace->dimensions().origin().getY();

	// Upper x and y coordinates
	double end_x = init_x + continuousSpace->dimensions().extents().getX();
	double end_y = init_y + continuousSpace->dimensions().extents().getY();

	// Initialize coordinate
	double xhome = 0;
	double yhome = 0;
	double xwork = 0;
	double ywork = 0;
	int workProcess = -1;
	int homeProcess = -1;

	// Geography
	point p;
	std::vector<int> tpoints; // Points of triangle
	std::vector<std::vector<double>> triangle;
	int tid;
	repast::IntUniformGenerator ui  = repast::Random::instance()->createUniIntGenerator(1, npoints);
	double r1, r2;

	for(int i = 0; i < countOfAgents; i++){
		//std::cout << "rank " << crank << " agent " << i << std::endl;
		agentWorkLoc.clear();
		tpoints.clear();
		triangle.clear();
		//Find homeplace within bounds
		do{
			agentWorkLoc.clear();
			tpoints.clear();
			triangle.clear();
			// Generate random point
			while(tpoints.size() != 3){
				tid = ui.next();
				if ( std::find(tpoints.begin(), tpoints.end(), tid) == tpoints.end() ){
					tpoints.push_back(tid);
					triangle.push_back({boost::geometry::get<0>(points[tid]), boost::geometry::get<1>(points[tid])});
				}
			}
			// Push to triangle
			xhome = repast::Random::instance()->nextDouble()*(end_x - init_x) + init_x;
			yhome = repast::Random::instance()->nextDouble()*(end_y - init_y) + init_y;
			r1 = repast::Random::instance()->nextDouble();
			r2 = repast::Random::instance()->nextDouble();
			p.set<0>(xhome);
			p.set<1>(yhome);
			agentWorkLoc.push_back(xhome);
			agentWorkLoc.push_back(yhome);
		}while(!continuousSpace->bounds().contains(agentWorkLoc) || !boost::geometry::within(p, ext_borders.at(crank)) );

		do{
			agentWorkLoc.clear();
			tpoints.clear();
			triangle.clear();
			// Generate random point
			while(tpoints.size() != 3){
				tid = ui.next();
				if ( std::find(tpoints.begin(), tpoints.end(), tid) == tpoints.end() ){
					tpoints.push_back(tid);
					triangle.push_back({boost::geometry::get<0>(points[tid]), boost::geometry::get<1>(points[tid])});
				}
			}
			r1 = repast::Random::instance()->nextDouble();
			r2 = repast::Random::instance()->nextDouble();
			xwork = (1 - std::pow(r1, 0.5))*triangle.at(0).at(0) + ((1-r2)*std::pow(r1, 0.5))*triangle.at(1).at(0) + (r2*std::pow(r1,0.5))*triangle.at(2).at(0);
			ywork = (1 - std::pow(r1, 0.5))*triangle.at(0).at(1) + ((1-r2)*std::pow(r1, 0.5))*triangle.at(1).at(1) + (r2*std::pow(r1,0.5))*triangle.at(2).at(1);
			p.set<0>(xwork);
			p.set<1>(ywork);
			agentWorkLoc.push_back(xwork);
			agentWorkLoc.push_back(ywork);
		}while(!boost::geometry::within(p, poly));

        repast::Point<double> initialLocation(xhome, yhome);

		// Create agent
		repast::AgentId id(i, crank, 0);
		id.currentRank(crank);
		RepastHPCAgent* agent = new RepastHPCAgent(id);

		// Initialize agents
		// agent->setProcessHome(crank);
		homeProcess = getProcess(xhome, yhome);
		agent->setProcessHome(homeProcess);
		agent->initAgent(xhome, yhome, xwork, ywork);

		// Initialize random disease stage
		double randomDisease = repast::Random::instance()->nextDouble();
		if(randomDisease <= 0.1){
			agent->setDiseaseStage(INFECTED);
		}else if (randomDisease <= 0.99){
			agent->setDiseaseStage(SUSCEPTIBLE);
		}else{
			agent->setDiseaseStage(EXPOSED);
		}

		workProcess = getProcess(xwork, ywork);
		agent->setProcessWork(workProcess);

		// initDisease
		agent->initDisease();

		// Add agent to context
		context.addAgent(agent);
		// Boost
		repast::RepastProcess::instance()->moveAgent(agent->getId(), agent->getProcessHome());
        continuousSpace->moveTo(id, initialLocation);
	}

	// Add policies
	policyEnforcer.schedulePolicy(NONE, 0, 5.99, runner);
	policyEnforcer.schedulePolicy(FULL_QUARANTINE, 6, 9.99, runner);
	policyEnforcer.schedulePolicy(ID_BASED_CURFEW, 10, 16.99, runner);
	policyEnforcer.schedulePolicy(NONE, 17, 19.99, runner);
	policyEnforcer.schedulePolicy(ID_BASED_CURFEW, 20, 26.99, runner);
	policyEnforcer.schedulePolicy(FULL_QUARANTINE, 27, stopAt, runner);

}

int RepastHPCModel::getProcess(double x, double y){
	int pX;
	int pY;

	for (int i = 0; i<procsX; i++){
		if ( originX + (extentX*i)/procsX <= x && x <= originX + (extentX*(i+1))/procsX ){
			pX = i;
			break;
		}
	}

	for (int i = 0; i<procsY; i++){
		if ( originY + (extentY*i)/procsY <= y && y<= originY + (extentY*(i+1))/procsY ){
			pY = i;
			break;
		}
	}

	return procsY*pX + pY;
}

bool RepastHPCModel::fileExists(std::string& fileName) {
    return static_cast<bool>(std::ifstream(fileName));
}

template <typename filename, typename T1, typename T2, typename T3, typename T4, typename T5>
bool RepastHPCModel::writeCsvFile(filename &fileName, T1 column1, T2 column2, T3 column3, T4 column4, T5 column5) {
    std::lock_guard<std::mutex> csvLock(logMutex);
    std::fstream file;
    file.open (fileName, std::ios::out | std::ios::app);
    if (file) {
        file << "\"" << column1 << "\",";
        file << "\"" << column2 << "\",";
        file << "\"" << column3 << "\",";
		file << "\"" << column4 << "\",";
		file << "\"" << column5 << "\"";
        file <<  std::endl;
        return true;
    } else {
        return false;
    }
}

void RepastHPCModel::step(){
	int crank = repast::RepastProcess::instance()->rank();
	int ctick = repast::RepastProcess::instance()->getScheduleRunner().currentTick();

	// Print current tick
	if (crank == 0){
		std::cout << " tick: " << ctick << std::endl;
	}

	// Get all agents of the current rank
	std::vector<RepastHPCAgent*> agents;
	// Vector of available agents to infect
	std::vector<RepastHPCAgent*> agents_av;
	// Iterator for susceptible agents
	std::vector<RepastHPCAgent*>::iterator it1;

	// Select all agents
	context.selectAgents(repast::SharedContext<RepastHPCAgent>::LOCAL, context.size(), agents);

	// Uncomment to write init agent states
	// if(ctick==1){
	// 	for(auto agent : agents){
	// 		if (!writeCsvFile( csvFile, 0, agent->getId(), agent->getXCoord(), agent->getYCoord(), agent->getDiseaseStage() )) {
	// 			std::cerr << "Failed to write to file: " << csvFile << "\n";
	// 		}
	// 	}
	// }

	// Iterator for agents
	std::vector<RepastHPCAgent*>::iterator it;

	// Vector of agents to remove
	std::vector<RepastHPCAgent*> agents_del;
	// Iterator of agents to remove
	std::vector<RepastHPCAgent*>::iterator it_del;
	std::vector<RepastHPCAgent*>::iterator toErase;

	it = agents.begin();

	while (it != agents.end()){
		if ((*it)->getDiseaseStage() == SUSCEPTIBLE){
			agents_av.push_back(*it);
		}
			it++;
	}

	it = agents.begin();

	// Actions
	while(it != agents.end()){
		// Update agent disease stage
		if ((*it)->isActiveCase()){
			(*it)->diseaseActions(ctick);
			}

		// Ask agents to move if allowed
		if ( policyEnforcer.isAllowedToGoOut((*it)->getId(), ctick) ){
			(*it)->move(ext_borders.at(crank), continuousSpace, originX, maxX, originY, maxY);
		}

		if ((*it)->getDiseaseStage() == INFECTED){
			it1 = agents_av.begin();
			agents_del.clear();

			while(it1 != agents_av.end()){
				// Get distance between two agents
				distance = (*it)->getDistance((*it1)->getXCoord(), (*it1)->getYCoord());

				// Rand for getting exposed
				rand_exposed = repast::Random::instance()->nextDouble();

					if (distance <= infectionRadius && Probabilities::isGettingExposed(rand_exposed, (*it)->getIncubatioShift())){
						// Increase infections
						(*it)->setInfections( (*it)->getInfections() + 1);

						// Set exposed
						(*it1)->setExposed();

						// Remove from vector of susceptibles
						agents_del.push_back(*it1);
					}
					it1++;
				}

				// Delete suceptible agents that became exposed
				it_del = agents_del.begin();
				while(it_del != agents_del.end() && agents_del.size()>0){
					toErase = std::find(agents_av.begin(), agents_av.end(), *it_del);

					if (toErase != agents_av.end()){
						agents_av.erase(toErase);
					}
					it_del++;
				}
			}

			// Update incubation shift
			if ((*it)->isActiveCase()){
				(*it)->stepIncubationShift(1);
			}

			it++;
		}

	// Start iterator to wake up or return to home agents
	it = agents.begin();

	while(it != agents.end() && agents.size()>0){
		// Ask agents to wake up
		if (policyEnforcer.isAllowedToGoOut((*it)->getId(), ctick) && (*it)->getDiseaseStage()!=DEAD && (ctick)%TickConverter::TICKS_PER_DAY <= (*it)->getWakeUpTime()  && (*it)->getWakeUpTime()  <=  (ctick+1)%TickConverter::TICKS_PER_DAY){
			if ( (*it)->getProcessHome() != (*it)->getProcessWork() ){
				repast::RepastProcess::instance()->moveAgent((*it)->getId(), (*it)->getProcessWork());
			}
			(*it)->wakeUp(continuousSpace);
			}
		else if ((*it)->getDiseaseStage()!=DEAD && (ctick)%TickConverter::TICKS_PER_DAY <= (*it)->getReturnToHomeTime()  && (*it)->getReturnToHomeTime()  <=  (ctick+1)%TickConverter::TICKS_PER_DAY){
			repast::RepastProcess::instance()->moveAgent((*it)->getId(), (*it)->getProcessHome());
			(*it)->returnHome(continuousSpace);
		}

		it++;
	}

	repast::RepastProcess::instance()->synchronizeAgentStatus<RepastHPCAgent, RepastHPCAgentPackage, RepastHPCAgentPackageProvider, RepastHPCAgentPackageReceiver>(context, *provider, *receiver, *receiver);
	//Uncomment to save agent states in a CSV File
	agents.clear();
	context.selectAgents(repast::SharedContext<RepastHPCAgent>::LOCAL, context.size(), agents);

	// Write states of agents
	// for(auto agent : agents){
	// 	if (!writeCsvFile( csvFile, ctick, agent->getId(), agent->getXCoord(), agent->getYCoord(), agent->getDiseaseStage() )) {
	// 	std::cerr << "Failed to write to file: " << csvFile << "\n";
	// 	}
	// }

	continuousSpace->balance();
	repast::RepastProcess::instance()->synchronizeAgentStatus<RepastHPCAgent, RepastHPCAgentPackage, RepastHPCAgentPackageProvider, RepastHPCAgentPackageReceiver>(context, *provider, *receiver, *receiver);
	repast::RepastProcess::instance()->synchronizeProjectionInfo<RepastHPCAgent, RepastHPCAgentPackage, RepastHPCAgentPackageProvider, RepastHPCAgentPackageReceiver>(context, *provider, *receiver, *receiver);
	repast::RepastProcess::instance()->synchronizeAgentStates<RepastHPCAgentPackage, RepastHPCAgentPackageProvider, RepastHPCAgentPackageReceiver>(*provider, *receiver);
}

void RepastHPCModel::initSchedule(repast::ScheduleRunner& runner){
	runner.scheduleEvent(1, 1, repast::Schedule::FunctorPtr(new repast::MethodFunctor<RepastHPCModel> (this, &RepastHPCModel::step)));

	runner.scheduleEndEvent(repast::Schedule::FunctorPtr(new repast::MethodFunctor<RepastHPCModel> (this, &RepastHPCModel::recordResults)));
	runner.scheduleStop(stopAt);

	// Data collection
	runner.scheduleEvent(1, 1, repast::Schedule::FunctorPtr(new repast::MethodFunctor<repast::DataSet>(agentValues, &repast::DataSet::record)));
	runner.scheduleEvent(1, 1, repast::Schedule::FunctorPtr(new repast::MethodFunctor<repast::DataSet>(agentValues, &repast::DataSet::write)));
	runner.scheduleEndEvent(repast::Schedule::FunctorPtr(new repast::MethodFunctor<repast::DataSet>(agentValues, &repast::DataSet::write)));
}

void RepastHPCModel::recordResults(){
	if(repast::RepastProcess::instance()->rank() == 0){
		props->putProperty("Result","Passed");
		std::vector<std::string> keyOrder;
		keyOrder.push_back("RunNumber");
		keyOrder.push_back("stop.at");
		keyOrder.push_back("Result");
		props->writeToSVFile("./output/results.csv", keyOrder);
    }
}