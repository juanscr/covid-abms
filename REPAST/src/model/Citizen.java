package model;

import java.util.ArrayList;

import com.vividsolutions.jts.geom.Coordinate;
import com.vividsolutions.jts.geom.Geometry;

import repast.simphony.context.Context;
import repast.simphony.engine.schedule.ScheduledMethod;
import repast.simphony.query.space.continuous.ContinuousWithin;
import repast.simphony.random.RandomHelper;
import repast.simphony.space.continuous.NdPoint;
import repast.simphony.space.gis.Geography;
import simulation.Scheduler;

public class Citizen {

	// Personal attributes
	private int age;
	private ArrayList<Citizen> family;

	// Health attributes
	private DiseaseStage diseaseStage;
	private int timeInfected;
	private int lengthInfectious;
	private double incubationTime;
	private boolean inQuarantine;

	// Projection attributes
	private Context<Object> context;
	private Geography<Object> geography;
	private Geometry boundary;
	private NdPoint homeplace;
	
	// Movement attributes
	private static final int MAX_MOVEMENT = 1000;

	public Citizen(Context<Object> context, Geography<Object> geography, Geometry boundary,
			       int age, DiseaseStage stage) {
		super();
		this.context = context;
		this.age = age;
		this.geography = geography;
		this.diseaseStage = stage;
		this.boundary = boundary;
		this.family = new ArrayList<Citizen>();
	}

	@ScheduledMethod(start = 1, interval = 1)
	public void step() {
		travel();
		if (diseaseStage == DiseaseStage.INFECTED) {
			infect();
		}
	}

	public void travel() {
		// Geography movement
		double distance = 2 * RandomHelper.nextDoubleFromTo(0, MAX_MOVEMENT) - MAX_MOVEMENT;
		double theta = RandomHelper.nextDoubleFromTo(0, 2 * Math.PI);
		geography.moveByVector(this, distance, theta);
		
		if (!boundary.contains(geography.getGeometry(this)))
			geography.moveByVector(this, -2 * distance, theta);
	}

	public void relocate(NdPoint destination) {
		// Geography movement
		Geometry geometry = geography.getGeometry(this);
		Coordinate coordinate = geometry.getCoordinate();
		coordinate.x = destination.getX();
		coordinate.y = destination.getY();
		geography.move(this, geometry);
	}

	public void infect() {
		ContinuousWithin<Object> within = new ContinuousWithin<Object>(context, this, 1);
		for (Object obj : within.query()) {
			Citizen citizen = (Citizen) obj;
			if (citizen.diseaseStage == DiseaseStage.SUSCEPTIBLE) {
				double r = RandomHelper.nextDoubleFromTo(0, 1);
				if (r < Probabilities.INFECTION_PROBABILITY) {
					citizen.setExposed();
				}
			}
		}
	}

	public void setExposed() {
		diseaseStage = DiseaseStage.EXPOSED;
		this.incubationTime = Probabilities.getRandomIncubationTime();
		Scheduler.getInstance().scheduleOneTimeEvent(this.incubationTime, this, "setInfected");
	}

	public void setInfected() {
		diseaseStage = DiseaseStage.INFECTED;
	}

	public DiseaseStage getDiseaseStage() {
		return diseaseStage;
	}

	public void setDiseaseStage(DiseaseStage diseaseStage) {
		this.diseaseStage = diseaseStage;
	}

	public int getTimeInfected() {
		return timeInfected;
	}

	public void setTimeInfected(int timeInfected) {
		this.timeInfected = timeInfected;
	}

	public int getLengthInfectious() {
		return lengthInfectious;
	}

	public void setLengthInfectious(int lengthInfectious) {
		this.lengthInfectious = lengthInfectious;
	}

	public ArrayList<Citizen> getFamily() {
		return family;
	}

	public void setFamily(ArrayList<Citizen> family) {
		this.family = family;
	}

	public NdPoint getHomeplace() {
		return homeplace;
	}

	public void setHomeplace(NdPoint homeplaceLocation) {
		this.homeplace = homeplaceLocation;
	}

	public int getAge() {
		return age;
	}

}
