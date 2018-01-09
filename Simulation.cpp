#include "Game.h"
#include "Connectivity.h"
#include "Draw.h"

enum SimulationSteps
{
	SimulateBuildings = 0,
	SimulatePower = MAX_BUILDINGS,
  SimulatePopulation,
  SimulateRefreshTiles,
	SimulateNextMonth
};

void SimulateBuilding(Building* building)
{
  if(building->type)
  {
    if(building->hasPower)
    {
      if(building->populationDensity < MAX_POPULATION_DENSITY)
      {
        building->populationDensity++;
      }
      building->heavyTraffic = building->populationDensity > MAX_POPULATION_DENSITY / 2;
    }
    else
    {
      building->heavyTraffic = false;
      if(building->populationDensity > 0)
      {
        building->populationDensity --;
      }
    }
  }
	// TODO
}

void CountPopulation()
{
  State.residentialPopulation = State.industrialPopulation = State.commercialPopulation = 0;

  for(int n = 0; n < MAX_BUILDINGS; n++)
  {
    switch(State.buildings[n].type)
    {
      default:
      break;
    }
  }
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
    case SimulateRefreshTiles:
    ResetVisibleTileCache();
    break;
    case SimulatePopulation:
    CountPopulation();
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
