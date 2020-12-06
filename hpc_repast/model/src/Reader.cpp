#include "Reader.h"

// Read the bounds of model
void Reader::getBounds(std::string path, std::string filename, double* xmin, double* extX, double* ymin, double* extY){
    std::string filepath = path + filename;

    // Read the json file
    pt::ptree data;
    pt::read_json(filepath, data);

    // Set values
    *xmin = data.get<double>("xmin");
    *extX = data.get<double>("xext");
    *ymin = data.get<double>("ymin");
    *extY = data.get<double>("yext");
}

// Read borders of polygons
void Reader::getBorders(std::string path, std::string filename, int rank, std::vector<Zone*>& zones, std::vector<Border*>& border){
    std::string filepath = path + filename + std::to_string(rank) + ".json";
    std::vector<int> cores;

    // Read the json file
    pt::ptree data;
    pt::read_json(filepath, data);
    std::string key;
    std::string wkt;

    // Number of SIT Zones
    int n = data.get<int>("len");

    // Read polygon
    Border* br = new Border();
    std::string b = data.get<std::string>("border.polygon");
    multipolygon poly;
    boost::geometry::read_wkt(b, poly);
    br->core = rank;
    br->border = poly;
    border.push_back(br);

    // Read cores ID
    for(pt::ptree::value_type & coreID : data.get_child("cores")){
        cores.push_back(std::stoi(coreID.second.data()));
    }

    // Read adjacent borders to process
    int m = data.get<int>("border.n");

    for (int i = 0; i < m; i++){
        Border* br = new Border();
        multipolygon mpol;
        key = "border.n" +  std::to_string(i);
        wkt = data.get<std::string>(key);
        boost::geometry::read_wkt(wkt, mpol);
        // Set data to structure
        br->border = mpol;
        br->core = cores.at(i+1);
        border.push_back(br);
    }

    // Read SIT Zones
    for(int i = 0; i < n; i++){
        // Create SIT Zone
        Zone* z = new Zone();
        z->ID = i;
        z->core = rank;

        // Read attributes
        key = std::to_string(i);
        z->label = data.get<std::string>(key + ".id");
        z->walkingAverage = data.get<double>(key + ".walk");

        // Create polygon
        wkt = data.get<std::string>(key + ".polygon");
        z->wkt = wkt;
        z->createMultipolygon();

        // Read neighbors ID
        for(pt::ptree::value_type & neighborID : data.get_child(key + ".neighborsCore")){
            z->adjacentZonesID.push_back(std::stoi(neighborID.second.data()));
        }

        // Read neighbors Label
        for(pt::ptree::value_type & neighborLabel : data.get_child(key + ".neighbors")){
            z->adjacentZonesLabel.push_back(std::stoi(neighborLabel.second.data()));
        }

        zones.push_back(z);
     }
}

// Read agents
void Reader::getAgents(std::string path, std::string filename, int rank, int* n, std::vector<RepastHPCAgent*>& agents){
    std::string filepath = path + filename + std::to_string(rank) + ".json";

    // Read agents data
    pt::ptree data;
    pt::read_json(filepath, data);

    // Number of agents
    *n = data.get<int>("len");

    // key for get agents data
    std::string key;
    int d;

    for (int i = 0; i < *n; i++) {
        key = std::to_string(i);
        repast::AgentId id(i, rank, 0);
        id.currentRank(rank);

        RepastHPCAgent* agent = new RepastHPCAgent(id);

        // Set personal attributes
        agent->setAge(data.get<int>(key + ".age"));
        agent->setFamily(data.get<int>(key + ".family"));
        agent->setStratum(data.get<int>(key + ".stratum"));

        // Set disease stage
        d = data.get<int>(key + ".diseaseStage");
        agent->setDiseaseStage(checkDiseaseStage(d));

        // Homeplace
        agent->setProcessHome(data.get<int>(key + ".homeCore"));
        agent->setHomePlace({data.get<double>(key + ".homeX"), data.get<double>(key + ".homeY")});
        agent->setHomeZone(data.get<std::string>(key + ".homeZoneName"));
        agent->setAverageWalk(data.get<double>(key + ".homeWalk"));

        // Workplace
        agent->setProcessWork(data.get<int>(key + ".workCore"));
        agent->setWorkPlace({data.get<double>(key + ".workX"), data.get<double>(key + ".workY")});
        agent->setWorkZone(data.get<std::string>(key + ".workZoneName"));

        // Initial location
        agent->setXCoord(data.get<double>(key + ".homeX"));
        agent->setYCoord(data.get<double>(key + ".homeY"));

        // Save agents
        agents.push_back(agent);
    }
}

DiseaseStage Reader::checkDiseaseStage(int d){
    switch (d){
        case 0:
            return SUSCEPTIBLE;
            break;
        case 1:
            return EXPOSED;
            break;
        case 2:
            return INFECTED;
            break;
        case 3:
            return IMMUNE;
            break;
        case 4:
            return DEAD;
            break;
        default:
            return SUSCEPTIBLE;
            break;
    }
}

void Reader::getPolicies(int crank, std::string path, std::string filename, int stopAt, std::vector<policy>& policies){
    std::string filepath = path + filename;

    // Read policies data
    pt::ptree data;
    pt::read_json(filepath, data);

    // Number of policies
    int  n = data.get<int>("len");

    std::string key;
    int d;

    for (int i = 0; i < n; i++){
        key =  std::to_string(i);

        // Create a policy
        policy pol;

        // ID of policy
        int d = data.get<int>(key + ".type");

        // Set properties
        //type
        pol.p = checkPolicyType(d);

        //start
        pol.start = data.get<double>(key + ".start");

        //end
        if (i == n-1){
            pol.end = stopAt;
        }else{
            pol.end = data.get<double>(key + ".end");
        }

        // Add allowed ages
        if(d== 1 || d == 2){
            pol.ageMin = data.get<int>(key + ".ageMin");
            pol.ageMax = data.get<int>(key + ".ageMax");
        }

        // Add curfews if apply
        if (d == 2){
            // Set curfew
            for(pt::ptree::value_type &row : data.get_child(key + ".curfew")){

                // IDs of day
                std::vector<int> curfDay;

                for(pt::ptree::value_type &cell : row.second){
                    curfDay.push_back(cell.second.get_value<int>());
                }

                // Add day to curfew vector
                pol.curfew.push_back(curfDay);

            }
        }

        // Factor of allowed agents that decide to move
        pol.factor = data.get<double>(key + ".factor");

        // Is obligatory to use a mask?
        pol.mask  = (data.get<int>(key + ".mask") == 1);

        // Append to vector of policies
        policies.push_back(pol);
    }
}

void Reader::getPolicyInfo(std::string path, std::string filename, compliance& cmp, maskUsage& msu){
    std::string filepath = path + filename;

    // Read policy info
    pt::ptree data;
    pt::read_json(filepath, data);

    // Values of policy compliance
    std::vector<double> pc;
    for(pt::ptree::value_type & value : data.get_child("compliance.values")){
        pc.push_back(std::stoi(value.second.data()));
    }

    // Deviations of policy compliance
    std::vector<double> ps;
    for(pt::ptree::value_type & value : data.get_child("compliance.std")){
        ps.push_back(std::stoi(value.second.data()));
    }

    // Set values for policy compliance
    cmp.values = pc;
    cmp.std = ps;

    // Values of mask usage
    std::vector<double> mc;
    for(pt::ptree::value_type & value : data.get_child("maskUsage.values")){
        mc.push_back(std::stoi(value.second.data()));
    }

    // Deviations of mask usage
    std::vector<double> ms;
    for(pt::ptree::value_type & value : data.get_child("maskUsage.std")){
        ms.push_back(std::stoi(value.second.data()));
    }

    // Set value for mask usage
    msu.values = mc;
    msu.std = ms;
}

Policy Reader::checkPolicyType(int d){
    switch (d)
    {
    case 0:
        return NONE;
        break;
    case 1:
        return FULL_QUARANTINE;
        break;
    case 2:
        return ID_BASED_CURFEW;
        break;
    default:
        return NONE;
        break;
    }
}

bool Reader::fileExists(std::string& fileName) {
    return static_cast<bool>(std::ifstream(fileName));
}