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

	public int newCaseCounter = 0;
	public ArrayList<Integer> newCases = new ArrayList<Integer>();

	public int deathCounter = 0;
	public ArrayList<Integer> dailyDeaths = new ArrayList<Integer>();

	@ScheduledMethod(start = 24, interval = 24)
	public void recordDailyNewCases() {
		stream.add(dailyNewCases);
		dailyNewCases = new ArrayList<Citizen>();
		newCases.add(newCaseCounter);
		newCaseCounter = 0;
	}

	@ScheduledMethod(start = 24, interval = 24)
	public void recordDailyDeaths() {
		dailyDeaths.add(deathCounter);
		deathCounter = 0;
	}

	public void reportNewCase(Citizen citizen) {
		dailyNewCases.add(citizen);
		newCaseCounter++;
	}

	@Override
	public void reportDeath(Citizen citizen) {
		deathCounter++;
	}

	@ScheduledMethod(start = ModelParameters.SIMULATION_END - 1)
	public void flush() {
		String outFile = ModelParameters.OUTPUT_FOLDER + "/" + ModelParameters.DAILY_NEW_CASES_FILE;
		FileWriter writer;
		try {
			writer = new FileWriter(outFile);
			/*writer.append("Day; Zone\n");
			for (int i = 0; i < stream.size(); i++) {
				for (Citizen citizen : stream.get(i)) {
					writer.append((i + 1) + ";" + citizen.getZone().getId() + "\n");
				}
			}*/
			writer.append("Day; NewCases\n");
			for (int i = 0; i < newCases.size(); i++) {
				int nc = newCases.get(i);
				writer.append((i + 1) + ";" + nc + "\n");
			}
			writer.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
		outFile = ModelParameters.OUTPUT_FOLDER + "/" + ModelParameters.DAILY_DEATHS;
		writer = null;
		try {
			writer = new FileWriter(outFile);
			writer.append("Day; Deaths\n");
			for (int i = 0; i < dailyDeaths.size(); i++) {
				int deaths = dailyDeaths.get(i);
				writer.append((i + 1) + ";" + deaths + "\n");
			}
			writer.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

}