//
//  main.cpp
//  Traffic Flow
//
//  Created by Hubert on 14-2-7.
//  Copyright (c) 2014年 Geek20. All rights reserved.
//

#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <vector>

class Road;
class Car
{
public:
    //static int totalSpeed;
    Car(Road *road, int lane, int place, int maxspeed, int speed = 0){
		this->road = road;
        this->lane = lane;
        this->place = place;
        this->maxspeed = maxspeed;
        this->speed = speed;
    }
    ~Car()
    {}
    virtual void Motion() = 0;
	
	Road *road;
    int lane;
    int place;
    int speed;
    int maxspeed;
};

class Road
{
	friend class Car;
public:
	Road(int w, int l)
	{
		width = w;
		length = l;
		data = new Car **[width];
		for (int i = 0; i < width; i++)
			data[i] = new Car *[length];
		tdata = new Car **[width];
		for (int i = 0; i < width; i++)
			tdata[i] = new Car *[length];
		frontpos = new int *[width];
		for (int i = 0; i < width; i++)
			frontpos[i] = new int[length];
		backpos = new int *[width];
		for (int i = 0; i < width; i++)
			backpos[i] = new int[length];
		clearData();
		clearBuffer();
		calOrder();
		std::cout << "order init finished\n";
	}
	~Road()
	{
		int i;
		for (int i = 0; i < width; i++)
		{
			delete data[i];
			delete tdata[i];
			delete frontpos[i];
			delete backpos[i];
		}
		delete data;
		delete tdata;
		delete frontpos;
		delete backpos;
	}
	void calOrder()
	{
		int l, k, sk, pk;
		//std::cout << "calOrder start, width = " << width << ", length = " << length << "\n";
		for (l = 0; l < width; l++)
		{
			for (sk = 0; sk < length && data[l][sk] == 0; sk++);
			//std::cout << "calOrder iteration " << l << ", sk = " << sk << "\n";
			if (sk >= length) //no vehicles on the lane
			{
				for (k = 0; k < length; k++)
					frontpos[l][k] = -1;
				for (k = 0; k < length; k++)
					backpos[l][k] = -1;
			}
			else
			{
				pk = sk;
				for (k = (sk + 1) % length; k != sk; k = (k + 1) % length)
				{
					backpos[l][k] = pk;
					if (data[l][k])
						pk = k;
				}
				backpos[l][k] = pk;
				if (data[l][k])
					pk = k;

				pk = sk;
				for (k = (sk - 1 + length) % length; k != sk; k = (k - 1 + length) % length)
				{
					if (data[l][k])
						pk = k;
					frontpos[l][k] = pk;
				}
				if (data[l][k])
					pk = k;
				frontpos[l][k] = pk;
			}
			/*
			for (k = 0; k < length; k++)
				std::cout << "frontpos[" << l << "][" << k << "] = " << frontpos[l][k] << "\n";
			for (k = 0; k < length; k++)
				std::cout << "backpos[" << l << "][" << k << "] = " << backpos[l][k] << "\n";
			*/
		}
	}
	Car *frontCar(int l, int k)
	{
		if (frontpos[l][k] == -1)
			return 0;
		return data[l][frontpos[l][k]];
	}
	Car *backCar(int l, int k)
	{
		if (backpos[l][k] == -1)
			return 0;
		return data[l][backpos[l][k]];
	}
	void carMove(int sl, int sk, int tl, int tk) //cars with speed 0 also need to call this method
	{
		tdata[tl][tk] = data[sl][sk];
	}
	void flush() //must flush after all cars have been moved. This updates the cellular automaton
	{
		Car ***ttdata;
		ttdata = tdata;
		tdata = data;
		data = ttdata;
		clearBuffer();
	}
	void clearData()
	{
		int i;
		for (i = 0; i < width; i++)
			memset(data[i], 0, sizeof(data[i][0]) * length);
	}
	void clearBuffer()
	{
		int i;
		for (i = 0; i < width; i++)
			memset(tdata[i], 0, sizeof(tdata[i][0]) * length);
	}
	Car **operator [](unsigned s)
	{
		return data[s];
	}
	int width, length;
	//Car *data[width][length];
	Car ***data, ***tdata;
	int **frontpos, **backpos;
};

class NS : public Car
{
	friend class Car;
public:
	NS(Road *road, int lane, int place, int maxspeed, int speed = 0, double pslow = 0.5) : Car(road, lane, place, maxspeed, speed)
	{
		this->pslow = pslow;
	}
    void Motion()
    {
        //Speed up
        speed = std::min(maxspeed , speed + 1);
        //Determined speed down
        Car *frontCar = road->frontCar(lane, (place + 1) % road->length);
        Car *frontCarOther = road->frontCar(!lane, place);
        Car *backCarOther = road->backCar(!lane, place);
        int distanceThisLane = frontCar ? (frontCar->place - this->place + road->length) % road->length : road->length;
        int distanceOtherLane = frontCarOther ? (frontCarOther->place - this->place + road->length) % road->length : road->length;
        int distanceSafe = backCarOther ? (this->place - backCarOther->place + road->length) % road->length : road->length;
        
        speed = std::max(std::min(distanceThisLane - 1, speed), 0);
        //Undetermined speed down
        
        if(rand() < RAND_MAX * pslow)
        {
            speed = std::max(speed - 1, 0);
        }
        
        //Move
        //totalSpeed+=speed; //this should not be here
		road->carMove(lane, place, lane, (place + speed) % road->length); //does not modify the info of the car
        place = (place + speed) % road->length;
    }
protected:
	double pslow;
};

#define LENGTH 1000
#define DENSITY 0.01
#define SPEEDMAX 5
#define ITERATION 1000

float avgSpeed[ITERATION]={0};
Road road(2, LENGTH);
int main(int argc, const char * argv[])
{
    std::cout << "Hello, World!\n";
    //srand(0);
    srand((unsigned)time(0));
	std::vector<Car *> cars;
    for (int i = 0; i<LENGTH; i++) {
        if (rand()<RAND_MAX*DENSITY) {
			Car *c = new NS(&road, 0, i, SPEEDMAX);
            road[0][i] = c;
			cars.push_back(c);
        }
        if (rand()<RAND_MAX*DENSITY) {
			Car *c = new NS(&road, 1, i, SPEEDMAX);
            road[1][i] = c;
			cars.push_back(c);
        }
    }
    //int* speedData = new int[amount];
    //int speedData[20];
    int flow=0;
    int passTime=0;
    long totaltotalspeed = 0;
    for (int i = 0; i<ITERATION; i++) {
		road.calOrder();
		for (int j = 0; j < cars.size(); j++)
		{
			cars[j]->Motion();
			//std::cout << "car " << (long)cars[j] << ": (" << cars[j]->lane << ", " << cars[j]->place << "), " << cars[j]->speed << "/" << cars[j]->maxspeed << "\n";
		}
		road.flush();
		int totalSpeed = 0;
		for (int j = 0; j < cars.size(); j++)
			totalSpeed += cars[j]->speed;
        avgSpeed[i] = (totalSpeed) / (cars.size() + 0.0);
        totaltotalspeed += totalSpeed;
    }
    double totavgspeed = (double)totaltotalspeed / (ITERATION * cars.size());
    double density = (cars.size() / (double)road.length);
    double flux = totavgspeed * density;
    // insert code here...
    std::cout << "Avg Speed: " << totavgspeed << "\n";
    std::cout << "Density  : " << density << "\n";
    std::cout << "Flux     : " << flux << "\n";
    return 0;
}

