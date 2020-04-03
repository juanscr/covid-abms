package model;

import java.util.ArrayList;
import repast.simphony.context.Context;
import repast.simphony.engine.schedule.ScheduledMethod;
import repast.simphony.query.space.continuous.ContinuousWithin;
import repast.simphony.random.RandomHelper;
import repast.simphony.space.continuous.ContinuousSpace;
import repast.simphony.space.continuous.NdPoint;
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
	private ContinuousSpace<Object> space;
	private NdPoint homeplace;

	public Citizen(Context<Object> context, ContinuousSpace<Object> space, int age, DiseaseStage stage) {
		super();
		this.context = context;
		this.space = space;
		this.age = age;
		this.diseaseStage = stage;
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
		double distance = 2 * RandomHelper.nextIntFromTo(0, 1) - 1;
		double theta = RandomHelper.nextDoubleFromTo(0, 2 * Math.PI);
		space.moveByVector(this, distance, theta, 0);
	}

	public void relocate(NdPoint destination) {
		space.moveTo(this, destination.getX(), destination.getY());
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
