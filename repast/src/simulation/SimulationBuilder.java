package simulation;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import org.opengis.feature.simple.SimpleFeature;
import com.vividsolutions.jts.geom.Coordinate;
import com.vividsolutions.jts.geom.Geometry;
import com.vividsolutions.jts.geom.GeometryFactory;
import com.vividsolutions.jts.geom.MultiPolygon;
import com.vividsolutions.jts.geom.Point;
import geography.Border;
import geography.Zone;
import model.Citizen;
import model.DiseaseStage;
import model.Heuristics;
import model.Policy;
import model.PolicyEnforcer;
import output.OutputManager;
import repast.simphony.context.Context;
import repast.simphony.context.space.gis.GeographyFactory;
import repast.simphony.context.space.gis.GeographyFactoryFinder;
import repast.simphony.dataLoader.ContextBuilder;
import repast.simphony.engine.environment.RunEnvironment;
import repast.simphony.gis.util.GeometryUtil;
import repast.simphony.parameter.Parameters;
import repast.simphony.space.continuous.NdPoint;
import repast.simphony.space.gis.Geography;
import repast.simphony.space.gis.GeographyParameters;
import util.TickConverter;

public class SimulationBuilder implements ContextBuilder<Object> {

	@Override
	public Context<Object> build(Context<Object> context) {
		context.setId("covid19ABMS");

		// Read Aburra Valley border geometry
		List<SimpleFeature> borderFeatures = Reader
				.loadGeometryFromShapefile("../sit-zone-information/maps/EOD_border.shp");
		Geometry borderGeometry = (MultiPolygon) borderFeatures.get(0).getDefaultGeometry();
		/*
		 * AffineTransformation transformation = new AffineTransformation();
		 * transformation.scale(2, 2); borderGeometry =
		 * transformation.transform(borderGeometry);
		 */

		// Read EOD matrix
		HashMap<String, Object> eod = Reader.loadEODMatrix("../sit-zone-information/eod_2017.csv");
		HashMap<String, Object> walks = Reader.loadEODWalksMatrix("../sit-zone-information/eod_2017_walks.csv");

		// Read SIT zones and create list with zones
		List<SimpleFeature> zonesFeatures = Reader.loadGeometryFromShapefile("../sit-zone-information/maps/EOD.shp");
		ArrayList<Zone> zoneList = new ArrayList<Zone>();
		HashMap<Integer, Integer> rows = (HashMap<Integer, Integer>) walks.get("rows");
		ArrayList<Double> averageWalks = (ArrayList<Double>) walks.get("walks");
		double maxWalk = Collections.max(averageWalks);
		double sumWalks = 0;
		for (Double d : averageWalks) {
			sumWalks += d;
		}
		double averageWalk = sumWalks / averageWalks.size();
		for (SimpleFeature feature : zonesFeatures) {
			Geometry zoneGeometry = (MultiPolygon) feature.getDefaultGeometry();
			// zoneGeometry = transformation.transform(zoneGeometry);
			int id = Integer.parseInt((String) feature.getAttribute("SIT_2017"));
			double walk = 0;
			if (rows.containsKey(id)) {
				walk = averageWalks.get(rows.get(id));
			} else {
				walk = averageWalk;
			}
			double zoneWalk = Citizen.MAX_MOVEMENT_IN_DESTINATION * walk / maxWalk;
			zoneList.add(new Zone(id, zoneGeometry, zoneWalk));
		}

		// Geography projection
		GeographyParameters<Object> params = new GeographyParameters<Object>();
		GeographyFactory geographyFactory = GeographyFactoryFinder.createGeographyFactory(null);
		Geography<Object> geography = geographyFactory.createGeography("Valle de Aburra", context, params);

		// Add border to context and projection
		Border border = new Border(borderGeometry);
		border.setGeometryInGeography(geography);
		context.add(border);

		// Add zones to context and projection
		for (Zone zone : zoneList) {
			zone.setGeometryInGeography(geography);
			context.add(zone);
		}

		// Get simulation parameters
		Parameters simParams = RunEnvironment.getInstance().getParameters();

		// Schedule policies
		PolicyEnforcer policyEnforcer = new PolicyEnforcer();
		String selectedPolicy = simParams.getString("policy");
		int policyStart = simParams.getInteger("policyStart");
		int policyEnd = simParams.getInteger("policyEnd");
		Policy policy = null;
		switch (selectedPolicy) {
		case "full-quarantine":
			policy = Policy.FULL_QUARANTINE;
			break;
		case "id-based-curfew":
			policy = Policy.ID_BASED_CURFEW;
		default:
			break;
		}
		if (policy != null) {
			policyEnforcer.schedulePolicy(policy, policyStart, policyEnd);
		}
		context.add(policyEnforcer);

		// Susceptible citizens
		int susceptibleCount = simParams.getInteger("susceptibleCount");
		for (int i = 0; i < susceptibleCount; i++) {
			Citizen citizen = new Citizen(geography, DiseaseStage.SUSCEPTIBLE, policyEnforcer);
			context.add(citizen);
		}

		// Infected citizens
		int infectedCount = simParams.getInteger("infectedCount");
		for (int i = 0; i < infectedCount; i++) {
			Citizen citizen = new Citizen(geography, DiseaseStage.INFECTED, policyEnforcer);
			context.add(citizen);
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
		List<Coordinate> agentCoordinates = GeometryUtil.generateRandomPointsInPolygon(borderGeometry,
				citizenList.size());
		GeometryFactory geometryFactory = new GeometryFactory();
		for (int i = 0; i < agentCoordinates.size(); i++) {
			Citizen citizen = citizenList.get(i);
			Coordinate coordinate = agentCoordinates.get(i);
			Point pointAgent = geometryFactory.createPoint(coordinate);
			geography.move(citizen, pointAgent);
			citizen.setGeometry(geography.getGeometry(citizen));
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
		HashMap<Zone, ArrayList<NdPoint>> houses = new HashMap<Zone, ArrayList<NdPoint>>();
		for (Citizen citizen : uniqueFamilies) {
			Heuristics.createHouse(citizen, houses, geography, zoneList);
		}

		// Assign workplaces
		for (Citizen citizen : citizenList) {
			Heuristics.assignWorkplace(citizen, eod, zoneList);
		}

		OutputManager outputManager = new OutputManager(citizenList);
		context.add(outputManager);

		RunEnvironment.getInstance().endAt(3 * 30 * TickConverter.TICKS_PER_DAY);

		return context;
	}

}