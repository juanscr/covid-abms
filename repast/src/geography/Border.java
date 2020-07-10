package geography;

import com.vividsolutions.jts.geom.Geometry;
import geography.GISPolygon;

/**
 * Geo-spatial border
 * 
 * @author Paula Escudero
 * @author Mateo Bonnett
 * @author David Plazas
 * @author Juan Sebasti�n C�rdenas
 * @author David Andr�s Romero
 */
public class Border extends GISPolygon {

	/**
	 * Create a new geo-spatial border
	 * 
	 * @param geometry Reference to geometry
	 */
	public Border(Geometry geometry) {
		super(geometry);
	}

}