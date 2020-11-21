#ifndef PACKAGE
#define PACKAGE

#include "repast_hpc/TDataSource.h"
#include "repast_hpc/SVDataSet.h"
#include "Agent.h"

/* Agent Package Provider */
class AgentPackageProvider {
	private:
	repast::SharedContext<RepastHPCAgent>* agents;
public:
    AgentPackageProvider(repast::SharedContext<RepastHPCAgent>* agentPtr);
    void providePackage(RepastHPCAgent * agent, std::vector<AgentPackage>& out);
    void provideContent(repast::AgentRequest req, std::vector<AgentPackage>& out);
};

/* Agent Package Receiver */
class AgentPackageReceiver {
	private:
	repast::SharedContext<RepastHPCAgent>* agents;

public:
    AgentPackageReceiver(repast::SharedContext<RepastHPCAgent>* agentPtr);
    RepastHPCAgent * createAgent(AgentPackage package);
    void updateAgent(AgentPackage package);
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

#endif