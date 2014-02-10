#define WIDTH 2
#define LENGTH 3000
#define DENSITY_MIN 0.01
#define DENSITY_MAX 0.8
#define ITERATION 10000
#define OMIT_ITERATION 1000
//#define pWWH 0.5
#define SPEEDMAX1 22
#define pSM1 0.5
#define SPEEDMAX2 15
//#define pEXIT 0.2

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
	double sumdexit;
	int switchtoL[MAXWIDTH], switchtoR[MAXWIDTH];
	double paras, sumd, sumf;
	int switchcnt;
	int brake[MAXBRAKE];
	int suddenbrake, weightedsuddenbrake;
	int missexit;
	Result(int wid, double *as, double *d, double de, int *stl, int *str, int *brk, int sbrk, int wsbrk, int me)
	{
		width = wid;
		memcpy(avgspeed, as, sizeof(avgspeed[0]) * width);
		memcpy(density, d, sizeof(density[0]) * width);
		sumf = 0;
		sumd = 0;
		for (int i = 0; i < width; i++)
		{
			flux[i] = density[i] == 0.0 ? 0.0 : avgspeed[i] * density[i];
			sumf += flux[i];
			sumd += density[i];
		}
		sumdexit = de;
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
		weightedsuddenbrake = wsbrk;
		missexit = me;
	}
	static void outputHead(int width, FILE *fp)
	{
		for (int i = 0; i < width; i++)
			fprintf(fp, "avgspeed%d\tdensity%d\tflux%d\tstl%d\tstr%d\t", i, i, i, i, i);
		fprintf(fp, "par avgspeed\tsum density\tpercent exit\tsum flux\tswitchcnt\tsbrkcnt\twsbrkcnt\tmissexit\n");
	}
	void output(FILE *fp)
	{
	    std::cout << "Avg Speed : " << paras << "\n";
	    std::cout << "Density   : " << sumd << "\n";
	    std::cout << "Flux      : " << sumf << "\n";
	    std::cout << "Pass Count: " << switchcnt << "\n";
		for (int i = 0; i < width; i++)
			fprintf(fp, "%lf\t%lf\t%lf\t%d\t%d\t", avgspeed[i], density[i], flux[i], switchtoL[i], switchtoR[i]);
		fprintf(fp, "%lf\t%lf\t%lf\t%lf\t%d\t%d\t%d\t%d\n", paras, sumd, sumdexit, sumf, switchcnt, suddenbrake, weightedsuddenbrake, missexit);
	}
};

//float avgSpeed[ITERATION]={0};
double pEXIT = 0.2;
Result simulate(int width, double expdensity)
{
	Road road(width, LENGTH);
    std::cout << "Hello, World!\n";
    //srand(0);
	std::vector<Car *> cars;
	if (Road::exits) ///add blocks
	{
		int l = width;
		for (int k = Road::enterbuffer; k < road.length - Road::exitbuffer/*length + offlength*/; k++)
		{
			Car *c = new Block(cars.size(), &road, l, k);
			cars.push_back(c);
		}
	}
	int exitcnt = 0, carcnt = 0;
    for (int i = 0; i < road.length; i++) {
		for (int l = 0; l < width; l++) {
		
	        if (rand() < RAND_MAX * expdensity / width) {
				NS *c;
				int maxspeed = (rand() < RAND_MAX * pSM1) ? SPEEDMAX1 : SPEEDMAX2;
				c = new NS(cars.size(), &road, l, i, maxspeed);
				if (rand() < RAND_MAX * pEXIT)
				{
					c->setToExit(true);
					exitcnt++;
				}
				carcnt++;
	            //road[l][i] = c;
				cars.push_back(c);
	        }
		}
	}
	double pe = exitcnt / (double)carcnt;
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
    Car::weightedsuddenbrake = 0;
    Car::missexit = 0;
    
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
        
        if (i % (ITERATION / 10) == 0)
        	printf("iteration %d%%\n", i * 100 / ITERATION);
        
    }
    for (int i = 0; i < cars.size(); i++)
    	delete cars[i];
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
    //int wid = Road::exits ? width + 1 : width;
    int wid = width;
    for (int i = 0; i < wid; i++)
    {
		double rl = (double)road.length;
		if (Road::exits && i == width)
			rl = (double)(road.offlength + road.exitbuffer + road.enterbuffer);
		as[i] = ttspeed[i] / (double)ttcount[i];
	    d[i] = ttcount[i] / (double)cntiteration / rl;
	}
    return Result(wid, as, d, pe, Road::switchtoL, Road::switchtoR, Car::brake, Car::suddenbrake, Car::weightedsuddenbrake, Car::missexit);
}

char FILENAME[2048];

void batch(double pe, bool cl, bool cr, bool cf, bool cb, bool cp, bool ci)
{
	int i;
	FILE *fp;
    
    if (ci == true)
    {
		cb = false;
    	cp = false;
	}
    
    pEXIT = pe;
    Road::exits = (pe != 0.0);
    Car::leftpass = cl;
    Car::rightpass = cr;
    Car::freepass = cf;
    Car::blindness = cb;
    Car::perceiveddis = cp;
    Car::intelligent = ci;
    
    char *pass = (char *)(Car::leftpass ? "leftpass" : Car::rightpass ? "rightpass" : Car::freepass ? "freepass" : "nopass");
    char *blnd = (char *)(Car::blindness ? "blindness" : "no blindness");
    char *perc = (char *)(Car::perceiveddis ? "perceiveddis" : "realdis");
	sprintf(FILENAME, "result NS, %.2lf-%.2lfdensity, %.2lfx%d+%.2lfx%dspeed, %.2lfpEXIT, %s, %s, %s, driverpos%.2lf.txt",
		DENSITY_MIN, DENSITY_MAX, pSM1, SPEEDMAX1, 1.0 - pSM1, SPEEDMAX2, Road::exits ? pEXIT : 0.0, pass, blnd, perc, Car::driverpos);
	printf("outputfile: %s\n", FILENAME);
	
	fp = fopen(FILENAME, "r");
	if (!fp)
	{
		fclose(fp);
		fp = fopen(FILENAME, "w");
		Result::outputHead(WIDTH, fp);//(WIDTH + (Road::exits ? 1 : 0), fp);
		fclose(fp);
	}
	else
		fclose(fp);
    srand((unsigned)time(0));
    
	for (i = 0; i < 3; i++)
	{
		printf("Simulation #%d\n", i);
		fp = fopen(FILENAME, "a");
		simulate(WIDTH, DENSITY_MIN + (DENSITY_MAX - DENSITY_MIN) * (rand() / (double)RAND_MAX)).output(fp);
		fclose(fp);
	}
	return;
}

int main(int argc, const char * argv[])
{
	int i;
	for (i = 0; i < 1000; i++)
	{
		batch(0.2, true, false, false, true, true, false);
		batch(0.2, false, true, false, true, true, false);
		batch(0.2, false, false, true, true, true, false);
	}
	return 0;
}
