#pragma once

#include <iostream>
#include <deque>
#include <vector>
using namespace std;

class DataItem
{
public:
	long long timeStamp;
	int count;
	vector<double> zValue;
};

class Buffer{
public:
	void setDataBuf(short **p){
		data=p;
	}
	short int **data;
	int count;
};

class DataContext
{
public:
	DataContext(){
		serialNumber=0;
		scanCount=0;		
		zResolution=0;
		zOffset=0;
		xResolution=0;
		xOffset=0;
		profileWidth=0;
		a =new short*[2000];
		for (int i=0;i<2000;i++)
		{
			a[i]=new short[600];
		}
		memory[0].setDataBuf(a);
		b =new short*[2000];
		for (int i=0;i<2000;i++)
		{
			b[i]=new short[600];
		}
		memory[1].setDataBuf(b);
	}
	unsigned int serialNumber;
	unsigned long scanCount;
	long long timeStamp;
	Buffer memory[2];
	unsigned int profileWidth;
	double xResolution;
	double zResolution;
	double xOffset;
	double zOffset;

	HANDLE full,empty;
	int bufIdx;
private:
	short int **b;
	short int **a;
};