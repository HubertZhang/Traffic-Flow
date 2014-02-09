#ifndef CAR_H
#define CAR_H

class Road;
class Car
{
public:
	static bool leftpass;
	static bool freepass;
	static double driverpos; //position of driver to the left side of the car
    //static int totalSpeed;
    Car(Road *road, int lane, int place, int maxspeed, int speed = 0);
    ~Car();
    virtual Car *duplicate() = 0;
    virtual void Motion() = 0;
	
	int distanceThisLane();
	int distanceOtherLane(int off);
	int distanceBack(int off);
	int distanceFrontSeen(int off);
   
	bool switchCondition(int off, int hopeSpeed); //offs = 1 or -1
	bool switchSafeCondition(int off);
	bool switchBackCondition(int off, int hopeSpeed);
	
	Road *road;
    int lane;
    int place;
    int speed;
    int maxspeed;
};

#endif
