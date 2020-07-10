package geography;

import com.vividsolutions.jts.geom.Geometry;

/**
 * Geo-spatial zone
 * 
 * @author Paula Escudero
 * @author Mateo Bonnett
 * @author David Plazas
 * @author Juan Sebastián Cárdenas
 * @author David Andrés Romero
 */
public class Zone extends GISPolygon {

	/**
	 * Walking average (unit: meters)
	 */
	private double walkingAverage;

	/**
	 * Create a new geo-spatial zone
	 * 
	 * @param id             Zone id
	 * @param geometry       Reference to geometry
	 * @param walkingAverage Walking average
	 */
	public Zone(int id, Geometry geometry, double walkingAverage) {
		super(id, geometry);
		this.walkingAverage = walkingAverage;
	}

	/**
	 * Get walking average
	 */
	public double getWalkingAverage() {
		return this.walkingAverage;
	}

}