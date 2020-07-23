package model;

import java.util.Arrays;
import cern.jet.random.Gamma;
import cern.jet.random.Normal;
import repast.simphony.random.RandomHelper;
import util.TickConverter;

/**
 * Probabilities
 * 
 * @author Paula Escudero
 * @author Mateo Bonnett
 * @author David Plazas
 * @author Juan Sebastián Cárdenas
 * @author David Andrés Romero
 */
public abstract class Probabilities {

	/**
	 * Age ranges (unit: age). Reference: <pending>
	 */
	public static final int AGE_RANGES[][] = { { 0, 9 }, { 10, 19 }, { 20, 29 }, { 30, 39 }, { 40, 49 }, { 50, 59 },
			{ 60, 69 }, { 70, 79 }, { 80, 121 } };

	/**
	 * Age probabilities (unit: probability). Reference: <pending>
	 */
	public static final double AGE_PROBABILITIES[] = { 0.1443, 0.169, 0.1728, 0.1487, 0.1221, 0.1104, 0.0728, 0.0393,
			0.0206 };

	/**
	 * Daily travels (unit: travels). Reference: <pending>
	 */
	public static final int[] DAILY_TRAVELS = { 335, 2169, 3704, 9833, 70018, 328893, 610550, 481395, 314939, 244620,
			245991, 322370, 318179, 313987, 327201, 309527, 395493, 613719, 466570, 210368, 128377, 93656, 60591,
			23699 };

	/**
	 * Day shift probability (unit: probability). Reference: <pending>
	 */
	public static final double DAY_SHIFT_PROBABILITY = 0.7;

	/**
	 * Infection alpha parameter. Reference: <pending>
	 */
	public static final double INFECTION_ALPHA = 2.11;

	/**
	 * Infection beta parameter. Reference: <pending>
	 */
	public static final double INFECTION_BETA = 1.3;

	/**
	 * Infection minimum parameter. Reference: <pending>
	 */
	public static final double INFECTION_MIN = -2.4;

	/**
	 * Discharge alpha parameter. Reference: <pending>
	 */
	public static final double DISCHARGE_ALPHA = 1.99;

	/**
	 * Discharge beta parameter. Reference: <pending>
	 */
	public static final double DISCHARGE_BETA = 7.77;

	/**
	 * Incubation period mean parameter (unit: days). Reference: <pending>
	 */
	public static final double MEAN_INCUBATION_PERIOD = 5.52;

	/**
	 * Incubation period standard deviation parameter (unit: days). Reference:
	 * <pending>
	 */
	public static final double STD_INCUBATION_PERIOD = 2.41;

	/**
	 * Get random number based on a triangular distribution
	 * 
	 * @param min  Minimum
	 * @param mode Mode
	 * @param max  Maximum
	 */
	public static double getRandomTriangular(double min, double mode, double max) {
		double beta = (mode - min) / (max - min);
		double r = RandomHelper.nextDoubleFromTo(0, 1);
		double t = 0.0;
		if (r < beta) {
			t = Math.sqrt(beta * r);
		} else {
			t = Math.sqrt((1 - beta) * (1 - r));
		}
		return min + (max - min) * t;
	}

	/**
	 * Get random id. Reference: <pending>
	 */
	public static int getRandomId() {
		return RandomHelper.nextIntFromTo(0, 9);
	}

	/**
	 * Get random age (unit: age). Reference: <pending>
	 */
	public static int getRandomAge() {
		double r = RandomHelper.nextDoubleFromTo(0, 1);
		double cummulativeProbability = 0;
		for (int i = 0; i < AGE_PROBABILITIES.length; i++) {
			cummulativeProbability += AGE_PROBABILITIES[i];
			if (r < cummulativeProbability) {
				return RandomHelper.nextIntFromTo(AGE_RANGES[i][0], AGE_RANGES[i][1]);
			}
		}
		return -1;
	}

	/**
	 * Get random incubation period (unit: days). Reference: <pending>
	 */
	public static double getRandomIncubationPeriod() {
		double t = Math.pow(MEAN_INCUBATION_PERIOD, 2) + Math.pow(STD_INCUBATION_PERIOD, 2);
		double mu = Math.log(Math.pow(MEAN_INCUBATION_PERIOD, 2) / Math.sqrt(t));
		double sigma = Math.log(t / Math.pow(MEAN_INCUBATION_PERIOD, 2));
		Normal normalDistribution = RandomHelper.createNormal(mu, sigma);
		double y = normalDistribution.nextDouble();
		return Math.exp(y);
	}

	/**
	 * Get random patient type. Reference: <pending>
	 */
	public static PatientType getRandomPatientType() {
		double r1 = RandomHelper.nextDoubleFromTo(0, 1);
		if (r1 < 0.111) {
			return PatientType.NO_SYMPTOMS;
		} else {
			double r2 = RandomHelper.nextDoubleFromTo(0, 1);
			if (r2 < 0.814) {
				return PatientType.MODERATE_SYMPTOMS;
			} else if (r2 < 0.953) {
				return PatientType.SEVERE_SYMPTOMS;
			} else {
				return PatientType.CRITICAL_SYMPTOMS;
			}
		}
	}

	/**
	 * Is the patient going to die? Reference: <pending>
	 */
	public static boolean isGoingToDie(PatientType patientType) {
		double r = RandomHelper.nextDoubleFromTo(0, 1);
		switch (patientType) {
		case SEVERE_SYMPTOMS:
			return r < 0.15;
		case CRITICAL_SYMPTOMS:
			return r < 0.5;
		default:
			return false;
		}
	}

	/**
	 * Is the citizen getting exposed? Reference: <pending>
	 */
	public static boolean isGettingExposed(double incubationShift) {
		double r = RandomHelper.nextDoubleFromTo(0, 1);
		Gamma gamma = RandomHelper.createGamma(INFECTION_ALPHA, 1.0 / INFECTION_BETA);
		if (incubationShift < INFECTION_MIN) {
			return false;
		}
		double days = TickConverter.ticksToDays(incubationShift);
		double p = gamma.pdf(days - INFECTION_MIN);
		return r < p;
	}

	/**
	 * Get random time to discharge (unit: days). Reference: <pending>
	 */
	public static double getRandomTimeToDischarge() {
		Gamma gamma = RandomHelper.createGamma(DISCHARGE_ALPHA, 1.0 / DISCHARGE_BETA);
		return gamma.nextDouble();
	}

	/**
	 * Get random wake up time (unit: hours). Reference: <pending>
	 */
	public static double getRandomWakeUpTime(Shift workShift) {
		int[] travels;
		int displacement;
		if (workShift == Shift.DAY) {
			travels = Arrays.copyOfRange(DAILY_TRAVELS, 4, 10);
			displacement = 3;
		} else {
			travels = Arrays.copyOfRange(DAILY_TRAVELS, 18, 22);
			displacement = 17;
		}
		int sum = 0;
		for (int t : travels) {
			sum += t;
		}
		double r = RandomHelper.nextDoubleFromTo(0, 1);
		int acum = 0;
		for (int i = 0; i < travels.length; i++) {
			acum += travels[i];
			if (r <= acum / sum) {
				return RandomHelper.nextDoubleFromTo(0, 1) + i + displacement;
			}
		}
		return -1;
	}

	/**
	 * Get random return to home time (unit: hours). Reference: <pending>
	 */
	public static double getRandomReturnToHomeTime(Shift workShift) {
		int[] travels;
		int displacement;
		if (workShift == Shift.DAY) {
			travels = Arrays.copyOfRange(DAILY_TRAVELS, 13, 19);
			displacement = 12;
		} else {
			travels = Arrays.copyOfRange(DAILY_TRAVELS, 1, 6);
			displacement = 1;
		}
		int sum = 0;
		for (int t : travels) {
			sum += t;
		}
		double r = RandomHelper.nextDoubleFromTo(0, 1);
		int acum = 0;
		for (int i = 0; i < travels.length; i++) {
			acum += travels[i];
			if (r <= acum / sum) {
				return RandomHelper.nextDoubleFromTo(0, 1) + i + displacement;
			}
		}
		return 0;
	}

	/**
	 * Get random work shift. Reference: <pending>
	 */
	public static Shift getRandomWorkShift() {
		double random = RandomHelper.nextDoubleFromTo(0, 1);
		return (random < Probabilities.DAY_SHIFT_PROBABILITY) ? Shift.DAY : Shift.NIGHT;
	}

}