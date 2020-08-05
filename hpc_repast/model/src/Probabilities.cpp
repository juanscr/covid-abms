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
 * Generates a random gamma variable with a given shape and scale parameter respectively
*/
double Probabilities::getRandomGamma(double alpha, double theta) {
    double x = 0;
    double u2;
    double w;
    if (alpha >= 0.5) {
        double a = alpha - 0.5;
        double b = alpha / a;
        double c = 2.0 / a;
        double d = c + 2;
        double s = std::sqrt(alpha);
        double h1 = (0.865 + 0.064 / alpha) / s;
        double h2 = (0.4343 - 0.105 / s) / s;
        do {
            double u = repast::Random::instance() -> nextDouble();
            double u1 = repast::Random::instance() -> nextDouble();
            double u2 = u1 + h1 * u - h2;
            if (u2 < 0 or u2 > 1)
                continue;
            double w = b * (u1 / u2);
            x = a * w;
        } while (c * u2 > d - w - 1 / w && c * std::log(u2) > std::log(w) - w + 1);
    }
    else {
        double z = 0.07 + 0.75 * std::sqrt(1 - alpha);
        double b = 1 + std::exp(-z) * alpha / z;
        bool condition = true;
        do {
            double u = repast::Random::instance() -> nextDouble();
            double v = repast::Random::instance() -> nextDouble();
            double p = b * u;
            if (p <= 1) {
                x = z * std::pow(p, 1 / alpha);
                condition = v > (2 - x) / (2 + x) && v > std::exp(-x);
            }
            else {
                x = -std::log(z * (b - p) / alpha);
                double y = x / z;
                condition = v * (alpha + y - alpha * y) >= 1 && v > std::pow(y, 1 / alpha);
            }
        } while (condition);
    }

    return theta * x;
}

double Probabilities::getGammaPDF(double x, double alpha, double theta){
    double pdf = std::pow(x, alpha-1)*std::exp(-x/theta);

    return pdf/(std::tgamma(alpha)*std::pow(theta, alpha));
}


/**
 * Is the citizen getting exposed? Reference: <pending>
 */
bool Probabilities::isGettingExposed(double r, double incubationShift){

    if (incubationShift < INFECTION_MIN){
        return false;
    }

    double days = TickConverter::ticksToDays(incubationShift);
    double p = getGammaPDF(days - INFECTION_MIN, INFECTION_ALPHA, INFECTION_BETA);

    return r<p;
}

int seed;
std::default_random_engine generator(seed);
void Probabilities::setSeed(int newSeed){
    generator.seed(newSeed);
}

double Probabilities::getRandomTimeToDischarge(){
    std::gamma_distribution<double> dist_gamma(DISCHARGE_ALPHA, DISCHARGE_BETA);
    return dist_gamma(generator);
}

/**
 * * Get random wake up time (unit: hours). Reference: <pending>
*/
double Probabilities::getRandomWakeUpTime(Shift workShift){

    double displacement;
    int init;
    int end;

    if (workShift == DAY){
        init = 4;
        end = 10;
        displacement = 3;
    }else{
        init = 18;
        end = 22;
        displacement = 17;
    }

    int size = end - init;
    int travels[size];
    for (int i = 0; i < size; i++){
        travels[i] = Probabilities::DAILY_TRAVELS[init + i];
    }

    double sum = 0;
    for (int i: travels){
        sum += i;
    }

    double r = repast::Random::instance()->nextDouble();
    double acum = 0;

    for (int i = 0; i < size; i++){
        acum += travels[i];
        if (r <= acum/sum){
            return repast::Random::instance()->nextDouble() + displacement + i;
        }
    }
    return -1;
}

/**
 * Get random return to home time (unit: hours). Reference: <pending>
*/
double Probabilities::getRandomReturnToHomeTime(Shift workShift){
    double displacement;
    int init;
    int end;

    if (workShift == DAY){
        init = 13;
        end = 19;
        displacement = 12;
    }else{
        init = 1;
        end = 6;
        displacement = 1;
    }

    int size = end - init;
    int travels[size];
    for (int i = 0; i < size; i++){
        travels[i] = Probabilities::DAILY_TRAVELS[init + i];
    }

    double sum = 0;
    for (int i: travels){
        sum += i;
    }

    double r = repast::Random::instance()->nextDouble();
    double acum = 0;

    for (int i = 0; i < size; i++){
        acum += travels[i];
        if (r <= acum/sum){
            return repast::Random::instance()->nextDouble() + displacement + i;
        }
    }
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