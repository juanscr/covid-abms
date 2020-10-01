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

void RepastHPCAgent::initAgent(double xhome, double yhome, double xwork, double ywork){
    age = Probabilities::getRandomAge(repast::Random::instance()->nextDouble(), repast::Random::instance()->nextDouble());
    diseaseStage = SUSCEPTIBLE;

    // Set spatial and temporal attributes to agents
	setXCoord(xhome);
	setYCoord(yhome);
	setHomePlace(std::vector<double> {xhome, yhome});
	setWorkPlace(std::vector<double> {xwork, ywork});
	setAtHome(true);
	setWorkShift(Probabilities::getRandomWorkShift(repast::Random::instance()->nextDouble()));

    setWakeUpTime(Probabilities::getRandomWakeUpTime(getWorkShit()));
    setReturnToHomeTime(Probabilities::getRandomReturnToHomeTime(getWorkShit()));

}

void RepastHPCAgent::setAge(int newAge){
    age = newAge;
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

void RepastHPCAgent::wakeUp(repast::SharedContinuousSpace<RepastHPCAgent, repast::StrictBorders, repast::SimpleAdder<RepastHPCAgent> >* space){
    // Go to workplace
    atHome = false;

    // Update coordinates
    xcoord = workplace.at(0);
    ycoord = workplace.at(1);

    // Set workplace position
    agentLoc.clear();
    agentLoc.push_back(xcoord);
    agentLoc.push_back(ycoord);

    if (!space->bounds().contains(agentLoc)){
        std::cout << id_ << " not in bounds " << std::endl;
    }

    // Update location
    space->moveTo(id_, agentLoc);
}

void RepastHPCAgent::returnHome(repast::SharedContinuousSpace<RepastHPCAgent, repast::StrictBorders, repast::SimpleAdder<RepastHPCAgent> >* space){
    // Go to workplace
    atHome = true;

    // Update coordinates
    xcoord = homeplace.at(0);
    ycoord = homeplace.at(1);

    // Set workplace position
    agentLoc.clear();
    agentLoc.push_back(xcoord);
    agentLoc.push_back(ycoord);

    // Update location
    space->moveTo(id_, agentLoc);
}

void RepastHPCAgent::move(polygon p, repast::SharedContinuousSpace<RepastHPCAgent, repast::StrictBorders, repast::SimpleAdder<RepastHPCAgent> >* space, double minX, double maxX, double minY, double maxY){
    std::vector<double> agentLoc;
    space->getLocation(id_, agentLoc);
    double newX, newY;

    if (diseaseStage != DEAD){
        //Geography::genDistance(minX, maxX, minY, maxY, agentLoc[0], agentLoc[1], MAX_MOVEMENT_IN_DESTINATION, &newX, &newY);
        Geography::genDistancePoly(p, agentLoc[0], agentLoc[1], MAX_MOVEMENT_IN_DESTINATION, &newX, &newY);

        // Update coordinates
        xcoord = newX;
        ycoord = newY;

        // Set location
        agentLoc.clear();

        agentLoc.push_back(newX);
        agentLoc.push_back(newY);

        // Update location
        space->moveTo(id_, agentLoc);
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
    }else
    {
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

RepastHPCAgentPackage::RepastHPCAgentPackage(){ }

RepastHPCAgentPackage::RepastHPCAgentPackage(int _id, int _rank, int _type, int _currentRank, int _processWork, int _processHome,
int _age,
bool _atHome, Shift _workShift, double _wakeUpTime, double _returnToHomeTime,
DiseaseStage _diseaseStage, PatientType _patientType,
double _incubationTime, double _incubationShift, double _ticksToInfected, bool _diseaseStageEnd, double _ticksToDiseaseEnd, int _infections,
std::vector<double> _homeplace, std::vector<double> _workplace,
double _xcoord, double _ycoord):
id(_id), rank(_rank), type(_type), currentRank(_currentRank), processWork(_processWork), processHome(_processHome),
age(_age), atHome(_atHome),
workShift(_workShift), wakeUpTime(_wakeUpTime), returnToHomeTime(_returnToHomeTime),
diseaseStage(_diseaseStage), patientType(_patientType), incubationTime(_incubationTime), incubationShift(_incubationShift), ticksToInfected(_ticksToInfected), diseaseStageEnd(_diseaseStageEnd), ticksToDiseaseEnd(_ticksToDiseaseEnd), infections(_infections),
homeplace(_homeplace), workplace(_workplace), xcoord(_xcoord), ycoord(_ycoord) { }