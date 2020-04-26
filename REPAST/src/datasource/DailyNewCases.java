package datasource;

import java.util.ArrayList;
import model.Observer;
import repast.simphony.engine.schedule.ScheduledMethod;

public class DailyNewCases implements Observer {

	public int dailyNewCases = 0;
	public ArrayList<Integer> stream = new ArrayList<Integer>();
	
	@ScheduledMethod(start=24, interval=24)
	public void recordDailyNewCases() {
		stream.add(dailyNewCases);
		dailyNewCases = 0;
		System.out.println(stream);
	}
	
	public void reportNewCase() {
		dailyNewCases++;
	}
	
}