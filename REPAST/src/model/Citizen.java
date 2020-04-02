package model;

import java.util.ArrayList;
import java.util.List;

import cern.jet.random.Gamma;
import repast.simphony.engine.environment.RunEnvironment;
import repast.simphony.engine.schedule.ISchedule;
import repast.simphony.engine.schedule.ScheduleParameters;
import repast.simphony.engine.schedule.ScheduledMethod;
import repast.simphony.essentials.RepastEssentials;
import repast.simphony.query.space.grid.GridCell;
import repast.simphony.query.space.grid.GridCellNgh;
import repast.simphony.random.RandomHelper;
import repast.simphony.space.continuous.ContinuousSpace;
import repast.simphony.space.continuous.NdPoint;
import repast.simphony.space.grid.Grid;
import repast.simphony.space.grid.GridPoint;
import simulation.Scheduler;

public class Citizen {

	// Personal attributes
	private int age;
	private ArrayList<Citizen> family;

	// Health attributes
	private DiseaseStage diseaseStage;
	private int timeInfected;
	private int lengthInfectious;
	private int incubationTime;
	private boolean inQuarantine;

	// Geographical attributes
	private ContinuousSpace<Object> space;
	private Grid<Object> grid;
	private NdPoint homeplace;
	
	// Constants
	private static final double INFECTION_PROBABILITY = 0.8;

	private static final double ALPHA_INFECTIOUS = 3.37;
	private static final double BETA_INFECTIOUS = 1.49;
	private static final double MIN_INFECTIOUS = 2.0;

	public Citizen(ContinuousSpace<Object> space, Grid<Object> grid, int age, DiseaseStage stage) {
		super();
		this.space = space;
		this.grid = grid;
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
		double distance = 1;
		double theta = RandomHelper.nextDoubleFromTo(-2*Math.PI, 2*Math.PI);
		if (theta < 0) {
			theta = 2 * Math.PI - theta;
			distance *= -1;
		}
		NdPoint destination = space.moveByVector(this, distance, theta, 0);
		relocate(destination);
	}
	
	public void relocate(NdPoint destination) {
		space.moveTo(this, destination.getX(), destination.getY());
		grid.moveTo(this, (int) destination.getX(), (int) destination.getY());
	}

	public void infect() {
		GridPoint pt = grid.getLocation(this);
		GridCellNgh<Citizen> nghCreator = new GridCellNgh<Citizen>(grid, pt, Citizen.class, 0, 0);
		List<GridCell<Citizen>> gridCells = nghCreator.getNeighborhood(true);

		for (GridCell<Citizen> cell : gridCells) {
			for (Citizen citizen : cell.items()) {
				if (citizen.diseaseStage == DiseaseStage.SUSCEPTIBLE) {
					double r = RandomHelper.nextDoubleFromTo(0, 1);
					if (r < INFECTION_PROBABILITY) {
						citizen.setExposed();
					}
				}
			}
		}
	}

	public void setExposed() {
		diseaseStage = DiseaseStage.EXPOSED;
		
		// Generate a gamma function random generator.
		Gamma gammaFunction = RandomHelper.createGamma(ALPHA_INFECTIOUS, 1 / BETA_INFECTIOUS);
		double timeInfectious = (gammaFunction.nextDouble() + MIN_INFECTIOUS) * 24;
		
		// Schedule the infection event
		Scheduler.getInstance().scheduleOneTimeEvent(timeInfectious, this, "setInfected");
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