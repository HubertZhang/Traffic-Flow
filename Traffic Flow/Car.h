#ifndef CAR_H
#define CAR_H

class Road;
class Car
{
public:
    //static int totalSpeed;
    Car(Road *road, int lane, int place, int maxspeed, int speed = 0);
    ~Car();
    virtual Car *duplicate() = 0;
    virtual void Motion() = 0;
	
	int distanceThisLane();
	int distanceOtherLane(int off);
	int distanceBack(int off);
   
	bool switchCondition(int off, int hopeSpeed); //offs = 1 or -1
	bool switchSafeCondition(int off);
	
	Road *road;
    int lane;
    int place;
    int speed;
    int maxspeed;
};

#endif