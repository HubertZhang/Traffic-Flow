#ifndef CARS_H
#define CARS_H

#include "Car.h"

class NS : public Car
{
public:
	static const int preexitdis = 444;
	//double maxacc, thrdec, rnddec, pslow, ppass;
	NS(int id, Road *road, int lane, int place, int maxspeed, int speed = 0, 
		int maxacc = 3, int maxdec = 10, int thrdec = 6, int rnddec = 3, double pslow = 0.5, double ppass = 0.5)
		: Car(id, road, lane, place, maxspeed, speed)
	{
		this->maxacc = maxacc;
		this->maxdec = maxdec;
		this->thrdec = thrdec;
		this->rnddec = rnddec;
		this->pslow = pslow;
		this->ppass = ppass;
		
		this->toexit = false;
		
		this->phopetoexit = false;
	}
	/*
	Car *duplicate()
	{
		return new NS(road, lane, place, maxspeed, speed, pslow, ppass);
	}
	*/
	void setToExit(bool b = true)
	{
		toexit = b;
	}
    void Motion()
    {
		//Pass Conditions
		bool pass = false;
		int spd = this->speed;
		int posmaxspeed = maxspeed;
		int hopeSpeed = std::min(posmaxspeed , spd + maxacc);
		int off = (int)(!lane) - lane;
		//printf("switch %d, safe %d\n", (int)switchCondition(off, hopeSpeed), (int)switchSafeCondition(off));
		//printf("dtl = %d, off = %d\n", distanceThisLane(), off);
		
		bool hopetoexit = false;
		if (Road::exits && this->toexit && place >= road->length - preexitdis) //move to the rightmost lane to exit
		{
			hopetoexit = true;
			pass = false;
			if (lane < road->width - 1)
			{
				off = 1;
				if (switchSafeCondition(off))
					pass = true;
			}
		}
		else if (Road::exits && lane == road->width - 1 && place < Road::enterbuffer) //entering
		{
			pass = false;
			off = 1;
			if (switchSafeCondition(off))
				pass = true;
		}
		else //not exiting
		{
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
		}//not exiting
		
		if (Road::exits && this->toexit && phopetoexit && !hopetoexit)
		{
			if (lane != road->width - 1)
				missexit++;
		}
		phopetoexit = hopetoexit;
		
		int sl = currentSpeedLimit();
		if (spd <= sl) //speed up
			spd = std::min(std::min(maxspeed, sl), spd + maxacc);
		else //follow speed limit
        	spd = std::max(sl, spd - thrdec);

        //Deterministic speed down
        int dol = distancePerceived(distanceOtherLane(off));
		if (blindness)
        	dol = std::min(dol, distancePerceived(distanceFrontSeen(off)) + 1);
        if (pass)
        	spd = std::max(std::min(std::min(dol, distancePerceived(distanceThisLane())) - 1, spd), 0); //-1 or not
		else
        	spd = std::max(std::min(distancePerceived(distanceThisLane()) - 1, spd), 0);
        
        //Undeterministic speed down
        if(!intelligent && rand() < RAND_MAX * pslow)
        {
            spd = std::max(spd - rnddec, 0);
        }
        
        if (speed > spd) //brake
        {
			brake[speed - spd]++;
		}
        if (speed - spd > thrdec) //sudden brake
        {
			suddenbrake++;
			weightedsuddenbrake += speed - spd - thrdec;
		}
        
        //Move
        road->carMoveOff(lane, place, pass ? off : 0, spd); //this modifies the position of the car
        speed = spd;
    }
protected:
	int maxacc, maxdec, rnddec, thrdec;
	double pslow, ppass;
	bool toexit;
	
	bool phopetoexit;
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
