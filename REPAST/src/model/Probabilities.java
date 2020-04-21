package model;

import cern.jet.random.Normal;
import repast.simphony.random.RandomHelper;

public abstract class Probabilities {

	// Taken from (DANE, 2018)
	private static double ageProbs[] = { 14.43, 16.9, 17.28, 14.87, 12.21, 11.04, 7.28, 3.93, 2.06 };
	private static int ageRanges[][] = { { 0, 9 }, { 10, 19 }, { 20, 29 }, { 30, 39 }, { 40, 49 }, { 50, 59 },
			{ 60, 69 }, { 70, 79 }, { 80, 121 } };

	// Estimated
	public static final double INFECTION_PROBABILITY = 0.35;
	public static final double ACTIVE_CASE_PROBABILITY = 0.7;
	private static final double MEAN_INCUBATION_TIME = 5.52 * 24;
	private static final double STD_INCUBATION_TIME = 2.41 * 24;

	public static double getTriangular(double min, double mode, double max) {
		double beta = (mode - min) / (max - min);
		double randomNumber = RandomHelper.nextDoubleFromTo(0, 1);
		double t = 0.0;
		if (randomNumber < beta) {
			t = Math.sqrt(beta * randomNumber);
		} else {
			t = Math.sqrt((1 - beta) * (1 - randomNumber));
		}
		return min + (max - min) * t;
	}

	public static int getRandomAge() {
		double r = RandomHelper.nextDoubleFromTo(0, 1) * 100;
		double acumProb = 0;
		for (int i = 0; i < ageProbs.length; i++) {
			acumProb += ageProbs[i];
			if (r < acumProb) {
				return RandomHelper.nextIntFromTo(ageRanges[i][0], ageRanges[i][1]);
			}
		}
		return -1;
	}

	public static double getRandomIncubationTime() {
		double t = Math.pow(MEAN_INCUBATION_TIME, 2) + Math.pow(STD_INCUBATION_TIME, 2);
		double mu = Math.log(Math.pow(MEAN_INCUBATION_TIME, 2) / Math.sqrt(t));
		double sigma = Math.log(t / Math.pow(MEAN_INCUBATION_TIME, 2));
		Normal normalDistribution = RandomHelper.createNormal(mu, sigma);
		double y = normalDistribution.nextDouble();
		return Math.exp(y);
	}

	public static PatientType getRandomPatientType() {
		double r = RandomHelper.nextDoubleFromTo(0, 1);
		if (r < 0.30) {
			return PatientType.NO_SYMPTOMS;
		} else if (r < 0.85) {
			return PatientType.MODERATE_SYMPTOMS;
		} else if (r < 0.95) {
			return PatientType.SEVERE_SYMPTOMS;
		} else {
			return PatientType.CRITICAL_SYMPTOMS;
		}
	}

	public static boolean isGoingToDie(PatientType patientType) {
		double r = RandomHelper.nextDoubleFromTo(0, 1);
		switch (patientType) {
		case SEVERE_SYMPTOMS:
			return r < 0.15;
		case CRITICAL_SYMPTOMS:
			return r < 0.5;
		default:
			return false;
		}
	}

	public static boolean isGettingExposed() {
		double r = RandomHelper.nextDoubleFromTo(0, 1);
		return r < Probabilities.INFECTION_PROBABILITY;
	}

	public static boolean isDevelopingActiveCase() {
		double r = RandomHelper.nextDoubleFromTo(0, 1);
		return r < Probabilities.ACTIVE_CASE_PROBABILITY;
	}
	
	public static double getRandomTimeToDeath(PatientType patientType) {
		switch (patientType) {
		case CRITICAL_SYMPTOMS:
		case SEVERE_SYMPTOMS:
			return RandomHelper.nextDoubleFromTo(10, 20) * 24;
		default:
			return 24 * 3 * 30;
		}
	}

	public static double getRandomTimeToImmune(PatientType patientType) {
		return 10 * 24;
	}

	public static double getRandomWakeUpTime() {
		return RandomHelper.nextDoubleFromTo(4, 8);
	}

	public static double getRandomReturnToHomeTime() {
		return RandomHelper.nextDoubleFromTo(16, 19);
	}

}