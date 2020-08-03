#include "Probabilities.h"

double Probabilities::getRandomTriangular(double min, double mode, double max, double r){
    double beta = (mode - min) / (max - min);

	double t = 0.0;
	if (r < beta) {
        t = std::sqrt(beta * r);

    } else {
        t = std::sqrt((1 - beta) * (1 - r));
	}

    return min + (max - min) * t;
}

/**
 * Get random age (unit: age). Reference: <pending>
*/
int Probabilities::getRandomAge(double r_range, double r_age){

   double cummulativeProbability = 0;
   int age = -1;

    // Select a range of ages
    for (int  i = 0; i < sizeof(Probabilities::age_probabilities) ; i++){

        cummulativeProbability += Probabilities::age_probabilities[i];

        if (r_range < cummulativeProbability){

            age = (Probabilities::age_ranges[i][1] - Probabilities::age_ranges[i][0])*r_age + Probabilities::age_ranges[i][0];

            return std::round(age);
        }
    }
    return age;
}

/**
 * Get random incubation period (unit: days). Reference: <pending>
 * */
double Probabilities::getRandomIncubationPeriod() {
    double t = std::pow(Probabilities::MEAN_INCUBATION_PERIOD, 2) + std::pow(Probabilities::STD_INCUBATION_PERIOD, 2);
    double mu = std::log(std::pow(Probabilities::MEAN_INCUBATION_PERIOD, 2) / std::sqrt(t));
    double sigma = std::log(t / std::pow(Probabilities::MEAN_INCUBATION_PERIOD, 2));

    repast::NormalGenerator normalDistribution  = repast::Random::instance()->createNormalGenerator(mu, sigma);
	double y = normalDistribution.next();

	return std::exp(y);
}

/**
 * Get random patient type. Reference: <pending>
*/
PatientType Probabilities::getRandomPatientType(double r1, double r2){

    if (r1 < 0.111){
        return NO_SYMPTOMS;
    }else{
        if (r2 < 0.814){
            return MODERATE_SYMPTOMS;
        }
        else if (r2 < 0.953){
            return SEVERE_SYMPTOMS;
        } else{
            return CRITICAL_SYMPTOMS;
        }
    }
}

/**
 * Is the patient going to die? Reference: <pending>
*/
bool Probabilities::isGoingToDie(double r, PatientType patientType){

    switch (patientType)
    {
    case SEVERE_SYMPTOMS:
        return r < 0.15;
        break;
    case CRITICAL_SYMPTOMS:
        return r < 0.5;
        break;
    default:
        return false;
        break;
    }
}

/**
 * Is the citizen getting exposed? Reference: <pending>
 */
bool Probabilities::isGettingExposed(double r, double incubationShift){

    if (incubationShift < INFECTION_MIN){
        return false;
    }

    return true;
}

double Probabilities::getRandomWakeUpTime(Shift workShift){
    int displacement;
    return -1;
}

/**
 * Get random work shift. Reference: <pending>
*/
Shift Probabilities::getRandomWorkShift(double r){
    if (r < Probabilities::DAY_SHIFT_PROBABILITIES){
        return DAY;
    }else{
        return NIGHT;
    }
}

Probabilities::Probabilities(void){}