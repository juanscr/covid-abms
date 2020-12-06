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
    private:
        // Current policies
        std::vector<policy> currentPolicies;
        // Added policies to schedule
        std::vector<policy> addedPolicies;

    public:
        // Create Policy Enforcer
        PolicyEnforcer();

        // Schedule a policy
        void schedulePolicy(policy p, repast::ScheduleRunner& runner);

        // Add a policy
        void addPolicy();

        // Remove a policy
        void removePolicy();

        // Is the citizen allowed to go out?
        bool isAllowedToGoOut(bool isolation, RepastHPCAgent* agent, int hour, double, int day);
};
#endif