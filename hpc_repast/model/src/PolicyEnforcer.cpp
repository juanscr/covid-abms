#include "PolicyEnforcer.h"
PolicyEnforcer::PolicyEnforcer(void){
}

void PolicyEnforcer::schedulePolicy(Policy policy, double startDay, double endDay, repast::ScheduleRunner& runner){
    // Add policy
    double startTick = TickConverter::daysToTicks(startDay);
    runner.scheduleEvent(startTick-0.01, repast::Schedule::FunctorPtr(new repast::MethodFunctor<PolicyEnforcer>(this,&PolicyEnforcer::addPolicy)));
    addedPolicies.push_back(policy);
    // Remove policy
    double endTick = TickConverter::daysToTicks(endDay);
    runner.scheduleEvent(endTick, repast::Schedule::FunctorPtr(new repast::MethodFunctor<PolicyEnforcer>(this,&PolicyEnforcer::removePolicy)));
}

void PolicyEnforcer::addPolicy(){
    currentPolicies.push_back(addedPolicies.at(0));
}

void PolicyEnforcer::removePolicy(){
    currentPolicies.erase(currentPolicies.begin());
    addedPolicies.erase(addedPolicies.begin());
}

bool PolicyEnforcer::isAllowedToGoOut(repast::AgentId id_, int ctick){
    bool allowed = true;
    if(currentPolicies.at(0) == FULL_QUARANTINE){
        allowed = false;
    }
    if(currentPolicies.at(0) == ID_BASED_CURFEW){
        int nid = id_.id() % 10;
        int day = (int) TickConverter::ticksToDays(ctick) % 7;
        allowed = false;
        if (std::find(curfewIds.at(day).begin(), curfewIds.at(day).end(), nid) != curfewIds.at(day).end()){
            allowed = true;
        }
    }
    return allowed;
}