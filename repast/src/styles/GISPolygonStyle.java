package styles;

import gov.nasa.worldwind.render.SurfacePolyline;
import gov.nasa.worldwind.render.SurfaceShape;
import java.awt.Color;
import geography.GISPolygon;
import repast.simphony.visualization.gis3D.style.SurfaceShapeStyle;

/**
 * Polygon style
 * 
 * @author Paula Escudero
 * @author Mateo Bonnett
 * @author David Plazas
 * @author Juan Sebastián Cárdenas
 * @author David Andrés Romero
 */
public class GISPolygonStyle implements SurfaceShapeStyle<GISPolygon> {

	/**
	 * Get surface shape
	 * 
	 * @param polygon Polygon
	 * @param shape   Shape
	 */
	@Override
	public SurfaceShape getSurfaceShape(GISPolygon polygon, SurfaceShape shape) {
		return new SurfacePolyline();
	}

	/**
	 * Get fill color
	 * 
	 * @param polygon Polygon
	 */
	@Override
	public Color getFillColor(GISPolygon polygon) {
		return Color.black;
	}

	/**
	 * Get fill opacity
	 * 
	 * @param polygon Polygon
	 */
	@Override
	public double getFillOpacity(GISPolygon polygon) {
		return 0.25;
	}

	/**
	 * Get line color
	 * 
	 * @param polygon Polygon
	 */
	@Override
	public Color getLineColor(GISPolygon polygon) {
		return Color.black;
	}

	/**
	 * Get line opacity
	 * 
	 * @param polygon Polygon
	 */
	@Override
	public double getLineOpacity(GISPolygon polygon) {
		return 1.0;
	}

	/**
	 * Get line width
	 * 
	 * @param polygon Polygon
	 */
	@Override
	public double getLineWidth(GISPolygon polygon) {
		return 3;
	}

}