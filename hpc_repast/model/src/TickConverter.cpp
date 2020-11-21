#include "TickConverter.h"

/**
 * Days to ticks
 * @param days Days
 */
double TickConverter::daysToTicks(double days){
    return days * TickConverter::TICKS_PER_DAY;
}

/**
 * Ticks to days
 * @param ticks Ticks
*/
double TickConverter::ticksToDays(double ticks){
    return ticks / TickConverter::TICKS_PER_DAY;
}