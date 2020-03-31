package model;

import java.util.ArrayList;
import repast.simphony.engine.schedule.ScheduledMethod;
import repast.simphony.random.RandomHelper;
import repast.simphony.space.SpatialMath;
import repast.simphony.space.continuous.ContinuousSpace;
import repast.simphony.space.continuous.NdPoint;
import repast.simphony.space.grid.Grid;

public class Citizen {
	private int age;                  
	private DiseaseStage stage;       
	private int timeInfected;         
	private int lengthInfectious;
	private int timeIncubation;
	private ArrayList<Citizen> family;
	private NdPoint homeplaceLocation;
	private boolean inQuarantine;
	private ContinuousSpace<Object> space;
	private Grid<Object> grid;
	
	

	public Citizen(ContinuousSpace < Object > space , Grid < Object > grid, int age, DiseaseStage stage) {
		super();
		this.age = age;
		this.stage = stage;
		this.timeInfected = 0;
		this.lengthInfectious = 0;
		this.timeIncubation = 0;
		this.inQuarantine = false;
		this.space = space;
		this.grid = grid;
		
		
	}
	
	@ScheduledMethod ( start = 1 , interval = 1)
	public void step() {
		
		NdPoint myPoint = space.getLocation(this);
		double dirX = RandomHelper.nextDoubleFromTo(0, 1);
		double dirY = RandomHelper.nextDoubleFromTo(0, 1);
		NdPoint otherPoint = new NdPoint(dirX, dirY);
		double angle = SpatialMath.calcAngleFor2DMovement(space, myPoint , otherPoint);
		space.moveByVector(this, 1, angle, 0);
		myPoint = space.getLocation(this);
		grid.moveTo(this, (int) myPoint.getX(), (int) myPoint.getY());
		
	}

	public DiseaseStage getStage() {
		return stage;
	}

	public void setStage(DiseaseStage stage) {
		this.stage = stage;
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

	public NdPoint getHomeplaceLocation() {
		return homeplaceLocation;
	}

	public void setHomeplaceLocation(NdPoint homeplaceLocation) {
		this.homeplaceLocation = homeplaceLocation;
	}

	public int getAge() {
		return age;
	}
	
	
	
	
}


