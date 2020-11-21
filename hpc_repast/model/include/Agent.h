/* Agent.h */
#ifndef AGENT
#define AGENT

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

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/multi_polygon.hpp>

/* Agents */
class RepastHPCAgent{

private:
    // Personal atributes
    repast::AgentId id_;
    // Process where agent's workplace is
    int processWork;
    int processHome;
	int age;
    int family;

    // Tick converter
    double currentTick;

    // Boost geography
    typedef boost::geometry::model::d2::point_xy<double> point;
    typedef boost::geometry::model::polygon<point> polygon;
    typedef boost::geometry::model::multi_polygon<polygon> multipolygon;

    // Position
    double xcoord;
    double ycoord;
    std::vector<double> agentLoc;
    const double MAX_MOVEMENT_IN_DESTINATION = 500;

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
    std::string homeZone;
    std::string workZone;
    double averageWalk;

public:
    RepastHPCAgent(repast::AgentId id);
	RepastHPCAgent(){}
    ~RepastHPCAgent();

    // Current core
    int cr;

    /* Initialization methods */
    void initAgent(int stopAt);

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

    // Family
    int getFamily(){return family;}
    void setFamily(int newFamily);

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

    std::string getHomeZone(){return homeZone;}
    void setHomeZone(std::string newHomeZone);

    std::string getWorkZone(){return workZone;}
    void setWorkZone(std::string newWorkZone);

    double getAverageWalk(){return averageWalk;}
    void setAverageWalk(double newAverageWalk);

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

    // Wake up
    void wakeUp();

    // Return home
    void returnHome();

    // Ask agent to move
    void move(int rank, std::vector<Border*> p, double minX, double maxX, double minY, double maxY);

    // Ask if agent is active case
    bool isActiveCase();

    // Actions according to disease stage of agent
    void diseaseActions(double currentTick);
};

/* Serializable Agent Package */
struct AgentPackage {
public:
    int id;
    int rank;
    int type;
    int currentRank;
    int processWork;
    int processHome;
    int age;
    int family;
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
    double averageWalk;
    std::string homeZone;
    std::string workZone;

    /* Constructors */
    AgentPackage(); // For serialization
    AgentPackage(int _id, int _rank, int _type, int _currentRank, int _processWork, int _processHome, int _age, int _family,
    bool _atHome, Shift _workShift, double _wakeUpTime, double _returnToHomeTime,
    DiseaseStage _diseaseStage, PatientType patientType,
    double incubationTime, double incubationShift, double ticksToInfected, bool diseaseStageEnd, double ticksToDiseaseEnd, int infections,
    std::vector<double> _homeplace, std::vector<double> _workplace,
    double _xcoord, double _ycoord, double _averageWalk, std::string _homeZone, std::string _workZone);

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
        ar & family;
        ar & atHome;
        ar & workShift;
        ar & wakeUpTime;
        ar & returnToHomeTime;
        ar & diseaseStage;
        ar & homeplace;
        ar & workplace;
        ar & xcoord;
        ar & ycoord;
        ar & averageWalk;
        ar & homeZone;
        ar & workZone;
        ar & ticksToInfected;
        ar & diseaseStageEnd;
        ar & ticksToDiseaseEnd;
        ar & infections;
    }
};

#endif