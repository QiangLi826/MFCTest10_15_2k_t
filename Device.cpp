#include "StdAfx.h"
#include "Device.h"
#include "ControlView.cpp"

Device::Device()
{

	for (int i=0;i<SENSOR_COUNT;i++)
	{
		systemSensor[i] = new Go2System;
	}

	this->Xresolution=2.2;
	this->Zresolution=0;
	this->startTimer=0;
	this->finishTimer=0;
	//APP  void *
}
int Device::start=0;
int Device::isStop=0;

void Device::init(bool  save_data_flag,int save_config,int chk1,int chk2,int chk3,int chk4,int xStep,int yStep)
{
	this->save_data_flag=save_data_flag;
	this->save_config=save_config;
	this->chk1=chk1;
	this->chk2=chk2;
	this->chk3=chk3;
	this->chk4=chk4;
	this->xStep=xStep;
	this->yStep=yStep;

}
int Device::status=0;
int Device::startDevices(){
	if (App_Construct() != APP_OK) return status; 
	if (App_DisplayAbout() != APP_OK) return status;  
	if (App_DiscoverAndConnect() != APP_OK) return status;
	if (App_Configure() != APP_OK) return status;
	isStop=0;
	myParam = new MyStoreParam;
	for (int i=0;i<SENSOR_COUNT;i++)
	{
		context[i].full = CreateSemaphore (NULL, 0, 1,NULL);
		//创建缓冲区空闲信号量
		context[i].empty = CreateSemaphore (NULL, 1, 1, NULL);
		context[i].memory[0].count=0;
		context[i].memory[1].count=0;
		context[i].bufIdx=0;
	}
	myParam->context=context;
	myParam->save_data_flag=save_data_flag;
	myParam->xStep=xStep;
	myParam->yStep=yStep;
	mutex = CreateMutex(NULL, FALSE, NULL);
	storeDataThread = AfxBeginThread(&receiveData,myParam);
	return GO2_OK;   

}

int GO2_CALL onData(void* ctx, void* sys, void* data)
{
	int status=0;
	unsigned int i;
	unsigned long long timeStamp;
	unsigned long long frameIndex;
	long long encoderStamp;


	DataContext *context =(DataContext *) ctx;

	// Gocator data messages are always tagged with time and encoder stamps, plus frame index
	// if buffered together with the profile data, this information can be used to match data between sensors, used in resampling strategies etc
	timeStamp = Go2Data_Timestamp(data); 
	encoderStamp = Go2Data_Encoder(data);  
	frameIndex = Go2Data_FrameIndex(data);  

	DataItem  item;
	item.timeStamp=timeStamp;

	for (i = 0; i < Go2Data_ItemCount(data); ++i)
	{
		Go2Data dataItem = Go2Data_ItemAt(data, i); 
		if (Go2Object_Type(dataItem) == GO2_TYPE_PROFILE_DATA)
		{   
			unsigned int profilePointCount = Go2ProfileData_Width(dataItem); 
			unsigned int profileSizeBytes = profilePointCount * sizeof(short int);  
			short* zData = Go2ProfileData_Ranges(dataItem);
			double xRes = Go2ProfileData_XResolution(dataItem);
			double xOff = Go2ProfileData_XOffset(dataItem);
			double zRes = Go2ProfileData_ZResolution(dataItem);
			double zOff = Go2ProfileData_ZOffset(dataItem);
			short* temp=zData;
			item.count=profilePointCount;



			int indx;
			switch (context->serialNumber)
			{
			case DEVICE1:
				indx=0;
				break;
			case DEVICE2:
				indx=1;
				break;
			case DEVICE3:
				indx=2;
				break;
			}

			if (context->scanCount == 0)
			{	
				context->profileWidth=profilePointCount;
				context->xOffset=xOff;
				context->xResolution=xRes;
				context->zOffset=zOff;
				context->zResolution=zRes;

			}


			
			for (unsigned int i=0;i<profilePointCount;i++)
			{
				context->memory[context->bufIdx].data[context->memory[context->bufIdx].count][i]=*zData++;
			}
			
			context->memory[context->bufIdx].count++;
//			item.zValue.clear();

 			context->scanCount++;	
			if (context->memory[context->bufIdx].count==BUFFER_SIZE)
			{
			
				WaitForSingleObject(context->empty,INFINITE);
				context->bufIdx = ++context->bufIdx%2;
				context->memory[context->bufIdx].count=0;
				ReleaseSemaphore(context->full, 1, NULL); 
			}

			temp = NULL;
			delete temp;
			zData =NULL;		

			delete zData;

		}       
	}

	if ((status = Go2Data_Destroy(data)) != GO2_OK)
	{
		printf("Error: Go2Data_Destroy:%d\n", status);
		return GO2_ERROR;
	}
	context=NULL;
	delete context;
	return GO2_OK;
}





int Device::App_Construct()
{

	unsigned int i;

	if (!systemSensor)
	{
		cout<<"Error: Could not allocate application object\n";
		return APP_ERROR; 
	}


	if ((status = Go2Api_Initialize()) != GO2_OK)
	{
		cout<<"Error: Go2Api_Initialize:"<<status;
		return APP_ERROR;
	}


	for (i = 0; i < SENSOR_COUNT; i++)
	{
		if ((status = Go2System_Construct(&systemSensor[i])) != GO2_OK)
		{
			cout<<"Error: Go2System_Construct:"<< status;
			return APP_ERROR;
		}
		context[i].scanCount = 0;
	}


	return APP_OK; 
}
int Device::App_DisplayAbout()
{
	unsigned int version = Go2Api_Version(); 
	cout<<"\n -------------------------------------------------------------\n";

	cout<<"|               Gocator 多传感器应用。                        |"<<endl;

	cout<<" -------------------------------------------------------------\n\n";

	return APP_OK;
}
int Device::App_DiscoverAndConnect()
{

	Go2AddressInfo* addresses; 
	Go2Char text[128]; 
	unsigned int* deviceIds; 
	unsigned int deviceCount, i;
	
	if ((status = Go2System_Discover(&deviceIds, &addresses, &deviceCount)) != GO2_OK)
	{
		cout<<"Error: Go2System_Discover:"<<status<<endl;
		return APP_ERROR;
	}		

	if (deviceCount != SENSOR_COUNT)
	{
		cout<< deviceCount<<" devices detected! This application will only operate with " <<deviceCount<<"sensors."<<SENSOR_COUNT; 
	}
	else
	{
		cout<<SENSOR_COUNT<<" devices were detected!"<<endl; 

		cout<<"the following are the detail information: "<<endl;
		for (i = 0; i < SENSOR_COUNT; i++)
		{
			if ((status = Go2System_Connect(systemSensor[i], addresses[i].address)) != GO2_OK)
			{
				cout<<"Device Id:"<<deviceIds[i]<<"连接超时，正在重试..."<<endl;
				if (status==TIMEOUT)
				{
					while(status==TIMEOUT)
					{
						status=Go2System_Connect(systemSensor[i], addresses[i].address);
					}
				}else{				
					cout<<"Error: Go2System_Connect:"<<status<<endl;
					return APP_ERROR;
					}
			}
			if ((status = Go2IPAddress_Format(addresses[i].address, text, GO2_COUNT(text))) != GO2_OK) 
			{
				cout<<"Error: Go2IPAddress_Format:"<<status<<endl;
				return APP_ERROR;
			}
			cout<<"Device Id:"<<deviceIds[i]<<" connected!"<<" Ip Address:"<<text<<endl;

		}

	}

	if ((status = Go2Free(deviceIds)) != GO2_OK)
	{
		cout<<"Error: Go2Free:"<< status<<endl;
		return APP_ERROR;
	}
	if ((status = Go2Free(addresses)) != GO2_OK)    
	{
		cout<<"Error: Go2Free:"<<status<<endl;
		return APP_ERROR;
	}

	return APP_OK; 

}
int Device::App_Configure()
{
	unsigned int i;
	Go2Output outputModule = NULL; 
	Go2Ethernet ethernet = NULL; 
	Go2Sensor sensor = NULL;

	cout<<"Go2Sensor_SetExposure:"<< APP_MAX_EXPOSURE - APP_EXPOSURE_MARGIN<<endl;
	for (i = 0; i < SENSOR_COUNT; i++)
	{

		if ((status = Go2System_SetMode(systemSensor[i], GO2_MODE_PROFILE_MEASUREMENT)) != GO2_OK)
		{
			cout<<"Error: Go2System_SetMode:"<< status<<endl;
			return status;
		}


		if ((status = Go2System_SetTriggerSource(systemSensor[i], GO2_TRIGGER_SOURCE_TIME)) != GO2_OK) 
		{
			cout<<"Error: Go2System_SetTriggerSource:"<< status<<endl;
			return status;
		}


		if ((status = Go2System_SetFrameRate(systemSensor[i], APP_SENSOR_SPEED)) != GO2_OK) 
		{
			cout<<"Error: Go2System_SetFrameRate:"<<status<<endl;
			return status;
		}

		sensor = Go2System_SensorAt(systemSensor[i], 0);


		if ((status = Go2Sensor_SetExposureMode(sensor, GO2_EXPOSURE_MODE_SINGLE)) != GO2_OK) 
		{
			cout<<"Error: Go2Sensor_SetExposureMode:"<<status<<endl;
			return status;
		}


		if ((status = Go2Sensor_SetExposure(sensor, APP_MAX_EXPOSURE - APP_EXPOSURE_MARGIN)) != GO2_OK) 
		{
			printf("Error: Go2Sensor_SetExposure:%d\n", status);
			return APP_ERROR;
		}

		if ((status = Go2Sensor_SetExposureDelay(sensor, i%2 * APP_MAX_EXPOSURE)) != GO2_OK) 
		{
			cout<<"Error: Go2Sensor_SetExposureDelay:"<< status<<endl;
			return status;
		}
		// for testing purposes, set the exposure delay to 0 on all sensors and compile the code again
		// did the average height result shift due to cross-talk between sensors?
		// if ((status = Go2Sensor_SetExposureDelay(sensor, 0)) != GO2_OK) 

		outputModule = Go2System_Output(systemSensor[i]); 
		ethernet = Go2Output_Ethernet(outputModule); 


		if ((status = Go2Ethernet_ClearSources(ethernet, GO2_OUTPUT_DATA_SOURCE_TYPE_PROFILE)) != GO2_OK)
		{
			cout<<"Error: Go2Ethernet_ClearSources:%d\n"<<status<<endl;
			return status;
		}


		if ((status = Go2Ethernet_AddSource(ethernet, GO2_OUTPUT_DATA_SOURCE_TYPE_PROFILE, GO2_PROFILE_SOURCE_MAIN)) != GO2_OK) 
		{
			cout<<"Error: Go2Ethernet_ClearSources:"<<status<<endl;
			return status;
		}


	}



	return GO2_OK;
}

int Device::ScanData()
{

	Go2Sensor sensor;
	int status;
	unsigned long long currentTime, startTime;
	unsigned int i;
	for (i = 0; i < SENSOR_COUNT; i++)
	{

		sensor = Go2System_SensorAt(systemSensor[i], 0);
		context[i].serialNumber = Go2Sensor_Id(sensor);

		if ((status = Go2System_ConnectData(systemSensor[i], onData, &context[i])) != GO2_OK)
		{
			cout<<"Error: Go2System_ConnectData:"<<status<<endl;
			return status;
		}
	}

	//when starting a multi-sensor system it's a required step to get the current system time from one sensor and then 
	//schedule a synchronized future start for all sensors 
	if ((status = Go2System_GetTime(systemSensor[0], &currentTime)) != GO2_OK)
	{
		cout<<"Error: Go2System_GetTime:"<<status<<endl;
		return status;
	}
	startTime = currentTime + SENSOR_START_DELAY;
	this->startTimer=GetTickCount()+3000;
	for (i = 0; i < SENSOR_COUNT; i++)
	{
		if ((status = Go2System_ScheduledStart(systemSensor[i], startTime, GO2_NULL)) != GO2_OK)
		{
			cout<<"Error: Go2System_SheduledStart:"<<status<<endl;
			return status;
		}
	}
	cout<<"==========================begin receiving data======================="<<endl;
	cout<<"@Scanning in progress...\n@you can click 断开连接 to stop these sensors receiving data!\n"<<endl;
	start=1;
	
	

	return GO2_OK;
}
bool Device::stopDevices()
{
	int status;
	int i;
	// display results
	cout<<endl;
	for (i = 0; i < SENSOR_COUNT; i++)
	{
		cout<<"Sensor Id: "<<context[i].serialNumber<<" ,";
		cout<<"Scan Count = "<<context[i].scanCount<<endl;
	}


	for (i = 0; i < SENSOR_COUNT; i++)
	{
		if ((status = Go2System_Stop(systemSensor[i])) != GO2_OK)
		{
			cout<<"Error: Go2System_Stop:"<<status<<endl;
			return false;
		}
		if ((Go2System_DisconnectData(systemSensor[i])) != GO2_OK)
		{
			cout<<"Error: Go2System_DisconnectData:"<<status<<endl;
			return false;
		}
	}
	this->finishTimer=GetTickCount();
	this->App_Disconnect();
	this->App_Destroy();
	
	return true;

}
bool Device::App_Disconnect()
{

	int status;
	unsigned int i;

	for (i = 0; i < SENSOR_COUNT; i++)
	{
		if ((status = Go2System_Disconnect(systemSensor[i])) != GO2_OK)
		{
			cout<<"Error: Go2System_Disconnect:"<<status<<endl;
			return false;
		}
	}
	cout<<"\nSystem disconnected."<<endl; 
	start=0;

	isStop=1;
	cout<<"| 传感器运行时间："<<(this->finishTimer-this->startTimer)/1000<<"秒"<<endl;
	if(save_config)
	{
		saveConfigInfor();
	}
	return true;
}
void Device::saveConfigInfor(){
	time_t rawtime; 
	time(&rawtime);
	char tmp[64];
	strftime( tmp, sizeof(tmp), "data/config%Y年%m月%d日%H时%M分.txt",localtime(&rawtime) );
	ofstream ofs(tmp);
	ofs<<"——————————本次扫描详情————————"<<endl;
	for (int i = 0; i < SENSOR_COUNT; i++)
	{

		ofs<<"| 设备"<<i+1<<"相关信息如下："<<endl;
		
		ofs<<"| serialNumber:"<<context[i].serialNumber<<endl;
		if (this->chk2)
		{
			ofs<<"| profileWidth:"<<context[i].profileWidth<<endl;
		}

		ofs<<"| xOffset:"<<context[i].xOffset<<endl;

		if (this->chk1)
		{
			ofs<<"| xResolution:"<<context[i].xResolution<<endl;
			ofs<<"| zResolution:"<<context[i].zResolution<<endl;
		}
		
		if (this->chk3)
		{
			ofs<<"| store scanCount:"<<context[i].scanCount-context[i].scanCount%1000<<endl;
		}
		
		ofs<<"|"<<endl<<"|"<<endl;
	}
	if (this->chk4)
	{
		ofs<<"| 传感器运行时间："<<(this->finishTimer-this->startTimer)/1000<<"秒"<<endl;
	}
	ofs<<"————————————————————————"<<endl;

	ofs.close();
}
bool Device::App_Destroy()
{

	int status;
	unsigned int i;

	for (i = 0; i < SENSOR_COUNT; i++)
	{
		if ((status = Go2Object_Destroy(systemSensor[i])) != GO2_OK)
		{
			cout<<"Error: Go2Object_Destroy:"<< status<<endl;
			return false;
		}
	}


	if ((status = Go2Api_Terminate()) != GO2_OK)
	{
		cout<<"Error: Go2Api_Terminate:"<<status<<endl;
		return false;
	}

	return true; 
}

Device::~Device()
{
	for (int i=0;i<SENSOR_COUNT;i++)
	{
		delete [] systemSensor[i];
	}
	/*delete [] context;*/
}