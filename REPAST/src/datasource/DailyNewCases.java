package datasource;

import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;

import model.Citizen;
import model.ModelParameters;
import model.Observer;
import repast.simphony.engine.schedule.ScheduledMethod;

public class DailyNewCases implements Observer {

	public ArrayList<Citizen> dailyNewCases = new ArrayList<Citizen>();
	public ArrayList<ArrayList<Citizen>> stream = new ArrayList<ArrayList<Citizen>>();

	@ScheduledMethod(start = 24, interval = 24)
	public void recordDailyNewCases() {
		stream.add(dailyNewCases);
		dailyNewCases = new ArrayList<Citizen>();
	}

	public void reportNewCase(Citizen citizen) {
		dailyNewCases.add(citizen);
	}

	@ScheduledMethod(start = ModelParameters.SIMULATION_END - 1)
	public void flush() {
		String outFile = ModelParameters.OUTPUT_FOLDER + "/" + ModelParameters.DAILY_NEW_CASES_FILE;
		FileWriter writer;
		try {
			writer = new FileWriter(outFile);
			writer.append("Day; Zone\n");
			for (int i = 0; i < stream.size(); i++) {
				for (Citizen citizen : stream.get(i)) {
					writer.append((i + 1) + ";" + citizen.getZone().getId() + "\n");
				}
			}
			writer.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

}