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

bool PolicyEnforcer::isAllowedToGoOut(repast::Random* r, bool isolation, RepastHPCAgent* agent, int hour, double f, int day){
    // Agent attribures
    repast::AgentId id_ = agent->getId();
    int age = agent->getAge();
    bool allowed = true;

    // Get current policy
    policy cp = currentPolicies.at(0);

    // Do not move agent if agent's patient type is several or critical
    if(isolation && agent->getDiseaseStage() == INFECTED){
        bool isSevere = (agent->getPatientType() == SEVERE_SYMPTOMS || agent->getPatientType() == CRITICAL_SYMPTOMS);
        if(isSevere){
            return false;
        }
    }

    // Check if agent is inactive at homeplace
    if(agent->isSleeping(hour)){
        return false;
    }

    // Check if agent can move
    bool factor = (f <= cp.factor);

    if(cp.p == NONE || !agent->getComplies()){
        allowed = true;
    }else{
        bool cr = (age >= cp.ageMin && age <= cp.ageMax);

        if(cp.p == FULL_QUARANTINE && !cr){
            allowed = false;
        }

        if (cp.p == ID_BASED_CURFEW){
            if(!cr){
                allowed = false;
            }else{
                int nid = id_.id() % 10;
                allowed = false;
                if (std::find(cp.curfew.at(day).begin(), cp.curfew.at(day).end(), nid) != cp.curfew.at(day).end()){
                    allowed = true;
                }
            }
        }
    }

    return (allowed && factor);
}