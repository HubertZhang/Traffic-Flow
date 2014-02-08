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
		/*
		//freepass
		if (switchCondition(off, maxspeed) && switchSafeCondition(off))
			pass = rand() < (RAND_MAX * ppass);
		*/
		//leftpass
		if (lane == 1)
		{
			if (switchCondition(off, hopeSpeed) && switchSafeCondition(off))
				pass = rand() < (RAND_MAX * ppass);
		}
		if (lane == 0)
		{
			if (switchBackCondition(off, hopeSpeed) && switchSafeCondition(off))
				pass = true;
		}
		
        //Speed up
        spd = std::min(maxspeed , spd + 1);
        //Deterministic speed down
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
        /*
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
		*/
        road->carMoveOff(lane, place, pass ? off : 0, spd); //this modifies the position of the car
        speed = spd;
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
		bool pass = false;
		int off = (int)(!lane) - lane;
		int spd = speed;
		int hopeSpeed = maxspeed;
		/*
		//freepass
		if (switchCondition(off, maxspeed) && switchSafeCondition(off))
			pass = rand() < (RAND_MAX * ppass);
		*/
		//leftpass
		if (lane == 1)
		{
			if (switchCondition(off, hopeSpeed) && switchSafeCondition(off))
				pass = rand() < (RAND_MAX * ppass);
		}
		if (lane == 0)
		{
			if (switchBackCondition(off, hopeSpeed) && switchSafeCondition(off))
				pass = true;
		}
		
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
		/*
        if (pass)
        {
			
			road->carMove(lane, place, lane + off, (place + spd) % road->length); //does not modify the info of the car, must be called before modifing
			place = (place + speed) % road->length;
        	lane += off;
			speed = spd;
        }
		else
		{
			road->carMove(lane, place, lane, (place + spd) % road->length); //does not modify the info of the car
			speed = spd;
        	place = (place + speed) % road->length;
		}
		*/
        road->carMoveOff(lane, place, pass ? off : 0, spd); //this modifies the position of the car
        speed = spd;
	}
protected:
	double pslow, ppass;
};

class Block : public Car //a block that does not move
{
public:
	Block(Road *road, int lane, int place, int maxspeed = 0) : Car(road, lane, place, maxspeed, 0)
	{}
    void Motion()
    {}
};

#endif
