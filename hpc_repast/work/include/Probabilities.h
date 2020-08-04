/*Probabilities.h */

#ifndef PROBABILITIES_H
#define PROBABILITIES_H

#include "repast_hpc/AgentId.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/SharedContinuousSpace.h"
#include <stdio.h>
#include <vector>
#include "Shift.h"
#include "PatientType.h"
#include "DiseaseStage.h"
#include "TickConverter.h"

class Probabilities{

    /**
	 * Age ranges (unit: age). Reference: <pending>
	 */
    const int age_ranges [9][2] = { { 0, 9 }, { 10, 19 }, { 20, 29 },
         { 30, 39 }, { 40, 49 }, { 50, 59 },
         { 60, 69 }, { 70, 79 }, { 80, 121 } };

    /**
	 * Age probabilities (unit: probability). Reference: <pending>
     * 
	 */
    const double age_probabilities [9] = {0.1443, 0.169, 0.1728, 0.1487,0.1221, 0.1104, 0.0728, 0.0393, 0.0206};

    /**
	 * Daily travels (unit: travels). Reference: <pending>
	 */
    const int DAILY_TRAVELS [24] = { 335, 2169, 3704, 9833, 70018, 328893, 610550, 481395, 314939, 244620,
    245991, 322370, 318179, 313987, 327201, 309527, 395493, 613719, 466570, 210368, 128377, 93656, 60591,
    23699 };
    
    /**
	 * Day shift probability (unit: probability). Reference: <pending>
	 */
    const double DAY_SHIFT_PROBABILITIES = 0.7;

    /**
	 * Infection alpha parameter. Reference: <pending>
	 */
    const double INFECTION_ALPHA = 2.11;

    /**
	 * Infection beta parameter. Reference: <pending>
	 */
    const double INFECTION_BETA = 1.3;

    /**
	 * Infection minimum parameter. Reference: <pending>
	 */
    const double INFECTION_MIN = -2.4;

    /**
	 * Discharge alpha parameter. Reference: <pending>
	 */
	const double DISCHARGE_ALPHA = 1.99;

	/**
	 * Discharge beta parameter. Reference: <pending>
	 */
	const double DISCHARGE_BETA = 7.77;

	/**
	 * Incubation period mean parameter (unit: days). Reference: <pending>
	 */
	const double MEAN_INCUBATION_PERIOD = 5.52;

	/**
	 * Incubation period standard deviation parameter (unit: days). Reference:
	 * <pending>
	 */
	const double STD_INCUBATION_PERIOD = 2.41;

    /**
	 * Age ranges (unit: age). Reference: <pending>
	 */
    private:
    public:
    /**
    Methods
    */

        /**
         * Get random number based on a triangular distribution
         * 
         * @param min  Minimum
         * @param mode Mode
         * @param max  Maximum
         * @param r Random in U[0,1)
         */
        double getRandomTriangular(double min, double mode, double max, double r);

        /**
         * * Get random age (unit: age). Reference: <pending>
         * */
        int getRandomAge(double r_range, double r_age);

        /**
         * Get random incubation period (unit: days). Reference: <pending>
         * */
        double getRandomIncubationPeriod();

        /**
         * Get random patient type. Reference: <pending>
        */
       PatientType getRandomPatientType(double r1, double r2);

       /**
        * Is the patient going to die? Reference: <pending>
        * */
       bool isGoingToDie(double r, PatientType patientType);

       /**
        * Is the citizen getting exposed? Reference: <pending>
        * */
	    bool isGettingExposed(double r, double incubationShift);

        Shift getRandomWorkShift(double r);
        double getRandomWakeUpTime(Shift workShift);
        double getRandomGamma(double alpha, double theta);
        Probabilities();


};

#endif
