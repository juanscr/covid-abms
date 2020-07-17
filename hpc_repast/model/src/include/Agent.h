/* Demo_03_Agent.h */

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

/* Agents */
class RepastHPCAgent{
	
private:
    // Personal atributes
    repast::AgentId id_;
    double c;
    double total;
	int age;
    // family

    // Routine attributes
    bool atHome;
    bool workShift;
    double wakeUpTime;
    double returnToHomeTime;

    // Health attributes
    DiseaseStage diseaseStage;
    PatientType patientType;
    double timeToDeath;
    double timeToImmune;
    double incubationTime;
    double incubationShift;

    // Projection attributes
    std::vector<double> homeplace;
    std::vector<double> workplace;
    
    
public:
    RepastHPCAgent(repast::AgentId id);
	RepastHPCAgent(){}
    RepastHPCAgent(repast::AgentId id, double newC, double newTotal, int newAge,DiseaseStage diseaseStage);
	
    ~RepastHPCAgent();
	
    /* Required Getters */
    virtual repast::AgentId& getId(){return id_;}
    virtual const repast::AgentId& getId() const {return id_;}
	
    /* Getters specific to this kind of Agent */
    double getC(){return c;}
    double getTotal(){return total;}
    int getAge(){ return age; }
    
    bool getAtHome(){return atHome;}
    void setAtHome(bool new_atHome);

    bool getWorkShit(){return workShift;}
    void setWorkShift(Shift new_workShift);


    double getWakeUpTime(){return wakeUpTime;}
    double getReturnToHomeTime(){return returnToHomeTime;}
    
    DiseaseStage getDiseaseStage(){return diseaseStage;}
    
    PatientType getPatientType(){return patientType;}
    double getTimeToDeath(){return timeToDeath;}
    double getTimeToImmune(){return timeToImmune;}
    double getIncubationTime(){return incubationTime;}
    double getIncubatioShift(){return incubationShift;}
    std::vector<double> getHomePlace(){return homeplace;}
    std::vector<double> getWorkPlace(){return workplace;}
    

    /* Setter */
    void set(int currentRank, double newC, double newTotal, int newAge,DiseaseStage diseaseStage);
	
    /* Actions */
    bool cooperate();// Will indicate whether the agent cooperates or not; probability determined by = c / total
    void play(repast::SharedContext<RepastHPCAgent>* context);// Choose three other agents from the given context and see if they cooperate or not
    void move(repast::SharedContinuousSpace<RepastHPCAgent, repast::StrictBorders, repast::SimpleAdder<RepastHPCAgent> >* space);
    
};

/* Serializable Agent Package */
struct RepastHPCAgentPackage {
	
public:
    int    id;
    int    rank;
    int    type;
    int    currentRank;
    double c;
    double total;
    int age;
    bool atHome;
    bool dayShift;
    double wakeUpTime;
    double returnToHomeTime;
    DiseaseStage diseaseStage;
    PatientType patientType;
    double timeToDeath;
    double timeToImmune;
    double incubationTime;
    double incubationShift;
    std::vector<double> homeplace;
    std::vector<double> workplace;
	
    /* Constructors */
    RepastHPCAgentPackage(); // For serialization
    RepastHPCAgentPackage(int _id, int _rank, int _type, int _currentRank, double _c, double _total, int _age,
    bool _atHome, bool _dayShift, double _wakeUpTime, double _returnToHomeTime,
    DiseaseStage _diseaseStage, PatientType patientType, double timeToDeath, double timeToImmune, double incubationTime, double incubationShift,
    std::vector<double> _homeplace, std::vector<double> _workplace);
	
    /* For archive packaging */
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version){
        ar & id;
        ar & rank;
        ar & type;
        ar & currentRank;
        ar & c;
        ar & total;
        ar & age;
        ar & atHome;
        ar & dayShift;
        ar & wakeUpTime;
        ar & returnToHomeTime;
        ar & diseaseStage;
        ar & homeplace;
        ar & workplace;
    }
	
};


#endif