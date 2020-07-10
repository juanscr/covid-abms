package styles;

import model.Citizen;
import model.DiseaseStage;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.image.BufferedImage;
import java.util.HashMap;
import java.util.Map;
import gov.nasa.worldwind.WorldWind;
import gov.nasa.worldwind.render.BasicWWTexture;
import gov.nasa.worldwind.render.Material;
import gov.nasa.worldwind.render.Offset;
import gov.nasa.worldwind.render.PatternFactory;
import gov.nasa.worldwind.render.WWTexture;
import repast.simphony.visualization.gis3D.PlaceMark;
import repast.simphony.visualization.gis3D.style.MarkStyle;

public class CitizenStyle implements MarkStyle<Citizen> {

	private Map<String, WWTexture> textureMap;
	private static final float MAX_SCALE = 2f;

	public CitizenStyle() {
		textureMap = new HashMap<String, WWTexture>();

		Dimension dimension = new Dimension(3, 3);

		// Create colors
		// Black
		BufferedImage image = PatternFactory.createPattern(PatternFactory.PATTERN_CIRCLE, dimension, MAX_SCALE,
				Color.BLACK);
		textureMap.put("black circle", new BasicWWTexture(image));

		// Orange
		image = PatternFactory.createPattern(PatternFactory.PATTERN_CIRCLE, dimension, MAX_SCALE, Color.ORANGE);
		textureMap.put("orange circle", new BasicWWTexture(image));

		// Green
		image = PatternFactory.createPattern(PatternFactory.PATTERN_CIRCLE, dimension, MAX_SCALE, Color.GREEN);
		textureMap.put("green circle", new BasicWWTexture(image));

		// Red
		image = PatternFactory.createPattern(PatternFactory.PATTERN_CIRCLE, dimension, MAX_SCALE, Color.RED);
		textureMap.put("red circle", new BasicWWTexture(image));

		// Blue
		image = PatternFactory.createPattern(PatternFactory.PATTERN_CIRCLE, dimension, MAX_SCALE, Color.BLUE);
		textureMap.put("blue circle", new BasicWWTexture(image));

		// Gray color
		image = PatternFactory.createPattern(PatternFactory.PATTERN_CIRCLE, dimension, MAX_SCALE, Color.GRAY);
		textureMap.put("gray circle", new BasicWWTexture(image));
	}

	@Override
	public WWTexture getTexture(Citizen citizen, WWTexture texture) {
		DiseaseStage diseaseStage = citizen.getDiseaseStage();
		switch (diseaseStage) {
		case DEAD:
			return textureMap.get("black circle");
		case EXPOSED:
			return textureMap.get("orange circle");
		case IMMUNE:
			return textureMap.get("green circle");
		case INFECTED:
			return textureMap.get("red circle");
		case SUSCEPTIBLE:
			return textureMap.get("blue circle");
		default:
			return textureMap.get("gray circle");
		}
	}

	@Override
	public double getScale(Citizen citizen) {
		return MAX_SCALE;
	}

	@Override
	public Offset getIconOffset(Citizen obj) {
		return Offset.CENTER;
	}

	@Override
	public Offset getLabelOffset(Citizen obj) {
		return null;
	}

	@Override
	public PlaceMark getPlaceMark(Citizen object, PlaceMark mark) {

		// PlaceMark is null on first call.
		if (mark == null)
			mark = new PlaceMark();

		mark.setAltitudeMode(WorldWind.CLAMP_TO_GROUND);
		mark.setLineEnabled(false);

		return mark;
	}

	@Override
	public double getElevation(Citizen obj) {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public double getHeading(Citizen obj) {
		return 0;
	}

	@Override
	public Color getLabelColor(Citizen obj) {
		return null;
	}

	@Override
	public double getLineWidth(Citizen obj) {
		return 1;
	}

	@Override
	public Font getLabelFont(Citizen obj) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String getLabel(Citizen obj) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Material getLineMaterial(Citizen obj, Material lineMaterial) {
		if (lineMaterial == null) {
			lineMaterial = new Material(Color.RED);
		}
		return lineMaterial;
	}
}