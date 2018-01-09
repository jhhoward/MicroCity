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

#define AVERAGE_POPULATION_DENSITY 8
#define SIM_AVERAGING_STRENGTH 1
#define SIM_INCREMENT_POP_THRESHOLD 20
#define SIM_DECREMENT_POP_THRESHOLD -20
#define SIM_EMPLOYMENT_BOOST 10
#define SIM_UNEMPLOYMENT_PENALTY 10
#define SIM_INDUSTRIAL_OPPORTUNITY_BOOST 10
#define SIM_COMMERCIAL_OPPORTUNITY_BOOST 10
#define SIM_LOCAL_BUILDING_DISTANCE 20
#define SIM_LOCAL_BUILDING_INFLUENCE 10
#define SIM_STADIUM_BOOST 10
#define SIM_PARK_BOOST 10
#define SIM_MAX_CRIME 50
#define SIM_RANDOM_STRENGTH_MASK 31

uint8_t GetNumRoadConnections(Building* building)
{
	const BuildingInfo* info = GetBuildingInfo(building->type);
	uint8_t width = pgm_read_byte(&info->width);
	uint8_t height = pgm_read_byte(&info->height);
	uint8_t count = 0;

	if(building->y > 0)
	{
		for(uint8_t i = 0; i < width; i++)
		{
			if(GetConnections(building->x + i, building->y - 1) & RoadMask)
			{
				count++;
			}
		}
	}
	if(building->y + height < MAP_HEIGHT)
	{
		for(uint8_t i = 0; i < width; i++)
		{
			if(GetConnections(building->x + i, building->y + height) & RoadMask)
			{
				count++;
			}
		}
	}
	if(building->x > 0)
	{
		for(uint8_t i = 0; i < height; i++)
		{
			if(GetConnections(building->x - 1, building->y + i) & RoadMask)
			{
				count++;
			}
		}
	}
	if(building->x + width < MAP_WIDTH)
	{
		for(uint8_t i = 0; i < height; i++)
		{
			if(GetConnections(building->x + width, building->y + i) & RoadMask)
			{
				count++;
			}
		}
	}
		
	return count;
}

uint8_t GetManhattanDistance(Building* a, Building* b)
{
	uint8_t x = a->x > b->x ? a->x - b->x : b->x - a->x;
	uint8_t y = a->y > b->y ? a->y - b->y : b->y - a->y;
	return x + y;
}

void SimulateBuilding(Building* building)
{
	if (building->type == Residential || building->type == Commercial || building->type == Industrial)
	{
		if (building->hasPower)
		{
			int score = 0;
			
			// random effect
			score += (GetRand() & SIM_RANDOM_STRENGTH_MASK) - (SIM_RANDOM_STRENGTH_MASK / 2);
			
			// tend towards average population density
			score += (AVERAGE_POPULATION_DENSITY - building->populationDensity) * SIM_AVERAGING_STRENGTH;
			
			// general population effect
			switch(building->type)
			{
				case Residential:
				if(State.residentialPopulation < State.industrialPopulation)
				{
					score += SIM_EMPLOYMENT_BOOST;
				}
				if(State.residentialPopulation > State.industrialPopulation + State.commercialPopulation)
				{
					score -= SIM_UNEMPLOYMENT_PENALTY;
				}
				break;
				case Industrial:
				if(State.industrialPopulation < State.residentialPopulation || State.industrialPopulation < State.commercialPopulation)
				{
					score += SIM_INDUSTRIAL_OPPORTUNITY_BOOST;
				}
				break;
				case Commercial:
				if(State.commercialPopulation < State.residentialPopulation || State.commercialPopulation < State.industrialPopulation)
				{
					score += SIM_COMMERCIAL_OPPORTUNITY_BOOST;
				}
				break;
			}
			
			// If at least 3 road tiles are adjacent then assume that it is connected to the road network
			bool isRoadConnected = GetNumRoadConnections(building) >= 3;
			
			uint8_t closestPoliceStationDistance = 255;
			
			// influence from local buildings
			if(isRoadConnected)
			{
				for(int n = 0; n < MAX_BUILDINGS; n++)
				{
					Building* otherBuilding = &State.buildings[n];
					
					if(building != otherBuilding && otherBuilding->type && (otherBuilding->hasPower || otherBuilding->type == Park))
					{
						uint8_t distance = GetManhattanDistance(building, otherBuilding);
						
						if(otherBuilding->type == PoliceDept && distance < closestPoliceStationDistance)
						{
							closestPoliceStationDistance = distance;
						}
						
						if(distance <= SIM_LOCAL_BUILDING_DISTANCE && GetNumRoadConnections(otherBuilding) >= 3)
						{
							switch(otherBuilding->type)
							{
								case Industrial:
								if(otherBuilding->populationDensity >= building->populationDensity && (building->type == Residential || building->type == Commercial))
								{
									score += SIM_LOCAL_BUILDING_INFLUENCE;
								}
								break;
								case Residential:
								if(otherBuilding->populationDensity >= building->populationDensity && (building->type == Commercial || building->type == Industrial))
								{
									score += SIM_LOCAL_BUILDING_INFLUENCE;
								}
								break;
								case Commercial:
								if(otherBuilding->populationDensity >= building->populationDensity && building->type == Residential)
								{
									score += SIM_LOCAL_BUILDING_INFLUENCE;
								}
								break;
								case Stadium:
								if(building->type == Residential || building->type == Commercial)
								{
									score += SIM_STADIUM_BOOST;
								}
								break;
								case Park:
								if(building->type == Residential)
								{
									score += SIM_PARK_BOOST;
								}
								break;
								default:
								break;
							}
						}
					}
				}
			}
			
			// simulate crime based on how far the closest police station is and how populated the area is
			int crime = (building->populationDensity * (closestPoliceStationDistance - 8));
			if(crime > SIM_MAX_CRIME)
			{
				crime = SIM_MAX_CRIME;
			}
			if(crime > 0)
			{
				score -= crime;
			}
			
			// increase or decrease population density based on score
			if (building->populationDensity < MAX_POPULATION_DENSITY && score >= SIM_INCREMENT_POP_THRESHOLD)
			{
				building->populationDensity++;
			}
			else if(building->populationDensity > 0 && score <= SIM_DECREMENT_POP_THRESHOLD)
			{
				building->populationDensity--;
			}
			
			building->heavyTraffic = building->populationDensity > MAX_POPULATION_DENSITY / 2;
		}
		else
		{
			building->heavyTraffic = false;
			if (building->populationDensity > 0)
			{
				building->populationDensity--;
			}
		}
	}
}

void CountPopulation()
{
	State.residentialPopulation = State.industrialPopulation = State.commercialPopulation = 0;

	for (int n = 0; n < MAX_BUILDINGS; n++)
	{
		switch (State.buildings[n].type)
		{
		case Residential:
			State.residentialPopulation += State.buildings[n].populationDensity;
			break;
		case Industrial:
			State.industrialPopulation += State.buildings[n].populationDensity;
			break;
		case Commercial:
			State.commercialPopulation += State.buildings[n].populationDensity;
			break;
		default:
			break;
		}
	}
}

void Simulate()
{
	if (State.simulationStep < MAX_BUILDINGS)
	{
		SimulateBuilding(&State.buildings[State.simulationStep]);
	}
	else switch (State.simulationStep)
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
		if (State.month >= 12)
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
