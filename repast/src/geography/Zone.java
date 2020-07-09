package geography;

import com.vividsolutions.jts.geom.Geometry;

public class Zone extends Border {

	private int id;
	private double walkingAverage;

	public Zone(Geometry geometry, int id, double walkingAverage) {
		super(geometry);
		this.id = id;
		this.walkingAverage = walkingAverage;
	}

	public int getId() {
		return id;
	}
	
	public double getWalkingAverage() {
		return walkingAverage;
	}

}