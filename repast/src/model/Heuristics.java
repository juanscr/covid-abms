package model;

import java.util.ArrayList;
import java.util.HashMap;
import com.vividsolutions.jts.geom.Coordinate;
import geography.Zone;
import repast.simphony.gis.util.GeometryUtil;
import repast.simphony.random.RandomHelper;
import repast.simphony.space.continuous.NdPoint;
import repast.simphony.space.gis.Geography;

public abstract class Heuristics {

	public static final int familyProbs[] = { 19, 23, 24, 19, 8, 7 };
	public static final int houseRadius = 0;

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

	public static void createHouse(Citizen citizen, HashMap<Zone, ArrayList<NdPoint>> houses,
			Geography<Object> geography, ArrayList<Zone> zoneList) {

		// Select random zone
		int zoneIndex = RandomHelper.nextIntFromTo(0, zoneList.size() - 1);
		Zone selectedZone = zoneList.get(zoneIndex);

		Coordinate coordinate = GeometryUtil.generateRandomPointsInPolygon(selectedZone.getGeometry(), 1).get(0);
		NdPoint houseSelected = new NdPoint(coordinate.x, coordinate.y);

		// Have disjointed houses
		if (houses.containsKey(selectedZone)) {
			int i = 0;
			while (i < houses.get(selectedZone).size()) {
				NdPoint house = houses.get(selectedZone).get(i);
				double distance = Math.pow((house.getX() - houseSelected.getX()), 2);
				distance += Math.pow((house.getY() - houseSelected.getY()), 2);
				int count = 0;
				i++;
				while (Math.sqrt(distance) < 2 * houseRadius) {
					coordinate = GeometryUtil.generateRandomPointsInPolygon(selectedZone.getGeometry(), 1).get(0);
					houseSelected = new NdPoint(coordinate.x, coordinate.y);
					distance = Math.pow((house.getX() - houseSelected.getX()), 2);
					distance += Math.pow((house.getY() - houseSelected.getY()), 2);
					if (count == 10) {
						i = 0;
						zoneIndex = RandomHelper.nextIntFromTo(0, zoneList.size());
						selectedZone = zoneList.get(zoneIndex);
						break;
					}
					count++;
				}
			}
		} else {
			houses.put(selectedZone, new ArrayList<NdPoint>());
		}

		houses.get(selectedZone).add(houseSelected);

		// Select house for family
		double positionX;
		double positionY;
		for (Citizen citizenFamily : citizen.getFamily()) {
			citizenFamily.setHomeplace(houseSelected);
			positionX = houseSelected.getX() - houseRadius / 2 + RandomHelper.nextDoubleFromTo(0, houseRadius);
			positionY = houseSelected.getY() - houseRadius / 2 + RandomHelper.nextDoubleFromTo(0, houseRadius);
			citizenFamily.relocate(new NdPoint(positionX, positionY));
			citizenFamily.setCurrentZone(selectedZone);
		}
	}

	public static void assignWorkplace(Citizen citizen, HashMap<String, Object> eod, ArrayList<Zone> zoneList) {
		int zoneId = citizen.getCurrentZone().getId();
		HashMap<Integer, Integer> rows = (HashMap<Integer, Integer>) eod.get("rows");

		if (rows.containsKey(zoneId)) {
			int row = rows.get(zoneId);
			ArrayList<ArrayList<Double>> eodMatrix = (ArrayList<ArrayList<Double>>) eod.get("eod");
			ArrayList<Double> travels = eodMatrix.get(row);

			int player1 = RandomHelper.nextIntFromTo(0, travels.size() - 1);
			for (int i = 0; i < 10; i++) {
				int player2 = RandomHelper.nextIntFromTo(0, travels.size() - 1);
				double decision = RandomHelper.nextDoubleFromTo(0, 1);
				double sum = travels.get(player1) + travels.get(player2);
				if (travels.get(player1) < travels.get(player2)) {
					int temp = player1;
					player1 = player2;
					player2 = temp;
				}
				if (decision >= travels.get(player1) / sum) {
					player1 = player2;
				}
			}

			HashMap<Integer, Integer> columns = (HashMap<Integer, Integer>) eod.get("columns");
			int id = columns.get(player1);
			for (Zone zone : zoneList) {
				if (zone.getId() == id) {
					Coordinate coordinate = GeometryUtil.generateRandomPointsInPolygon(zone.getGeometry(), 1).get(0);
					citizen.setWorkplace(new NdPoint(coordinate.x, coordinate.y));
					return;
				}
			}
		}
		Coordinate coordinate = GeometryUtil.generateRandomPointsInPolygon(citizen.getCurrentZone().getGeometry(), 1)
				.get(0);
		citizen.setWorkplace(new NdPoint(coordinate.x, coordinate.y));
	}

}