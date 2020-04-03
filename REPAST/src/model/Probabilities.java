package model;

import cern.jet.random.Gamma;
import repast.simphony.random.RandomHelper;

public abstract class Probabilities {

	// Taken from (DANE, 2018)
	private static double ageProbs[] = { 14.43, 16.9, 17.28, 14.87, 12.21, 11.04, 7.28, 3.93, 2.06 };
	private static int ageRanges[][] = { { 0, 9 }, { 10, 19 }, { 20, 29 }, { 30, 39 }, { 40, 49 }, { 50, 59 },
			{ 60, 69 }, { 70, 79 }, { 80, 121 } };

	// Estimated
	public static final double INFECTION_PROBABILITY = 0.8;
	private static final double ALPHA_INCUBATION_TIME = 3.37;
	private static final double BETA_INCUBATION_TIME = 1.49;
	private static final double MIN_INCUBATION_TIME = 2.0;

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
		Gamma gammaDistribution = RandomHelper.createGamma(ALPHA_INCUBATION_TIME, 1 / BETA_INCUBATION_TIME);
		return (gammaDistribution.nextDouble() + MIN_INCUBATION_TIME) * 24;
	}

}