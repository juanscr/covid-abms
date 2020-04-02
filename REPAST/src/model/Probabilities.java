package model;

import repast.simphony.random.RandomHelper;

public abstract class Probabilities {

	// Taken from (DANE, 2018)
	private static double ageProbs[] = { 14.43, 16.9, 17.28, 14.87, 12.21, 11.04, 7.28, 3.93, 2.06 };
	private static int ageRanges[][] = { { 0, 9 }, { 10, 19 }, { 20, 29 }, { 30, 39 }, { 40, 49 }, { 50, 59 },
			                             { 60, 69 }, { 70, 79 }, { 80, 121 } };

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
	
	public static double getTriangular(double min, double mode, double max) {
		double beta = (mode - min) / (max - min);
		double randomNumber = RandomHelper.nextDoubleFromTo(0, 1);
		double t;
		if (randomNumber < beta) {
			t = Math.sqrt(beta * randomNumber);
		} else {
			t = Math.sqrt((1 - beta) * (1 - randomNumber));
		}
		
		return min + (max - min) * t;
	}
}