/*
 * object.h
 *
 *  Created on: Oct 26, 2015
 *      Author: liwei
 */

#ifndef OBJECT_H_
#define OBJECT_H_

#include "ObjectInterface.h"
#include <enki/PhysicalEngine.h>
#include <enki/robots/e-puck/EPuck.h>

class Object : public ObjectInterface
{
public:

	Object(double InitialXCoordinate, double InitialYCoordinate);

	double GetRadius() const;
	double GetXCoordinate() const;
	double GetYCoordinate() const;
	double GetAngle() const;
	int GetColor() const;
	Enki::PhysicalObject* GetObjectPointer();

	void SetXCoordinate(const double XCoordinate);
	void SetYCoordinate(const double YCoordinate);

private:
	Enki::PhysicalObject* myEnkiPhysicalObject;
	double myRadius;
	int myColor;
};

#endif /* OBJECT_H_ */
