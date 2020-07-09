package util;

public class TickConverter {

	/**
	 * Ticks per week (unit: ticks)
	 */
	public static final double TICKS_PER_WEEK = 7;

	/**
	 * Ticks per day (unit: ticks)
	 */
	public static final double TICKS_PER_DAY = 24;

	/**
	 * Days to ticks
	 * 
	 * @param days Days
	 */
	public static double daysToTicks(double days) {
		return days * TICKS_PER_DAY;
	}

}