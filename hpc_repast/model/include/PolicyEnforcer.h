#ifndef POLICYENFORCER_H
#define POLICYENFORCER_H

#include "repast_hpc/AgentId.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/SharedContinuousSpace.h"
#include <stdio.h>
#include <vector>
#include "TickConverter.h"
#include "Policy.h"
#include "Agent.h"

class PolicyEnforcer{
    public:
        // Current policies
        std::vector<policy> currentPolicies;

        // Added policies to schedule
        std::vector<policy> addedPolicies;

        // Create Policy Enforcer
        PolicyEnforcer();

        // Schedule a policy
        void schedulePolicy(policy p, repast::ScheduleRunner& runner);

        // Add a policy
        void addPolicy();

        // Remove a policy
        void removePolicy();

        // Is the citizen allowed to go out?
        bool isAllowedToGoOut(repast::Random* r, bool isolation, RepastHPCAgent* agent, int hour, double, int day);

        // Policy compliance by socioeconomic stratum
        compliance cmp;

        // Mask usage by socioeconomic stratum
        maskUsage msu;
};
#endif