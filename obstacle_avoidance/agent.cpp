/*
 * agent.cpp
 *
 *  Created on: Oct 26, 2015
 *      Author: liwei
 */
#include "agent.h"

Agent::Agent(double InitialXCoordinate, double InitialYCoordinate, double InitialAngle) :
	myColor(RobotColor), myRadius(RobotRadius),
	myEnkiEpuck(new Enki::EPuck(Enki::EPuck::CAPABILITY_BASIC_SENSORS))
{
	mySensorReading = 0;
	SetXCoordinate(InitialXCoordinate);
	SetYCoordinate(InitialYCoordinate);
	SetAngle(InitialAngle);
	SetLeftSpeed(EPuckMaximumSpeed);
	SetRightSpeed(EPuckMaximumSpeed);
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

double Agent::GetLeftSpeed()
{
	return myEnkiEpuck->leftSpeed;
}

double Agent::GetRightSpeed()
{
	return myEnkiEpuck->rightSpeed;
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
	mySensorReading = WallColor;

	double MinimumSensingDistance = 1.0/0.0;

	const double ri = myRadius;
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

double Agent::GetInfraredSensorValue(const int index)
{
	std::cout << myEnkiEpuck->infraredSensor0.getRayDist(1) << " ";
	if (index == 0)
		return myEnkiEpuck->infraredSensor0.getValue();
	if (index == 1)
		return myEnkiEpuck->infraredSensor1.getValue();
	if (index == 2)
		return myEnkiEpuck->infraredSensor2.getValue();
	if (index == 3)
		return myEnkiEpuck->infraredSensor3.getValue();
	if (index == 4)
		return myEnkiEpuck->infraredSensor4.getValue();
	if (index == 5)
		return myEnkiEpuck->infraredSensor5.getValue();
	if (index == 6)
		return myEnkiEpuck->infraredSensor6.getValue();
	if (index == 7)
		return myEnkiEpuck->infraredSensor7.getValue();
	return 0;
}

void Agent::ObstacleAvoidance()
{
	double Sx, Sy;
	double alpha=0;
	int dir;
	double leftDefaultSpeed = EPuckMaximumSpeed;
	double rightDefaultSpeed = EPuckMaximumSpeed;

	double prox0 = myEnkiEpuck->infraredSensor0.getValue();
	double prox1 = myEnkiEpuck->infraredSensor1.getValue();
	double prox2 = myEnkiEpuck->infraredSensor2.getValue();
	double prox3 = myEnkiEpuck->infraredSensor3.getValue();
	double prox4 = myEnkiEpuck->infraredSensor4.getValue();
	double prox5 = myEnkiEpuck->infraredSensor5.getValue();
	double prox6 = myEnkiEpuck->infraredSensor6.getValue();
	double prox7 = myEnkiEpuck->infraredSensor7.getValue();

	double proximity0x = prox0*cos(1.27);
	double proximity1x = prox1*cos(0.77);
	double proximity2x = prox2*cos(0.00);
	double proximity3x = prox3*cos(5.21);
	double proximity4x = prox4*cos(4.21);
	double proximity5x = prox5*cos(3.14);
	double proximity6x = prox6*cos(2.37);
	double proximity7x = prox7*cos(1.87);

	double proximity0y = prox0*sin(1.27);
	double proximity1y = prox1*sin(0.77);
	double proximity2y = prox2*sin(0.00);
	double proximity3y = prox3*sin(5.21);
	double proximity4y = prox4*sin(4.21);
	double proximity5y = prox5*sin(3.14);
	double proximity6y = prox6*sin(2.37);
	double proximity7y = prox7*sin(1.87);

	//calculate resultant vector (Sx,Sy)
	Sy = (proximity0y+proximity1y+proximity2y+proximity3y+proximity4y+proximity5y+proximity6y+proximity7y)/3731;
	Sx = (proximity0x+proximity1x+proximity2x+proximity3x+proximity4x+proximity5x+proximity6x+proximity7x)/3731;
	//Use the resultant vector to calculate the obstacle avoidance contribution 'alpha' and turning direction 'd'
	if ((GetLeftSpeed() + GetRightSpeed()) < 0){         //move backward
	    if(Sy < 0) {
	        alpha = sqrt(Sy*Sy+Sx*Sx)/3;
	        if(alpha>1.0){ alpha = 1.0;}

	        if (Sx>0){			//turn CW
	           dir = 1;  }
	        else{
	           dir = -1;
	        }
	    }
	}

	else{
	    if(Sy > 0) {
	        alpha = sqrt(Sy*Sy+Sx*Sx)/3;
	        if(alpha>1.0){ alpha = 1.0;}

	        if (Sx<0){			//turn CW
	           dir = 1;  }
	        else {
	           dir = -1;
	        }
	    }
	}

	std::cout << alpha << " " << dir << std::endl;
	double sl = (1-alpha)*leftDefaultSpeed + dir*alpha*(EPuckMaximumSpeed);
	double sr = (1-alpha)*rightDefaultSpeed + dir*alpha*(-EPuckMaximumSpeed);
	SetLeftSpeed(sl);
	SetRightSpeed(sr);

}



