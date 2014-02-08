#include <iostream>
#include <cstdlib>

#include "Car.h"
#include "Road.h"

int Road::passcnt = 0;

Road::Road(int w, int l)
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
}
void Road::calOrder()
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
void Road::carMove(int sl, int sk, int tl, int tk) //cars with speed 0 also need to call this method
{
	if (sl != tl)
		passcnt++;
	tdata[tl][tk] = data[sl][sk];
	data[sl][sk] = data[sl][sk]->duplicate();
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
	clearBuffer();
}
void Road::fill0Data()
{
	int i, j;
	for (i = 0; i < width; i++)
		for (j = 0; j < length; j++)
		{
			data[i][j] = 0;
		}
}
void Road::fill0Buffer()
{
	int i, j;
	for (i = 0; i < width; i++)
		for (j = 0; j < length; j++)
		{
			tdata[i][j] = 0;
		}
}
void Road::clearData()
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
void Road::clearBuffer()
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
Car **Road::operator [](unsigned s)
{
	return data[s];
}
bool Road::findRepetition()
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
void Road::print(Car ***d, FILE *out)
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