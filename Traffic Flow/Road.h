#ifndef ROAD_H
#define ROAD_H

#define MAXWIDTH 4

#include <cstdio>

class Road
{
public:
	static int switchtoL[MAXWIDTH], switchtoR[MAXWIDTH];
	static int switchcnt;
	static bool exits; //whether the freeway has an entrance and an exit
	static int exitbuffer, enterbuffer; //buffer length
	static int offlength; //length outside the freeway
	static int offspeedlimit; //speed limit outside the freeway
	static int speedlimit; //speed limit inside the freeway
	
	Road(int w, int l);
	~Road();
	void registerCars(std::vector<Car *> _cars);
	void calOrder();
	Car *frontCar(int l, int k);
	Car *backCar(int l, int k);
	//void carMove(int sl, int sk, int tl, int tk); //cars with speed 0 also need to call this method. Deprecated
	void carMoveOff(int sl, int sk, int offl, int offk); //cars with speed 0 also need to call this method. This modifies the position of the car
	bool moveOffRoad(int sl, int sk, int offl, int offk = 0);
	void flush(); //must flush after all cars have been moved. This updates the cellular automaton
	void fill0Data();
	void fill0Buffer();
	//void clearData();
	//void clearBuffer();
	int speedLimit(int l, int k);
	Car **operator [](unsigned s);
	bool findRepetition();
	void print(Car ***d = 0, FILE *out = stdout);
	
	int width, length; //width includes exits, length does not include offlength
	//Car *data[width][length];
	Car ***data, ***tdata;
	int **frontpos, **backpos;
	std::vector<Car *> cars, carsbuf;
	
	int lengthOf(int l);
};

#endif
