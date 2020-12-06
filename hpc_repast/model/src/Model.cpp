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

RepastHPCModel::RepastHPCModel(std::string propsFile, int argc, char** argv, boost::mpi::communicator* comm): context(comm){
	props = new repast::Properties(propsFile, argc, argv, comm);
	stopAt = repast::strToInt(props->getProperty("stop.at"));

	// Current rank
	int crank = repast::RepastProcess::instance()->rank();

    // Number of agents
	countOfAgents = repast::strToInt(props->getProperty("count.of.agents"));

	bufferSize = repast::strToDouble(props->getProperty("buffer.size"));
	infectionRadius = repast::strToDouble(props->getProperty("infection.radius"));
	isolation = (repast::strToInt(props->getProperty("isolation")) == 1);
	workMove = repast::strToDouble(props->getProperty("work.move"));

	// Number of processors
	procsX = repast::strToInt(props->getProperty("procs.x"));
	procsY = repast::strToInt(props->getProperty("procs.y"));

	// Read the bounds path
	geographyPath = props->getProperty("geography.path");
	boundsFile = props->getProperty("bounds.file");
	geographyFile = props->getProperty("geography.file");

	// Read bounds
	Reader::getBounds(geographyPath, boundsFile, &originX, &extentX, &originY, &extentY);
	maxX = originX + extentX;
	maxY = originY + extentY;

	// Read SIT Zones
	Reader::getBorders(geographyPath, geographyFile, crank, zones, border);

	// Read the agents path
	agentsPath = props->getProperty("agents.path");
	agentsFile = props->getProperty("agents.file");

	// Read policy files
	policyPath = props->getProperty("policy.path");
	policyFile = props->getProperty("policy.file");
	policyInfo = props->getProperty("policy.info");
	Reader::getPolicies(crank, policyPath, policyFile, stopAt, policies);
	Reader::getPolicyInfo(policyPath, policyInfo, policyEnforcer.cmp, policyEnforcer.msu);

	// Read mask factor
	maskFactor = repast::strToDouble(props->getProperty("mask.factor"));

	// Check if agents states must be recorded
	write = props->getProperty("write.states") == "1";

	// Distributions
	initializeRandom(*props, comm);
	r = repast::Random::instance();
	g.seed(crank * 1000);

	if(repast::RepastProcess::instance()->rank() == 0){
		props->writeToSVFile("./output/record.csv");
	};

	provider = new AgentPackageProvider(&context);
	receiver = new AgentPackageReceiver(&context);

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

	// Uncomment to save agent states in a CSV File
	if(repast::RepastProcess::instance()->rank() == 0 && write){
		if(!Reader::fileExists(csvFile))
        writeCsvFile(csvFile, "Tick", "ID", "X", "Y", "DiseaseStage");
	};
}

RepastHPCModel::~RepastHPCModel(){
	delete props;
	delete provider;
	delete receiver;
	delete agentValues;
}

void RepastHPCModel::init(repast::ScheduleRunner& runner){
	// Current rank
	int crank = repast::RepastProcess::instance()->rank();
	int countOfAgents;

	// Read agents information
	Reader::getAgents(agentsPath, agentsFile, crank, &countOfAgents, agents0);

	for (RepastHPCAgent* agent : agents0){
		// Set the disease stage
		agent -> initAgent(r);

		// Set initial compliance
		agent->setComplies(true);
		agent->setUsesMask(false);

		// Setup initial disease stage
		agent -> initDisease(r, &g);
		if(agent->getDiseaseStage() == INFECTED){
			while(agent->getPatientType() == SEVERE_SYMPTOMS || agent->getPatientType() == CRITICAL_SYMPTOMS){
				agent->initDisease(r, &g);
			}
		}

		// Add agent to context
		context.addAgent(agent);
	}

	for (policy p: policies){
		policyEnforcer.schedulePolicy(p, runner);
	}
}

void RepastHPCModel::step(){
	int crank = repast::RepastProcess::instance()->rank();
	int ctick = repast::RepastProcess::instance()->getScheduleRunner().currentTick();
	day = (int) TickConverter::ticksToDays(ctick) % 7;
	hour = ctick % 24;

	// Print current tick
	if (crank == 0){
		std::cout << " day: " << ctick/24 << " hour: " << hour <<   std::endl;
	}

	// Agents in the current rank
	std::vector<RepastHPCAgent*> agents;
	// Number of agents
	int n = context.size();
	if (n > 0){
		context.selectAgents(repast::SharedContext<RepastHPCAgent>::LOCAL, agents);
	}

	// Write agent states
	if(ctick == 1){
		RepastHPCModel::agentStates(agents, 0, write);
	}

	// Update disease stage and position
	agentsUpdate(agents, ctick, crank);

	// Select susceptible and infected agents
	std::vector<RepastHPCAgent*> agentsS; // Susceptibles
	std::vector<RepastHPCAgent*> agentsI; // Infected
	agentsSelect(agents, agentsS, agentsI);

	// Sort agents by coordinate
	agentsSort(agentsS);
	agentsSort(agentsI);

	// Interaction of susceptible and infected
	agentsInfect(agentsS, agentsI);

	// Step the incubation shift
	agentsIncubation(agents);

	// Ask agents to wake up or return to home
	agentsMove(agents, ctick, crank);

	// Update agents status
	repast::RepastProcess::instance()->synchronizeAgentStatus<RepastHPCAgent, AgentPackage, AgentPackageProvider, AgentPackageReceiver>(context, *provider, *receiver, *receiver);
	if(write && context.size() > 0){
		agents.clear();
		context.selectAgents(repast::SharedContext<RepastHPCAgent>::LOCAL, context.size(), agents);
		// Write agent states
		agentStates(agents, ctick, write);
	}
}

void RepastHPCModel::agentsUpdate(std::vector<RepastHPCAgent*>& agents, int tick, int rank){
	for(RepastHPCAgent* agent : agents){
		// Update disease stage
		if(agent->isActiveCase()){
			agent->diseaseActions(r, tick, &g);
		}
		// Update positions
		if(policyEnforcer.isAllowedToGoOut(r, isolation, agent, tick, rand(), day)){
			agent->move(r, rank, border, hour, workMove);
		}else{
			agent->cr = agent->getId().currentRank();
		}
	}
}

void RepastHPCModel::agentsSelect(std::vector<RepastHPCAgent*> agents, std::vector<RepastHPCAgent*>& s, std::vector<RepastHPCAgent*>& i){
	for (RepastHPCAgent* agent : agents){
		if(agent->getDiseaseStage() == SUSCEPTIBLE){
			s.push_back(agent);
		} else if (agent->getDiseaseStage() == INFECTED){
			i.push_back(agent);
		}
	}
}

bool positionCompare(RepastHPCAgent* a, RepastHPCAgent* b){
	// First criteria
    double x1 = a->getXCoord();
	double y1 = a->getYCoord();
	// Second criteria
	double x2 = b->getXCoord();
	double y2 = b->getYCoord();
	if (x1 != x2){
		return x1 < x2;
	}
	return y1 < y2;
}

void RepastHPCModel::agentsSort(std::vector<RepastHPCAgent*>& a){
	std::stable_sort(a.begin(), a.end(), positionCompare);
}

void RepastHPCModel::agentsInfect(std::vector<RepastHPCAgent*>& S, std::vector<RepastHPCAgent*>& I){
	int s0 = 0;
	double distance;
	bool cr;
	RepastHPCAgent* susceptible;
	RepastHPCAgent* infected;
	double k1 = Geography::delta_lat*infectionRadius;
	double k = Geography::delta_lon*infectionRadius;
	// Infected agents interact with susceptibles
	for(int i = 0; i < I.size(); i++){
		try {
			infected = I.at(i);
			cr = true;
			for(int s = s0; s < S.size(); s++){
				try {
					susceptible = S.at(s);
					distance = infected->getDistance(susceptible->getXCoord(), susceptible->getYCoord());
					// Check if susceptible can be infected
					if (distance <= infectionRadius && susceptible->getDiseaseStage() == SUSCEPTIBLE){
						rand_exposed = rand();
						if(Probabilities::isGettingExposed(rand_exposed, infected->getIncubatioShift(), infected->getUsesMask(), susceptible->getUsesMask(), maskFactor)){
							infected->setInfections(1);
							susceptible->setExposed(r);
						}
					}
					if(cr && i < I.size()-1){
						if( (abs(infected->getXCoord()-susceptible->getXCoord()) <=k or abs(infected->getYCoord() - susceptible->getYCoord())<=k) and susceptible->getXCoord() < I.at(i+1)->getXCoord() ){
							cr = false;
							s0 = s;
						}
					}
					if( (susceptible->getXCoord() - infected->getXCoord()) > k1 && abs(susceptible->getYCoord() - infected->getYCoord()) > k){
						break;
					}
				} catch(const std::exception& e) {
					std::cerr << e.what() << '\n';
				}
			}
		} catch(const std::exception& e) {
			std::cerr << e.what() << '\n';
		}
	}
}

void RepastHPCModel::agentsIncubation(std::vector<RepastHPCAgent*>& a){
	for(RepastHPCAgent* agent : a){
		if(agent->isActiveCase()){
			agent->stepIncubationShift(1);
		}
	}
}

void RepastHPCModel::agentsMove(std::vector<RepastHPCAgent*>& a, int tick, int rank){
	bool c = true;
	bool m = false;
	int sc;
	double mu, sigma;
	for(RepastHPCAgent* agent : a){
		if(agent->getDiseaseStage() != IMMUNE && agent->getDiseaseStage() != DEAD){
			// If agent wakes up
			if(hour == agent->getWakeUpTime()){
				sc = agent->getStratum() - 1;
				if (policyEnforcer.currentPolicies.at(0).p != NONE){
					// Check if agent breaks the policy
					mu = policyEnforcer.cmp.values.at(sc);
					sigma = mu * policyEnforcer.cmp.std.at(sc);

					// Update compliance
					c = Probabilities::getComply(r, mu, sigma);
				}

				// Update compliance of policy
				agent->setComplies(c);

				// Update compliance of mask usage
				if(policyEnforcer.currentPolicies.at(0).mask){
					mu = policyEnforcer.msu.values.at(sc);
					sigma = policyEnforcer.msu.values.at(sc);
					m = Probabilities::getUsage(r, mu, sigma);
					agent->setUsesMask(m);
				}

				if(policyEnforcer.isAllowedToGoOut(r, isolation, agent, hour, rand(), day)){
					if (rank != agent->getProcessWork()){
						repast::RepastProcess::instance()->moveAgent(agent->getId(), agent->getProcessWork());
					}
					agent->wakeUp();
				}
			} else if (hour == agent->getReturnToHomeTime()){
				if (rank != agent->getProcessHome()){
					repast::RepastProcess::instance()->moveAgent(agent->getId(), agent->getProcessHome());
				}
				agent->returnHome();
			} else if (agent->cr != rank){
				repast::RepastProcess::instance()->moveAgent(agent->getId(), agent->cr);
			}
		}
	}
}

void RepastHPCModel::initSchedule(repast::ScheduleRunner& runner){
	// Schedule events
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

void RepastHPCModel::agentStates(std::vector<RepastHPCAgent*> a, int tick, bool write){
	if(!write){
		return;
	}
	for(auto agent : a){
		if (!writeCsvFile( csvFile, tick, agent->getId(), agent->getXCoord(), agent->getYCoord(), agent->getDiseaseStage() )) {
			std::cerr << "Failed to write to file: " << csvFile << "\n";
		}
	}
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
		file.close();
        return true;
    } else {
        return false;
    }
}

double RepastHPCModel::rand(){
	return r->nextDouble();
}