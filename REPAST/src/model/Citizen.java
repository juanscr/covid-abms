package model;

import java.util.ArrayList;

import com.vividsolutions.jts.geom.Coordinate;
import com.vividsolutions.jts.geom.Geometry;
import repast.simphony.context.Context;
import repast.simphony.engine.environment.RunEnvironment;
import repast.simphony.engine.schedule.ISchedulableAction;
import repast.simphony.engine.schedule.ISchedule;
import repast.simphony.gis.util.GeometryUtil;
import repast.simphony.parameter.Parameters;
import repast.simphony.query.space.gis.GeographyWithin;
import repast.simphony.random.RandomHelper;
import repast.simphony.space.continuous.NdPoint;
import repast.simphony.space.gis.Geography;
import repast.simphony.space.graph.Network;
import simulation.Scheduler;

public class Citizen {

	// Personal attributes
	private int age;
	private ArrayList<Citizen> family;

	// Routine attributes
	private boolean atHome;
	private double wakeUpTime;
	private double returnToHomeTime;

	// Health attributes
	private DiseaseStage diseaseStage;
	private PatientType patientType;
	private double timeToDeath;
	private double timeToImmune;
	private double incubationTime;

	// Projection attributes
	private Context<Object> context;
	private Geography<Object> geography;
	private Geometry boundary;
	private NdPoint homeplace;
	private NdPoint workplace;

	// Simulation attributes
	Parameters params;

	// Event attributes
	private ISchedulableAction stepAction;
	private ISchedulableAction wakeUpAction;
	private ISchedulableAction returnHomeAction;

	// Movement attributes
	private static final int MAX_MOVEMENT = 1000;

	public Citizen(Context<Object> context, Geography<Object> geography, Geometry boundary, Parameters params, int age,
			DiseaseStage stage) {
		super();
		this.context = context;
		this.geography = geography;
		this.boundary = boundary;
		this.age = age;
		this.diseaseStage = stage;
		this.params = params;
		this.age = age;
		this.diseaseStage = stage;
		this.family = new ArrayList<Citizen>();
		init();
	}

	public void step() {
		if (!atHome)
			travel();
		if (diseaseStage == DiseaseStage.INFECTED) {
			infect();
		}
	}

	public void wakeUp() {
		atHome = false;
		relocate(workplace);
	}

	public void returnHome() {
		atHome = true;
		relocate(homeplace);
	}

	public void relocate(NdPoint destination) {
		// Geography movement
		Geometry geometry = geography.getGeometry(this);
		Coordinate coordinate = geometry.getCoordinate();
		coordinate.x = destination.getX();
		coordinate.y = destination.getY();
		geography.move(this, geometry);
	}

	public void setExposed() {
		if (Probabilities.isDevelopingActiveCase()) {
			diseaseStage = DiseaseStage.EXPOSED;
			assignPatientType();
			incubationTime = Probabilities.getRandomIncubationTime();
			Scheduler.getInstance().scheduleOneTimeEvent(incubationTime, this, "setInfected");
		} else {
			diseaseStage = DiseaseStage.SUSCEPTIBLE;
		}
	}

	public void setInfected() {
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

	public int getAge() {
		return age;
	}

	public int isSusceptible() {
		return diseaseStage == DiseaseStage.SUSCEPTIBLE ? 1 : 0;
	}

	public int isExposed() {
		return diseaseStage == DiseaseStage.EXPOSED ? 1 : 0;
	}

	public int isInfected() {
		return diseaseStage == DiseaseStage.INFECTED ? 1 : 0;
	}

	public int isImmune() {
		return diseaseStage == DiseaseStage.IMMUNE ? 1 : 0;
	}

	public int isDead() {
		return diseaseStage == DiseaseStage.DEAD ? 1 : 0;
	}

	public int isActiveCase() {
		return Math.min(isExposed() + isInfected(), 1);
	}

	private void linkInInfectionNetwork(Citizen citizen) {
		@SuppressWarnings("unchecked")
		Network<Object> net = (Network<Object>) context.getProjection("infectionNetwork");
		net.addEdge(this, citizen);
	}

	private void init() {
		initDisease();
		dailyRoutine();
		assignWorkplace();
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

	private void dailyRoutine() {
		atHome = true;
		wakeUpTime = Probabilities.getRandomWakeUpTime();
		returnToHomeTime = Probabilities.getRandomReturnToHomeTime();
		stepAction = Scheduler.getInstance().scheduleRecurringEvent(1, this, 1, "step");
		wakeUpAction = Scheduler.getInstance().scheduleRecurringEvent(wakeUpTime, this, ModelParameters.HOURS_IN_DAY,
				"wakeUp");
		returnHomeAction = Scheduler.getInstance().scheduleRecurringEvent(returnToHomeTime, this,
				ModelParameters.HOURS_IN_DAY, "returnHome");
	}

	private void assignWorkplace() {
		// TODO: Assign workplaces referring to EOD
		Coordinate coordinate = GeometryUtil.generateRandomPointsInPolygon(boundary, 1).get(0);
		this.workplace = new NdPoint(coordinate.x, coordinate.y);
	}

	private void travel() {
		// Geography movement
		double distance = 2 * RandomHelper.nextDoubleFromTo(0, MAX_MOVEMENT) - MAX_MOVEMENT;
		double theta = RandomHelper.nextDoubleFromTo(0, 2 * Math.PI);
		geography.moveByVector(this, distance, theta);
	}

	private void infect() {
		double distance = params.getDouble("infectionRadius");
		Geometry citizenGeometry = geography.getGeometry(this);
		GeographyWithin<Object> within = new GeographyWithin<Object>(geography, distance, citizenGeometry);
		for (Object obj : within.query()) {
			if (obj instanceof Citizen) {
				Citizen citizen = (Citizen) obj;
				if (citizen.diseaseStage == DiseaseStage.SUSCEPTIBLE && Probabilities.isGettingExposed()) {
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

}