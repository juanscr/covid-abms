package geography;

import com.vividsolutions.jts.geom.Geometry;

import repast.simphony.space.gis.Geography;

public class Zone {
	
	private Geometry geometry;
	private String id;
	
	public Zone(Geometry geometry, String id) {
		this.geometry = geometry;
		this.id = id;
	}
	
	public void setGeometryInGeography(Geography<Object> geography) {
		geography.move(this, geometry);
	}

}
