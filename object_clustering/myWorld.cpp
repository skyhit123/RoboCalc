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
	for (int i = 0; i < (NumberOfAgent); i++){
		myArrayOfAgents.push_back(new Agent);  //sensor ability: none; noise: 0.05
		addObject(myArrayOfAgents[i]->GetEpuckPointer());
		myArrayOfItems.push_back(myArrayOfAgents.back());
	}
	InitializeConfiguration();
}

void myWorld::InitializeConfiguration()
{

	for(unsigned i = 0; i < myArrayOfAgents.size(); i++)
	{
		double xi;
		double yi;
		bool Overlap;

		do
		{
			const double LengthOfSides = ArenaWidth;
			xi = LengthOfSides*gsl_rng_uniform(rng);
		    yi = LengthOfSides*gsl_rng_uniform(rng);
			Overlap = false;

			for (unsigned j = 0; j < i; j ++ )
			{
				double xj = myArrayOfAgents[j]->GetXCoordinate();
				double yj = myArrayOfAgents[j]->GetYCoordinate();

				double dij = sqrt(pow(xi - xj, 2.0) + pow(yi - yj, 2.0));

				if (dij < (myArrayOfAgents[i]->GetRadius() + myArrayOfAgents[j]->GetRadius()))
				{
					Overlap = true;
					break;
				}
			}

		} while(Overlap == true);

		myArrayOfAgents[i]->SetXCoordinate(xi);
		myArrayOfAgents[i]->SetYCoordinate(yi);
		myArrayOfAgents[i]->SetAngle(2.0*M_PI*(gsl_rng_uniform(rng) - 0.5));
	}

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

			if(Side > 0 && PerpendicularDistance <= EPuckRadius)
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