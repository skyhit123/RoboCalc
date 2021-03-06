/*
 * agent.h
 *
 *  Created on: Oct 26, 2015
 *      Author: liwei
 */

#ifndef AGENT_H_
#define AGENT_H_

#include "ObjectInterface.h"
#include "object.h"
#include "parameters.h"
#include <enki/PhysicalEngine.h>
#include <enki/robots/e-puck/EPuck.h>

class Agent
{
public:
	Agent(double InitialXCoordinate, double InitialYCoordinate, double InitialAngle);

	virtual ~Agent() {};

	unsigned GetSensorReading();
	virtual double GetXCoordinate() const;
	virtual double GetYCoordinate() const;
	virtual double GetAngle() const;
	virtual double GetRadius() const;
	virtual int GetColor() const;
	Enki::EPuck* GetEpuckPointer();
	double GetInfraredSensorValue(const int index);
	bool ClearObject();

	void SetSensorReading(unsigned sensorReading);
	void SetXCoordinate(double XCoordinate);
	void SetYCoordinate(double YCoordinate);
	void SetAngle(double Angle);
	void SetLeftSpeed(double leftSpeed);
	void SetRightSpeed(double rightSpeed);
	void OuputCameraInfro();

	unsigned UpdateSensorValue(const std::vector<ObjectInterface*> ArrayOfItems);
	void UpdateWheelValue();
private:
	unsigned mySensorReading, myColor;
	double myRadius;
	bool canPickedFlag, basketSeenFlag, clearObject;
	Enki::EPuck* myEnkiEpuck;
};

#endif /* AGENT_H_ */
