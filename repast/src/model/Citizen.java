package model;

import java.util.ArrayList;
import com.vividsolutions.jts.geom.Coordinate;
import com.vividsolutions.jts.geom.Geometry;
import geography.Zone;
import repast.simphony.context.Context;
import repast.simphony.engine.environment.RunEnvironment;
import repast.simphony.engine.schedule.ISchedulableAction;
import repast.simphony.engine.schedule.ISchedule;
import repast.simphony.parameter.Parameters;
import repast.simphony.query.space.gis.GeographyWithin;
import repast.simphony.random.RandomHelper;
import repast.simphony.space.continuous.NdPoint;
import repast.simphony.space.gis.Geography;
import repast.simphony.space.graph.Network;
import simulation.Scheduler;

public class Citizen implements Subject {

	// Personal attributes
	private int id;
	private int age;
	private ArrayList<Citizen> family;

	// Routine attributes
	private boolean atHome;
	private boolean dayShift;
	private double wakeUpTime;
	private double returnToHomeTime;

	// Health attributes
	private DiseaseStage diseaseStage;
	private PatientType patientType;
	private double timeToDeath;
	private double timeToImmune;
	private double incubationTime;
	private double incubationShift;

	// Projection attributes
	private Context<Object> context;
	private Geography<Object> geography;
	private Geometry geometry;
	private NdPoint homeplace;
	private NdPoint workplace;
	private Zone zone;
	private GeographyWithin<Object> within;

	// Simulation attributes
	private Parameters params;
	private ArrayList<Observer> observers;

	// Event attributes
	private ISchedulableAction stepAction;
	private ISchedulableAction wakeUpAction;
	private ISchedulableAction returnHomeAction;

	public Citizen(Context<Object> context, Geography<Object> geography, Geometry boundary, Parameters params, int id,
			int age, DiseaseStage stage) {
		super();
		this.id = id;
		this.context = context;
		this.geography = geography;
		this.age = age;
		this.diseaseStage = stage;
		this.params = params;
		this.age = age;
		this.diseaseStage = stage;
		this.family = new ArrayList<Citizen>();
		this.observers = new ArrayList<Observer>();
		init();
	}

	public void step() {
		travel();
		if (diseaseStage == DiseaseStage.INFECTED)
			infect();
		if (diseaseStage == DiseaseStage.INFECTED || diseaseStage == DiseaseStage.EXPOSED)
			incubationShift++;
	}

	public void wakeUp() {
		if (PolicyEnforcer.getInstance().isAllowedToGoOut(this)) {
			atHome = false;
			relocate(workplace);
		}
	}

	public void returnHome() {
		if (!atHome) {
			atHome = true;
			relocate(homeplace);
		}
	}

	public void relocate(NdPoint destination) {
		// Geography movement
		Coordinate coordinate = geometry.getCoordinate();
		coordinate.x = destination.getX();
		coordinate.y = destination.getY();
		geography.move(this, geometry);
		if (within == null) {
			double distance = params.getDouble("infectionRadius");
			within = new GeographyWithin<Object>(geography, distance, geometry);
		}
	}

	public void setExposed() {
		diseaseStage = DiseaseStage.EXPOSED;
		assignPatientType();
		incubationTime = Probabilities.getRandomIncubationTime();
		incubationShift = -incubationTime;
		double timeToInfectious = Math.max(incubationTime + Probabilities.INFECTION_MIN, 1);
		Scheduler.getInstance().scheduleOneTimeEvent(timeToInfectious, this, "setInfected");
	}

	public void setInfected() {
		notifyNewCase();
		diseaseStage = DiseaseStage.INFECTED;
		boolean isGoingToDie = Probabilities.isGoingToDie(patientType);
		if (isGoingToDie) {
			timeToDeath = Probabilities.getRandomTimeToDeath(patientType);
			Scheduler.getInstance().scheduleOneTimeEvent(timeToDeath, this, "kill");
		} else {
			timeToImmune = Probabilities.getRandomTimeToImmune(patientType);
			Scheduler.getInstance().scheduleOneTimeEvent(timeToImmune, this, "setImmune");
		}
	}

	public void setImmune() {
		diseaseStage = DiseaseStage.IMMUNE;
	}

	public void kill() {
		diseaseStage = DiseaseStage.DEAD;
		removeScheduledEvents();
		notifyDeath();
	}

	@Override
	public void attach(Observer o) {
		observers.add(o);
	}

	@Override
	public void detach(Observer o) {
		observers.remove(o);
	}

	@Override
	public void notifyNewCase() {
		for (Observer o : observers) {
			o.reportNewCase(this);
		}
	}

	public int getId() {
		return id;
	}

	public DiseaseStage getDiseaseStage() {
		return diseaseStage;
	}

	public void setDiseaseStage(DiseaseStage diseaseStage) {
		this.diseaseStage = diseaseStage;
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

	public NdPoint getWorkplace() {
		return workplace;
	}

	public void setWorkplace(NdPoint workplaceLocation) {
		this.workplace = workplaceLocation;
	}

	public int getAge() {
		return age;
	}

	public Zone getZone() {
		return zone;
	}

	public void setZone(Zone zone) {
		this.zone = zone;
	}

	public void setGeometry(Geometry geometry) {
		this.geometry = geometry;
	}

	private void linkInInfectionNetwork(Citizen citizen) {
		@SuppressWarnings("unchecked")
		Network<Object> net = (Network<Object>) context.getProjection("infectionNetwork");
		net.addEdge(this, citizen);
	}

	private void init() {
		initDisease();
		selectShift();
		dailyRoutine();
	}

	private void initDisease() {
		switch (diseaseStage) {
		case EXPOSED:
			setExposed();
			break;
		case INFECTED:
			assignPatientType();
			setInfected();
			break;
		default:
			break;
		}
	}

	private void selectShift() {
		double random = RandomHelper.nextDoubleFromTo(0, 1);
		dayShift = random < Probabilities.DAY_SHIFT_PROBABILITY;
	}

	private void dailyRoutine() {
		atHome = true;
		wakeUpTime = Probabilities.getRandomWakeUpTime(this.dayShift);
		returnToHomeTime = Probabilities.getRandomReturnToHomeTime(this.dayShift);
		stepAction = Scheduler.getInstance().scheduleRecurringEvent(1, this, 1, "step");
		wakeUpAction = Scheduler.getInstance().scheduleRecurringEvent(wakeUpTime, this, ModelParameters.HOURS_IN_DAY,
				"wakeUp");
		returnHomeAction = Scheduler.getInstance().scheduleRecurringEvent(returnToHomeTime, this,
				ModelParameters.HOURS_IN_DAY, "returnHome");
	}

	private void travel() {
		double distance = 2 * RandomHelper.nextDoubleFromTo(0, zone.getWalkAverage()) - zone.getWalkAverage();
		if (!atHome)
			distance = 2 * RandomHelper.nextDoubleFromTo(0, ModelParameters.MAX_MOVEMENT_IN_DESTINATION)
					- ModelParameters.MAX_MOVEMENT_IN_DESTINATION;
		double theta = RandomHelper.nextDoubleFromTo(0, 2 * Math.PI);
		geography.moveByVector(this, distance, theta);
	}

	private void infect() {
		for (Object obj : within.query()) {
			if (obj instanceof Citizen) {
				Citizen citizen = (Citizen) obj;
				if (citizen.diseaseStage == DiseaseStage.SUSCEPTIBLE
						&& Probabilities.isGettingExposed(incubationShift)) {
					citizen.setExposed();
					linkInInfectionNetwork(citizen);
				}
			}
		}
	}

	private void assignPatientType() {
		patientType = Probabilities.getRandomPatientType();
	}

	private void removeScheduledEvents() {
		ISchedule schedule = RunEnvironment.getInstance().getCurrentSchedule();
		schedule.removeAction(stepAction);
		schedule.removeAction(wakeUpAction);
		schedule.removeAction(returnHomeAction);
	}

	@Override
	public void notifyDeath() {
		for (Observer o : observers) {
			o.reportDeath(this);
		}
	}

}