#ifndef POLICYENFORCER_H
#define POLICYENFORCER_H

#include "repast_hpc/AgentId.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/SharedContinuousSpace.h"
#include <stdio.h>
#include <vector>
#include "TickConverter.h"
#include "Policy.h"

class PolicyEnforcer{
    private:
        // Current policies
        std::vector<Policy> currentPolicies;
        // Added policies to schedule
        std::vector<Policy> addedPolicies;

        // Curfew IDs for each day. Reference: <pending>
        std::vector < std::vector<int> > curfewIds  {{7, 8}, {9, 0}, {1, 2}, {3, 4}, {5, 6}, {7, 8}, {9, 0, 1}};

    public:
        // Variable to add policy

        // Create Policy Enforcer
        PolicyEnforcer();

        // Schedule a policy
        void schedulePolicy(Policy policy, double startDay, double endDay, repast::ScheduleRunner& runner);

        // Add a policy
        void addPolicy();

        // Remove a policy
        void removePolicy();

        // Is the citizen allowed to go out?
        bool isAllowedToGoOut(repast::AgentId id_, int ctick);
};
#endif