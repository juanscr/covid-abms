package styles;

import java.awt.Color;

import geography.Border;
import gov.nasa.worldwind.render.SurfacePolyline;
import gov.nasa.worldwind.render.SurfaceShape;
import repast.simphony.visualization.gis3D.style.SurfaceShapeStyle;

public class BorderStyle implements SurfaceShapeStyle<Border> {
	
	@Override
	public SurfaceShape getSurfaceShape(Border borderObject, SurfaceShape shape) {
		return new SurfacePolyline();
	}
	
	@Override
	public Color getLineColor(Border zone) {
		return Color.BLACK;
	}

	@Override
	public double getLineOpacity(Border obj) {
		return 1;
	}

	@Override
	public double getLineWidth(Border obj) {
		return 1;
	}

	@Override
	public Color getFillColor(Border obj) {
		return Color.WHITE;
	}

	@Override
	public double getFillOpacity(Border obj) {
		// TODO Auto-generated method stub
		return 0;
	}

}