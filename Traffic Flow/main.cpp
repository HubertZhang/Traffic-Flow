#define LENGTH 3000
#define DENSITY_MIN 0.01
#define DENSITY_MAX 0.4
#define ITERATION 10000
#define OMIT_ITERATION 1000
//#define pWWH 0.5
#define SPEEDMAX1 22
#define pSM1 0.5
#define SPEEDMAX2 15

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
	double avgspeed[2], density[2], flux[2];
	int switchtoL[2], switchtoR[2];
	double paras, sumd, sumf;
	int switchcnt;
	Result(double *as, double *d, int *stl, int *str)
	{
		avgspeed[0] = as[0];
		avgspeed[1] = as[1];
		density[0] = d[0];
		density[1] = d[1];
		flux[0] = avgspeed[0] * density[0];
		flux[1] = avgspeed[1] * density[1];
		sumf = flux[0] + flux[1];
		sumd = density[0] + density[1];
		paras = sumf / sumd;
		
		switchtoL[0] = stl[0];
		switchtoL[1] = stl[1];
		switchtoR[0] = str[0];
		switchtoR[1] = str[1];
		switchcnt = stl[0] + stl[1] + str[0] + str[1];
	}
	static void outputHead(FILE *fp)
	{
		for (int i = 0; i < 2; i++)
			fprintf(fp, "avgspeed%d\tdensity%d\tflux%d\tstl%d\tstr%d\t", i, i, i, i, i);
		fprintf(fp, "par avgspeed\tsum density\tsum flux\tswitchcnt\n");
	}
	void output(FILE *fp)
	{
		for (int i = 0; i < 2; i++)
			fprintf(fp, "%lf\t%lf\t%lf\t%d\t%d\t", avgspeed[i], density[i], flux[i], switchtoL[i], switchtoR[i]);
		fprintf(fp, "%lf\t%lf\t%lf\t%d\n", paras, sumd, sumf, switchcnt);
	}
};

//float avgSpeed[ITERATION]={0};
Result simulate(double expdensity)
{
	Road road(2, LENGTH);
    std::cout << "Hello, World!\n";
    //srand(0);
	std::vector<Car *> cars;
    for (int i = 0; i<LENGTH; i++) {
		for (int l = 0; l < 2; l++) {
		
	        if (rand() < RAND_MAX * expdensity) {
				Car *c;
				int maxspeed = (rand() < RAND_MAX * pSM1) ? SPEEDMAX1 : SPEEDMAX2;
				c = new NS(cars.size(), &road, l, i, maxspeed);
	            //road[l][i] = c;
				cars.push_back(c);
	        }
		}
	}
	road.registerCars(cars);
	printf("registering complete\n");
    //int* speedData = new int[amount];
    //int speedData[20];
    int flow=0;
    //int passTime=0;
    long ttspeed[2] = {0};
    long ttcount[2] = {0};
    int cntiteration = 0;
    memset(Road::switchtoL, 0, sizeof(Road::switchtoL));
    memset(Road::switchtoR, 0, sizeof(Road::switchtoR));
    Road::switchcnt = 0;
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
    double totavgspeed = (double)(ttspeed[0] + ttspeed[1]) / (ttcount[0] + ttcount[1]);
    double density = (cars.size() / (double)road.length);
    double flux = totavgspeed * density;
    // insert code here...
    std::cout << "Avg Speed : " << totavgspeed << "\n";
    std::cout << "Density   : " << density << "\n";
    std::cout << "Flux      : " << flux << "\n";
    std::cout << "Pass Count: " << Road::switchcnt << "\n";
    //return Result(totavgspeed, density, flux, Road::passcnt);
    double as[2], d[2];
    as[0] = ttspeed[0] / (double)ttcount[0];
    as[1] = ttspeed[1] / (double)ttcount[1];
    d[0] = ttcount[0] / (double)cntiteration / (double)road.length;
    d[1] = ttcount[1] / (double)cntiteration / (double)road.length;
    return Result(as, d, Road::switchtoL, Road::switchtoR);
}

char FILENAME[2048];

int main(int argc, const char * argv[])
{
	int i;
	FILE *fp;
    
    Car::leftpass = false;
    Car::rightpass = false;
    Car::freepass = true;
    Car::blindness = true;
    char *pass = (char *)(Car::leftpass ? "leftpass" : Car::rightpass ? "rightpass" : Car::freepass ? "freepass" : "nopass");
	sprintf(FILENAME, "result NS, %.2lf-%.2lfdensity, %.2lfx%d+%.2lfx%dspeed, %s, %s, driverpos%.2lf.txt", DENSITY_MIN, DENSITY_MAX, pSM1, SPEEDMAX1, 1.0 - pSM1, SPEEDMAX2, pass, Car::blindness ? "blindness" : "no blindness", Car::driverpos);
	printf("outputfile: %s\n", FILENAME);
	
	fp = fopen(FILENAME, "r");
	if (!fp)
	{
		fclose(fp);
		fp = fopen(FILENAME, "w");
		Result::outputHead(fp);
		fclose(fp);
	}
	else
		fclose(fp);
    srand((unsigned)time(0));
    
	for (i = 0; i < 1000; i++)
	{
		printf("Simulation #%d\n", i);
		fp = fopen(FILENAME, "a");
		simulate(DENSITY_MIN + (DENSITY_MAX - DENSITY_MIN) * (rand() / (double)RAND_MAX)).output(fp);
		fclose(fp);
	}
	return 0;
}
