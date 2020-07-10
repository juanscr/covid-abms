package output;

import java.util.ArrayList;
import model.Citizen;
import model.DiseaseStage;

/**
 * Output manager
 * 
 * @author Paula Escudero
 * @author Mateo Bonnett
 * @author David Plazas
 * @author Juan Sebastián Cárdenas
 * @author David Andrés Romero
 */
public class OutputManager {

	/**
	 * Citizens
	 */
	private ArrayList<Citizen> citizens;

	/**
	 * Create a new output manager
	 * 
	 * @param citizens Citizens
	 */
	public OutputManager(ArrayList<Citizen> citizens) {
		this.citizens = citizens;
	}

	/**
	 * Measure the current effective reproductive number
	 */
	public double measureRe() {
		int infected = 0;
		int infections = 0;
		for (Citizen citizen : this.citizens) {
			if (citizen.getDiseaseStage() == DiseaseStage.INFECTED) {
				infections += citizen.getInfections();
				infected++;
			}
		}
		double re = 0.0;
		if (infected > 0) {
			re = (double) infections / infected;
		}
		return re;
	}

}