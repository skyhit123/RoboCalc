/*
 * object.cpp
 *
 *  Created on: Oct 26, 2015
 *      Author: liwei
 */

#include "object.h"
#include "parameters.h"

Object::Object(double InitialXCoordinate, double InitialYCoordinate) : myRadius(ObjectRadius), myColor(ObjectColor)
{
	myEnkiPhysicalObject = new Enki::PhysicalObject;
	myEnkiPhysicalObject->setCylindric(ObjectRadius, ObjectHeight, ObjectMass);
	myEnkiPhysicalObject->setColor(Enki::Color::red);
	myEnkiPhysicalObject->dryFrictionCoefficient = ObjectDryFrictionCoefficient;
	myEnkiPhysicalObject->viscousFrictionCoefficient = ObjectViscousFrictionCoefficient;
	SetXCoordinate(InitialXCoordinate);
	SetYCoordinate(InitialYCoordinate);
}

double Object::GetRadius() const
{
	return myRadius;
}

double Object::GetXCoordinate() const
{
	return myEnkiPhysicalObject->pos.x;
}

double Object::GetYCoordinate() const
{
	return myEnkiPhysicalObject->pos.y;
}

double Object::GetAngle() const
{
	return myEnkiPhysicalObject->angle;
}

int Object::GetColor() const
{
	return myColor;
}

void Object::SetXCoordinate(const double XCoordinate)
{
	myEnkiPhysicalObject->pos.x = XCoordinate;
}

void Object::SetYCoordinate(const double YCoordinate)
{
	myEnkiPhysicalObject->pos.y = YCoordinate;
}

Enki::PhysicalObject* Object::GetObjectPointer()
{
	return myEnkiPhysicalObject;
}
