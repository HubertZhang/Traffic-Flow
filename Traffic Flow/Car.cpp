#include <iostream>
#include <vector>

#include "Car.h"
#include "Road.h"

bool Car::leftpass = false;
bool Car::rightpass = false;
bool Car::freepass = false;
bool Car::blindness = false;
double Car::driverpos = 0.25;

//static int totalSpeed;
Car::Car(int id, Road *road, int lane, int place, int maxspeed, int speed){
	this->id = id;
	this->road = road;
    this->lane = lane;
    this->place = place;
    this->maxspeed = maxspeed;
    this->speed = speed;
    
    this->driverdepth = 0.4;
}
Car::Car(const Car &b)
{
	copy(b);
}
Car::~Car()
{}
//virtual Car *duplicate() = 0;
void Car::Motion()
{
	printf("Error! Moving a virtual car!");
}
void Car::copy(const Car &b)
{
	road = b.road;
	lane = b.lane;
	place = b.place;
	maxspeed = b.maxspeed;
	speed = b.speed;
	
	id = b.id;
	
	driverdepth = b.driverdepth;
}

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
int Car::distanceFrontSeen(int off)
{
	if (lane + off < 0 || lane + off >= road->width)
		return -1;
	double p;
	if (off == 1)
		p = 1.0 - driverpos;
	else if (off == -1)
		p = driverpos;
	else
		return -1;
	if (p == 0.0)
		return road->length;
	double gap = distanceThisLane() - 1.0;
	double d = (gap + driverdepth) / p + gap;
	return std::min((int)(d + 1.0 - 1E-7) /*ceil(d)*/, road->length);
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
bool Car::switchBackCondition(int off, int hopeSpeed)
{
	if (lane + off < 0 || lane + off >= road->width)
		return false;
	int dol = distanceOtherLane(off);
	return hopeSpeed <= dol - 1 || distanceThisLane() <= dol;
}

int Car::currentSpeedLimit()
{
	return road->speedLimit(lane, place);
}
