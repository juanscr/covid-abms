package model;

import java.util.ArrayList;
import com.vividsolutions.jts.geom.Coordinate;
import com.vividsolutions.jts.geom.Envelope;
import com.vividsolutions.jts.geom.Geometry;
import geography.Zone;
import repast.simphony.engine.environment.RunEnvironment;
import repast.simphony.engine.schedule.ISchedulableAction;
import repast.simphony.engine.schedule.ISchedule;
import repast.simphony.engine.schedule.ScheduledMethod;
import repast.simphony.gis.util.GeometryUtil;
import repast.simphony.parameter.Parameters;
import repast.simphony.random.RandomHelper;
import repast.simphony.space.continuous.NdPoint;
import repast.simphony.space.gis.Geography;
import simulation.EventScheduler;
import util.TickConverter;

public class Citizen {

	/**
	 * Displacement factor
	 */
	public static final double DISPLACEMENT_FACTOR = 0.0001;

	/**
	 * Maximum movement in destination (unit: meters). Reference: <pending>
	 */
	public static final int MAX_MOVEMENT_IN_DESTINATION = 50;

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
		randomWalk();
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
		this.diseaseStage = DiseaseStage.EXPOSED;
		double incubationPeriod = Probabilities.getRandomIncubationPeriod();
		this.incubationShift = -TickConverter.daysToTicks(incubationPeriod);
		double infectiousPeriod = Math.max(incubationPeriod + Probabilities.INFECTION_MIN, 1);
		double ticks = TickConverter.daysToTicks(infectiousPeriod);
		EventScheduler eventScheduler = EventScheduler.getInstance();
		eventScheduler.scheduleOneTimeEvent(ticks, this, "setInfected");
	}

	/**
	 * Set infected
	 */
	public void setInfected() {
		this.diseaseStage = DiseaseStage.INFECTED;
		PatientType patientType = Probabilities.getRandomPatientType();
		double daysToEvent = 0.0;
		String method = "";
		if (Probabilities.isGoingToDie(patientType)) {
			daysToEvent = Probabilities.getRandomTimeToDeath(patientType);
			method = "kill";
		} else {
			daysToEvent = Probabilities.getRandomTimeToImmune(patientType);
			method = "setImmune";
		}
		double ticks = TickConverter.daysToTicks(daysToEvent);
		EventScheduler eventScheduler = EventScheduler.getInstance();
		eventScheduler.scheduleOneTimeEvent(ticks, this, method);
	}

	/**
	 * Set immune
	 */
	public void setImmune() {
		this.diseaseStage = DiseaseStage.IMMUNE;
	}

	/**
	 * Kill citizen
	 */
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

	/**
	 * Get citizen id
	 */
	public int getId() {
		return this.id;
	}

	/**
	 * Get disease stage
	 */
	public DiseaseStage getDiseaseStage() {
		return this.diseaseStage;
	}

	/**
	 * Get family members
	 */
	public ArrayList<Citizen> getFamily() {
		return this.family;
	}

	/**
	 * Set family members
	 * 
	 * @param family List of family members
	 */
	public void setFamily(ArrayList<Citizen> family) {
		this.family = family;
	}

	/**
	 * Get homeplace
	 */
	public NdPoint getHomeplace() {
		return this.homeplace;
	}

	/**
	 * Set homeplace
	 * 
	 * @param location Homeplace location
	 */
	public void setHomeplace(NdPoint location) {
		this.homeplace = location;
	}

	/**
	 * Get workplace
	 */
	public NdPoint getWorkplace() {
		return this.workplace;
	}

	/**
	 * Set workplace
	 * 
	 * @param location Workplace location
	 */
	public void setWorkplace(NdPoint location) {
		this.workplace = location;
	}

	/**
	 * Get age
	 */
	public int getAge() {
		return this.age;
	}

	/**
	 * Get current zone
	 */
	public Zone getZone() {
		return this.zone;
	}

	/**
	 * Set current zone
	 * 
	 * @param zone Zone
	 */
	public void setZone(Zone zone) {
		this.zone = zone;
	}

	/**
	 * Set reference to geometry
	 * 
	 * @param geometry Reference to geometry
	 */
	public void setGeometry(Geometry geometry) {
		this.geometry = geometry;
	}

	/**
	 * Is susceptible?
	 */
	public int isSusceptible() {
		return diseaseStage == DiseaseStage.SUSCEPTIBLE ? 1 : 0;
	}

	/**
	 * Is exposed?
	 */
	public int isExposed() {
		return diseaseStage == DiseaseStage.EXPOSED ? 1 : 0;
	}

	/**
	 * Is infected?
	 */
	public int isInfected() {
		return diseaseStage == DiseaseStage.INFECTED ? 1 : 0;
	}

	/**
	 * Is immune?
	 */
	public int isImmune() {
		return diseaseStage == DiseaseStage.IMMUNE ? 1 : 0;
	}

	/**
	 * Is dead?
	 */
	public int isDead() {
		return diseaseStage == DiseaseStage.DEAD ? 1 : 0;
	}

	/**
	 * Initialize disease
	 */
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

	/**
	 * Schedule recurring events
	 */
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

	/**
	 * Walk randomly
	 */
	private void randomWalk() {
		double distance = 0.0;
		if (this.atHome) {
			distance = this.zone.getWalkingAverage();
		} else {
			distance = MAX_MOVEMENT_IN_DESTINATION;
		}
		distance = distance * DISPLACEMENT_FACTOR;
		double x = RandomHelper.nextDoubleFromTo(-distance, distance);
		double y = RandomHelper.nextDoubleFromTo(-distance, distance);
		this.geography.moveByDisplacement(this, x, y);
	}

	/**
	 * Infect nearby susceptible individuals
	 */
	private void infect() {
		Parameters simParams = RunEnvironment.getInstance().getParameters();
		double distance = simParams.getDouble("infectionRadius");
		Geometry searchArea = GeometryUtil.generateBuffer(geography, geography.getGeometry(this), distance);
		Envelope searchEnvelope = searchArea.getEnvelopeInternal();
		Iterable<Citizen> citizens = geography.getObjectsWithin(searchEnvelope, Citizen.class);
		for (Citizen citizen : citizens) {
			if (citizen.diseaseStage == DiseaseStage.SUSCEPTIBLE && Probabilities.isGettingExposed(incubationShift)) {
				citizen.setExposed();
			}
		}
	}

	/**
	 * Is an active case (infected or exposed)?
	 */
	private boolean isActiveCase() {
		return this.diseaseStage == DiseaseStage.INFECTED || this.diseaseStage == DiseaseStage.EXPOSED;
	}

	/**
	 * Remove scheduled events
	 */
	private void removeScheduledEvents() {
		ISchedule schedule = RunEnvironment.getInstance().getCurrentSchedule();
		for (int i = 0; i < this.scheduledActions.size(); i++) {
			ISchedulableAction action = this.scheduledActions.get(i);
			schedule.removeAction(action);
			this.scheduledActions.remove(action);
		}
	}

}