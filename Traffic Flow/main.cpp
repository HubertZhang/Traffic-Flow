#define LENGTH 1000
#define DENSITY 0.1
#define SPEEDMAX 5
#define ITERATION 10000

#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "Car.h"
#include "Road.h"
#include "Cars.h"

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
    //int passTime=0;
    long totaltotalspeed = 0;
    Road::passcnt = 0;
    for (int i = 0; i<ITERATION; i++) {
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
        avgSpeed[i] = (totalSpeed) / (cars.size() + 0.0);
        totaltotalspeed += totalSpeed;
        /*
        road.print();
        road.print(0, stderr);
        int n = 10000000;
        while (--n);
        */
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
    std::cout << "Pass Count: " << Road::passcnt << "\n";
    system("pause");
    return 0;
}
