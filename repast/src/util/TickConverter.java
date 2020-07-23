package util;

/**
 * Tick converter
 * 
 * @author Paula Escudero
 * @author Mateo Bonnett
 * @author David Plazas
 * @author Juan Sebasti�n C�rdenas
 * @author David Andr�s Romero
 */
public class TickConverter {

	/**
	 * Ticks per week (unit: ticks)
	 */
	public static final int TICKS_PER_WEEK = 7;

	/**
	 * Ticks per day (unit: ticks)
	 */
	public static final int TICKS_PER_DAY = 24;

	/**
	 * Days to ticks
	 * 
	 * @param days Days
	 */
	public static double daysToTicks(double days) {
		return days * TICKS_PER_DAY;
	}

	/**
	 * Ticks to days
	 * 
	 * @param ticks Ticks
	 */
	public static double ticksToDays(double ticks) {
		return ticks / TICKS_PER_DAY;
	}

}