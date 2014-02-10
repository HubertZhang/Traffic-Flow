#ifndef CAR_H
#define CAR_H

#define MAXBRAKE 50

class Road;
class Car
{
public:
	static bool leftpass, rightpass;
	static bool freepass;
	static bool blindness;
	static double driverpos; //position of driver to the left side of the car
	static int brake[MAXBRAKE];
	static int suddenbrake, weightedsuddenbrake;
	static int missexit;
	
	static bool perceiveddis;
	static bool intelligent;
	
    //static int totalSpeed;
    Car(int id, Road *road, int lane, int place, int maxspeed, int speed = 0);
    Car(const Car &b);
    ~Car();
    //virtual Car *duplicate() = 0;
    virtual void Motion();
    void copy(const Car &b);
	
	int distanceThisLane();
	int distanceOtherLane(int off);
	int distanceBack(int off);
	int distanceFrontSeen(int off);
	
	int distancePerceived(int dist);
   
	bool switchCondition(int off, int hopeSpeed); //offs = 1 or -1
	bool switchSafeCondition(int off);
	bool switchBackCondition(int off, int hopeSpeed);
	
	int currentSpeedLimit();
	
	Road *road;
    int lane;
    int place;
    int speed;
    int maxspeed;
    int id;
    
    double driverdepth;
};

#endif
