package simulation;

import model.Citizen;
import model.DiseaseStage;
import model.Probabilities;
import repast.simphony.context.Context;
import repast.simphony.context.space.continuous.ContinuousSpaceFactory;
import repast.simphony.context.space.continuous.ContinuousSpaceFactoryFinder;
import repast.simphony.context.space.grid.GridFactory;
import repast.simphony.context.space.grid.GridFactoryFinder;
import repast.simphony.dataLoader.ContextBuilder;
import repast.simphony.space.continuous.ContinuousAdder;
import repast.simphony.space.continuous.ContinuousSpace;
import repast.simphony.space.continuous.NdPoint;
import repast.simphony.space.continuous.PointTranslator;
import repast.simphony.space.continuous.RandomCartesianAdder;
import repast.simphony.space.grid.Grid;
import repast.simphony.space.grid.GridAdder;
import repast.simphony.space.grid.GridBuilderParameters;
import repast.simphony.space.grid.GridPointTranslator;
import repast.simphony.space.grid.SimpleGridAdder;
import repast.simphony.space.grid.WrapAroundBorders;

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

		// Grid projection
		GridFactory gridFactory = GridFactoryFinder.createGridFactory(null);
		GridPointTranslator borderRule = new WrapAroundBorders();
		GridAdder<Object> gridAdder = new SimpleGridAdder<Object>();
		GridBuilderParameters<Object> params = new GridBuilderParameters<Object>(borderRule, gridAdder, true, 500, 500);
		Grid<Object> grid = gridFactory.createGrid("grid", context, params);

		// Age probabilities

		// Susceptible citizens
		int susceptibleCount = 10;
		for (int i = 0; i < susceptibleCount; i++) {
			int age = Probabilities.getRandomAge();
			context.add(new Citizen(space, grid, age, DiseaseStage.SUSCEPTIBLE));
		}

		// Infected citizens
		int infectedCount = 10;
		for (int i = 0; i < infectedCount; i++) {
			int age = Probabilities.getRandomAge();
			context.add(new Citizen(space, grid, age, DiseaseStage.INFECTED));
		}

		// Synchronize space and grid locations
		for (Object obj : context) {
			NdPoint pt = space.getLocation(obj);
			grid.moveTo(obj, (int) pt.getX(), (int) pt.getY());
		}

		return context;
	}

}