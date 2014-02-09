#ifndef ROAD_H
#define ROAD_H

#include <cstdio>

class Road
{
public:
	static int passcnt;
	
	Road(int w, int l);
	~Road();
	void registerCars(std::vector<Car *> _cars);
	void calOrder();
	Car *frontCar(int l, int k);
	Car *backCar(int l, int k);
	//void carMove(int sl, int sk, int tl, int tk); //cars with speed 0 also need to call this method. Deprecated
	void carMoveOff(int sl, int sk, int offl, int offk); //cars with speed 0 also need to call this method. This modifies the position of the car
	void flush(); //must flush after all cars have been moved. This updates the cellular automaton
	void fill0Data();
	void fill0Buffer();
	//void clearData();
	//void clearBuffer();
	Car **operator [](unsigned s);
	bool findRepetition();
	void print(Car ***d = 0, FILE *out = stdout);
	
	int width, length;
	//Car *data[width][length];
	Car ***data, ***tdata;
	int **frontpos, **backpos;
	std::vector<Car *> cars, carsbuf;
};

#endif
