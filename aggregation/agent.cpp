/*
 * agent.cpp
 *
 *  Created on: Oct 26, 2015
 *      Author: liwei
 */
#include "agent.h"

Agent::Agent(double InitialXCoordinate, double InitialYCoordinate, double InitialAngle) :
	myColor(RobotColor), myRadius(RobotRadius), myEnkiEpuck(new Enki::EPuck(0))
{
	mySensorReading = WallColor;
	SetXCoordinate(InitialXCoordinate);
	SetYCoordinate(InitialYCoordinate);
	SetAngle(InitialAngle);
}

unsigned Agent::GetSensorReading()
{
	return mySensorReading;
}

void Agent::SetSensorReading(unsigned sensorReading)
{
	mySensorReading = sensorReading;
}

double Agent::GetXCoordinate() const
{
	return myEnkiEpuck->pos.x;
}


double Agent::GetYCoordinate() const
{
	return myEnkiEpuck->pos.y;
}

double Agent::GetAngle() const
{
	return myEnkiEpuck->angle;
}

int Agent::GetColor() const
{
	return myColor;
}

double Agent::GetRadius() const
{
	return myRadius;
}

void Agent::SetXCoordinate(double XCoordinate)
{
	 myEnkiEpuck->pos.x = XCoordinate;
}


void Agent::SetYCoordinate(double YCoordinate)
{
	 myEnkiEpuck->pos.y = YCoordinate;
}

void Agent::SetAngle(double Angle)
{
	 myEnkiEpuck->angle = Angle;
}

void Agent::SetLeftSpeed(double leftSpeed)
{
	 myEnkiEpuck->leftSpeed = leftSpeed;
}

void Agent::SetRightSpeed(double rightSpeed)
{
	 myEnkiEpuck->rightSpeed = rightSpeed;
}

Enki::EPuck* Agent::GetEpuckPointer()
{
	return myEnkiEpuck;
}

void Agent::UpdateSensorValue(const std::vector<ObjectInterface*> ArrayOfItems)
{
	mySensorReading = WallColor;   //default: see nothing (or the wall)

	double MinimumSensingDistance = 1.0/0.0;

	const double ri = GetRadius();
	const double xi = GetXCoordinate();
	const double yi = GetYCoordinate();
	const double ai = GetAngle();

	for (unsigned i = 0; i < ArrayOfItems.size(); i++)
	{
		if (ArrayOfItems[i] != this)
		{
			const double rj = ArrayOfItems[i]->GetRadius();
			const double xj = ArrayOfItems[i]->GetXCoordinate();
			const double yj = ArrayOfItems[i]->GetYCoordinate();

			const double side = -sin(ai)*(yi - yj) - cos(ai)*(xi - xj);
			const double dper = fabs(cos(ai)*(yi - yj) - sin(ai)*(xi - xj));

			if (side > 0 && dper < rj)
			{
				const double dij = sqrt(pow(xj - xi, 2.0) + pow(yj - yi, 2.0));
				const double dsense = sqrt(pow(dij, 2.0) - pow(dper, 2.0)) - sqrt(pow(rj, 2.0) - pow(dper, 2.0)) - ri;  //sensing distance

				if (dsense < MinimumSensingDistance)       //check the nearest item
				{
					MinimumSensingDistance = dsense;
					mySensorReading = ArrayOfItems[i]->GetColor();
				}
			}
		}
	}

}

void Agent::UpdateSpeed()
{
	myEnkiEpuck->leftSpeed = EPuckMaximumSpeed * AgentController[2*mySensorReading];
	myEnkiEpuck->rightSpeed = EPuckMaximumSpeed * AgentController[2*mySensorReading + 1];
}
