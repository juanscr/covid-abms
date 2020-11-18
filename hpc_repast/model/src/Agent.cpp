/*Agent.cpp */
#include "Agent.h"
#include "Geography.h"

RepastHPCAgent::RepastHPCAgent(repast::AgentId id): id_(id){}

RepastHPCAgent::~RepastHPCAgent(){ }

/* Setters */
void RepastHPCAgent::set(int currentRank){
    id_.currentRank(currentRank);
}

void RepastHPCAgent::setProcessWork(int newProcessWork){
    processWork = newProcessWork;
}

void RepastHPCAgent::setProcessHome(int newProcessHome){
    processHome = newProcessHome;
}

void RepastHPCAgent::initAgent(int stopAt){
	setAtHome(true);
	setWorkShift(Probabilities::getRandomWorkShift(repast::Random::instance()->nextDouble()));
    setWakeUpTime(Probabilities::getRandomWakeUpTime(getWorkShit()));
    setReturnToHomeTime(Probabilities::getRandomReturnToHomeTime(getWorkShit()));
}

void RepastHPCAgent::setAge(int newAge){
    age = newAge;
}

void RepastHPCAgent::setFamily(int newFamily){
    family = newFamily;
}

void RepastHPCAgent::setXCoord(double newXCoord){
    xcoord = newXCoord;
}

void RepastHPCAgent::setYCoord(double newYCoord){
    ycoord = newYCoord;
}

void RepastHPCAgent::setHomePlace(std::vector<double> newHomeplace){
    homeplace = newHomeplace;
}

void RepastHPCAgent::setWorkPlace(std::vector<double> newWorkplace){
    workplace = newWorkplace;
}


void RepastHPCAgent::setHomeZone(std::string newHomeZone){
    homeZone = newHomeZone;
};

void RepastHPCAgent::setWorkZone(std::string newWorkZone){
    workZone = newWorkZone;
};

void RepastHPCAgent::setAverageWalk(double newAverageWalk){
    averageWalk = newAverageWalk;
}

void RepastHPCAgent::setAtHome(bool new_atHome){
    atHome = new_atHome;
}

void RepastHPCAgent::setWorkShift(Shift new_workShift){
    workShift = new_workShift;
}

void RepastHPCAgent::setDiseaseStage(DiseaseStage newDiseaseStage){
    diseaseStage = newDiseaseStage;
}

void RepastHPCAgent::setWakeUpTime(double newWakeUpTime){
    wakeUpTime = newWakeUpTime;
}

void RepastHPCAgent::setReturnToHomeTime(double newReturnToHomeTime){
    returnToHomeTime = newReturnToHomeTime;
}

void RepastHPCAgent::setIncubationShift(double newIncubationShift){
    incubationShift = newIncubationShift;
}

void RepastHPCAgent::stepIncubationShift(double stepIncubationShift){
    incubationShift += stepIncubationShift;
}

void RepastHPCAgent::setTicksToInfected(double newTicksToInfected){
    ticksToInfected = newTicksToInfected;
}

void RepastHPCAgent::setDiseaseStageEnd(bool newDiseaseStageEnd){
    diseaseStageEnd = newDiseaseStageEnd;
}

void RepastHPCAgent::setTicksToDiseaseEnd(double newTicksToDiseaseEnd){
    ticksToDiseaseEnd =  newTicksToDiseaseEnd;
}

double RepastHPCAgent::getDistance(double x, double y){
    return Geography::getDistance(xcoord, ycoord, x, y);
}

void RepastHPCAgent::initDisease(){
    switch (diseaseStage)
    {
    case EXPOSED:
        setExposed();
        break;
    case INFECTED:
        setInfected();
        break;
    default:
        break;
    }
}

void RepastHPCAgent::wakeUp(){
    // Go to workplace
    atHome = false;
    std::vector<double> agentloc;

    // Update coordinates
    xcoord = workplace.at(0);
    ycoord = workplace.at(1);

    // Set workplace position
    agentloc.push_back(xcoord);
    agentloc.push_back(ycoord);
}

void RepastHPCAgent::returnHome(){
    // Go to workplace
    atHome = true;
    std::vector<double> agentloc;

    // Update coordinates
    xcoord = homeplace.at(0);
    ycoord = homeplace.at(1);

    // Set workplace position
    agentloc.push_back(xcoord);
    agentloc.push_back(ycoord);
}

void RepastHPCAgent::move(int rank, std::vector<Border*> p, double minX, double maxX, double minY, double maxY){
    double newX, newY;
    double ax, ay;
    double distance;
    int core;

    if(atHome){
        ax = homeplace.at(0);
        ay = homeplace.at(1);
        distance = averageWalk;
    }else{
        ax = workplace.at(0);
        ay = workplace.at(1);
        distance = MAX_MOVEMENT_IN_DESTINATION;
    }

    if (diseaseStage != DEAD){
        // Generate position
        core = Geography::genDistancePoly(rank, id_, p, ax, ay, xcoord, ycoord, distance, &newX, &newY);
        cr = core;

        // Update coordinates
        xcoord = newX;
        ycoord = newY;
    }else{
        if(atHome){
            cr = processHome;
        }else{
            cr = processWork;
        }
    }
}

bool RepastHPCAgent::isActiveCase(){
    return diseaseStage == INFECTED || diseaseStage == EXPOSED;
}

void RepastHPCAgent::setExposed(){
    // Get day of infection
    currentTick = repast::RepastProcess::instance()->getScheduleRunner().currentTick();

    // Change disease status
    diseaseStage = EXPOSED;

    // Get incubation period
    double incubationPeriod = Probabilities::getRandomIncubationPeriod();
    incubationShift = TickConverter::daysToTicks(incubationPeriod);

    // Get infectivity period
    double infectiousPeriod = max(incubationPeriod + (Probabilities::INFECTION_MIN), 1.0);
    ticksToInfected = currentTick + TickConverter::daysToTicks(infectiousPeriod);
}

void RepastHPCAgent::setInfected(){
    // Get day of final disease stage
    currentTick = repast::RepastProcess::instance()->getScheduleRunner().currentTick();

    double r1 = repast::Random::instance()->nextDouble();
    double r2 = repast::Random::instance()->nextDouble();
    double r3 = repast::Random::instance()->nextDouble();

    diseaseStage = INFECTED;

    PatientType patient = Probabilities::getRandomPatientType(r1, r2);

    if (Probabilities::isGoingToDie(r3, patient)){
        diseaseStageEnd = true;
    }else{
        diseaseStageEnd = false;
    }

    double daysToEvent = Probabilities::getRandomTimeToDischarge() - Probabilities::INFECTION_MIN;
    ticksToDiseaseEnd = currentTick + TickConverter::daysToTicks(daysToEvent);
}

void RepastHPCAgent::diseaseActions(double currentTick){
    if (diseaseStage == EXPOSED){
        if ( (currentTick-1) <= ticksToInfected && ticksToInfected <= currentTick ){
            setInfected();
        }
    } else if (diseaseStage == INFECTED){
        if ( (currentTick-1) <= ticksToDiseaseEnd && ticksToDiseaseEnd <= currentTick ){
            if (diseaseStageEnd){
                setDead();
            }else{
                setImmune();
            }
        }
    }
}

void RepastHPCAgent::setImmune(){
    diseaseStage = IMMUNE;
}

void RepastHPCAgent::setDead(){
    diseaseStage = DEAD;
}

void RepastHPCAgent::setInfections(int newInfections){
    infections = newInfections;
}

/* Serializable Agent Package Data */
AgentPackage::AgentPackage(){ }

AgentPackage::AgentPackage(int _id, int _rank, int _type, int _currentRank, int _processWork, int _processHome,
int _age, int _family,
bool _atHome, Shift _workShift, double _wakeUpTime, double _returnToHomeTime,
DiseaseStage _diseaseStage, PatientType _patientType,
double _incubationTime, double _incubationShift, double _ticksToInfected, bool _diseaseStageEnd, double _ticksToDiseaseEnd, int _infections,
std::vector<double> _homeplace, std::vector<double> _workplace,
double _xcoord, double _ycoord, double _averageWalk, std::string _homeZone, std::string _workZone):
id(_id), rank(_rank), type(_type), currentRank(_currentRank), processWork(_processWork), processHome(_processHome),
age(_age), family(_family), atHome(_atHome),
workShift(_workShift), wakeUpTime(_wakeUpTime), returnToHomeTime(_returnToHomeTime),
diseaseStage(_diseaseStage), patientType(_patientType), incubationTime(_incubationTime), incubationShift(_incubationShift), ticksToInfected(_ticksToInfected), diseaseStageEnd(_diseaseStageEnd), ticksToDiseaseEnd(_ticksToDiseaseEnd), infections(_infections),
homeplace(_homeplace), workplace(_workplace), xcoord(_xcoord), ycoord(_ycoord), averageWalk(_averageWalk), homeZone(_homeZone), workZone(_workZone) { }