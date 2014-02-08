#include "Car.h"
#include "Road.h"

//static int totalSpeed;
Car::Car(Road *road, int lane, int place, int maxspeed, int speed){
	this->road = road;
    this->lane = lane;
    this->place = place;
    this->maxspeed = maxspeed;
    this->speed = speed;
}
Car::~Car()
{}
//virtual Car *duplicate() = 0;
//virtual void Motion() = 0;

int Car::distanceThisLane()
{
	Car *frontCar = road->frontCar(lane, (place + 1) % road->length);
	//printf(" frontcar of %ld = %ld ", (long)this, (long)frontCar);
    return frontCar && frontCar != this ? (frontCar->place - this->place + road->length) % road->length : road->length;
}
int Car::distanceOtherLane(int off)
{
	if (off == 0)
		return distanceThisLane();
	if (lane + off < 0 || lane + off >= road->width)
		return -1;
    Car *frontCarOther = road->frontCar(lane + off, place);
    return frontCarOther ? (frontCarOther->place - this->place + road->length) % road->length : road->length;
}
int Car::distanceBack(int off)
{
	if (lane + off < 0 || lane + off >= road->width)
		return -1;
	Car *backCarOther = road->backCar(lane + off, place);
    return backCarOther && backCarOther != this ? (this->place - backCarOther->place + road->length) % road->length : road->length;
}

bool Car::switchCondition(int off, int hopeSpeed) //offs = 1 or -1
{
	if (lane + off < 0 || lane + off >= road->width)
		return false;
	int dtl = distanceThisLane();
	return dtl - 1 < hopeSpeed && distanceOtherLane(off) > dtl;  //dtl-1?
}
bool Car::switchSafeCondition(int off)
{
	if (lane + off < 0 || lane + off >= road->width)
		return false;
	Car *backCarOther = road->backCar(lane + off, place);
	return backCarOther ? distanceBack(off) > backCarOther->maxspeed : true;
	//what should the distance be conpared with?
	//backCarOther->maxspeed is not reasonable
}
