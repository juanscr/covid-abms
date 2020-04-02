package styles;

import model.Citizen;
import model.DiseaseStage;
import java.awt.Color;
import repast.simphony.visualizationOGL2D.DefaultStyleOGL2D;

public class CitizenStyle extends DefaultStyleOGL2D {

	@Override
	public Color getColor(Object object) {
		Citizen citizen = (Citizen) object;
		DiseaseStage diseaseStage = citizen.getDiseaseStage();
		switch (diseaseStage) {
		case SUSCEPTIBLE:
			return Color.BLUE;
		case EXPOSED:
			return Color.ORANGE;
		case INFECTED:
			return Color.RED;
		case IMMUNE:
			return Color.GREEN;
		case DEAD:
			return Color.BLACK;
		default:
			return Color.GRAY;
		}
	}

	@Override
	public float getScale(Object object) {
		Citizen citizen = (Citizen) object;
		int age = citizen.getAge();
		if (age < 20) {
			return 1f;
		} else if (age < 60) {
			return 5f;
		} else {
			return 10f;
		}
	}

}