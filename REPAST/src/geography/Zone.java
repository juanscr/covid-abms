package geography;

import com.vividsolutions.jts.geom.Geometry;

import repast.simphony.space.gis.Geography;

public class Zone extends Border {
	
	private int id;
	
	public Zone(Geometry geometry, int id) {
		super(geometry);
		this.id = id;
	}
	
	public int getId() {
		return this.id;
	}

}
