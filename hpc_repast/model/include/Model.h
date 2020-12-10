/* Model.h */
#ifndef MODEL
#define MODEL

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
#include "PolicyEnforcer.h"
#include "Reader.h"
#include "Package.h"

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/multi_polygon.hpp>
#include <fstream>

class RepastHPCModel{
	int stopAt;
	int countOfAgents;
	double bufferSize;
	int procsX;
	int procsY;
	int day;
	int hour;
	int crank;
	//Sit zopne path
	double originX;
	double originY;
	double extentX;
	double extentY;
	double maxX;
	double maxY;
	double distance;
	// Disease stage attributes
	double infectionRadius;
	bool isolation;
	double workMove;
	double rand_exposed;
	double maskFactor;
	bool trackCases;

	// Boost geography
	typedef boost::geometry::model::d2::point_xy<double> point;
	typedef boost::geometry::model::polygon<point> polygon;
	typedef boost::geometry::model::multi_polygon<polygon> multipolygon;
	polygon poly;
	// std::vector<Geography::border> border;
	std::vector<Zone*> zones;
	std::vector<Border*> border;
	std::string geographyPath;
	std::string boundsFile;
	std::string geographyFile;
	std::string agentsPath;
	std::string agentsFile;
	std::vector<RepastHPCAgent*> agents0;

	// CSV File with agent states
	std::mutex logMutex;
	std::string csvFile;
	std::string csvTrack;
	std::vector<std::string> tracker;
	bool write;

	// Policies
	PolicyEnforcer policyEnforcer;
	std::string policyPath;
	std::string policyFile;
	std::string policyInfo;
	std::vector<policy> policies;

	// Packages
	repast::Properties* props;
	repast::SharedContext<RepastHPCAgent> context;
	AgentPackageProvider* provider;
	AgentPackageReceiver* receiver;
	repast::SVDataSet* agentValues;

	// Random generators
	repast::Random* r;
	std::default_random_engine g;

public:
	RepastHPCModel(std::string propsFile, int argc, char** argv, boost::mpi::communicator* comm);
	~RepastHPCModel();
	void init(repast::ScheduleRunner& runner);
	void requestAgents();
	void cancelAgentRequests();
	void removeLocalAgents();
	// Generate random uniform
	double rand();
	// Actions for each tick
	void step();
	// Update agents: diseaseStage and position
	void agentsUpdate(std::vector<RepastHPCAgent*>& agents, int tick, int rank);
	// Select agents according to disease stage
	void agentsSelect(std::vector<RepastHPCAgent*> agents, std::vector<RepastHPCAgent*>& s, std::vector<RepastHPCAgent*>& i);
	// Sort agents by coordinate
	void agentsSort(std::vector<RepastHPCAgent*>& a);
	// Make infect agents to interact with susceptibles
	void agentsInfect(std::vector<RepastHPCAgent*>& S, std::vector<RepastHPCAgent*>& I);
	// Step the incubation shift
	void agentsIncubation(std::vector<RepastHPCAgent*>& a);
	// Move agents to workplace or homplace if necessary
	void agentsMove(std::vector<RepastHPCAgent*>& a, int tick, int rank);
	// Write agents states
	void agentStates(std::vector<RepastHPCAgent*> a, int tick, bool write);
	void initSchedule(repast::ScheduleRunner& runner);
	void recordResults();
	template <typename filename, typename T1, typename T2, typename T3, typename T4, typename T5>
	bool writeCsvFile(filename &fileName, T1 column1, T2 column2, T3 column3, T4 column4, T5 column5);
	void writeTrack(std::vector<RepastHPCAgent*> a);
	void createTracker(int rank);
	void writeCSVTrack();
};
#endif