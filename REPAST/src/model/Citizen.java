package model;

import java.util.ArrayList;
import java.util.List;
import repast.simphony.engine.schedule.ScheduledMethod;
import repast.simphony.query.space.grid.GridCell;
import repast.simphony.query.space.grid.GridCellNgh;
import repast.simphony.random.RandomHelper;
import repast.simphony.space.continuous.ContinuousSpace;
import repast.simphony.space.continuous.NdPoint;
import repast.simphony.space.grid.Grid;
import repast.simphony.space.grid.GridPoint;

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
	private NdPoint homePlace;

	public Citizen(ContinuousSpace<Object> space, Grid<Object> grid, int age, DiseaseStage stage) {
		super();
		this.space = space;
		this.grid = grid;
		this.age = age;
		this.diseaseStage = stage;
		
		family = new ArrayList<Citizen>();
	}

	@ScheduledMethod(start = 1, interval = 1)
	public void step() {
		travel();
		if (diseaseStage == DiseaseStage.INFECTED) {
			infect();
		}
	}

	public void travel() {
		// Move in continuous projection
		NdPoint myPoint = space.getLocation(this);
		double angle = RandomHelper.nextDoubleFromTo(-Math.PI / 2, Math.PI / 2);
		double distance = 1;
		if (angle < 0) {
			angle = 2 * Math.PI - angle;
			distance = -1;
		}
		space.moveByVector(this, distance, angle, 1);

		// Move in grid projection
		myPoint = space.getLocation(this);
		grid.moveTo(this, (int) myPoint.getX(), (int) myPoint.getY());
	}
	
	public void travel(NdPoint move) {
		space.moveTo(this, move.getX(), move.getY());
		grid.moveTo(this, (int) move.getX(), (int) move.getY());
	}

	public void infect() {
		GridPoint pt = grid.getLocation(this);
		GridCellNgh<Citizen> nghCreator = new GridCellNgh<Citizen>(grid, pt, Citizen.class, 0, 0);
		List<GridCell<Citizen>> gridCells = nghCreator.getNeighborhood(true);

		for (GridCell<Citizen> cell : gridCells) {
			for (Citizen citizen : cell.items()) {
				if (citizen.diseaseStage == DiseaseStage.SUSCEPTIBLE) {
					double r = RandomHelper.nextDoubleFromTo(0, 1);
					if (r < 0.80f) {
						citizen.setExposed();
					}
				}
			}
		}
	}

	public void setExposed() {
		diseaseStage = DiseaseStage.EXPOSED;
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

	public NdPoint getHomePlace() {
		return homePlace;
	}

	public void setHomePlace(NdPoint homeplaceLocation) {
		this.homePlace = homeplaceLocation;
	}

	public int getAge() {
		return age;
	}

}