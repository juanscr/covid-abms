package model;

import java.util.ArrayList;
import repast.simphony.random.RandomHelper;
import repast.simphony.space.continuous.ContinuousSpace;
import repast.simphony.space.continuous.NdPoint;

public abstract class Heuristics {

	private static int familyProbs[] = { 19, 23, 24, 19, 8, 7 };
	private static int houseRadius = 1;

	public static void getFamily(Citizen toSelect, ArrayList<Citizen> citizenList) {
		ArrayList<Citizen> family = new ArrayList<Citizen>();
		family.add(toSelect);

		if (toSelect.getAge() < 18) {
			// Conditional probability
			double familyProbsKids[] = new double[familyProbs.length - 1];
			double sumProb = 100 - familyProbs[0];
			double sumPartial = 0;
			for (int j = 1; j < familyProbsKids.length; j++) {
				familyProbsKids[j] = familyProbs[j + 1] * 100 / sumProb;
				sumPartial += familyProbsKids[j];
			}
			familyProbsKids[0] = 100 - sumPartial;

			// How much people are in the family
			int acum = 0;
			double r = RandomHelper.nextDoubleFromTo(0, 1) * 100;
			int numberFamily = 0;
			for (int i = 0; i < familyProbsKids.length; i++) {
				acum += familyProbsKids[i];
				if (r < acum) {
					numberFamily = i;
					break;
				}
			}

			// Select family
			Citizen adult = null;
			int indexAdult = 0;
			for (Citizen citizen : citizenList) {
				if (citizen.getAge() >= 18 && citizen.getFamily().size() == 0) {
					adult = citizen;
					break;
				}
				indexAdult += 1;
			}

			if (adult == null) {
				// Case in which every adult has a family
				for (Citizen citizen : citizenList) {
					if (citizen.getAge() >= 18 && citizen.getFamily().size() != 0) {
						family = citizen.getFamily();
						family.add(toSelect);
						break;
					}
				}
			} else {
				// Case in which I have an adult in my family
				family.add(adult);

				if (numberFamily > 0) {
					for (int i = 0; i < citizenList.size(); i++) {
						if (i == indexAdult || citizenList.get(i).equals(toSelect))
							continue;

						if (citizenList.get(i).getFamily().size() == 0)
							family.add(citizenList.get(i));

						if (numberFamily == family.size() - 2)
							break;
					}
				}
			}
		} else {
			int acum = 0;
			double r = RandomHelper.nextDoubleFromTo(0, 1) * 100;
			int numberFamily = 0;
			for (int i = 0; i < familyProbs.length; i++) {
				acum += familyProbs[i];
				if (r < acum) {
					numberFamily = i;
					break;
				}
			}
			// How much people are in the family
			if (numberFamily > 0) {
				for (int i = 0; i < citizenList.size(); i++) {
					if (citizenList.get(i).equals(toSelect))
						continue;

					if (citizenList.get(i).getFamily().size() == 0)
						family.add(citizenList.get(i));

					if (numberFamily == family.size() - 1)
						break;
				}
			}
		}

		for (Citizen member : family) {
			member.setFamily(family);
		}
	}

	public static void createHouse(Citizen citizen, ArrayList<NdPoint> houses, ContinuousSpace<Object> space) {
		double randomX = RandomHelper.nextDoubleFromTo(0, space.getDimensions().getWidth());
		double randomY = RandomHelper.nextDoubleFromTo(0, space.getDimensions().getHeight());
		NdPoint houseSelected = new NdPoint(randomX, randomY);

		// Have disjointed houses
		for (NdPoint house : houses) {
			while (space.getDistance(house, houseSelected) < 2 * houseRadius) {
				randomX = RandomHelper.nextDoubleFromTo(0, space.getDimensions().getWidth());
				randomY = RandomHelper.nextDoubleFromTo(0, space.getDimensions().getHeight());
				houseSelected = new NdPoint(randomX, randomY);
			}
		}

		houses.add(houseSelected);

		// Select house for family
		double positionX;
		double positionY;
		for (Citizen citizenFamily : citizen.getFamily()) {
			citizenFamily.setHomeplace(houseSelected);
			positionX = Math.min(Math.max(houseSelected.getX() - 0.5 + RandomHelper.nextDoubleFromTo(0, 1),0),499);
			positionY = Math.min(Math.max(houseSelected.getY() - 0.5 + RandomHelper.nextDoubleFromTo(0, 1),0),499);
			citizenFamily.relocate(new NdPoint(positionX, positionY));
		}
	}

}
