package model;

import repast.simphony.random.RandomHelper;

public abstract class Probabilities {

	// Taken from (DANE, 2018)
	private static double age_probs[] = { 14.43, 16.9, 17.28, 14.87, 12.21, 11.04, 7.28, 3.93, 2.06 };
	private static int age_ranges[][] = { { 0, 9 }, { 10, 19 }, { 20, 29 }, { 30, 39 }, { 40, 49 }, { 50, 59 }, { 60, 69 }, { 70, 79 }, { 80, 121 } };

	public static int getRandomAge() {
		double r = RandomHelper.nextDoubleFromTo(0, 1) * 100;
		double acumProb = 0;
		for (int i = 0; i < age_probs.length; i++) {
			acumProb += age_probs[i];
			if (r < acumProb) {
				return RandomHelper.nextIntFromTo(age_ranges[i][0], age_ranges[i][1]);
			}
		}
		return -1;
	}
	
}