#ifndef POLICY
#define POLICY

enum Policy{
    NONE, FULL_QUARANTINE, ID_BASED_CURFEW, LOCKDOWN
};

// Struct for reading policies
struct policy{
    Policy p;
    double start;
    double end;
    int ageMin;
    int ageMax;
    int hourStart;
    int hourEnd;
    double factor;
    std::vector<std::vector<int>> curfew;
    bool mask;
};

// Struct for compliance
struct compliance{
    std::vector<double> values;
    std::vector<double> std;
};

// Struct for compliance
struct maskUsage{
    std::vector<double> values;
    std::vector<double> std;
};

#endif