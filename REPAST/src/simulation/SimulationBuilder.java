package simulation;

import java.util.ArrayList;
import java.util.List;

import org.geotools.renderer.GTRenderer;
import org.geotools.renderer.lite.StreamingRenderer;
import org.opengis.feature.simple.SimpleFeature;

import com.vividsolutions.jts.geom.Geometry;

import model.Citizen;
import model.DiseaseStage;
import model.Heuristics;
import model.Probabilities;
import repast.simphony.context.Context;
import repast.simphony.context.space.continuous.ContinuousSpaceFactory;
import repast.simphony.context.space.continuous.ContinuousSpaceFactoryFinder;
import repast.simphony.context.space.gis.GeographyFactory;
import repast.simphony.context.space.gis.GeographyFactoryFinder;
import repast.simphony.dataLoader.ContextBuilder;
import repast.simphony.space.continuous.ContinuousAdder;
import repast.simphony.space.continuous.ContinuousSpace;
import repast.simphony.space.continuous.NdPoint;
import repast.simphony.space.continuous.PointTranslator;
import repast.simphony.space.continuous.RandomCartesianAdder;
import repast.simphony.space.gis.GISAdder;
import repast.simphony.space.gis.Geography;
import repast.simphony.space.gis.GeographyParameters;
import repast.simphony.space.gis.RandomGISAdder;

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
		
		// Loading Medellin Map
		List<SimpleFeature> medellinFeature = Reader.loadFeaturesFromShapefile("../covid/maps/EOD.shp");
		Geometry medellinGeometry = (Geometry) medellinFeature.get(0).getDefaultGeometry();
		for (int i = 1; i < medellinFeature.size(); i++)
			medellinGeometry.union((Geometry) medellinFeature.get(i).getDefaultGeometry());
		
		System.out.println(medellinGeometry);
		
		// Geography settings
		GeographyParameters<Object> geoParams = new GeographyParameters<Object>();
		GISAdder<Object> geographyAdder = new RandomGISAdder<Object>(medellinGeometry);
		geoParams.setAdder(geographyAdder);
		GeographyFactory geoFactory = GeographyFactoryFinder.createGeographyFactory(null);
		Geography<Object> geography = geoFactory.createGeography("Medellin", context, geoParams);
		
		// Susceptible citizens
		int susceptibleCount = 100;
		for (int i = 0; i < susceptibleCount; i++) {
			int age = Probabilities.getRandomAge();
			context.add(new Citizen(context, space, geography, age, DiseaseStage.SUSCEPTIBLE));
		}

		// Infected citizens
		int infectedCount = 10;
		for (int i = 0; i < infectedCount; i++) {
			int age = Probabilities.getRandomAge();
			context.add(new Citizen(context, space, geography, age, DiseaseStage.INFECTED));
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