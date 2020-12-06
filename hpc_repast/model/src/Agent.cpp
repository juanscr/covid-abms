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

void RepastHPCAgent::initAgent(repast::Random* r){
    atHome = true;

    // Select workshift
	setWorkShift(Probabilities::getRandomWorkShift(r->nextDouble(), age));

    // Get wake up time
    setWakeUpTime(Probabilities::getRandomWakeUpTime(r, workShift));

    // Get return to home time
    setReturnToHomeTime(Probabilities::getRandomReturnToHomeTime(r, workShift));

    // Get sleep time
    Probabilities::getSleepingTime(r, age, wakeUpTime, returnToHomeTime, &sleepStart, &sleepEnd);

    // if(id_.currentRank() == 0){
    //     std::cout << id_ << " wu " << wakeUpTime << " rh " << returnToHomeTime << " ss " << sleepStart << " se " << sleepEnd << std::endl;
    // }
}

void RepastHPCAgent::setAge(int newAge){
    age = newAge;
}

void RepastHPCAgent::setFamily(int newFamily){
    family = newFamily;
}

void RepastHPCAgent::setStratum(int newStratum){
    stratum =  newStratum;
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

void RepastHPCAgent::setWakeUpTime(int newWakeUpTime){
    wakeUpTime = newWakeUpTime;
}

void RepastHPCAgent::setReturnToHomeTime(int newReturnToHomeTime){
    returnToHomeTime = newReturnToHomeTime;
}

void RepastHPCAgent::setSleepStart(int newSleepStart){
    sleepStart = newSleepStart;
}

void RepastHPCAgent::setSleepEnd(int newSleepEnd){
    sleepEnd = newSleepEnd;
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

void RepastHPCAgent::setPatientType(PatientType newPatientType){
    patientType = newPatientType;
}

double RepastHPCAgent::getDistance(double x, double y){
    return Geography::getDistance(xcoord, ycoord, x, y);
}

void RepastHPCAgent::initDisease(repast::Random* r, std::default_random_engine* g){
    switch (diseaseStage)
    {
    case EXPOSED:
        setExposed(r);
        break;
    case INFECTED:
        setInfected(r, g);
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

void RepastHPCAgent::move(repast::Random* r, int rank, std::vector<Border*> p, int hour, double w){
    double newX, newY;
    double ax, ay;
    double distance;
    int core;

    // Check if sleeping at homeplace
    if(isSleeping(hour)){
        return;
    }

    // Check if can move at workplace
    if(isWorking(r, w)){
        return;
    }

    // Agent can move at homemplace or workplace
    if(atHome){
        distance = averageWalk;
    }else{
        distance = MAX_MOVEMENT_IN_DESTINATION;
    }

    if (diseaseStage != DEAD && diseaseStage != IMMUNE){
        // Generate position
        core = Geography::genDistancePoly(r, rank, id_, p, xcoord, ycoord, distance, &newX, &newY);
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

bool RepastHPCAgent::isSleeping(int hour){
    bool inInterval = false;

    if(sleepStart <= sleepEnd){
        if(sleepStart <= hour && hour <= sleepEnd){
            inInterval = true;
        }
    }else{
        if(sleepStart <= hour || hour <= sleepEnd){
            inInterval = true;
        }
    }

    if(inInterval){
        if(!atHome){
            atHome = true;
        }
        xcoord = homeplace.at(0);
        ycoord = homeplace.at(1);
        cr = processHome;
        return true;
    } else {
        return false;
    }
}

bool RepastHPCAgent::isWorking(repast::Random* r, double w){
    if(!atHome && r->nextDouble() > w){
        cr = id_.currentRank();
        return true;
    }else{
        return false;
    }
}

bool RepastHPCAgent::isActiveCase(){
    return diseaseStage == INFECTED || diseaseStage == EXPOSED;
}

void RepastHPCAgent::setExposed(repast::Random* r){
    // Get day of infection
    currentTick = repast::RepastProcess::instance()->getScheduleRunner().currentTick();

    // Change disease status
    diseaseStage = EXPOSED;

    // Get incubation period
    double incubationPeriod = Probabilities::getRandomIncubationPeriod(r);
    incubationShift = TickConverter::daysToTicks(incubationPeriod);

    // Get infectivity period
    double infectiousPeriod = max(incubationPeriod + (Probabilities::INFECTION_MIN), 1.0);
    ticksToInfected = currentTick + TickConverter::daysToTicks(infectiousPeriod);
}

void RepastHPCAgent::setInfected(repast::Random* r, std::default_random_engine* g){
    // Get day of final disease stage
    currentTick = repast::RepastProcess::instance()->getScheduleRunner().currentTick();

    double r1 = r->nextDouble();
    double r2 = r->nextDouble();
    double r3 = r->nextDouble();

    diseaseStage = INFECTED;

    PatientType patient = Probabilities::getRandomPatientType(r1, r2);
    patientType = patient;

    if (Probabilities::isGoingToDie(r3, patient)){
        diseaseStageEnd = true;
    }else{
        diseaseStageEnd = false;
    }

    double daysToEvent = Probabilities::getRandomTimeToDischarge(g, r) - Probabilities::INFECTION_MIN;
    ticksToDiseaseEnd = currentTick + TickConverter::daysToTicks(daysToEvent);
}

void RepastHPCAgent::diseaseActions(repast::Random* r, double currentTick, std::default_random_engine* g){
    if (diseaseStage == EXPOSED){
        if ( (currentTick-1) <= ticksToInfected && ticksToInfected <= currentTick ){
            setInfected(r, g);
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
int _age, int _family, int _stratum,
bool _atHome, Shift _workShift, int _wakeUpTime, int _returnToHomeTime, int _sleepStart, int _sleepEnd,
DiseaseStage _diseaseStage, PatientType _patientType,
double _incubationTime, double _incubationShift, double _ticksToInfected, bool _diseaseStageEnd, double _ticksToDiseaseEnd, int _infections,
std::vector<double> _homeplace, std::vector<double> _workplace,
double _xcoord, double _ycoord, double _averageWalk, std::string _homeZone, std::string _workZone):
id(_id), rank(_rank), type(_type), currentRank(_currentRank), processWork(_processWork), processHome(_processHome),
age(_age), family(_family), stratum(_stratum), atHome(_atHome),
workShift(_workShift), wakeUpTime(_wakeUpTime), returnToHomeTime(_returnToHomeTime),
diseaseStage(_diseaseStage), patientType(_patientType), incubationTime(_incubationTime), incubationShift(_incubationShift), ticksToInfected(_ticksToInfected), diseaseStageEnd(_diseaseStageEnd), ticksToDiseaseEnd(_ticksToDiseaseEnd), infections(_infections),
homeplace(_homeplace), workplace(_workplace), xcoord(_xcoord), ycoord(_ycoord), averageWalk(_averageWalk), homeZone(_homeZone), workZone(_workZone) { }