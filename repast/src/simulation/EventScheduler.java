package simulation;

import repast.simphony.engine.environment.RunEnvironment;
import repast.simphony.engine.schedule.ISchedulableAction;
import repast.simphony.engine.schedule.ISchedule;
import repast.simphony.engine.schedule.ScheduleParameters;
import repast.simphony.essentials.RepastEssentials;

public class EventScheduler {

	private static EventScheduler instance;

	private EventScheduler() {
	}

	public static EventScheduler getInstance() {
		if (instance == null) {
			instance = new EventScheduler();
		}
		return instance;
	}

	public void scheduleOneTimeEvent(double ticksToEvent, Object obj, String methodName, Object... methodParams) {
		ISchedule schedule = RunEnvironment.getInstance().getCurrentSchedule();
		double currentTick = Math.max(RepastEssentials.GetTickCount(), 0);
		double startTime = currentTick + ticksToEvent;
		ScheduleParameters params = ScheduleParameters.createOneTime(startTime);
		schedule.schedule(params, obj, methodName, methodParams);
	}

	public ISchedulableAction scheduleRecurringEvent(double ticksToEvent, Object obj, double tickInterval,
			String methodName, Object... methodParams) {
		ISchedule schedule = RunEnvironment.getInstance().getCurrentSchedule();
		double currentTick = Math.max(RepastEssentials.GetTickCount(), 0);
		double startTime = currentTick + ticksToEvent;
		ScheduleParameters params = ScheduleParameters.createRepeating(startTime, tickInterval);
		return schedule.schedule(params, obj, methodName, methodParams);
	}

}