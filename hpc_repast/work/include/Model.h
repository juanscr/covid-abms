/* Model.h */

#ifndef DEMO_03_MODEL
#define DEMO_03_MODEL

#include <boost/mpi.hpp>
#include "repast_hpc/Schedule.h"
#include "repast_hpc/Properties.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/AgentRequest.h"
#include "repast_hpc/TDataSource.h"
#include "repast_hpc/SVDataSet.h"
#include "repast_hpc/SharedContinuousSpace.h"
#include "repast_hpc/GridComponents.h"

#include "Agent.h"
#include "Probabilities.h"


/* Agent Package Provider */
class RepastHPCAgentPackageProvider {
	private:
	repast::SharedContext<RepastHPCAgent>* agents;

public:
    RepastHPCAgentPackageProvider(repast::SharedContext<RepastHPCAgent>* agentPtr);
    void providePackage(RepastHPCAgent * agent, std::vector<RepastHPCAgentPackage>& out);
    void provideContent(repast::AgentRequest req, std::vector<RepastHPCAgentPackage>& out);
};

/* Agent Package Receiver */
class RepastHPCAgentPackageReceiver {
	private:
	repast::SharedContext<RepastHPCAgent>* agents;

public:

    RepastHPCAgentPackageReceiver(repast::SharedContext<RepastHPCAgent>* agentPtr);
    RepastHPCAgent * createAgent(RepastHPCAgentPackage package);
    void updateAgent(RepastHPCAgentPackage package);

};

/* Data Collection */

class DataSource_AgentDiseaseStage : public repast::TDataSource<int>{
	private:
		repast::SharedContext<RepastHPCAgent>* context;
		DiseaseStage diseaseStage;
	public:
		DataSource_AgentDiseaseStage(repast::SharedContext<RepastHPCAgent>* c, DiseaseStage ds);
		int getData();
};


class RepastHPCModel{
	int stopAt;
	int countOfAgents;
	double bufferSize;
	int procsX;
	int procsY;
	double originX;
	double originY;
	double extentX;
	double extentY;
	double maxX;
	double maxY;
	double infectionRadius;
	double distance;
	double rand_exposed;
	int seed;

	repast::Properties* props;
	repast::SharedContext<RepastHPCAgent> context;
	RepastHPCAgentPackageProvider* provider;
	RepastHPCAgentPackageReceiver* receiver;

	repast::SVDataSet* agentValues;
    repast::SharedContinuousSpace<RepastHPCAgent, repast::StrictBorders, repast::SimpleAdder<RepastHPCAgent> >* continuousSpace;

public:
	RepastHPCModel(std::string propsFile, int argc, char** argv, boost::mpi::communicator* comm);
	~RepastHPCModel();
	void init(repast::ScheduleRunner& runner);
	void requestAgents();
	void cancelAgentRequests();
	void removeLocalAgents();
	int getProcess(double x, double y);
	void step();
	void initSchedule(repast::ScheduleRunner& runner);
	void recordResults();
};

#endif