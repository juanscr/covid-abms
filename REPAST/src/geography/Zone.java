package geography;

import com.vividsolutions.jts.geom.Geometry;

public class Zone extends Border {

	private int id;
	private double walkAverage;

	public Zone(Geometry geometry, int id, double walkAverage) {
		super(geometry);
		this.id = id;
		this.walkAverage = walkAverage;
	}

	public int getId() {
		return id;
	}
	
	public double getWalkAverage() {
		return walkAverage;
	}

}