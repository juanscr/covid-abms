#include "PolicyEnforcer.h"
PolicyEnforcer::PolicyEnforcer(void){}

void PolicyEnforcer::schedulePolicy(policy p, repast::ScheduleRunner& runner){
    // Add policy
    double startTick = TickConverter::daysToTicks(p.start);
    runner.scheduleEvent(startTick-0.01, repast::Schedule::FunctorPtr(new repast::MethodFunctor<PolicyEnforcer>(this,&PolicyEnforcer::addPolicy)));
    addedPolicies.push_back(p);
    // Remove policy
    double endTick = TickConverter::daysToTicks(p.end);
    runner.scheduleEvent(endTick, repast::Schedule::FunctorPtr(new repast::MethodFunctor<PolicyEnforcer>(this,&PolicyEnforcer::removePolicy)));
}

void PolicyEnforcer::addPolicy(){
    currentPolicies.push_back(addedPolicies.at(0));
}

void PolicyEnforcer::removePolicy(){
    currentPolicies.erase(currentPolicies.begin());
    addedPolicies.erase(addedPolicies.begin());
}

bool PolicyEnforcer::isAllowedToGoOut(bool isolation, RepastHPCAgent* agent, int hour, double f, int day){
    // Agent attribures
    repast::AgentId id_ = agent->getId();
    int age = agent->getAge();
    bool allowed = true;

    // Check if agent is inactive at homeplace
    if(agent->isSleeping(hour)){
        return false;
    }

    // Do not move agent if agent's patient type is several or critical
    if(isolation && agent->getDiseaseStage() == INFECTED){
        bool isSevere = (agent->getPatientType() == SEVERE_SYMPTOMS || agent->getPatientType() == CRITICAL_SYMPTOMS);
        if(isSevere){
            return false;
        }
    }

    // Get current policy
    policy cp = currentPolicies.at(0);

    // Check if agent can move
    bool factor = (f <= cp.factor);

    if(cp.p == NONE){
        allowed = true;
    }else{
        bool cr = (age >= cp.ageMin && age <= cp.ageMax);

        if(cp.p == FULL_QUARANTINE && !cr){
            allowed = false;
        }

        if (cp.p == ID_BASED_CURFEW){
            int c = cp.c;
            if(!cr){
                allowed = false;
            }else{
                int nid = id_.id() % 10;
                allowed = false;
                if (std::find(curfews.at(c).at(day).begin(), curfews.at(c).at(day).end(), nid) != curfews.at(c).at(day).end()){
                    allowed = true;
                }
            }
        }
    }

    return (allowed && factor);
}