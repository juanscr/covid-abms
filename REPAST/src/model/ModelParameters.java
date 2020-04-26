package model;

public abstract class ModelParameters {

	public static final double HOURS_IN_DAY = 24;
	
	public static final double DAYS_IN_MONTH = 30;
	
	public static final double SIMULATION_END = DAYS_IN_MONTH * HOURS_IN_DAY;

	public static final int MAX_MOVEMENT_IN_DESTINATION = 50;

	public static final String OUTPUT_FOLDER = "output";
	
	public static final String DAILY_NEW_CASES_FILE = "daily_cases.csv";

}