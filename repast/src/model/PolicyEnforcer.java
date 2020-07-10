package model;

import java.util.ArrayList;
import repast.simphony.engine.schedule.ScheduledMethod;
import repast.simphony.essentials.RepastEssentials;
import util.TickConverter;

public class PolicyEnforcer {

	private static PolicyEnforcer instance;
	private ArrayList<Policy> strategies;
	private ArrayList<int[]> strategySchedule;
	private int[][] allowedIds = { { 7, 8 }, { 9, 0 }, { 1, 2 }, { 3, 4 }, { 5, 6 }, { 7, 8 }, { 9, 0, 1 } };
	private int currentStrategy;
	private int day;

	private PolicyEnforcer() {
		this.strategies = new ArrayList<Policy>();
		this.strategySchedule = new ArrayList<int[]>();
	}

	public static PolicyEnforcer getInstance() {
		if (instance == null) {
			instance = new PolicyEnforcer();
		}
		return instance;
	}

	@ScheduledMethod(start = 24, interval = 24)
	public void nextDay() {
		day = (day + 1) % allowedIds.length;
	}

	@ScheduledMethod(start = 1, interval = 1)
	public void updateStrategy() {
		double tick = RepastEssentials.GetTickCount();
		int[] schedule = strategySchedule.get(currentStrategy);
		if (!(schedule[0] <= tick && tick <= schedule[1])) {
			currentStrategy++;
		}
	}

	public boolean isAllowedToGoOut(Citizen citizen) {
		if (inQuarantine()) {
			int[] dailyIds = allowedIds[day];
			for (int i = 0; i < dailyIds.length; i++) {
				if (dailyIds[i] == citizen.getId()) {
					return true;
				}
			}
			return false;
		} else {
			return true;
		}
	}

	public boolean inQuarantine() {
		int[] schedule = strategySchedule.get(currentStrategy);
		double tick = RepastEssentials.GetTickCount();
		if ((schedule[0] <= tick && tick <= schedule[1])) {
			return strategies.get(currentStrategy) == Policy.ID_BASED_RESTRICTION;
		} else {
			return false;
		}
	}

	public void scheduleQuarantine(int start, int end) {
		strategies.add(Policy.NONE);
		strategies.add(Policy.ID_BASED_RESTRICTION);
		int[] noneInterval = { 0, start * TickConverter.TICKS_PER_DAY };
		int[] quarantineInterval = { start * TickConverter.TICKS_PER_DAY, end * TickConverter.TICKS_PER_DAY };
		strategySchedule.add(noneInterval);
		strategySchedule.add(quarantineInterval);
		if (end * TickConverter.TICKS_PER_DAY < 3 * 30 * TickConverter.TICKS_PER_DAY) { // simulation end
			strategies.add(Policy.NONE);
			int[] lastNoneInterval = { end * TickConverter.TICKS_PER_DAY, 3 * 30 * TickConverter.TICKS_PER_DAY }; // simulation
																													// end
			strategySchedule.add(lastNoneInterval);
		}
	}

	public void noPolicies() {
		strategies.add(Policy.NONE);
		// simulation end
		int[] noneInterval = { 0, 3 * 30 * TickConverter.TICKS_PER_DAY };
		strategySchedule.add(noneInterval);
	}

}