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

        // Curfew IDs for each day. Reference: <pending>
        std::vector < std::vector<int> > curfewIds  {{7, 8}, {9, 0}, {1, 2}, {3, 4}, {5, 6}, {7, 8}, {9, 0, 1}};

        // Medellin's historical curferw IDs
        //0: Apr 02 - Apr 12
        std::vector < std::vector<int> > cId1 {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {0, 1, 2}, {3, 4 ,5}, {6, 7, 8}, {9, 0}};

        //1: Apr 13 - Apr 19
        std::vector < std::vector<int> > cId2 {{7, 8, 9}, {0, 1}, {2, 3}, {4, 5}, {6, 7}, {8, 9}, {0, 1}};

        //2: Apr 20 - Apr 26, May 04 - May 10, May 18 - May 24
        std::vector < std::vector<int> > cId3 {{2, 3}, {4, 5}, {6, 7}, {8, 9}, {0, 1}, {2, 3}, {4, 5, 6}};

        //3: Apr 27 - May 03, May 11 - May 17, May 25 - May 31
        std::vector < std::vector<int> > cId4 {{7, 8}, {9, 0}, {1, 2}, {3, 4}, {5, 6}, {7, 8}, {9, 0, 1}};

        //4: Jun 01 - Jun 07, Jun 15 - Jun 21, Jun 29 - Jul 05
        std::vector < std::vector<int> > cId5 {{0, 2, 4, 6, 8}, {1, 3, 5, 7, 9}, {0, 2, 4, 6, 8}, {1, 3, 5, 7, 9}, {0, 2, 4, 6, 8}, {1, 3, 5, 7, 9}, {0, 2, 4, 6, 8}};

        //5: Jun 08 - Jun 14, Jun 22 - 28, Jul 06 - Jul 12
        std::vector < std::vector<int> > cId6 {{1, 3, 5, 7, 9}, {0, 2, 4, 6, 8}, {1, 3, 5, 7, 9}, {0, 2, 4, 6, 8}, {1, 3, 5, 7, 9}, {0, 2, 4, 6, 8}, {1, 3, 5, 7, 9}};

        //6: Jul 13 - Jul 19, Aug 10 - Aug - 16
        std::vector < std::vector<int> > cId7 {{0, 1}, {2, 3}, {4, 5}, {6, 7}, {8, 9}, {-1}, {-1}};

        //7: Jul 20 - Jul 26
        std::vector < std::vector<int> > cId8 {{-1}, {0, 1, 2}, {3, 4, 5}, {6, 7}, {8, 9}, {-1}, {-1}};

        //8: Jul 27 - Aug 02
        std::vector < std::vector<int> > cId9 {{9, 0, 1}, {2, 3, 4}, {5, 6, 7}, {8, 9}, {-1}, {-1}, {-1}};

        //9: Aug 03 - Aug 09
        std::vector < std::vector<int> > cId10 {{0, 1}, {2, 3}, {4, 5, 6}, {7, 8, 9}, {-1}, {-1}, {-1}};

        //10: Ago 17 - Ago 23
        std::vector < std::vector<int> > cId11 {{-1}, {0, 1}, {2, 3}, {4, 5}, {6, 7}, {8, 9}, {0, 1}};

        //11_ Ago 24 - Ago 30
        std::vector < std::vector<int> > cId12 {{0, 1}, {2, 3}, {4, 5}, {6, 7}, {8, 9}, {2, 3}, {4, 5}};

        // Curfew list
        vector<vector<vector<int>>> curfews = {cId1, cId2, cId3, cId4, cId5, cId6, cId7, cId8, cId9, cId10, cId11, cId12};

    public:
        // Variable to add policy

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