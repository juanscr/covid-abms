package simulation;

import java.util.ArrayList;
import java.util.List;

import org.opengis.feature.simple.SimpleFeature;

import com.vividsolutions.jts.geom.Coordinate;
import com.vividsolutions.jts.geom.Geometry;
import com.vividsolutions.jts.geom.GeometryFactory;
import com.vividsolutions.jts.geom.MultiPolygon;
import com.vividsolutions.jts.geom.Point;
import com.vividsolutions.jts.geom.util.AffineTransformation;

import geography.Border;
import model.Citizen;
import model.DiseaseStage;
import model.Heuristics;
import model.Probabilities;
import repast.simphony.context.Context;
import repast.simphony.context.space.gis.GeographyFactory;
import repast.simphony.context.space.gis.GeographyFactoryFinder;
import repast.simphony.dataLoader.ContextBuilder;
import repast.simphony.gis.util.GeometryUtil;
import repast.simphony.space.continuous.NdPoint;
import repast.simphony.space.gis.Geography;
import repast.simphony.space.gis.GeographyParameters;

public class SimulationBuilder implements ContextBuilder<Object> {

	@Override
	public Context<Object> build(Context<Object> context) {
		context.setId("covid19ABMS");
		
		// Read Aburra Valley border geometry
		List<SimpleFeature> borderFeatures = Reader.loadGeometryFromShapefile("../covid/maps/EOD_border.shp");
		Geometry borderGeometry = (MultiPolygon) borderFeatures.get(0).getDefaultGeometry();
		AffineTransformation transformation = new AffineTransformation();
		transformation.scale(2, 2);
		borderGeometry = transformation.transform(borderGeometry);
		
		// Geography projection
		GeographyParameters<Object> params = new GeographyParameters<Object>();
		GeographyFactory geographyFactory = GeographyFactoryFinder.createGeographyFactory(null);
		Geography<Object> geography = geographyFactory.createGeography("Valle de Aburra", context, params);
		
		// Add border to context
		Border border = new Border(borderGeometry);
		border.setGeometryInGeography(geography);
		context.add(border);
		
		// Susceptible citizens
		int susceptibleCount = 80;
		for (int i = 0; i < susceptibleCount; i++) {
			int age = Probabilities.getRandomAge();
			context.add(new Citizen(context, geography, borderGeometry, age, DiseaseStage.SUSCEPTIBLE));
		}

		// Infected citizens
		int infectedCount = 10;
		for (int i = 0; i < infectedCount; i++) {
			int age = Probabilities.getRandomAge();
			context.add(new Citizen(context, geography, borderGeometry, age, DiseaseStage.INFECTED));
		}

		// Create citizen list
		ArrayList<Citizen> citizenList = new ArrayList<Citizen>();
		for (Object obj : context) {
			if (obj instanceof Citizen) {
				Citizen citizen = (Citizen) obj;
				citizenList.add(citizen);
			}
		}
		
		// Create geometry for agents
		List<Coordinate> agentCoordinates = 
				         GeometryUtil.generateRandomPointsInPolygon(borderGeometry, citizenList.size());
		GeometryFactory geometryFactory = new GeometryFactory();
		for (int i = 0; i < agentCoordinates.size(); i++) {
			Coordinate coordinate = agentCoordinates.get(i);
			Point pointAgent = geometryFactory.createPoint(coordinate);
			geography.move(citizenList.get(i), pointAgent);
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
			Heuristics.createHouse(citizen, houses, geography, borderGeometry);
		}

		return context;
	}

}