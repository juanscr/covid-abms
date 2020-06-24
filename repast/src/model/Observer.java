package model;

public interface Observer {
	
	public void reportNewCase(Citizen citizen);

	public void reportDeath(Citizen citizen);
	
}