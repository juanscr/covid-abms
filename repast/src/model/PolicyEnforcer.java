package model;

import java.util.ArrayList;
import repast.simphony.engine.schedule.ScheduledMethod;
import repast.simphony.essentials.RepastEssentials;

public class PolicyEnforcer {

	private static PolicyEnforcer instance;
	private ArrayList<Strategy> strategies;
	private ArrayList<int[]> strategySchedule;
	private int[][] allowedIds = { { 7, 8 }, { 9, 0 }, { 1, 2 }, { 3, 4 }, { 5, 6 }, { 7, 8 }, { 9, 0, 1 } };
	private int currentStrategy;
	private int day;

	private PolicyEnforcer() {
		this.strategies = new ArrayList<Strategy>();
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
			return strategies.get(currentStrategy) == Strategy.ID_BASED_RESTRICTION;
		} else {
			return false;
		}
	}

	public void scheduleQuarantine(int start, int end) {
		strategies.add(Strategy.NONE);
		strategies.add(Strategy.ID_BASED_RESTRICTION);
		int[] noneInterval = { 0, start * ModelParameters.HOURS_IN_DAY };
		int[] quarantineInterval = { start * ModelParameters.HOURS_IN_DAY, end * ModelParameters.HOURS_IN_DAY };
		strategySchedule.add(noneInterval);
		strategySchedule.add(quarantineInterval);
		if (end * ModelParameters.HOURS_IN_DAY < ModelParameters.SIMULATION_END) {
			strategies.add(Strategy.NONE);
			int[] lastNoneInterval = { end * ModelParameters.HOURS_IN_DAY, ModelParameters.SIMULATION_END };
			strategySchedule.add(lastNoneInterval);
		}
	}

	public void noPolicies() {
		strategies.add(Strategy.NONE);
		int[] noneInterval = { 0, ModelParameters.SIMULATION_END };
		strategySchedule.add(noneInterval);
	}
	
}