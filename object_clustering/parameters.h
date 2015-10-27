#ifndef PARAMETERS_H_
#define PARAMETERS_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
using namespace std;

const int NumberOfAgent = 5;
const double AreaPerRobot = 10000.0;
const double EPuckRadius = 3.7;
const double EPuckMaximumSpeed = 12.8;
const double ControlStepSize = 0.1;
const int MaximumStepSize = 1000;
const int OversamplingRate = 10;
const int ArenaWidth = 200;
const double AgentController[4] = {-0.7, -1.0, 1.0, -1.0};

const double RobotRadius = 3.7;
const double ObjectRadius = 5.0;

const unsigned WallColor = 0;
const unsigned ObjectColor = 1;
const unsigned RobotColor = 2;

extern gsl_rng *rng;

#endif