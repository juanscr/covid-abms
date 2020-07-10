package geography;

import com.vividsolutions.jts.geom.Geometry;
import repast.simphony.space.gis.Geography;

/**
 * Geo-spatial polygon
 * 
 * @author Paula Escudero
 * @author Mateo Bonnett
 * @author David Plazas
 * @author Juan Sebastián Cárdenas
 * @author David Andrés Romero
 */
public abstract class GISPolygon {

	/**
	 * Reference to geometry
	 */
	protected Geometry geometry;

	/**
	 * Polygon id
	 */
	protected int id;

	/**
	 * Reference to geography projection
	 */
	protected Geography<Object> geography;

	/**
	 * Create a new geo-spatial polygon
	 * 
	 * @param geometry Reference to geometry
	 */
	public GISPolygon(Geometry geometry) {
		this.geometry = geometry;
	}

	/**
	 * Create a new geo-spatial polygon
	 * 
	 * @param id       Polygon id
	 * @param geometry Reference to geometry
	 */
	public GISPolygon(int id, Geometry geometry) {
		this.id = id;
		this.geometry = geometry;
	}

	/**
	 * Set geometry in the geography projection
	 * 
	 * @param geography Reference to geography projection
	 */
	public void setGeometryInGeography(Geography<Object> geography) {
		this.geography = geography;
		this.geography.move(this, this.geometry);
	}

	/**
	 * Get polygon id
	 */
	public int getId() {
		return this.id;
	}

	/**
	 * Get reference to geometry
	 */
	public Geometry getGeometry() {
		return this.geometry;
	}

}