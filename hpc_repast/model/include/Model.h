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

#include <mutex>

#include "Agent.h"
#include "Probabilities.h"
#include "PolicyEnforcer.h"
#include "Zone.h"

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <fstream>

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
	//Sit zopne path
	std::string szp;
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

	// Testing geography
	typedef boost::geometry::model::d2::point_xy<double> point;
	typedef boost::geometry::model::polygon<point> polygon;
	polygon poly;
	std::string fig;
	int npoints;
	std::vector<point> points;
	std::vector<polygon> ext_borders;
	std::vector<Zone*> zones;

	// CSV File with agent states
	std::mutex logMutex;
	std::string csvFile;

	// Policy Enforcer
	PolicyEnforcer policyEnforcer;

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
	bool fileExists(std::string& fileName);
	template <typename filename, typename T1, typename T2, typename T3, typename T4, typename T5>
	bool writeCsvFile(filename &fileName, T1 column1, T2 column2, T3 column3, T4 column4, T5 column5);
};
#endif