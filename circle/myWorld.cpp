/*
 * myWorld.cpp
 *
 *  Created on: Oct 26, 2015
 *      Author: liwei
 */

#include "myWorld.h"
#include "parameters.h"
#include <sys/time.h>
#include <sys/stat.h>
using namespace Enki;
gsl_rng *rng;

myWorld::myWorld(double width, double height, const Color& wallsColor, unsigned maxSteps) :
	Enki::World(width, height, wallsColor), maxSteps(maxSteps)
{
	c_step = 0;
	const double LengthOfSides = ArenaWidth;
	gsl_seed_generator();

	for (int i = 0; i < (NumberOfAgent); i++){

		double InitialXCoordinate;
		double InitialYCoordinate;
		do{
			InitialXCoordinate = LengthOfSides*gsl_rng_uniform(rng);
			InitialYCoordinate = LengthOfSides*gsl_rng_uniform(rng);

		} while (CheckOverlap(InitialXCoordinate, InitialYCoordinate, RobotRadius) == true);
		double InitialAngle = 2.0*M_PI*(gsl_rng_uniform(rng) - 0.5);

		myArrayOfAgents.push_back(new Agent(InitialXCoordinate, InitialYCoordinate, InitialAngle));  //sensor ability: none; noise: 0.05
		addObject(myArrayOfAgents[i]->GetEpuckPointer());
		myArrayOfItems.push_back(myArrayOfAgents[i]);
	}

	gsl_rng_free(rng);
}

bool myWorld::CheckOverlap(const double XCoordinate, const double YCoordinate, const double Radius)
{

	bool Overlap = false;
	for(unsigned i = 0; i < myArrayOfItems.size(); i++)
	{
		double xj = myArrayOfItems[i]->GetXCoordinate();
		double yj = myArrayOfItems[i]->GetYCoordinate();

		double dij = sqrt(pow(XCoordinate - xj, 2.0) + pow(YCoordinate - yj, 2.0));

		if (dij < (Radius + myArrayOfItems[i]->GetRadius()))
		{
			Overlap = true;
			break;
		}

	}

	return Overlap;
}

void myWorld::UpdateAgentSpeed()
{
	for (unsigned i = 0; i < myArrayOfAgents.size(); i++){
		myArrayOfAgents[i]->UpdateSensorValue(myArrayOfItems);
		myArrayOfAgents[i]->UpdateSpeed();
		myArrayOfAgents[i]->ObstacleAvoidance();
	}
}

// returns true if the total simulation period has elapsed
bool myWorld::runStep()
{
	UpdateAgentSpeed();
	if (c_step != maxSteps)
	{
		step(ControlStepSize, OversamplingRate);
		c_step++;
/*
		if (c_step == 100)
		{
			removeObject(myArrayOfAgents[0]->GetEpuckPointer());
			myArrayOfAgents.erase (myArrayOfAgents.begin());
			myArrayOfItems.erase(myArrayOfItems.begin());
		}
		std::cout << myArrayOfAgents[4]->GetXCoordinate() << " " << myArrayOfAgents[4]->GetYCoordinate() << std::endl;
		std::cout << myArrayOfAgents.size() << myArrayOfObjects.size() << myArrayOfItems.size() << std::endl;
*/
		return false;
	}
	return true;
}

void myWorld::run()
{
	bool finished = false;
	while (finished == false)
	{
		finished = runStep();
	}
}

void myWorld::gsl_seed_generator()
{
	unsigned long int Seed;
	struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv,&tz);
    Seed = tv.tv_sec * 1000000 + tv.tv_usec;
    rng = gsl_rng_alloc(gsl_rng_default);
    gsl_rng_set(rng, Seed);

    std::cout << "Seed: " << Seed << std::endl;
}
