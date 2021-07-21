#pragma once
#include <iostream>
#include <fstream>
#include <map>
#include <math.h>
#include "Device.h"

#include <vector>



#include "constant.h"
using namespace std;

#pragma once



class DataProcess
{
public:
	DataProcess(void);
	void getProfileDataValues(Device &d ,double *profile, int profileCount);
	
	bool matchPrecise(vector<short> & preDataItem,vector<short> & curDataItem);

	void mendDataCloud(vector<short> & dataItem);
	void simplify(vector<short> & dataItem,int pointWithStep);

	~DataProcess(void);
private:

	

	void findHole();
	void mendPerHole(int l,int r);
	
	vector<short> zValue;
	double INVALID_RANGE_16BIT;
	double zResolution;
	double xResolution;

	int count;
	
	map<int,int> hole;
	//¶´±ê¼Ç
	int l,r;

};

