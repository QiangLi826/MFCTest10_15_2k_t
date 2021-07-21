#pragma once
#include <Go2.h>
#include <Go2Sensor.h>
#include <Go2System.h>
#include <Go2Api.h>
#include <iostream>
#include <queue>
#include <afxmt.h>
#include <vector>
#include <fstream>
#include "constant.h"

using namespace std;
#pragma once

#define SENSOR_COUNT	(3)								// Number of Gocator sensors in system
#define SENSOR_START_DELAY		(SENSOR_COUNT * 1000000)			// 1 second start delay per sensor for the scheduled start to allow for startup overhead in sensors
#define APP_SENSOR_SPEED	(2000)									// 300 Hz target sensor speed
#define  APP_SENSOR_GROUP   (2) 
#define APP_FRAME_PERIOD	(1000000 / APP_SENSOR_SPEED)			// Frame period in micro seconds
#define APP_MAX_EXPOSURE	/*(1000000 / APP_SENSOR_SPEED)*/(APP_FRAME_PERIOD / APP_SENSOR_GROUP)	// Maximum exposure time if each sensor is assumed to be in its own bank	
#define APP_EXPOSURE_MARGIN	(5)									// 10 micro second margin to ensure no sensor cross-talk
enum DEVICEID {DEVICE1=14422,DEVICE2=14424,DEVICE3=14622};

#define APP_ERROR	(0)

#include <Go2.h>



UINT  receiveData(LPVOID lpParamter) ;
extern HANDLE mutex;

class Device
{
public:
	Device();
	void init(bool  save_data_flag,int save_config,int chk1,int chk2,int chk3,int chk4,int xStep,int yStep);

	int startDevices(); 
	int ScanData();
	bool stopDevices();
	double Xresolution;
	double Zresolution;
	double Zoffset;
	static int start;
	static int isStop;
	static const int ConnectError=-1000;
	static const int APP_OK	=1;
	~Device();
private:
	Go2System systemSensor[SENSOR_COUNT];  //Go2System  void *
	DataContext context[SENSOR_COUNT];
	friend int GO2_CALL onData(void* ctx, void* sys, void* data);
	static int status;
	static const int TIMEOUT=-993;
	int startTimer;
	int finishTimer;

	int App_Construct(); 
	int App_DisplayAbout();
	bool App_Destroy(); 

	int App_DiscoverAndConnect();

	bool App_Disconnect();
	int App_Configure();
	
	void saveConfigInfor();

	bool  save_data_flag;
	int save_config;
	int chk1;
	int chk2;
	int chk3;
	int chk4;

	int xStep;
	int yStep;

	MyStoreParam *myParam ;
	CWinThread* storeDataThread ;

};

