/*
 * basket.cpp
 *
 *  Created on: Nov 3, 2015
 *      Author: liwei
 */

#include "basket.h"
#include "parameters.h"

Basket::Basket(double InitialXCoordinate, double InitialYCoordinate):
myRadius(BasketRadius),myColor(BasketColor)
{
	myEnkiPhysicalObject = new Enki::PhysicalObject;
	myEnkiPhysicalObject->setCylindric(BasketRadius, BasketHeight, BasketMass);
	myEnkiPhysicalObject->setColor(Enki::Color::blue);
	myEnkiPhysicalObject->dryFrictionCoefficient = BasketDryFrictionCoefficient;
	myEnkiPhysicalObject->viscousFrictionCoefficient = BasketViscousFrictionCoefficient;
	SetXCoordinate(InitialXCoordinate);
	SetYCoordinate(InitialYCoordinate);
}

double Basket::GetXCoordinate() const
{
	return myEnkiPhysicalObject->pos.x;
}

double Basket::GetYCoordinate() const
{
	return myEnkiPhysicalObject->pos.y;
}

double Basket::GetAngle() const
{
	return myEnkiPhysicalObject->angle;
}

int Basket::GetColor() const
{
	return myColor;
}

double Basket::GetRadius() const
{
	return myRadius;
}

void Basket::SetXCoordinate(const double XCoordinate)
{
	myEnkiPhysicalObject->pos.x = XCoordinate;
}

void Basket::SetYCoordinate(const double YCoordinate)
{
	myEnkiPhysicalObject->pos.y = YCoordinate;
}

Enki::PhysicalObject* Basket::GetBasketPointer()
{
	return myEnkiPhysicalObject;
}



