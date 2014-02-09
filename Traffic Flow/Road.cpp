#include <iostream>
#include <cstdlib>
#include <vector>

#include "Car.h"
#include "Road.h"

int Road::switchtoL[MAXWIDTH] = {0};
int Road::switchtoR[MAXWIDTH] = {0};
int Road::switchcnt = 0;
bool Road::exits = false;
int Road::exitbuffer = 45, Road::enterbuffer = 81;
int Road::offlength = 200; //length outside the freeway
int Road::offspeedlimit = 11; //speed limit outside the freeway
int Road::speedlimit = 22; //speed limit inside the freeway

Road::Road(int w, int l)
{
	width = w;
	length = l;
	if (exits)
	{
		width++;
		length += offlength;
	}
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
		
	length = l;
		
	fill0Data();
	fill0Buffer();
	calOrder();
	std::cout << "order init finished\n";
	carsbuf.clear();
}
Road::~Road()
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
	
	for (i = 0; i < carsbuf.size(); i++)
		delete carsbuf[i];
}
void Road::registerCars(std::vector<Car *> _cars)
{
	cars = _cars;
	fill0Data();
	fill0Buffer();
	int i;
	for (i = 0; i < cars.size(); i++)
		data[cars[i]->lane][cars[i]->place] = cars[i];
	carsbuf.clear();
	for (i = 0; i < cars.size(); i++)
		carsbuf.push_back(new Car(*cars[i]));
}
		
void Road::calOrder()
{
	int l, k, sk, pk;
	//std::cout << "calOrder start, width = " << width << ", length = " << length << "\n";
	for (l = 0; l < width; l++)
	{
		int len = lengthOf(l);
		for (sk = 0; sk < len && data[l][sk] == 0; sk++);
		//std::cout << "calOrder iteration " << l << ", sk = " << sk << "\n";
		if (sk >= len) //no vehicles on the lane
		{
			for (k = 0; k < len; k++)
				frontpos[l][k] = -1;
			for (k = 0; k < len; k++)
				backpos[l][k] = -1;
		}
		else
		{
			pk = sk;
			for (k = (sk + 1) % len; k != sk; k = (k + 1) % len)
			{
				backpos[l][k] = pk;
				if (data[l][k])
					pk = k;
			}
			backpos[l][k] = pk;
			if (data[l][k])
				pk = k;

			pk = sk;
			for (k = (sk - 1 + len) % len; k != sk; k = (k - 1 + len) % len)
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
		for (k = 0; k < len; k++)
			std::cout << "frontpos[" << l << "][" << k << "] = " << frontpos[l][k] << "\n";
		for (k = 0; k < len; k++)
			std::cout << "backpos[" << l << "][" << k << "] = " << backpos[l][k] << "\n";
		*/
	}
}
Car *Road::frontCar(int l, int k)
{
	if (frontpos[l][k] == -1)
		return 0;
	return data[l][frontpos[l][k]];
}
Car *Road::backCar(int l, int k)
{
	if (backpos[l][k] == -1)
		return 0;
	return data[l][backpos[l][k]];
}
/*
void Road::carMove(int sl, int sk, int tl, int tk) //cars with speed 0 also need to call this method
{
	if (sl != tl)
		passcnt++;
	tdata[tl][tk] = data[sl][sk];
	data[sl][sk] = data[sl][sk]->duplicate();
}
*/
void Road::carMoveOff(int sl, int sk, int offl, int offk) //this modifies the info of the car
{
	Car *c = data[sl][sk];
	//printf("Car move\n");
	if (!c)
	{
		printf("Moving null pointer!\n");
		int j = 0;
		int i = 1 / j;
		switchcnt += i;
		return;
	}
	if (moveOffRoad(sl, sk, offl, offk))
	{
		printf("Moving off road!\n");
		int j = 0;
		int i = 1 / j;
		switchcnt += i;
		return;
	}
	if (offl != 0)
	{
		if (offl == 1)
			switchtoR[sl]++;
		if (offl == -1)
			switchtoL[sl]++;
		switchcnt++;
	}
	int tl = sl + offl;
	int tk = (sk + offk) % lengthOf(tl);
	if (tdata[tl][tk])
	{
		printf("Car crash or buffer not cleared!\n");
		int j = 0;
		int i = 1 / j;
		switchcnt += i;
	}
	tdata[tl][tk] = c;
	//data[sl][sk] = c->duplicate();
	carsbuf[c->id]->copy(*c);
	data[sl][sk] = carsbuf[c->id];
	c->lane = tl;
	c->place = tk;
}
bool Road::moveOffRoad(int sl, int sk, int offl, int offk) //offk does not matter
{
	return (sl + offl < 0 || sl + offl >= width ||
		(Road::exits && sk >= length && sl == width - 1 && offl < 0));
}
void Road::flush() //must flush after all cars have been moved. This updates the cellular automaton
{
	Car ***ttdata;
	ttdata = tdata;
	tdata = data;
	data = ttdata;
	/*
	if (findRepetition())
	{
		print();
		printf("previous state:\n");
		print(tdata);
		exit(0);
		while (1);
	}*/
	//clearBuffer();
	fill0Buffer();
}
void Road::fill0Data()
{
	int i, j;
	for (i = 0; i < width; i++)
		for (j = 0; j < lengthOf(i); j++)
		{
			data[i][j] = 0;
		}
}
void Road::fill0Buffer()
{
	int i, j;
	for (i = 0; i < width; i++)
		for (j = 0; j < lengthOf(i); j++)
		{
			tdata[i][j] = 0;
		}
}
int Road::speedLimit(int l, int k)
{
	if (exits)
	{
		if (l == width - 1 && k >= length - exitbuffer)
			return offspeedlimit;
	}
	return speedlimit;
}
Car **Road::operator [](unsigned s)
{
	return data[s];
}
bool Road::findRepetition()
{
	int l, k, l2, k2;
	for (l = 0; l < width; l++)
		for (k = 0; k < lengthOf(l); k++)
			for (l2 = 0; l2 < width; l2++)
				for (k2 = 0; k2 < lengthOf(l2); k2++)
					if ((l != l2 || k != k2) && data[l][k] == data[l2][k2] && data[l][k])
						return true;
	return false;
}
void Road::print(Car ***d, FILE *out)
{
	if (d == 0)
		d = data;
	int l, k;
	for (l = 0; l < width; l++)
	{
		for (k = 0; k < lengthOf(l); k++)
			fputc(d[l][k] ? ((long)d[l][k] % 23) + 'A' : '.', out);
		fprintf(out, "\n");
	}
	fprintf(out, "\n");
}

int Road::lengthOf(int l)
{
	if (exits)
	{
		if (l == width - 1)
			return length + offlength;
	}
	return length;
}
