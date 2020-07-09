package model;

import java.util.ArrayList;

import com.vividsolutions.jts.geom.Coordinate;
import com.vividsolutions.jts.geom.Geometry;

import geography.Zone;
import repast.simphony.engine.environment.RunEnvironment;
import repast.simphony.engine.schedule.ISchedulableAction;
import repast.simphony.engine.schedule.ISchedule;
import repast.simphony.engine.schedule.ScheduledMethod;
import repast.simphony.parameter.Parameters;
import repast.simphony.query.space.gis.GeographyWithin;
import repast.simphony.random.RandomHelper;
import repast.simphony.space.continuous.NdPoint;
import repast.simphony.space.gis.Geography;
import simulation.EventScheduler;

public class Citizen {

	/**
	 * Citizen id
	 */
	private int id;

	/**
	 * Age
	 */
	private int age;

	/**
	 * Family
	 */
	private ArrayList<Citizen> family;

	/**
	 * Currently at home
	 */
	private boolean atHome;

	/**
	 * Work shift
	 */
	private Shift workShift;

	/**
	 * Wake up time
	 */
	private double wakeUpTime;

	/**
	 * Return to home time
	 */
	private double returnToHomeTime;

	/**
	 * Disease stage
	 */
	private DiseaseStage diseaseStage;

	/**
	 * Patient type
	 */
	private PatientType patientType;

	/**
	 * Ticks to incubation end
	 */
	private double incubationShift;

	/**
	 * Homeplace
	 */
	private NdPoint homeplace;

	/**
	 * Workplace
	 */
	private NdPoint workplace;

	/**
	 * Reference to geography projection
	 */
	private Geography<Object> geography;

	/**
	 * Reference to geometry
	 */
	private Geometry geometry;

	/**
	 * Current zone
	 */
	private Zone zone;

	/**
	 * Scheduled actions
	 */
	private ArrayList<ISchedulableAction> scheduledActions;

	/**
	 * Create a new citizen agent
	 * 
	 * @param geography    Reference to geography projection
	 * @param diseaseStage Disease stage
	 */
	public Citizen(Geography<Object> geography, DiseaseStage diseaseStage) {
		super();
		this.diseaseStage = diseaseStage;
		this.geography = geography;
		this.family = new ArrayList<>();
		this.scheduledActions = new ArrayList<>();
	}

	/**
	 * Initialize
	 */
	@ScheduledMethod(start = 0)
	public void init() {
		this.id = Probabilities.getRandomId();
		this.age = Probabilities.getRandomAge();
		this.atHome = true;
		this.workShift = Probabilities.getRandomWorkShift();
		this.wakeUpTime = Probabilities.getRandomWakeUpTime(this.workShift);
		this.returnToHomeTime = Probabilities.getRandomReturnToHomeTime(this.workShift);
		initDisease();
		scheduleRecurringEvents();
	}

	/**
	 * Step
	 */
	public void step() {
		travel();
		if (this.diseaseStage == DiseaseStage.INFECTED) {
			infect();
		}
		if (isActiveCase()) {
			this.incubationShift++;
		}
	}

	/**
	 * Wake up and go to workplace
	 */
	public void wakeUp() {
		PolicyEnforcer policyEnforcer = PolicyEnforcer.getInstance();
		if (policyEnforcer.isAllowedToGoOut(this)) {
			this.atHome = false;
			relocate(this.workplace);
		}
	}

	/**
	 * Return to homeplace
	 */
	public void returnHome() {
		if (!this.atHome) {
			this.atHome = true;
			relocate(this.homeplace);
		}
	}

	/**
	 * Set exposed
	 */
	public void setExposed() {
		EventScheduler eventScheduler = EventScheduler.getInstance();
		this.diseaseStage = DiseaseStage.EXPOSED;
		double incubationTime = Probabilities.getRandomIncubationTime();
		this.incubationShift = -incubationTime;
		double timeToInfectious = Math.max(incubationTime + Probabilities.INFECTION_MIN, 1);
		eventScheduler.scheduleOneTimeEvent(timeToInfectious, this, "setInfected");
	}

	/**
	 * Set infected
	 */
	public void setInfected() {
		EventScheduler eventScheduler = EventScheduler.getInstance();
		this.diseaseStage = DiseaseStage.INFECTED;
		this.patientType = Probabilities.getRandomPatientType();
		boolean isGoingToDie = Probabilities.isGoingToDie(this.patientType);
		if (isGoingToDie) {
			double timeToDeath = Probabilities.getRandomTimeToDeath(this.patientType);
			eventScheduler.scheduleOneTimeEvent(timeToDeath, this, "kill");
		} else {
			double timeToImmune = Probabilities.getRandomTimeToImmune(this.patientType);
			eventScheduler.scheduleOneTimeEvent(timeToImmune, this, "setImmune");
		}
	}

	/**
	 * Set immune
	 */
	public void setImmune() {
		this.diseaseStage = DiseaseStage.IMMUNE;
	}

	public void kill() {
		this.diseaseStage = DiseaseStage.DEAD;
		removeScheduledEvents();
	}

	/**
	 * Relocate to destination
	 * 
	 * @param destination Destination
	 */
	public void relocate(NdPoint destination) {
		Coordinate coordinate = this.geometry.getCoordinate();
		coordinate.x = destination.getX();
		coordinate.y = destination.getY();
		this.geography.move(this, this.geometry);
	}

	public int getId() {
		return this.id;
	}

	public DiseaseStage getDiseaseStage() {
		return this.diseaseStage;
	}

	public ArrayList<Citizen> getFamily() {
		return this.family;
	}

	public void setFamily(ArrayList<Citizen> family) {
		this.family = family;
	}

	public NdPoint getHomeplace() {
		return this.homeplace;
	}

	public void setHomeplace(NdPoint homeplaceLocation) {
		this.homeplace = homeplaceLocation;
	}

	public NdPoint getWorkplace() {
		return this.workplace;
	}

	public void setWorkplace(NdPoint workplaceLocation) {
		this.workplace = workplaceLocation;
	}

	public int getAge() {
		return this.age;
	}

	public Zone getZone() {
		return this.zone;
	}

	public void setZone(Zone zone) {
		this.zone = zone;
	}

	public void setGeometry(Geometry geometry) {
		this.geometry = geometry;
	}

	private void initDisease() {
		switch (this.diseaseStage) {
		case EXPOSED:
			setExposed();
			break;
		case INFECTED:
			setInfected();
			break;
		default:
			break;
		}
	}

	private void scheduleRecurringEvents() {
		EventScheduler eventScheduler = EventScheduler.getInstance();
		ISchedulableAction stepAction = eventScheduler.scheduleRecurringEvent(1, this, 1, "step");
		ISchedulableAction wakeUpAction = eventScheduler.scheduleRecurringEvent(this.wakeUpTime, this,
				ModelParameters.HOURS_IN_DAY, "wakeUp");
		ISchedulableAction returnHomeAction = eventScheduler.scheduleRecurringEvent(this.returnToHomeTime, this,
				ModelParameters.HOURS_IN_DAY, "returnHome");
		this.scheduledActions.add(stepAction);
		this.scheduledActions.add(wakeUpAction);
		this.scheduledActions.add(returnHomeAction);
	}

	private void travel() {
		double distance = 2 * RandomHelper.nextDoubleFromTo(0, zone.getWalkAverage()) - zone.getWalkAverage();
		if (!this.atHome) {
			distance = 2 * RandomHelper.nextDoubleFromTo(0, ModelParameters.MAX_MOVEMENT_IN_DESTINATION)
					- ModelParameters.MAX_MOVEMENT_IN_DESTINATION;
		}
		double theta = RandomHelper.nextDoubleFromTo(0, 2 * Math.PI);
		this.geography.moveByVector(this, distance, theta);
	}

	private void infect() {
		Parameters simParams = RunEnvironment.getInstance().getParameters();
		double distance = simParams.getDouble("infectionRadius");
		GeographyWithin<Object> within = new GeographyWithin<Object>(geography, distance, geometry);
		for (Object obj : within.query()) {
			if (obj instanceof Citizen) {
				Citizen citizen = (Citizen) obj;
				if (citizen.diseaseStage == DiseaseStage.SUSCEPTIBLE
						&& Probabilities.isGettingExposed(incubationShift)) {
					citizen.setExposed();
				}
			}
		}
	}

	/**
	 * Is an active case? (infected or exposed)
	 */
	private boolean isActiveCase() {
		return this.diseaseStage == DiseaseStage.INFECTED || this.diseaseStage == DiseaseStage.EXPOSED;
	}

	/**
	 * Remove scheduled events
	 */
	private void removeScheduledEvents() {
		ISchedule schedule = RunEnvironment.getInstance().getCurrentSchedule();
		for (ISchedulableAction action : this.scheduledActions) {
			schedule.removeAction(action);
			this.scheduledActions.remove(action);
		}
	}

}