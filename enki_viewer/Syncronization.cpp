#include "viewer/Viewer.h"
#include <enki/PhysicalEngine.h>
#include <enki/robots/e-puck/EPuck.h>
#include "EPuckModel.h"
#include <enki/robots/marxbot/Marxbot.h>
#include <QtGui/QApplication>
#include <QtGui/QtGui>
#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <math.h>       /* trigonometric functions */
#include <enki/Types.h>
#include <QtCore/QPoint>

using namespace Enki;
using namespace std;

float randFloat(float a, float b);  // Return random float between a and b

/* Parameters */
int size = 150;             // Size of the arena (cm)
int NoRobots = 20;          // Number of robots
float vel = 12.8;           // Speed of robots
float ViewAngle = M_PI/180*80;   // Camera half field of view
float range = 140;           // Camera range
float tau = 20;             // Oscillator time constant
float eps = 0.1;            // Update rate
float Tled = 0.1;          // Time to keep LEDs On
float Tref = 0.2;           // Refractory period

/* Declarations and initializations */
float eta;                  // Order Parameter
float NoCycles = 0;         // Number of cylces of the reference robot
float Tsync = -1;           // Number of cycles it takes to synchronize

/* My own class extending UserData (through child EPuckModel to be able to
 * visualize it) 
 * Add phase parameter to the robots */
class Phase : public EPuckModel
{
    public:
        float phase;
        Phase(ViewerWidget* viewer) : EPuckModel(viewer){
            phase = 0;
            deletedWithObject = 0;
        }
};

/* Implement specifics of my world */
class MyWorld : public World
{
    public:
        MyWorld(double width, double height, const Color& wallsColor) : World(width, height, wallsColor)
        {
        }
        /* Extend virtual funtion controlStep, where the dynamics of the world
         * are specified */
        void controlStep(double dt)
        {
            bool NeighFired;    // Has a neighbour fired
            for (ObjectsIterator i = objects.begin(); i != objects.end(); ++i)
            {
                /* Do random reorientation when reaching on walls */
                // Right Wall
                if (((*i)->pos.x + (*i)->getRadius() >= this->w) && ((*i)->angle < M_PI/2 && (*i)->angle > -M_PI/2))
                {
                    (*i)->angle = randFloat(M_PI/2,3*M_PI/2);
                }
                // Left Wall
                if (((*i)->pos.x - (*i)->getRadius() <= 0) && ((*i)->angle < -M_PI/2 || (*i)->angle > M_PI/2))
                {
                    (*i)->angle = randFloat(-M_PI/2,M_PI/2);
                }
                
                // Top Wall
                if (((*i)->pos.y + (*i)->getRadius() >= this->h) && ((*i)->angle < M_PI && (*i)->angle > 0))
                {
                    (*i)->angle = randFloat(-M_PI,0);
                }
                // Bottom Wall
                if (((*i)->pos.y - (*i)->getRadius() <= 0) && ((*i)->angle < 0 && (*i)->angle > -M_PI))
                {
                    (*i)->angle = randFloat(0,M_PI);
                }

                /* Update Phases */
                Phase *foo = (Phase*)((*i)->userData);
                foo->phase += dt/tau;       // Normal phase update
                EPuck *bar = dynamic_cast<EPuck*> (*i); 

                /* If phase reaches threshold reset it and turn LEDs on */
                if (foo->phase >= 1)
                {
                    foo->phase = 0;                             // Reset phase
                    bar->setLedRing(true);                      // Turn LEDs on
                    (*i)->userData = (EPuck::UserData*) foo;    // Reasign pointer as changing color deletes it
                
                    /* If first robot fired calculate order parameter */
                    if (i == objects.begin())
                    {   
                        eta = 0;      // Order parameter
                        NoCycles++;
                        for (ObjectsIterator j = objects.begin(); j != objects.end(); ++j)
                        {
                            Phase *dum = (Phase*)((*j)->userData);
                            eta += cos(2*M_PI*(dum->phase))/NoRobots;
                        }
                        if (eta > 0.95 && Tsync == -1)
                        {
                            Tsync = NoCycles;
                        }
//                        std::cout<<"OrderPar = "<<eta<<std::endl;
                    }

                }
                /* Keep LEDs on for some time and then turn off */
                else if (foo->phase >= Tled && ((*i)->getColor()==Color::red))
                {
                    bar->setLedRing(false);                     // Turn LEDs off
                    (*i)->userData = (EPuck::UserData*) foo;    // Reasign pointer as changing color deletes it
                }

                /* Look for firing neighbors and increase phase correspondingly */
                NeighFired = false;
                for (int j = 20; j < 40 ; j++)
                {
                    if ((bar)->camera.image[j].r() == 1)
                    {
                        NeighFired = true;
                        break;
                    }
//                   std::cout<<(bar)->camera.image[j]<<std::endl;
                }
//                std::cout<<"-----"<<std::endl;
                /* Increase phase if seen a neighbor firing */
                if (NeighFired && foo->phase >= Tref)
                {
                    foo->phase *= (1+eps);
                }
            }
        }
};


class EnkiPlayground : public ViewerWidget
{
    protected:
    bool subjectiveView;
    QVector<EPuck*> epucks;
    QMap<PhysicalObject*, int> bullets;
	
    public:
    EnkiPlayground(World *world, QWidget *parent = 0) :
    	ViewerWidget(world, parent),
    	subjectiveView(false)
    {
        // Define viewer camera position and altitudes
        altitude= world->h;
        pos = QPointF(-world->w * 0.5, -world->h * 0);
    }
    
    /// This function must be called ONLY AFTER call viewer.show() that will
    //initialize all required values of the viewer, such attributes are used by
    //custom user data's made their setup.
    void addRobots(World *world,int NoRobots)
    {
        for(int i=0;i<NoRobots;i++) {            
            EPuck *epuck = new EPuck(0x2);                                      // Initialize new EPuck with camera
            epuck->userData = (EPuck::UserData*) new Phase( this );             // Initialize phase
            epuck->pos = Point(randFloat(0,world->w),randFloat(0,world->h));    // Random initial position
            epuck->angle = randFloat(0,2*M_PI);                                 // Random initial orientation
            epuck->leftSpeed = vel;                                             // Left motor speed
            epuck->rightSpeed = vel;                                            // Right motor speed
            Phase *foo = (Phase*)(epuck->userData);
            foo->phase = randFloat(0,1);                                        // Random initial phase
            epuck->camera.setRange(range);                                      // Camera range
            epuck->camera.halfFieldOfView = ViewAngle;                          // Camera half field of view
    
//            epuck->pos = Point(world->w/2,180*i);
//            epuck->angle = M_PI/2;
//            epuck->setLedRing(true);

            world->addObject(epuck);                                            // Add Epuck to the world
        }
    }
    
    ~EnkiPlayground() {}
    
    virtual void timerEvent(QTimerEvent * event)
    {
    	static int fireCounter = 0;
    	
    	QMap<PhysicalObject*, int>::iterator i = bullets.begin();
    	while (i != bullets.end())
    	{
            QMap<PhysicalObject*, int>::iterator oi = i;
            ++i;
            if (oi.value()) oi.value()--;
            else
            {
                PhysicalObject* o = oi.key();
                world->removeObject(o);
                bullets.erase(oi);
                delete o;
            }
    	}
    	ViewerWidget::timerEvent(event);
    }
    
    virtual void keyPressEvent ( QKeyEvent * event )
    {
        if (event->key() == Qt::Key_C)
        {
//            subjectiveView = !subjectiveView;
//            if (subjectiveView)
//            pitch = M_PI/8;
//            event->accept();
    }
    else ViewerWidget::keyPressEvent(event);
    }
    
    virtual void sceneCompletedHook()
    {
        glColor3d(0,0,0);
//        renderText(10, height()-50, tr("rotate camera by moving mouse while pressing ctrl+left mouse button"));
//        renderText(10, height()-30, tr("move camera on x/y by moving mouse while pressing ctrl+shift+left mouse button"));
//        renderText(10, height()-10, tr("move camera on z by moving mouse while pressing ctrl+shift+right mouse button"));
        // Display Order Parameter	
        renderText(5, 15, QString("Order Parameter: %0").arg(eta));
        renderText(5, 35, QString("Number of Cycles: %0").arg(NoCycles));
        if (Tsync != -1) renderText(5, 55, QString("Tsync: %0").arg(Tsync));
    }
};

int main(int argc, char *argv[])
{
    /* initialize random seed: */
    srand (time(NULL));
	
    QApplication app(argc, argv);
    // Create the world and the viewer
    MyWorld world(size,size, Color(0.9, 0.9, 0.9));  //square size
    Enki::World *ww = &world;
    EnkiPlayground viewer(ww);
	
    viewer.show();
    viewer.addRobots(ww, NoRobots);
	
    return app.exec();
}

/* Return a random float in the interval [a,b] */
float randFloat(float a, float b)
{
    return ((b-a)*((float)rand()/RAND_MAX))+a;
}
