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

RepastHPCAgentPackageProvider::RepastHPCAgentPackageProvider(repast::SharedContext<RepastHPCAgent>* agentPtr): agents(agentPtr){ }

void RepastHPCAgentPackageProvider::providePackage(RepastHPCAgent * agent, std::vector<RepastHPCAgentPackage>& out){
    repast::AgentId id = agent->getId();
    RepastHPCAgentPackage package(id.id(), id.startingRank(), id.agentType(), id.currentRank(), agent->getC(), agent->getTotal(), agent->getAge(),
	agent->getAtHome(), agent->getWorkShit(), agent->getWakeUpTime(), agent->getReturnToHomeTime(),
	agent->getDiseaseStage(), agent->getPatientType(), agent->getTimeToDeath(), agent->getTimeToImmune(), agent->getIncubationTime(), agent->getIncubatioShift(),
	agent->getHomePlace(), agent->getWorkPlace());
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
    return new RepastHPCAgent(id, package.c, package.total, package.age, package.diseaseStage);
}

void RepastHPCAgentPackageReceiver::updateAgent(RepastHPCAgentPackage package){
    repast::AgentId id(package.id, package.rank, package.type);
    RepastHPCAgent * agent = agents->getAgent(id);
    agent->set(package.currentRank, package.c, package.total, package.age, package.diseaseStage);
}


DataSource_AgentTotals::DataSource_AgentTotals(repast::SharedContext<RepastHPCAgent>* c) : context(c){ }

int DataSource_AgentTotals::getData(){
	int sum = 0;
	repast::SharedContext<RepastHPCAgent>::const_local_iterator iter    = context->localBegin();
	repast::SharedContext<RepastHPCAgent>::const_local_iterator iterEnd = context->localEnd();
	while( iter != iterEnd) {
		sum+= (*iter)->getTotal();
		iter++;
	}
	return sum;
}

DataSource_AgentCTotals::DataSource_AgentCTotals(repast::SharedContext<RepastHPCAgent>* c) : context(c){ }

int DataSource_AgentCTotals::getData(){
	int sum = 0;
	repast::SharedContext<RepastHPCAgent>::const_local_iterator iter    = context->localBegin();
	repast::SharedContext<RepastHPCAgent>::const_local_iterator iterEnd = context->localEnd();
	while( iter != iterEnd) {
		sum+= (*iter)->getC();
		iter++;
	}
	return sum;
}

RepastHPCModel::RepastHPCModel(std::string propsFile, int argc, char** argv, boost::mpi::communicator* comm): context(comm){
	props = new repast::Properties(propsFile, argc, argv, comm);
	stopAt = repast::strToInt(props->getProperty("stop.at"));

    // Number of agents
	countOfAgents = repast::strToInt(props->getProperty("count.of.agents"));

	bufferSize = repast::strToDouble(props->getProperty("buffer.size"));
	
	// Number of processors
	procsX = repast::strToInt(props->getProperty("procs.x"));
	procsY = repast::strToInt(props->getProperty("procs.y"));

	// Size of space
	originX = repast::strToDouble(props->getProperty("origin.x"));
	originY = repast::strToDouble(props->getProperty("origin.y"));
	extentX = repast::strToDouble(props->getProperty("extent.x"));
	extentY = repast::strToDouble(props->getProperty("extent.y"));

	// Distributions
	
	initializeRandom(*props, comm);
	//seed = repast::strToInt(props->getProperty("random.seed"));
	//std::srand(seed);
	
	if(repast::RepastProcess::instance()->rank() == 0) props->writeToSVFile("./output/record.csv");
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
	DataSource_AgentTotals* agentTotals_DataSource = new DataSource_AgentTotals(&context);
	builder.addDataSource(createSVDataSource("Total", agentTotals_DataSource, std::plus<int>()));

	DataSource_AgentCTotals* agentCTotals_DataSource = new DataSource_AgentCTotals(&context);
	builder.addDataSource(createSVDataSource("C", agentCTotals_DataSource, std::plus<int>()));

	// Use the builder to create the data set
	agentValues = builder.createDataSet();	
}

RepastHPCModel::~RepastHPCModel(){
	delete props;
	delete provider;
	delete receiver;
	delete agentValues;
}

void RepastHPCModel::init(){
	// Initialize agents

	// Current rank
	int crank = repast::RepastProcess::instance()->rank();

	// Lower x coordinate
	double init_x = continuousSpace->dimensions().origin().getX();
	double init_y = continuousSpace->dimensions().origin().getY();

	double end_x = init_x +  continuousSpace->dimensions().extents().getX();
	double end_y = init_y + continuousSpace->dimensions().extents().getY();

	double xcoord = 0;
	double ycoord = 0;

	int age = 0;

	for(int i = 0; i < countOfAgents; i++){
		// Position
		xcoord = repast::Random::instance()->nextDouble()*(end_x - init_x) + init_x;
		ycoord = repast::Random::instance()->nextDouble()*(end_y - init_y) + init_y;

		// Define age
		age = probabilities.getRandomAge(repast::Random::instance()->nextDouble(), repast::Random::instance()->nextDouble());

        repast::Point<double> initialLocation(xcoord, ycoord);

		// Create agent
		repast::AgentId id(i, crank, 0);
		id.currentRank(crank);
		RepastHPCAgent* agent = new RepastHPCAgent(id, 100, 200, age,  SUSCEPTIBLE);

		// Set initial attributes to agents
		agent->setAtHome(true);
		agent -> setWorkShift(probabilities.getRandomWorkShift(repast::Random::instance()->nextDouble()));
		
		// Add agent to context
		context.addAgent(agent);
        continuousSpace->moveTo(id, initialLocation);
	}
}

void RepastHPCModel::doSomething(){
	// Current rank
	int whichRank = repast::RepastProcess::instance()->rank();

	std::vector<RepastHPCAgent*> agents;
	context.selectAgents(repast::SharedContext<RepastHPCAgent>::LOCAL, context.size(), agents);
	
    std::vector<RepastHPCAgent*>::iterator it = agents.begin();
    while(it != agents.end()){

		PatientType x = probabilities.getRandomPatientType(repast::Random::instance()->nextDouble(), repast::Random::instance()->nextDouble());

		if (whichRank == 0){
			std::cout << "Agent random number is " << x << "\n" << std::endl; 
		}
		
		(*it)->move(continuousSpace);
		it++;
    }

	continuousSpace->balance();
    repast::RepastProcess::instance()->synchronizeAgentStatus<RepastHPCAgent, RepastHPCAgentPackage, RepastHPCAgentPackageProvider, RepastHPCAgentPackageReceiver>(context, *provider, *receiver, *receiver);
    repast::RepastProcess::instance()->synchronizeProjectionInfo<RepastHPCAgent, RepastHPCAgentPackage, RepastHPCAgentPackageProvider, RepastHPCAgentPackageReceiver>(context, *provider, *receiver, *receiver);
	repast::RepastProcess::instance()->synchronizeAgentStates<RepastHPCAgentPackage, RepastHPCAgentPackageProvider, RepastHPCAgentPackageReceiver>(*provider, *receiver);  
}

void RepastHPCModel::initSchedule(repast::ScheduleRunner& runner){
	//runner.scheduleEvent(1, repast::Schedule::FunctorPtr(new repast::MethodFunctor<RepastHPCModel> (this, &RepastHPCModel::requestAgents)));
	runner.scheduleEvent(1, 1, repast::Schedule::FunctorPtr(new repast::MethodFunctor<RepastHPCModel> (this, &RepastHPCModel::doSomething)));
	//runner.scheduleEvent(1.1, 1, repast::Schedule::FunctorPtr(new repast::MethodFunctor<RepastHPCModel> (this, &RepastHPCModel::moveAgents)));
	runner.scheduleEndEvent(repast::Schedule::FunctorPtr(new repast::MethodFunctor<RepastHPCModel> (this, &RepastHPCModel::recordResults)));
	runner.scheduleStop(stopAt);
	
	// Data collection
	runner.scheduleEvent(1.5, 5, repast::Schedule::FunctorPtr(new repast::MethodFunctor<repast::DataSet>(agentValues, &repast::DataSet::record)));
	runner.scheduleEvent(10.6, 10, repast::Schedule::FunctorPtr(new repast::MethodFunctor<repast::DataSet>(agentValues, &repast::DataSet::write)));
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