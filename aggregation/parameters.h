#ifndef PARAMETERS_H_
#define PARAMETERS_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

const int NumberOfAgent = 20;
const double EPuckMaximumSpeed = 12.8;
const double ControlStepSize = 0.1;
const int MaximumStepSize = 5000;
const int OversamplingRate = 10;
const int ArenaWidth = 200;
const double AgentController[6] = {-0.7, -1.0, 1.0, -1.0};

const double RobotRadius = 3.7;
const unsigned WallColor = 0;
const unsigned RobotColor = 1;

#endif