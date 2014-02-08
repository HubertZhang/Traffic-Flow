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

int passcnt;

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
class Road
{
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
		fill0Data();
		fill0Buffer();
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
		if (sl != tl)
			passcnt++;
		tdata[tl][tk] = data[sl][sk];
		data[sl][sk] = data[sl][sk]->duplicate();
	}
	void flush() //must flush after all cars have been moved. This updates the cellular automaton
	{
		Car ***ttdata;
		ttdata = tdata;
		tdata = data;
		data = ttdata;
		if (findRepetition())
		{
			print();
			printf("previous state:\n");
			print(tdata);
			exit(0);
			while (1);
		}
		clearBuffer();
	}
	void fill0Data()
	{
		int i, j;
		for (i = 0; i < width; i++)
			for (j = 0; j < length; j++)
			{
				data[i][j] = 0;
			}
	}
	void fill0Buffer()
	{
		int i, j;
		for (i = 0; i < width; i++)
			for (j = 0; j < length; j++)
			{
				tdata[i][j] = 0;
			}
	}
	void clearData()
	{
		int i, j;
		for (i = 0; i < width; i++)
			for (j = 0; j < length; j++)
			{
				if (data[i][j])
					delete data[i][j];
				data[i][j] = 0;
			}
	}
	void clearBuffer()
	{
		int i, j;
		for (i = 0; i < width; i++)
			for (j = 0; j < length; j++)
			{
				if (tdata[i][j])
					delete tdata[i][j];
				tdata[i][j] = 0;
			}
	}
	Car **operator [](unsigned s)
	{
		return data[s];
	}
	bool findRepetition()
	{
		int l, k, l2, k2;
		for (l = 0; l < width; l++)
			for (k = 0; k < length; k++)
				for (l2 = 0; l2 < width; l2++)
					for (k2 = 0; k2 < length; k2++)
						if ((l != l2 || k != k2) && data[l][k] == data[l2][k2] && data[l][k])
							return true;
		return false;
	}
	void print(Car ***d = 0, FILE *out = stdout)
	{
		if (d == 0)
			d = data;
		int l, k;
		for (l = 0; l < width; l++)
		{
			for (k = 0; k < length; k++)
				fputc(d[l][k] ? ((long)d[l][k] % 23) + 'A' : '.', out);
			fprintf(out, "\n");
		}
		fprintf(out, "\n");
	}
	
	int width, length;
	//Car *data[width][length];
	Car ***data, ***tdata;
	int **frontpos, **backpos;
};

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
		return backCarOther ? distanceBack(off) >= backCarOther->maxspeed : true;
		//what should the distance be conpared with?
		//backCarOther->maxspeed is not reasonable
	}
	

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
        	spd = std::max(std::min(distanceOtherLane(off) - 1, spd), 0); //-1 or not
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
			spd = std::min(distanceOtherLane(off) - 1, maxspeed); //-1 or not
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

#define LENGTH 70
#define DENSITY 0.3
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
    for (int i = 0; i < cars.size(); i++)
    	for (int j = 0; j < cars.size(); j++)
    		if (i != j && cars[i] == cars[j])
    		{
				printf("repetition!\n");
				while (1);
			}
    //int* speedData = new int[amount];
    //int speedData[20];
    int flow=0;
    //int passTime=0;
    long totaltotalspeed = 0;
    passcnt = 0;
    for (int i = 0; i<ITERATION; i++) {
		road.calOrder();
		
		for (int j = 0; j < cars.size(); j++)
		{
			std::cout << "car " << (char)(((long)cars[j] % 23) + 'A') << " " << (long)cars[j] << ": (" << cars[j]->lane << ", " << cars[j]->place << "), " << cars[j]->speed << "/" << cars[j]->maxspeed << "\t";
			std::cout << "\t" << cars[j]->distanceThisLane() << "\n";
		}
        road.print();
        
		for (int j = 0; j < cars.size(); j++)
		{
			cars[j]->Motion();
		}
		//road.update();
		road.flush();
		int totalSpeed = 0;
		for (int j = 0; j < cars.size(); j++)
			totalSpeed += cars[j]->speed;
        avgSpeed[i] = (totalSpeed) / (cars.size() + 0.0);
        totaltotalspeed += totalSpeed;
        
        road.print();
        road.print(0, stderr);
        int n = 10000000;
        while (--n);
        
        if (i % (ITERATION / 100) == 0)
        	printf("iteration %d%%\n", i * 100 / ITERATION);
    }
    double totavgspeed = (double)totaltotalspeed / (ITERATION * cars.size());
    double density = (cars.size() / (double)road.length);
    double flux = totavgspeed * density;
    // insert code here...
    std::cout << "Avg Speed : " << totavgspeed << "\n";
    std::cout << "Density   : " << density << "\n";
    std::cout << "Flux      : " << flux << "\n";
    std::cout << "Pass Count: " << passcnt << "\n";
    return 0;
}

