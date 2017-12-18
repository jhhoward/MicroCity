#include "Game.h"
#include "Connectivity.h"

enum SimulationSteps
{
	SimulateBuildings = 0,
	SimulatePower = MAX_BUILDINGS,
	SimulateNextMonth
};

void SimulateBuilding(Building* building)
{
	// TODO
}

void Simulate()
{
	if(State.simulationStep < MAX_BUILDINGS)
	{
		SimulateBuilding(&State.buildings[State.simulationStep]);
	}
	else switch(State.simulationStep)
	{
		case SimulatePower:
		CalculatePowerConnectivity();
		break;
		case SimulateNextMonth:
		{
			State.simulationStep = 0;
			State.month++;
			if(State.month >= 12)
			{
				State.month = 0;
				State.year++;
				
				// TODO: trigger end of year report
			}
		}
		return;
	}
	
	State.simulationStep++;
}
