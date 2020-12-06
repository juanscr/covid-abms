#ifndef POLICY
#define POLICY

enum Policy{
    NONE, FULL_QUARANTINE, ID_BASED_CURFEW
};

// Struct for reading policies
struct policy{
    Policy p;
    double start;
    double end;
    int ageMin;
    int ageMax;
    double factor;
    std::vector<std::vector<int>> curfew;
};

#endif