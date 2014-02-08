#ifndef CARS_H
#define CARS_H

#include "Car.h"

class NS : public Car
{
public:
	NS(Road *road, int lane, int place, int maxspeed, int speed = 0, double pslow = 0.5, double ppass = 0.5)
		: Car(road, lane, place, maxspeed, speed)
	{
		this->pslow = pslow;
		this->ppass = ppass;
	}
	Car *duplicate()
	{
		return new NS(road, lane, place, maxspeed, speed, pslow, ppass);
	}
    void Motion()
    {
		//Pass Conditions
		bool pass = false;
		int spd = this->speed;
		int hopeSpeed = std::min(maxspeed , spd + 1);
		int off = (int)(!lane) - lane;
		//printf("switch %d, safe %d\n", (int)switchCondition(off, hopeSpeed), (int)switchSafeCondition(off));
		//printf("dtl = %d, off = %d\n", distanceThisLane(), off);
		if (switchCondition(off, hopeSpeed) && switchSafeCondition(off))
			pass = rand() < (RAND_MAX * ppass);
		
        //Speed up
        spd = std::min(maxspeed , spd + 1);
        //Deterministic speed down
        /*
        Car *frontCar = road->frontCar(lane, (place + 1) % road->length);
        Car *frontCarOther = road->frontCar(!lane, place);
        Car *backCarOther = road->backCar(!lane, place);
        int distanceThisLane = frontCar ? (frontCar->place - this->place + road->length) % road->length : road->length;
        int distanceOtherLane = frontCarOther ? (frontCarOther->place - this->place + road->length) % road->length : road->length;
        int distanceSafe = backCarOther ? (this->place - backCarOther->place + road->length) % road->length : road->length;
        */
        if (pass)
        	spd = std::max(std::min(std::min(distanceOtherLane(off), distanceThisLane()) - 1, spd), 0); //-1 or not
		else
        	spd = std::max(std::min(distanceThisLane() - 1, spd), 0);
        
        //Undeterministic speed down
        if(rand() < RAND_MAX * pslow)
        {
            spd = std::max(spd - 1, 0);
        }
        
        //Move
        //totalSpeed+=speed; //this should not be here
        if (pass)
        {
			road->carMove(lane, place, lane + off, (place + spd) % road->length); //does not modify the info of the car, must be called before modifing
			speed = spd;
        	place = (place + speed) % road->length;
        	lane += off;
		}
		else
		{
			road->carMove(lane, place, lane, (place + spd) % road->length); //does not modify the info of the car
			speed = spd;
        	place = (place + speed) % road->length;
		}
    }
protected:
	double pslow, ppass;
};

class WWH : public Car
{
public:
	WWH(Road *road, int lane, int place, int maxspeed, int speed = 0, double pslow = 0.5, double ppass = 0.5)
		: Car(road, lane, place, maxspeed, speed)
	{
		this->pslow = pslow;
		this->ppass = ppass;
	}
	Car *duplicate()
	{
		return new WWH(road, lane, place, maxspeed, speed, pslow, ppass);
	}
    void Motion()
    {
		//Pass Conditions
		bool pass = false;;
		int off = (int)(!lane) - lane;
		int spd = speed;
		if (switchCondition(off, maxspeed) && switchSafeCondition(off))
			pass = rand() < (RAND_MAX * ppass);
			
		//Speed up
		if (pass)
			spd = std::min(std::min(distanceOtherLane(off), distanceThisLane()) - 1, maxspeed); //-1 or not
		else
			spd = std::min(distanceThisLane() - 1, maxspeed);
		
        //Undetermined speed down
        if(rand() < RAND_MAX * pslow)
        {
            spd = std::max(spd - 1, 0);
        }
        
        //Move
        if (pass)
        {
			road->carMove(lane, place, lane + off, (place + spd) % road->length); //does not modify the info of the car, must be called before modifing
			speed = spd;
        	place = (place + speed) % road->length;
        	lane += off;
		}
		else
		{
			road->carMove(lane, place, lane, (place + spd) % road->length); //does not modify the info of the car
			speed = spd;
        	place = (place + speed) % road->length;
		}
	}
protected:
	double pslow, ppass;
};

class Block : public Car //a block that does not move
{
public:
	Block(Road *road, int lane, int place, int maxspeed = 0) : Car(road, lane, place, maxspeed, speed)
	{}
    void Motion()
    {}
};

#endif
