package geography;

import com.vividsolutions.jts.geom.Geometry;
import repast.simphony.space.gis.Geography;

public class Border {

	protected Geometry geometry;

	public Border(Geometry geometry) {
		this.geometry = geometry;
	}

	public void setGeometryInGeography(Geography<Object> geography) {
		geography.move(this, geometry);
	}
	
	public Geometry getGeometry() {
		return geometry;
	}

}
