/*
	aggregation version 1.00 - first approved version (Roderich Gross)

    Enki - a fast 2D robot simulator
    Copyright (C) 1999-2008 Stephane Magnenat <stephane at magnenat dot net>
    Copyright (C) 2004-2005 Markus Waibel <markus dot waibel at epfl dot ch>
    Copyright (c) 2004-2005 Antoine Beyeler <abeyeler at ab-ware dot com>
    Copyright (C) 2005-2006 Laboratory of Intelligent Systems, EPFL, Lausanne
    Copyright (C) 2006-2008 Laboratory of Robotics Systems, EPFL, Lausanne
    See AUTHORS for details

    This program is free software; the authors of any publication 
    arising from research using this software are asked to add the 
    following reference:
    Enki - a fast 2D robot simulator
    http://lis.epfl.ch/enki
    Stephane Magnenat <stephane at magnenat dot net>,
    Markus Waibel <markus dot waibel at epfl dot ch>
    Laboratory of Intelligent Systems, EPFL, Lausanne.

    You can redistribute this program and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "viewer/Viewer.h"
#include <enki/PhysicalEngine.h>
#include <enki/robots/e-puck/EPuck.h>
#include <QtGui/QApplication>
#include <QtGui/QtGui>
#include <iostream>
#include <fstream>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <sys/time.h>

using namespace Enki;
using namespace std;

gsl_rng *r;
// time
static double total_seconds = 0;
const double ctrl_stepsize = 1./10.;

static double d_robot_radius = 3.7;

static double d_world_width  = 0;
static double d_world_height = 0;

// counters
static unsigned c_agent  = 0;

// noise
static long int n_seed = 0;

// genes
const unsigned input_n = 9;
const unsigned output_n = 2;
static double genes[input_n * output_n];

class Agent: public EPuck
{
public:
	Agent();
	~Agent();
	virtual void controlStep(double dt);
	void simplePerceptron(double input[], double output[]);	
};


Agent::Agent() : EPuck(CAPABILITY_BASIC_SENSORS)
{
}

Agent::~Agent()
{
}

void Agent::controlStep(double dt)
{
	double input[9];
	double output[2];
	
	double maxvalue = 2965.98144; // 0 centimeter distance
	//double maxvalue = 1666.022; // 1 centimeter distance
	input[0] = 1;
	input[1] = min(1.0, infraredSensor0.getValue() / maxvalue);
	input[2] = min(1.0, infraredSensor1.getValue() / maxvalue);
	input[3] = min(1.0, infraredSensor2.getValue() / maxvalue);
	input[4] = min(1.0, infraredSensor3.getValue() / maxvalue);
	input[5] = min(1.0, infraredSensor4.getValue() / maxvalue);
	input[6] = min(1.0, infraredSensor5.getValue() / maxvalue);
	input[7] = min(1.0, infraredSensor6.getValue() / maxvalue);
	input[8] = min(1.0, infraredSensor7.getValue() / maxvalue);

	simplePerceptron(input, output);
 
	leftSpeed = 2.0 * maxSpeed * (output[0] - 0.5);
	rightSpeed= 2.0 * maxSpeed * (output[1] - 0.5);
//	leftSpeed = maxSpeed;
//	rightSpeed= maxSpeed;

	// do the motion
	EPuck::controlStep(dt);
}

void Agent::simplePerceptron(double input[], double output[]) {	
	// clean outputs
	for (unsigned j=0; j<output_n; j++) {
		output[j] = 0;
	}
    
	// calculate outputs
	for (unsigned i=0; i<input_n; i++) {
		for (unsigned j=0; j<output_n; j++) {
			output[j] += genes[i+j*input_n] * input[i];
		}
	}
    
	// apply saturation
	for (unsigned j=0; j<output_n; j++) {
		output[j] = 1.0/( 1. + exp( -output[j] ) );
	}
}

class EnkiTestWorld : public Enki::World
{
public:
	EnkiTestWorld(double width, double height, int maxSteps) :
		Enki::World(width, height),
		maxSteps(maxSteps)
	{
		for (unsigned i = 0; i < c_agent; i++)
			createAgent();
		c_step = 0;
		fitness_cum = 0;
		fitness_counter = 0;
	}
	
	~EnkiTestWorld()
	{
		double fitness = 0;
		if (fitness_counter > 0)
			fitness = fitness_cum / fitness_counter;
		std::cout << fitness;
		std::cout << " 2 ";
		std::cout << fitness;
		std::cout << " " << n_seed;
		std::cout << std::endl;
		
		ofstream of;
		of.open( "fitness.txt", ios::out );
		if( !of ) {
			std::cerr << "Cannot open fitness.txt for writing." << std::endl;
			exit(1);
		}
		of << fitness;
		of << " 2 ";
		of << fitness;
		of << " " << n_seed;
		of << std::endl;
		of.close();
	}
private:
	int maxSteps;
	unsigned c_step;
	double fitness_cum;
	double fitness_counter;
	
public:
	void createAgent()
	{
		Agent* a = new Agent();
		// make sure that the agent does not overlap with existing agents
		Enki::Point new_pos;
		bool conflict = true;
		unsigned c_conflict = 0;
		while (conflict) 
		{
			// choose a new position in the bottom half of the arena
			conflict = false;
			new_pos = Enki::Point(
				UniformRand(d_robot_radius, d_world_width  - d_robot_radius)(),
				UniformRand(d_robot_radius, d_world_height - d_robot_radius)()
			);

			// check if the agent overlapp some other in this position
			for (Enki::World::ObjectsIterator i = objects.begin(); i != objects.end(); ++i)
			{
				Agent* other = dynamic_cast<Agent*>(*i);
				if (other)
				{
					if ((other->pos - new_pos).norm2() < 4. * d_robot_radius * d_robot_radius)
					{
						conflict = true;
						c_conflict++;
						if (c_conflict > c_agent * c_agent + 1000)
						{
							std::cerr << "Aborting because of too many attempts to put agent in the arena." << std::endl;
							exit(1);
						}
						break;
					}
				}
			}
			if (conflict == false)
			{
				c_conflict = 0;
			}
		}
		
		a->pos = new_pos;
		a->angle = UniformRand(0, 2.*M_PI)();
		addObject(a);
	}
	
	double computeAggregationValue()
	{
		Vector centerVector(0, 0);
		for (Enki::World::ObjectsIterator i = objects.begin(); i != objects.end(); ++i)
		{
			Agent* a = dynamic_cast<Agent*>(*i);
			centerVector += a->pos;
		}
		centerVector = centerVector / (double)(c_agent);

		double distance = 0;
		for (Enki::World::ObjectsIterator i = objects.begin(); i != objects.end(); ++i)
		{
			Agent* a = dynamic_cast<Agent*>(*i);
			distance += (centerVector - a->pos).norm();
		}

		return distance / (double)c_agent;
	}

	void updateFitness()
	{
		if ((int)c_step >= maxSteps/2)
		{
			// at least half of the simulation period has elapsed
			fitness_cum += computeAggregationValue();
			fitness_counter++;
		}
	}

	// returns true if the total simulation period has elapsed
	bool runStep()
	{
		updateFitness();
		if ((int)c_step != maxSteps)
		{
			step(ctrl_stepsize, 10);
			c_step++;
			return false;	
		}
		return true;
	}

	
	void run()
	{
		bool finished = false;
		while (finished == false)
		{
			finished = runStep();
		}
	}
};

class EnkiTestViewer : public ViewerWidget
{
protected:
	unsigned timeMultiplier;
	int timerId;
	EnkiTestWorld* myworld;
		
public:
	EnkiTestViewer(EnkiTestWorld *world, unsigned timeMultiplier = 1, QWidget *parent = 0) :
		ViewerWidget(world, parent),
		timeMultiplier(timeMultiplier)	
	{
		myworld = world;
		timerId = startTimer(100);
	}
	
	virtual void timerEvent(QTimerEvent * event)
	{
		if (event->timerId() == timerId)
		{
			bool finished = false;
			for (unsigned i = 0; i < timeMultiplier; i++)
			{
				finished =  myworld->runStep();
				if (finished)
					break;
			}
			updateGL();

			if (finished)
				close();
		}
	}
	
	virtual void wheelEvent(QWheelEvent * event)
	{
		if (event->delta() > 0)
			timeMultiplier *= 2;
		else if (event->delta() < 0 && timeMultiplier > 1)
			timeMultiplier /= 2;
		setWindowTitle(QString(tr("Simulation runnig at %0 times real-time").arg(timeMultiplier)));
	}

};

// http://qtnode.net/wiki?title=Qt_with_cmake
int main(int argc, char *argv[])
{

	ifstream in;
	in.open( "solution.txt", ios::in );
	if( !in ) 
	{
		std::cerr << "ERROR opening file solution.txt\n";
    }
	    
    unsigned length = 0;
	if( !(in >> length) ) {
		std::cerr << "ERROR reading solution.txt - 1 integer and 18 double values required\n";
		exit(1);
	}
	if (input_n * output_n != length)
	{
		std::cerr << "ERROR reading solution.txt - 1 integer and 18 double values required\n";
		exit(1);
	}
	for (unsigned i = 0; i < length; i++ ) {
		if( !(in >> genes[i] ) ) {
			std::cerr << "ERROR reading solution.txt: 1 integer and " << length << " double values required\n";
			exit(1);
		}
	}

	if (argc < 4)
	{
		std::cerr << "Usage " << argv[0] << " viewer/noviewer seed(0 for time) experimentDuration(in seconds) [timeMultiplier]" << std::endl;
		std::cout << "Solution loaded: ";
		for (unsigned i=0; i<length; i++)
		{
			std::cout << genes[i] << " ";
		}
		std::cout << std::endl;
		return 1;
	}

	n_seed = atol(argv[2]);
	if (n_seed == 0) {
		struct timeval tv;
		struct timezone tz;
		gettimeofday(&tv,&tz);
		n_seed = tv.tv_sec * 1000000 + tv.tv_usec;			
	}
    r = gsl_rng_alloc(gsl_rng_default);
    gsl_rng_set(r, n_seed);


	total_seconds = (double)atoi(argv[3]);
	int maxSteps = (int) total_seconds / ctrl_stepsize;

	c_agent = 2;

	d_world_width  = 50.;  //UniformRand(50., 100.)()
	d_world_height = 50.;
	EnkiTestWorld world(d_world_width, d_world_height, maxSteps);
	
	if (strcmp(argv[1], "viewer") == 0)
	{
		unsigned timeMultiplier = 1;
		if (argc >= 5)
			timeMultiplier = atoi(argv[4]);
		
		QApplication app(argc, argv);
		EnkiTestViewer viewer(&world, timeMultiplier);
		viewer.show();
		return app.exec();
	}
	else if (strcmp(argv[1], "noviewer") == 0)
	{
		world.run();
		return 0;
	}
	else
	{
		std::cerr << "First argument must either be viewer or noviewer" << std::endl;
		return 2;
	}
}

