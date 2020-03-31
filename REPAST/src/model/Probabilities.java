package model;

import java.util.ArrayList;
import repast.simphony.random.RandomHelper;

public abstract class Probabilities {

	// Taken from (DANE, 2018)
	private static double ageProbs[] = { 14.43, 16.9, 17.28, 14.87, 12.21, 11.04, 7.28, 3.93, 2.06 };
	private static int ageRanges[][] = { { 0, 9 }, { 10, 19 }, { 20, 29 }, { 30, 39 }, { 40, 49 }, { 50, 59 }, { 60, 69 }, { 70, 79 }, { 80, 121 } };
	private static int familyProbs[] = { 19, 23, 24, 19, 8, 7 };

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
	
	public static void getFamily(Citizen toSelect, ArrayList<Citizen> citizenList) {
		ArrayList<Citizen> family = new ArrayList<Citizen>();
		family.add(toSelect);

		if (toSelect.getAge() < 18) {
			// Conditional probability
			int familyProbsKids[] = new int[familyProbs.length - 1];
			int sumProb = 100 - familyProbs[0];
			for (int j = 0; j < familyProbsKids.length; j++) {
				familyProbsKids[j] = familyProbs[j + 1] * 100 / sumProb; 
			}
			
			// How much people are in the family
			int acum = 0;
			double r = RandomHelper.nextDoubleFromTo(0, 1) * 100;
			int numberFamily = 0;
			for (int i = 0; i < familyProbsKids.length; i++) {
				acum += familyProbsKids[i];
				if (acum < r) {
					numberFamily = i - 1;
					break;
				}
			}
			
			// Select family
			Citizen adult = null;
			int indexAdult = 0;
			for (Citizen citizen : citizenList) {
				if (citizen.getAge() >= 18 && citizen.getFamily().equals(null)) {
					adult = citizen;
					break;
				}
				indexAdult += 1;
			}
			
			if (adult.equals(null)) {
				// Case in which every adult has a family
				for (Citizen citizen : citizenList) {
					if (citizen.getAge() >= 18 && !citizen.getFamily().equals(null)) {
						family = citizen.getFamily();
						family.add(toSelect);
						break;
					}
				}
			}
			else {
				// Case in which I have an adult in my family
				family.add(adult);
				
				if (numberFamily > 0) {
					for (int i = 0; i < citizenList.size(); i++) {
						if (i == indexAdult || citizenList.get(i).equals(toSelect))
							continue;
						
						if (citizenList.get(i).getFamily().equals(null)) {
							citizenList.add(citizenList.get(i));
						}
						if (numberFamily == citizenList.size() - 2) {
							break;
						}
					}
				}
			}
		} else {
			int acum = 0;
		    double r = RandomHelper.nextDoubleFromTo(0, 1) * 100;
		    int numberFamily = 0;
			for (int i = 0; i < familyProbs.length; i++) {
				acum += familyProbs[i];
				if (acum < r) {
					numberFamily = i - 1;
					break;
				}
			}
			
			// How much people are in the family
			if (numberFamily > 0) {
				for (int i = 0; i < citizenList.size(); i++) {
					if (citizenList.get(i).equals(toSelect))
						continue;
								
					if (citizenList.get(i).getFamily().equals(null)) {
						citizenList.add(citizenList.get(i));
					}
					if (numberFamily == citizenList.size() - 1) {
						break;
					}
			}
						}
		}
		
		for (Citizen member : family) {
			member.setFamily(family);
		}
	}
}