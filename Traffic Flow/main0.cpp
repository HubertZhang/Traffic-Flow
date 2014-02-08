//
//  main.cpp
//  Traffic Flow
//
//  Created by Hubert on 14-2-7.
//  Copyright (c) 2014年 Geek20. All rights reserved.
//

#include <iostream>
#include <ctime>
#define LENGTH 1000
#define DENSITY 0.4
#define SPEEDMAX 5
#define max(a, b) (a>b)? a:b
#define min(a, b) (a<b)? a:b
class Car
{
    int lane;
    int place;//
    int speed;
    //int type;
    Car* front[2];
    Car* back[2];
    friend class car;
    
public:
    static int totalSpeed;
    Car(int lane,int place){
        this->lane=lane;
        this->place=place;
    }
    void Motion()
    {
        //Speed up
        speed = max(SPEEDMAX,speed+1);
        //Determined speed down
        int distanceThisLane = (front[lane]->place-this->place+LENGTH)%LENGTH;
        int distanceOtherLane =(front[!lane]->place-this->place+LENGTH)%LENGTH;
        int distanceSafe=(back[!lane]->place-this->place+LENGTH)%LENGTH;
        
        speed = min(distanceThisLane-1,speed);
        //Undetermined speed down
        if(rand()%2)
        {
            speed = max(speed-1, 0);
        }
        //Move
        totalSpeed+=speed;
        place+=speed;
    }
};
#define ITERATION 10000

int main(int argc, const char * argv[])
{
    float avgSpeed[ITERATION]={0};
    int placeData[2][LENGTH]={0};
    //srand(0);
    srand((unsigned)time(0));
    int amount = 0;
    for (int i = 0; i<LENGTH; i++) {
        if (rand()<RAND_MAX*DENSITY) {
            amount++;
            placeData[0][i] =amount;
            
        }
        if (rand()<RAND_MAX*DENSITY) {
            amount++;
            placeData[1][i] =amount;
        }
    }
    int* speedData = new int[amount];
    //int speedData[20];
    int flow=0;
    int passTime=0;
    for (int i = 0; i<ITERATION; i++) {
        int newPlace[2][LENGTH]={0};
        int totalSpeed=0;
        int lastPosition[2];
        for (int k = 0; k<2; k++) {
            int temp=LENGTH-1;
            while (!placeData[k][temp]) {
                temp--;
            }
            lastPosition[k]=temp;
        }
        for (int j = 0;j<LENGTH; j++ ) {
            for (int k = 0; k<2; k++) if (placeData[k][j]) lastPosition[k]=j;
            for (int k = 0; k<2; k++) {
                if (placeData[k][j]) {
                    int index =placeData[k][j];
                    int speed = speedData[index];
                    totalSpeed+=speed;
                    speed = min(speed+1,SPEEDMAX);
                    int distanceThisLane=SPEEDMAX;
                    int distanceOtherLane=SPEEDMAX;
                    int distanceSafe=(j-lastPosition[!k]+LENGTH)%LENGTH;
                    for(int k1=SPEEDMAX;k1>0;k1--)
                    {
                        if (placeData[k][(j+k1)%LENGTH]) {
                            distanceThisLane = k1-1;
                        }
                        if (placeData[!k][(j+k1)%LENGTH]) {
                            distanceOtherLane = k1-1;
                        }
                    }
                    bool pass = false;
                    if (distanceThisLane>=speed) {
                        
                    }
                    else if (distanceSafe>=SPEEDMAX&&distanceOtherLane>distanceThisLane) {
                            speed = min(distanceOtherLane, speed);
                            pass = true;
                            passTime++;
                    }
                    else
                    {
                        speed =distanceThisLane;
                    }
                    speedData[index]=speed;
                    if (j+speed>LENGTH) {
                        flow++;
                    }
                    if(pass)
                    {
                        newPlace[!k][(j+speed)%LENGTH]=index;
                    }
                    else newPlace[k][(j+speed)%LENGTH]=index;
                }
            }
        }
        memcpy(placeData, newPlace, LENGTH*2*sizeof(int));
        avgSpeed[i] = (totalSpeed)/(amount+0.0);
    }
    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}

