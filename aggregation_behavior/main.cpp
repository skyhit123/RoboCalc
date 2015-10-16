#include <cstdlib>
#include <cstring>
#include <sys/time.h>
#include <sys/stat.h>
#include <math.h>
#include "parameters.h"

#include "viewer/Viewer.h"
#include <enki/PhysicalEngine.h>
#include <enki/robots/e-puck/EPuck.h>
#include <QtGui/QApplication>
#include <QtGui/QtGui>
#include <iostream>
#include <fstream>

gsl_rng *rng;
double AgentController[4] = {-0.7, -1.0, 1.0, -1.0};

void gsl_init();

using namespace Enki;
using namespace std;

/* Implement specifics of my world */
class MyWorld : public World
{
    public:
		MyWorld(double width, double height, const Color& wallsColor, unsigned maxSteps) :
        	Enki::World(width, height, wallsColor), maxSteps(maxSteps)
        {
        	c_step = 0;
			for (int i = 0; i < (NumberOfAgent); i++){
				epucks.push_back(new Enki::EPuck(0));  //sensor ability: none; noise: 0.05
				addObject(epucks[i]);
			}
			InitializeConfiguration();
        }

		void InitializeConfiguration()
		{

			for(int i = 0; i < (NumberOfAgent); i++)
			{
				double xi;
				double yi;
				bool Overlap;

				do
				{
		//			const double LengthOfSides = sqrt(AreaPerRobot*(NumberOfAgent+NumberOfModels));
					const double LengthOfSides = ArenaWidth;
					xi = LengthOfSides*gsl_rng_uniform(rng);
				    yi = LengthOfSides*gsl_rng_uniform(rng);
					Overlap = false;

					for (int j = 0; j < i; j ++ )
					{
						double xj = epucks[j]->pos.x;
						double yj = epucks[j]->pos.y;

						double dij = sqrt(pow(xi - xj, 2.0) + pow(yi - yj, 2.0));

						if (dij < (2.0 * EPuckRadius))
						{
							Overlap = true;
							break;
						}
					}

				} while(Overlap == true);

				epucks[i]->pos.x = xi;
				epucks[i]->pos.y = yi;
				epucks[i]->angle = 2.0*M_PI*(gsl_rng_uniform(rng) - 0.5);
			}

		}

		void UpdateAgentSpeed()
		{
			for (int i = 0; i < (NumberOfAgent); i++){
				int SensorReading = ComputeSensorReading(i);
				if (SensorReading == 0){
					epucks[i]->leftSpeed  = EPuckMaximumSpeed*AgentController[0];
					epucks[i]->rightSpeed = EPuckMaximumSpeed*AgentController[1];
				}
				else if (SensorReading == 1){
					epucks[i]->leftSpeed  = EPuckMaximumSpeed*AgentController[2];
					epucks[i]->rightSpeed = EPuckMaximumSpeed*AgentController[3];
				}
			}
		}

		int ComputeSensorReading(const int &Index)
		{
			double Output = 0;
			for(int j = 0; j < (NumberOfAgent); j ++ )
				if( j != Index )
				{
					const double xi = epucks[Index]->pos.x;
					const double yi = epucks[Index]->pos.y;
					const double ai = epucks[Index]->angle;

					const double xj = epucks[j]->pos.x;
					const double yj = epucks[j]->pos.y;

					const double Side = -sin(ai)*(yi - yj) - cos(ai)*(xi - xj);
					const double PerpendicularDistance = fabs((yi - yj)*cos(ai) - (xi - xj)*sin(ai));

					if(Side > 0 && PerpendicularDistance <= EPuckRadius)
					{
						Output = 1;
						break;
					}
				}

			return Output;
		}
    	// returns true if the total simulation period has elapsed
    	bool runStep()
    	{
    		UpdateAgentSpeed();
    		if (c_step != maxSteps)
    		{
    			step(ControlStepSize, OversamplingRate);
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
        /* Extend virtual funtion controlStep, where the dynamics of the world
         * are specified
         * void controlStep(double dt) */
    private:
        unsigned maxSteps;
        unsigned c_step;
        std::vector< Enki::EPuck * > epucks;
};

class EnkiViewer : public ViewerWidget
{
protected:
	unsigned timeMultiplier;
	int timerId;
	MyWorld* myworld;

public:
	EnkiViewer(MyWorld *world, unsigned timeMultiplier = 1, QWidget *parent = 0) :
		ViewerWidget(world, parent),
		timeMultiplier(timeMultiplier)
	{
		myworld = world;
		timerId = startTimer(100);  //0.1s timer
        // Define viewer camera position and altitudes
        altitude= world->h;
        pos = QPointF(-world->w * 0.5, -world->h * 0);
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

	virtual void wheelEvent(QWheelEvent * event)   //zoom in or out the video through middle button of mouse
	{
		if (event->delta() > 0)
			timeMultiplier *= 2;
		else if (event->delta() < 0 && timeMultiplier > 1)
			timeMultiplier /= 2;
		setWindowTitle(QString(tr("Simulation running at %0 times real-time").arg(timeMultiplier)));
	}

    virtual void sceneCompletedHook()  //put some texts over the video
    {
        glColor3d(0,0,0);
//        renderText(10, height()-50, tr("rotate camera by moving mouse while pressing ctrl+left mouse button"));
//        renderText(10, height()-30, tr("move camera on x/y by moving mouse while pressing ctrl+shift+left mouse button"));
//        renderText(10, height()-10, tr("move camera on z by moving mouse while pressing ctrl+shift+right mouse button"));
        renderText(5, 15, QString(tr("Simulation running at %0 times real-time").arg(timeMultiplier)));
    }

};

int main(int argc, char* argv[])
{
	gsl_init();

	MyWorld myWorld(ArenaWidth, ArenaWidth, Color(0.9, 0.9, 0.9), MaximumStepSize);

	if (strcmp(argv[1], "viewer") == 0){
		unsigned timeMultiplier = 1;
		QApplication app(argc, argv);
		EnkiViewer viewer(&myWorld, timeMultiplier);
		viewer.show();
		return app.exec();
	}
	else if (strcmp(argv[1], "noviewer") == 0){
		myWorld.run();
		return 0;
	}
	else{
		std::cerr << "First argument must either be viewer or noviewer" << std::endl;
		return 2;
	}
}

void gsl_init()
{
	gsl_rng_free(rng);
	unsigned long int Seed;
	struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv,&tz);
    Seed = tv.tv_sec * 1000000 + tv.tv_usec;
    rng = gsl_rng_alloc(gsl_rng_default);
    gsl_rng_set(rng, Seed);

    cout << "Seed: " << Seed << endl;
}

