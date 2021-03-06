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

gsl_rng *rng;
using namespace Enki;
using namespace std;

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
			InitialXCoordinate = LengthOfSides*0.7;
			InitialYCoordinate = LengthOfSides*0.5;

		} while (CheckOverlap(InitialXCoordinate, InitialYCoordinate, RobotRadius) == true);
//		double InitialAngle = 2.0*M_PI*(gsl_rng_uniform(rng) - 0.5);
		double InitialAngle = -M_PI;

		myArrayOfAgents.push_back(new Agent(InitialXCoordinate, InitialYCoordinate, InitialAngle));  //sensor ability: none; noise: 0.05
		addObject(myArrayOfAgents[i]->GetEpuckPointer());
//		myArrayOfItems.push_back(myArrayOfAgents[i]);
	}

	for (int i = 0; i < NumberOfObject; i++)
	{
		double InitialXCoordinate;
		double InitialYCoordinate;
		do
		{
			InitialXCoordinate = LengthOfSides*0.5;
			InitialYCoordinate = LengthOfSides*0.5;

		} while (CheckOverlap(InitialXCoordinate, InitialYCoordinate, ObjectRadius) == true);

		myArrayOfObjects.push_back(new Object(InitialXCoordinate, InitialYCoordinate));
		addObject(myArrayOfObjects[i]->GetObjectPointer());
		myArrayOfItems.push_back(myArrayOfObjects[i]);
	}

	for (int i = 0; i < NumberOfBasket; i++)
	{
		double InitialXCoordinate;
		double InitialYCoordinate;
		do
		{
			InitialXCoordinate = LengthOfSides*0.9;
			InitialYCoordinate = LengthOfSides*0.1;

		} while (CheckOverlap(InitialXCoordinate, InitialYCoordinate, BasketRadius) == true);

		myArrayOfBasket.push_back(new Basket(InitialXCoordinate, InitialYCoordinate));
		addObject(myArrayOfBasket[i]->GetBasketPointer());
		myArrayOfItems.push_back(myArrayOfBasket[i]);
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
		myArrayOfAgents[i]->OuputCameraInfro();
//		unsigned object_index = myArrayOfAgents[i]->UpdateSensorValue(myArrayOfItems);
//		myArrayOfAgents[i]->UpdateWheelValue();
/*		if (myArrayOfAgents[i]->ClearObject() == true && (!myArrayOfObjects.empty()))
		{
			removeObject(myArrayOfObjects[object_index]->GetObjectPointer());
			myArrayOfObjects.erase (myArrayOfObjects.begin()+object_index);
			myArrayOfItems.erase(myArrayOfItems.begin()+object_index);
//			std::cout << myArrayOfObjects[0]->GetXCoordinate() << " " << myArrayOfObjects[0]->GetYCoordinate() << std::endl;
//			std::cout << myArrayOfAgents.size() << myArrayOfObjects.size() << myArrayOfItems.size() << std::endl;

		}*/
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
