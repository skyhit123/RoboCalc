#ifndef PARAMETERS_H_
#define PARAMETERS_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

const int NumberOfAgent = 5;
const int NumberOfObject = 10;
const double EPuckMaximumSpeed = 12.8;
const double ControlStepSize = 0.1;
const int MaximumStepSize = 5000;
const int OversamplingRate = 10;
const int ArenaWidth = 200;
const double AgentController[6] = {0.5, 1.0, 1.0, 0.5, 0.1, 0.5};

const double RobotRadius = 3.7;
const double ObjectRadius = 5.0;

const unsigned WallColor = 0;
const unsigned ObjectColor = 1;
const unsigned RobotColor = 2;

const double ObjectHeight = 10.0;
const double ObjectMass = 35.0; // TODO
const double ObjectDryFrictionCoefficient = 0.58; //TODO
const double ObjectViscousFrictionCoefficient = 0.35; //TODO

#endif
