package model;

public abstract class ModelParameters {

	public static final int HOURS_IN_DAY = 24;

	public static final int DAYS_IN_MONTH = 30;

	public static final int SIMULATION_END = 2 * DAYS_IN_MONTH * HOURS_IN_DAY;

	public static final String OUTPUT_FOLDER = "output";

	public static final String DAILY_NEW_CASES_FILE = "daily_cases.csv";

	public static final String DAILY_DEATHS = "daily_deaths.csv";

	public static final String HOURLY_R0 = "hourly_r0.csv";

}