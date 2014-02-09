#ifndef CARS_H
#define CARS_H

#include "Car.h"

class NS : public Car
{
public:
	NS(int id, Road *road, int lane, int place, int maxspeed, int speed = 0, int maxacc = 3, int maxdec = 10, double pslow = 0.5, double ppass = 0.5)
		: Car(id, road, lane, place, maxspeed, speed)
	{
		this->pslow = pslow;
		this->ppass = ppass;
	}
	/*
	Car *duplicate()
	{
		return new NS(road, lane, place, maxspeed, speed, pslow, ppass);
	}
	*/
    void Motion()
    {
		//Pass Conditions
		bool pass = false;
		int spd = this->speed;
		int hopeSpeed = std::min(maxspeed , spd + 1);
		int off = (int)(!lane) - lane;
		//printf("switch %d, safe %d\n", (int)switchCondition(off, hopeSpeed), (int)switchSafeCondition(off));
		//printf("dtl = %d, off = %d\n", distanceThisLane(), off);
		
		if (freepass)
		{
			if (switchCondition(off, hopeSpeed) && switchSafeCondition(off))
				pass = rand() < (RAND_MAX * ppass);
		}
		if (leftpass)
		{
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
		}
		if (rightpass)
		{
			if (lane == 0)
			{
				if (switchCondition(off, hopeSpeed) && switchSafeCondition(off))
					pass = rand() < (RAND_MAX * ppass);
			}
			if (lane == 1)
			{
				if (switchBackCondition(off, hopeSpeed) && switchSafeCondition(off))
					pass = true;
			}
		}
		
        //Speed up
        spd = std::min(maxspeed , spd + 1);
        //Deterministic speed down
        int dol = distanceOtherLane(off);
		if (blindness)
        	dol = std::min(dol, distanceFrontSeen(off) + 1);
        if (pass)
        	spd = std::max(std::min(std::min(dol, distanceThisLane()) - 1, spd), 0); //-1 or not
		else
        	spd = std::max(std::min(distanceThisLane() - 1, spd), 0);
        
        //Undeterministic speed down
        if(rand() < RAND_MAX * pslow)
        {
            spd = std::max(spd - 1, 0);
        }
        
        //Move
        road->carMoveOff(lane, place, pass ? off : 0, spd); //this modifies the position of the car
        speed = spd;
    }
protected:
	double pslow, ppass;
};

class Block : public Car //a block that does not move
{
public:
	Block(int id, Road *road, int lane, int place, int maxspeed = 0) : Car(id, road, lane, place, maxspeed, 0)
	{}
    void Motion()
    {
		road->carMoveOff(lane, place, 0, 0);
	}
};

#endif
