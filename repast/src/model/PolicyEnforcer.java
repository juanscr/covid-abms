package model;

import java.util.ArrayList;
import repast.simphony.essentials.RepastEssentials;
import simulation.EventScheduler;
import util.TickConverter;

/**
 * Policy enforcer
 * 
 * @author Paula Escudero
 * @author Mateo Bonnett
 * @author David Plazas
 * @author Juan Sebastián Cárdenas
 * @author David Andrés Romero
 */
public class PolicyEnforcer {

	/**
	 * Current policies
	 */
	private ArrayList<Policy> currentPolicies;

	/**
	 * Curfew IDs for each day. Reference: <pending>
	 */
	private int[][] curfewIds = { { 7, 8 }, { 9, 0 }, { 1, 2 }, { 3, 4 }, { 5, 6 }, { 7, 8 }, { 9, 0, 1 } };

	/**
	 * Create a new policy enforcer
	 */
	public PolicyEnforcer() {
		this.currentPolicies = new ArrayList<Policy>();
	}

	/**
	 * Schedule policy
	 * 
	 * @param policy   Policy
	 * @param startDay Start day
	 * @param endDay   End day
	 */
	public void schedulePolicy(Policy policy, double startDay, double endDay) {
		EventScheduler eventScheduler = EventScheduler.getInstance();
		double startTick = TickConverter.daysToTicks(startDay);
		eventScheduler.scheduleOneTimeEvent(startTick, this, "addPolicy", policy);
		double endTick = TickConverter.daysToTicks(endDay);
		eventScheduler.scheduleOneTimeEvent(endTick, this, "removePolicy", policy);
	}

	/**
	 * Add policy
	 * 
	 * @param policy Policy
	 */
	public void addPolicy(Policy policy) {
		this.currentPolicies.add(policy);
	}

	/**
	 * Remove policy
	 * 
	 * @param policy Policy
	 */
	public void removePolicy(Policy policy) {
		this.currentPolicies.remove(policy);
	}

	/**
	 * Is the citizen allowed to go out?
	 * 
	 * @param citizen Citizen
	 */
	public boolean isAllowedToGoOut(Citizen citizen) {
		boolean allowed = true;
		if (this.currentPolicies.contains(Policy.FULL_QUARANTINE)) {
			allowed = false;
		}
		if (this.currentPolicies.contains(Policy.ID_BASED_CURFEW)) {
			int id = citizen.getId();
			double ticks = Math.max(RepastEssentials.GetTickCount(), 0);
			int day = (int) TickConverter.ticksToDays(ticks) % 7;
			int[] allowedIds = this.curfewIds[day];
			allowed = false;
			for (int i = 0; i < allowedIds.length; i++) {
				if (id == allowedIds[i]) {
					allowed = true;
					break;
				}
			}
		}
		return allowed;
	}

}