#ifndef TICK_CONVERTER_H
#define TICK_CONVERTER_H

class TickConverter{
    private:

    public:

    // Values

    /**
	 * Ticks per week (unit: ticks)
	*/
    const int TICKS_PER_WEEK = 7;

    /**
	 * Ticks per day (unit: ticks)
	 */
	const int TICKS_PER_DAY = 24;

    /**
	 * Days to ticks
	 * @param days Days
	 */
    double daysToTicks(double days);

    /**
	 * Ticks to days
	 * @param ticks Ticks
	 */
    double ticksToDays(double ticks);

};

#endif