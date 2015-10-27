/*
 * myWorld.cpp
 *
 *  Created on: Oct 26, 2015
 *      Author: liwei
 */

#include "myWorld.h"
#include "parameters.h"
using namespace Enki;

myWorld::myWorld(double width, double height, const Color& wallsColor, unsigned maxSteps) :
	Enki::World(width, height, wallsColor), maxSteps(maxSteps)
{
	c_step = 0;
	const double LengthOfSides = ArenaWidth;
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

	for (int i = 0; i < NumberOfObject; i++)
	{
		double InitialXCoordinate;
		double InitialYCoordinate;
		do
		{
			InitialXCoordinate = LengthOfSides*gsl_rng_uniform(rng);
			InitialYCoordinate = LengthOfSides*gsl_rng_uniform(rng);

		} while (CheckOverlap(InitialXCoordinate, InitialYCoordinate, ObjectRadius) == true);

		myArrayOfObjects.push_back(new Object(InitialXCoordinate, InitialYCoordinate));
		addObject(myArrayOfObjects[i]->GetObjectPointer());
		myArrayOfItems.push_back(myArrayOfObjects[i]);
	}
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
		int SensorReading = ComputeSensorReading(i);
		if (SensorReading == 0){
			myArrayOfAgents[i]->SetLeftSpeed(EPuckMaximumSpeed*AgentController[0]);
			myArrayOfAgents[i]->SetRightSpeed(EPuckMaximumSpeed*AgentController[1]);
		}
		else if (SensorReading == 1){
			myArrayOfAgents[i]->SetLeftSpeed(EPuckMaximumSpeed*AgentController[2]);
			myArrayOfAgents[i]->SetRightSpeed(EPuckMaximumSpeed*AgentController[3]);
		}
	}
}

unsigned myWorld::ComputeSensorReading(const unsigned &Index)
{
	myArrayOfAgents[Index]->SetSensorReading(0);
	for(unsigned j = 0; j < myArrayOfAgents.size(); j ++ )
		if( j != Index )
		{
			const double xi = myArrayOfAgents[Index]->GetXCoordinate();
			const double yi = myArrayOfAgents[Index]->GetYCoordinate();
			const double ai = myArrayOfAgents[Index]->GetAngle();

			const double xj = myArrayOfAgents[j]->GetXCoordinate();
			const double yj = myArrayOfAgents[j]->GetYCoordinate();

			const double Side = -sin(ai)*(yi - yj) - cos(ai)*(xi - xj);
			const double PerpendicularDistance = fabs((yi - yj)*cos(ai) - (xi - xj)*sin(ai));

			if(Side > 0 && PerpendicularDistance <= RobotRadius)
			{
				myArrayOfAgents[Index]->SetSensorReading(1);
				break;
			}
		}

	return myArrayOfAgents[Index]->GetSensorReading();
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
