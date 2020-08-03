/* Agent.h */
#ifndef DEMO_03_AGENT
#define DEMO_03_AGENT

#include "repast_hpc/AgentId.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/SharedContinuousSpace.h"
#include <stdio.h>
#include <vector>
#include "Shift.h"
#include "PatientType.h"
#include "DiseaseStage.h"
#include "Geography.h"
#include "Probabilities.h"

/* Agents */
class RepastHPCAgent{
    // Geography
    Geography geography;

private:
    // Personal atributes
    repast::AgentId id_;
    // Process where agent's workplace is
    int processWork;
    int processHome;
	int age;

    // Probabilities
    Probabilities probabilities;

    // Tick convertion
    TickConverter tickConverter;
    double currentTick;

    // family

    // Position
    double xcoord;
    double ycoord;
    std::vector<double> agentLoc;
    const double MAX_MOVEMENT_IN_DESTINATION = 50;

    // Routine attributes
    bool atHome;
    Shift workShift;
    double wakeUpTime;
    double returnToHomeTime;

    // Health attributes
    DiseaseStage diseaseStage;
    PatientType patientType;
    double incubationTime;
    double incubationShift;
    double ticksToInfected;
    bool diseaseStageEnd;
    double ticksToDiseaseEnd;
    int infections = 0;

    // Projection attributes
    std::vector<double> homeplace;
    std::vector<double> workplace;

    // Displacement factor: unit (meters)
    double displacementFactor = 50;

public:
    RepastHPCAgent(repast::AgentId id);
	RepastHPCAgent(){}
    ~RepastHPCAgent();

    /* Initialization methods */
    void initAgent(double xhome, double yhome, double xwork, double ywork);

    /* Required Getters */
    virtual repast::AgentId& getId(){return id_;}
    virtual const repast::AgentId& getId() const {return id_;}

    // Process of agent's workplace location
    int getProcessWork(){return processWork;}
    void setProcessWork(int newProcessWork);

    int getProcessHome(){return processHome;}
    void setProcessHome(int newProcessHome);

    /* Getters specific to this kind of Agent */
    int getAge(){ return age;}
    void setAge(int newAge);

    // Disease
    DiseaseStage getDiseaseStage(){return diseaseStage;}
    void setDiseaseStage(DiseaseStage newDiseaseStage);

    // Position
    double getXCoord(){return xcoord;}
    void setXCoord(double newXCoord);

    double getYCoord(){return ycoord;}
    void setYCoord(double newYCoord);

    std::vector<double> getHomePlace(){return homeplace;}
    void setHomePlace(std::vector<double> newHomeplace);

    std::vector<double> getWorkPlace(){return workplace;}
    void setWorkPlace(std::vector<double> newWorkplace);

    double getDistance(double x, double y);

    bool getAtHome(){return atHome;}
    void setAtHome(bool new_atHome);

    Shift getWorkShit(){return workShift;}
    void setWorkShift(Shift new_workShift);

    // Times
    double getWakeUpTime(){return wakeUpTime;}
    void setWakeUpTime(double newWakeUpTime);

    double getReturnToHomeTime(){return returnToHomeTime;}
    void setReturnToHomeTime(double newReturnToHomeTime);

    // Disease actions
    void initDisease();
    void setInfected();
    void setExposed();

    PatientType getPatientType(){return patientType;}
    double getIncubationTime(){return incubationTime;}

    double getIncubatioShift(){return incubationShift;}
    void setIncubationShift(double newIncubationShift);
    void stepIncubationShift(double stepIncubationShift);

    double getTicksToInfected(){return ticksToInfected;}
    void setTicksToInfected(double newTicksToInfected);

    bool getDiseaseStageEnd(){return diseaseStageEnd;}
    void setDiseaseStageEnd(bool newDiseaseStageEnd);

    double getTicksToDiseaseEnd(){return ticksToDiseaseEnd;}
    void setTicksToDiseaseEnd(double newTicksToDiseaseEnd);

    void setImmune();
    void setDead();

    int getInfections(){return infections;}
    void setInfections(int newInfections);

    /* Setter */
    void set(int currentRank);

    /* Actions */
    bool cooperate();// Will indicate whether the agent cooperates or not; probability determined by = c / total
    void play(repast::SharedContext<RepastHPCAgent>* context);// Choose three other agents from the given context and see if they cooperate or not

    // Wake up
    void wakeUp(repast::SharedContinuousSpace<RepastHPCAgent, repast::StrictBorders, repast::SimpleAdder<RepastHPCAgent> >* space);

    // Return home
    void returnHome(repast::SharedContinuousSpace<RepastHPCAgent, repast::StrictBorders, repast::SimpleAdder<RepastHPCAgent> >* space);

    // Ask agent to move
    void move(repast::SharedContinuousSpace<RepastHPCAgent, repast::StrictBorders, repast::SimpleAdder<RepastHPCAgent> >* space, double minX, double maxX, double minY, double maxY);

    // Ask if agent is active case
    bool isActiveCase();

    // Actions according to disease stage of agent
    void diseaseActions(double currentTick);

};

/* Serializable Agent Package */
struct RepastHPCAgentPackage {
public:
    int id;
    int rank;
    int type;
    int currentRank;
    int processWork;
    int processHome;
    int age;
    bool atHome;
    Shift workShift;
    double wakeUpTime;
    double returnToHomeTime;
    DiseaseStage diseaseStage;
    PatientType patientType;
    double incubationTime;
    double incubationShift;
    double ticksToInfected;
    bool diseaseStageEnd;
    double ticksToDiseaseEnd;
    int infections;
    std::vector<double> homeplace;
    std::vector<double> workplace;
    double xcoord;
    double ycoord;

    /* Constructors */
    RepastHPCAgentPackage(); // For serialization
    RepastHPCAgentPackage(int _id, int _rank, int _type, int _currentRank, int _processWork, int _processHome, int _age,
    bool _atHome, Shift _workShift, double _wakeUpTime, double _returnToHomeTime,
    DiseaseStage _diseaseStage, PatientType patientType,
    double incubationTime, double incubationShift, double ticksToInfected, bool diseaseStageEnd, double ticksToDiseaseEnd, int infections,
    std::vector<double> _homeplace, std::vector<double> _workplace,
    double _xcoord, double _ycoord);

    /* For archive packaging */
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version){
        ar & id;
        ar & rank;
        ar & type;
        ar & currentRank;
        ar & processWork;
        ar & processHome,
        ar & age;
        ar & atHome;
        ar & workShift;
        ar & wakeUpTime;
        ar & returnToHomeTime;
        ar & diseaseStage;
        ar & homeplace;
        ar & workplace;
        ar & xcoord;
        ar & ycoord;
        ar & ticksToInfected;
        ar & diseaseStageEnd;
        ar & ticksToDiseaseEnd;
        ar & infections;
    }
};

#endif