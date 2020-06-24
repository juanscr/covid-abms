package model;

public interface Subject {

	public void attach(Observer o);

	public void detach(Observer o);

	public void notifyNewCase();

	public void notifyDeath();
	
}