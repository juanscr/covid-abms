package simulation;

import java.util.ArrayList;

import model.Citizen;
import model.DiseaseStage;
import model.Heuristics;
import model.Probabilities;
import repast.simphony.context.Context;
import repast.simphony.context.space.continuous.ContinuousSpaceFactory;
import repast.simphony.context.space.continuous.ContinuousSpaceFactoryFinder;
import repast.simphony.dataLoader.ContextBuilder;
import repast.simphony.space.continuous.ContinuousAdder;
import repast.simphony.space.continuous.ContinuousSpace;
import repast.simphony.space.continuous.NdPoint;
import repast.simphony.space.continuous.PointTranslator;
import repast.simphony.space.continuous.RandomCartesianAdder;

public class SimulationBuilder implements ContextBuilder<Object> {

	@Override
	public Context<Object> build(Context<Object> context) {
		context.setId("covid19ABMS");

		// Continuous space projection
		ContinuousSpaceFactory spaceFactory = ContinuousSpaceFactoryFinder.createContinuousSpaceFactory(null);
		ContinuousAdder<Object> continuousAdder = new RandomCartesianAdder<Object>();
		PointTranslator translator = new repast.simphony.space.continuous.BouncyBorders();
		ContinuousSpace<Object> space = spaceFactory.createContinuousSpace("space", context, continuousAdder,
				translator, 500, 500);

		// Susceptible citizens
		int susceptibleCount = 2000;
		for (int i = 0; i < susceptibleCount; i++) {
			int age = Probabilities.getRandomAge();
			context.add(new Citizen(context, space, age, DiseaseStage.SUSCEPTIBLE));
		}

		// Infected citizens
		int infectedCount = 10;
		for (int i = 0; i < infectedCount; i++) {
			int age = Probabilities.getRandomAge();
			context.add(new Citizen(context, space, age, DiseaseStage.INFECTED));
		}

		// Create citizen list
		ArrayList<Citizen> citizenList = new ArrayList<Citizen>();
		for (Object obj : context) {
			Citizen citizen = (Citizen) obj;
			citizenList.add(citizen);
		}

		// Create families
		ArrayList<Citizen> uniqueFamilies = new ArrayList<Citizen>();
		for (Citizen citizen : citizenList) {
			if (citizen.getFamily().isEmpty()) {
				Heuristics.getFamily(citizen, citizenList);
				uniqueFamilies.add(citizen);
			}
		}

		// Create houses for each family
		ArrayList<NdPoint> houses = new ArrayList<NdPoint>();
		for (Citizen citizen : uniqueFamilies) {
			Heuristics.createHouse(citizen, houses, space);
		}

		return context;
	}

}