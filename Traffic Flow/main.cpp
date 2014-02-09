#define WIDTH 2
#define LENGTH 3000
#define DENSITY_MIN 0.01
#define DENSITY_MAX 0.4
#define ITERATION 10000
#define OMIT_ITERATION 1000
//#define pWWH 0.5
#define SPEEDMAX1 22
#define pSM1 0.5
#define SPEEDMAX2 12
#define pEXIT 0.1

#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "Car.h"
#include "Road.h"
#include "Cars.h"

struct Result
{
	int width;
	double avgspeed[MAXWIDTH], density[MAXWIDTH], flux[MAXWIDTH];
	int switchtoL[MAXWIDTH], switchtoR[MAXWIDTH];
	double paras, sumd, sumf;
	int switchcnt;
	int brake[MAXBRAKE];
	int suddenbrake;
	Result(int wid, double *as, double *d, int *stl, int *str, int *brk, int sbrk)
	{
		width = wid;
		memcpy(avgspeed, as, sizeof(avgspeed[0]) * width);
		memcpy(density, d, sizeof(density[0]) * width);
		sumf = 0;
		sumd = 0;
		for (int i = 0; i < width; i++)
		{
			flux[i] = avgspeed[i] * density[i];
			sumf += flux[i];
			sumd += density[i];
		}
		/*
		avgspeed[0] = as[0];
		avgspeed[1] = as[1];
		density[0] = d[0];
		density[1] = d[1];
		flux[0] = avgspeed[0] * density[0];
		flux[1] = avgspeed[1] * density[1];
		sumf = flux[0] + flux[1];
		sumd = density[0] + density[1];
		*/
		paras = sumf / sumd;
		
		/*
		switchtoL[0] = stl[0];
		switchtoL[1] = stl[1];
		switchtoR[0] = str[0];
		switchtoR[1] = str[1];
		*/
		memcpy(switchtoL, stl, sizeof(switchtoL[0]) * width);
		memcpy(switchtoR, str, sizeof(switchtoR[0]) * width);
		switchcnt = 0;
		for (int i = 0; i < width; i++)
			switchcnt += stl[i] + str[i];
		//switchcnt = stl[0] + stl[1] + str[0] + str[1];
		memcpy(brake, brk, sizeof(brake[0]) * MAXBRAKE);
		suddenbrake = sbrk;
	}
	static void outputHead(int width, FILE *fp)
	{
		for (int i = 0; i < width; i++)
			fprintf(fp, "avgspeed%d\tdensity%d\tflux%d\tstl%d\tstr%d\t", i, i, i, i, i);
		fprintf(fp, "par avgspeed\tsum density\tsum flux\tswitchcnt\tsbrkcnt\n");
	}
	void output(FILE *fp)
	{
	    std::cout << "Avg Speed : " << paras << "\n";
	    std::cout << "Density   : " << sumd << "\n";
	    std::cout << "Flux      : " << sumf << "\n";
	    std::cout << "Pass Count: " << switchcnt << "\n";
		for (int i = 0; i < width; i++)
			fprintf(fp, "%lf\t%lf\t%lf\t%d\t%d\t", avgspeed[i], density[i], flux[i], switchtoL[i], switchtoR[i]);
		fprintf(fp, "%lf\t%lf\t%lf\t%d\t%d\n", paras, sumd, sumf, switchcnt, suddenbrake);
	}
};

//float avgSpeed[ITERATION]={0};
Result simulate(int width, double expdensity)
{
	Road road(width, LENGTH);
    std::cout << "Hello, World!\n";
    //srand(0);
	std::vector<Car *> cars;
    for (int i = 0; i < road.length; i++) {
		for (int l = 0; l < width; l++) {
		
	        if (rand() < RAND_MAX * expdensity) {
				Car *c;
				int maxspeed = (rand() < RAND_MAX * pSM1) ? SPEEDMAX1 : SPEEDMAX2;
				c = new NS(cars.size(), &road, l, i, maxspeed);
	            //road[l][i] = c;
				cars.push_back(c);
	        }
		}
	}
	if (Road::exits) ///add blocks
	{
		int l = width;
		for (int k = Road::enterbuffer; k < road.length - Road::exitbuffer/*length + offlength*/; k++)
		{
			Car *c = new Block(cars.size(), &road, l, k);
			cars.push_back(c);
		}
	}
	road.registerCars(cars);
	printf("registering complete\n");
    //int* speedData = new int[amount];
    //int speedData[20];
    int flow=0;
    //int passTime=0;
    long ttspeed[MAXWIDTH] = {0};
    long ttcount[MAXWIDTH] = {0};
    int cntiteration = 0;
    memset(Road::switchtoL, 0, sizeof(Road::switchtoL));
    memset(Road::switchtoR, 0, sizeof(Road::switchtoR));
    Road::switchcnt = 0;
    memset(Car::brake, 0, sizeof(Car::brake));
    Car::suddenbrake = 0;
    
    for (int i = 0; i < ITERATION; i++) {
		road.calOrder();
		/*
		for (int j = 0; j < cars.size(); j++)
		{
			std::cout << "car " << (char)(((long)cars[j] % 23) + 'A') << " " << (long)cars[j] << ": (" << cars[j]->lane << ", " << cars[j]->place << "), " << cars[j]->speed << "/" << cars[j]->maxspeed << "\t";
			std::cout << "\t" << cars[j]->distanceThisLane() << "\n";
		}
        road.print();
        */
		for (int j = 0; j < cars.size(); j++)
		{
			cars[j]->Motion();
		}
		//road.update();
		road.flush();
		/*
	    for (int a = 0; a < cars.size(); a++)
	    	for (int b = 0; b < cars.size(); b++)
	    		if (a != b && cars[a]->lane == cars[b]->lane && cars[a]->place == cars[b]->place)
	    		{
					printf("overlap! %c and %c\n", (char)(((long)cars[a] % 23) + 'A'), (char)(((long)cars[b] % 23) + 'A'));
			        road.print();
					while (1);
				}
		*/
		int totalSpeed = 0;
		for (int j = 0; j < cars.size(); j++)
			totalSpeed += cars[j]->speed;
        //avgSpeed[i] = (totalSpeed) / (cars.size() + 0.0);
        if (i >= OMIT_ITERATION)
        {
			for (int j = 0; j < cars.size(); j++)
				if (cars[j]->maxspeed != 0) //wipe out blocks
				{
					ttspeed[cars[j]->lane] += cars[j]->speed;
					ttcount[cars[j]->lane]++;
				}
			cntiteration++;
		}
        /*
        road.print();
        road.print(0, stderr);
        int n = 10000000;
        while (--n);
        */
        /*
        if (i % (ITERATION / 100) == 0)
        	printf("iteration %d%%\n", i * 100 / ITERATION);
        */
    }
    /*
    double totavgspeed = (double)(ttspeed[0] + ttspeed[1]) / (ttcount[0] + ttcount[1]);
    double density = (cars.size() / (double)road.length);
    double flux = totavgspeed * density;
    // insert code here...
    std::cout << "Avg Speed : " << totavgspeed << "\n";
    std::cout << "Density   : " << density << "\n";
    std::cout << "Flux      : " << flux << "\n";
    std::cout << "Pass Count: " << Road::switchcnt << "\n";*/
    //return Result(totavgspeed, density, flux, Road::passcnt);
    double as[MAXWIDTH], d[MAXWIDTH];
    as[0] = ttspeed[0] / (double)ttcount[0];
    as[1] = ttspeed[1] / (double)ttcount[1];
    d[0] = ttcount[0] / (double)cntiteration / (double)road.length;
    d[1] = ttcount[1] / (double)cntiteration / (double)road.length;
    return Result(width, as, d, Road::switchtoL, Road::switchtoR, Car::brake, Car::suddenbrake);
}

char FILENAME[2048];

int main(int argc, const char * argv[])
{
	int i;
	FILE *fp;
    
    Road::exits = false;
    Car::leftpass = true;
    Car::rightpass = false;
    Car::freepass = false;
    Car::blindness = true;
    char *pass = (char *)(Car::leftpass ? "leftpass" : Car::rightpass ? "rightpass" : Car::freepass ? "freepass" : "nopass");
    char *blnd = (char *)(Car::blindness ? "blindness" : "no blindness");
	sprintf(FILENAME, "result NS, %.2lf-%.2lfdensity, %.2lfx%d+%.2lfx%dspeed, %.2lfpEXIT, %s, %s, driverpos%.2lf.txt",
		DENSITY_MIN, DENSITY_MAX, pSM1, SPEEDMAX1, 1.0 - pSM1, SPEEDMAX2, Road::exits ? pEXIT : 0.0, pass, blnd, Car::driverpos);
	printf("outputfile: %s\n", FILENAME);
	
	fp = fopen(FILENAME, "r");
	if (!fp)
	{
		fclose(fp);
		fp = fopen(FILENAME, "w");
		Result::outputHead(WIDTH, fp);
		fclose(fp);
	}
	else
		fclose(fp);
    srand((unsigned)time(0));
    
	for (i = 0; i < 1000; i++)
	{
		printf("Simulation #%d\n", i);
		fp = fopen(FILENAME, "a");
		simulate(WIDTH, DENSITY_MIN + (DENSITY_MAX - DENSITY_MIN) * (rand() / (double)RAND_MAX)).output(fp);
		fclose(fp);
	}
	return 0;
}
