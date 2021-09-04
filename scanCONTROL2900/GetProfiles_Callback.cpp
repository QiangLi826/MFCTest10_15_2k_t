//   GetProfiles_Callback.cpp: demo-application for using the LLT.dll
//
//   Version 3.9.0
//
//   Copyright 2019
//
//   MICRO-EPSILON GmbH & Co. KG
//   K鰊igbacher Str. 15
//   94496 Ortenburg
//   Germany
//---------------------------------------------------------------------------

#include "stdafx.h"
#include <iostream>
#include <conio.h>
#include "InterfaceLLT_2.h"
#include "GetProfiles_Callback.h"
#include "scanControlDataTypes.h"
#include <vector>
#include <sstream>
#include<Python.h>
#include "RD_Info.h"

using namespace std;

CInterfaceLLT* m_pLLT = NULL;
unsigned int m_uiResolution = 0;
TScannerType m_tscanCONTROLType = scanCONTROL2xxx;

unsigned int m_uiNeededProfileCount = 10;  //每个空闲队列存储profile数量。
unsigned int m_uiReceivedProfileCount = 0;
unsigned int m_uiProfileDataSize;
HANDLE m_hProfileEvent = CreateEvent(NULL, true, false, "ProfileEvent");

int m_frequency = 200;  //对接线激光时会再初始化一次。

#define PROFILE_BUFFER_SIZE 2

class ProfileDataContext
{
public:
	ProfileDataContext()
	{		
		full = CreateSemaphore (NULL, 0, 1,NULL);
		//创建缓冲区空闲信号量
		empty = CreateSemaphore (NULL, 1, 1, NULL);
		bufIdx=0; 
		count = 0;

	}
	vector<unsigned char> m_vucProfileBuffer[PROFILE_BUFFER_SIZE];
	HANDLE full,empty;
	int count;   //当前空闲队列有多少个profile
	int bufIdx;  //指向空闲队列。
};
ProfileDataContext m_ProfileDataContext;

#define MAX_PROFILE_SAMPLE_LENGTH 100
#include <deque>
std::deque<ProfileSample> m_dqProfileSample; //profile采样后缓存队列
CCriticalSection criticalSectionProfileSample;



std::deque<RD_Info> m_dqRD_Info; //车辙深度缓存队列
CCriticalSection criticalSectionRD_Info;


#define MAX_RD_MEAN_Info	100
std::deque<RD_Mean_Info> m_dqRD_mean_info; //RD 10米平均统计值缓存，用于计算100米平均值和200米平均值。

std::deque<RD_Mean_Info> m_dqRD_show;  //用于在界面上展示RD结果。
CCriticalSection criticalSectionRD_show;


extern void getVelocityByGPS(double& v, bool& isGPSInfoValid);
int g_RDLength = 10; //每次计算rd需要的里程,单位m

//判断计算10m rd的数据是否充足
bool isDataEnough()
{
	double count = 0;
	for (std::deque<RD_Info>::iterator it = m_dqRD_Info.begin(); 
		it != m_dqRD_Info.end(); ++it)
	{
		count += it->distance;
		//保证有g_IRILength米数据
		if (count >= g_RDLength)
			return true;
	}

	//发生这种情况应该是通过gps计算的车速太慢了。
	if (m_dqRD_Info.size() >= MAX_RD_INFO_LENGTH * 0.8)
	{
		return true;
	}

	return false;
}



//计算平均车辙深度线程
UINT calculateMeanRDThread(LPVOID lparam)
{	
	while(1){
		Sleep(50);

		double RD1=0;		
		double RD2=0;	
		criticalSectionRD_Info.Lock();
		int rd_size = m_dqRD_Info.size();
		if (!isDataEnough())
		{
			criticalSectionRD_Info.Unlock();
			continue;
		}


		double total_distance = 0;
		int i = 0;
		for (i; i < rd_size; i++)
		{
			RD1 += m_dqRD_Info.front().RD1;
			RD2 += m_dqRD_Info.front().RD2;
			total_distance += m_dqRD_Info.front().distance;
			m_dqRD_Info.pop_front();
			if (total_distance >= g_RDLength)
			{
				break;
			}			
		}

		RD1 = RD1/(i+1);
		RD2 = RD2/(i+1);
		double v = total_distance * m_frequency / (i + 1 );
		criticalSectionRD_Info.Unlock();



		RD_Mean_Info rd_mean;
		rd_mean.RD1 = RD1;
		rd_mean.RD2 = RD2;
		rd_mean.RD = RD1 >= RD2 ? RD1 : RD2;
		rd_mean.length = total_distance; //默认10米统计一次RD均值。
		rd_mean.v = v * 3600 / 1000;


		if (m_dqRD_mean_info.size() >= MAX_RD_MEAN_Info)
		{
			m_dqRD_mean_info.pop_front();
		}
		m_dqRD_mean_info.push_back(rd_mean);

		double RD_length_total = 0.0;
		double RD_result_total = 0.0;		
		int count = 0;
		for (int i = m_dqRD_mean_info.size() - 1; i >= 0; i--)
		{
			count += 1;
			RD_Mean_Info temp = m_dqRD_mean_info.at(i);
			RD_length_total += temp.length;
			RD_result_total += temp.RD;
		

			// 统计长度大于100米（可以有误差）
			if (rd_mean.RD_result_100m_average <= 0.0 && RD_length_total >= 100 * 0.98)
			{
				rd_mean.RD_result_100m_average = RD_result_total / count;
			}			

			// 统计长度大于200米（可以有误差）
			if (rd_mean.RD_result_200m_average <= 0.0 && RD_length_total >= 200 * 0.98)
			{
				rd_mean.RD_result_200m_average = RD_result_total / count;
				break;
			}
		}


		criticalSectionRD_show.Lock();
		if (m_dqRD_show.size() >= 10)
		{
			m_dqRD_show.pop_front();
		}
		m_dqRD_show.push_back(rd_mean);
		criticalSectionRD_show.Unlock();

	}
	return 0;
}




//计算车辙深度线程
UINT calculateRDThread(LPVOID lparam)
{
	//根据环境设置pythonHome
	Py_SetPythonHome(L"D:\\ProgramData\\Miniconda3_32");
	Py_Initialize();//使用python之前，要调用Py_Initialize();这个函数进行初始化
	PyRun_SimpleString("import sys"); /*调用python文件*/
	PyRun_SimpleString("sys.path.append('./scanCONTROL2900/')");
	PyObject * pModule = NULL;//声明变量
	PyObject * pFunc = NULL;// 声明变量
	pModule = PyImport_ImportModule("chezhe");//这里是要调用的文件名
	pFunc= PyObject_GetAttrString(pModule, "get_RD");//这里是要调用的函数名

	while (1) {		
		criticalSectionProfileSample.Lock();
		int size = m_dqProfileSample.size();
		if (size<=0){		
			criticalSectionProfileSample.Unlock();
			Sleep(10);
			continue;
		}		

	    ProfileSample profileSample = m_dqProfileSample.front();		
		m_dqProfileSample.pop_front();
		criticalSectionProfileSample.Unlock();

		vector<double> porfile = profileSample.vdValueZ;

		//cout <<"porfile.size:" << porfile.size() << "\n";
		//double f[17] = {0, 5, -5, -15, -15, -12.47, -2.48, 7.5, 10, 8, 0, -7.96, -10, -10, -2.48, 5, 0};
		PyObject* pyParams = PyList_New(0);           //初始化一个列表
		for (int i = 0; i < 17; i++)

		{
			PyList_Append(pyParams, Py_BuildValue("d", porfile[i]));//列表添加元素值浮点数
		}

		PyObject* args = PyTuple_New(1);              //定义一个python变量
		PyTuple_SetItem(args, 0, pyParams);			  // 变量格式转换成python格式

		PyObject* pRetVal = PyEval_CallObject(pFunc, args);//调用函数
		//PyArg_Parse取单个返回值
	
		RD_Info rd_info;
		PyArg_ParseTuple(pRetVal,"idd",&rd_info.kind, &rd_info.RD1, &rd_info.RD2);
		//cout <<"chezhe----kind:" << rd_info.kind << " RD1:"<< rd_info.RD1 <<  " RD2:" << rd_info.RD2 << "\n";	

		rd_info.distance = profileSample.distance;
		rd_info.v = profileSample.v;

		criticalSectionRD_Info.Lock();
		if (m_dqRD_Info.size()<MAX_RD_INFO_LENGTH)
		{
			m_dqRD_Info.push_back(rd_info);
		}
		else
		{
			cout <<"m_dqRD_Info is full"  << "\n";
		}
		criticalSectionRD_Info.Unlock();
	}

	Py_Finalize();//调用Py_Finalize，这个根Py_Initialize相对应的。
	return 0;
}

UINT main_scan(LPVOID lpParamter)
{
	//Sleep(2000);
	
	AfxBeginThread(&calculateMeanRDThread, NULL);
	AfxBeginThread(&calculateRDThread, NULL);


	vector<unsigned int> vuiInterfaces(MAX_INTERFACE_COUNT);
	vector<DWORD> vdwResolutions(MAX_RESOULUTIONS);
	unsigned int uiInterfaceCount = 0;
	unsigned int uiExposureTime = 100;
	unsigned int uiIdleTime = 3900;
	bool bLoadError = false;
	int iRetValue = 0;
	bool bOK = true;
	bool bConnected = false;

	// Creating a LLT-object
	// The LLT-Object will load the LLT.dll automaticly and give us a error if ther no LLT.dll
	m_pLLT = new CInterfaceLLT("LLT.dll", &bLoadError);

	if (bLoadError)
	{
		cout << "Error loading LLT.dll \n";

		// Wait for a keyboard hit
		while (!_kbhit()) {}

		// Deletes the LLT-object
		delete m_pLLT;
		return -1;
	}

	// Create a Device
	if (m_pLLT->CreateLLTDevice(INTF_TYPE_ETHERNET))
		cout << "CreateLLTDevice OK \n";
	else
		cout << "Error during CreateLLTDevice\n";

	// Gets the available interfaces from the scanCONTROL-device
	iRetValue = m_pLLT->GetDeviceInterfacesFast(&vuiInterfaces[0], (unsigned int)vuiInterfaces.size());

	if (iRetValue == ERROR_GETDEVINTERFACES_REQUEST_COUNT)
	{
		cout << "There are more or equal than " << vuiInterfaces.size() << " scanCONTROL connected \n";
		uiInterfaceCount = (unsigned int)vuiInterfaces.size();
	}
	else if (iRetValue < 0)
	{
		cout << "A error occured during searching for connected scanCONTROL \n";
		uiInterfaceCount = 0;
	}
	else
	{
		uiInterfaceCount = iRetValue;
	}

	if (uiInterfaceCount == 0)
		cout << "There is no scanCONTROL connected \n";
	else if (uiInterfaceCount == 1)
		cout << "There is 1 scanCONTROL connected \n";
	else
		cout << "There are " << uiInterfaceCount << " scanCONTROL's connected \n";

	if (uiInterfaceCount >= 1)
	{
		cout << "\nSelect the device interface " << vuiInterfaces[0] << "\n";
		if ((iRetValue = m_pLLT->SetDeviceInterface(vuiInterfaces[0], 0)) < GENERAL_FUNCTION_OK)
		{
			OnError("Error during SetDeviceInterface", iRetValue);
			bOK = false;
		}

		if (bOK)
		{
			cout << "Connecting to scanCONTROL\n";
			if ((iRetValue = m_pLLT->Connect()) < GENERAL_FUNCTION_OK)
			{
				OnError("Error during Connect", iRetValue);
				bOK = false;
			}
			else
				bConnected = true;
		}

		if (bOK)
		{
			cout << "Get scanCONTROL type\n";
			if ((iRetValue = m_pLLT->GetLLTType(&m_tscanCONTROLType)) < GENERAL_FUNCTION_OK)
			{
				OnError("Error during GetLLTType", iRetValue);
				bOK = false;
			}

			if (iRetValue == GENERAL_FUNCTION_DEVICE_NAME_NOT_SUPPORTED)
			{
				cout << "Can't decode scanCONTROL type. Please contact Micro-Epsilon for a newer version of the LLT.dll.\n";
			}

            if (m_tscanCONTROLType >= scanCONTROL27xx_25 && m_tscanCONTROLType <= scanCONTROL27xx_xxx)
            {
                cout << "The scanCONTROL is a scanCONTROL27xx\n\n";
            }
			else if (m_tscanCONTROLType >= scanCONTROL25xx_25 && m_tscanCONTROLType <= scanCONTROL25xx_xxx)
			{
				cout << "The scanCONTROL is a scanCONTROL25xx\n\n";
			}
            else if (m_tscanCONTROLType >= scanCONTROL26xx_25 && m_tscanCONTROLType <= scanCONTROL26xx_xxx)
            {
                cout << "The scanCONTROL is a scanCONTROL26xx\n\n";
            } 
            else if (m_tscanCONTROLType >= scanCONTROL29xx_25 && m_tscanCONTROLType <= scanCONTROL29xx_xxx)
            {
                cout << "The scanCONTROL is a scanCONTROL29xx\n\n";
            } 
            else if (m_tscanCONTROLType >= scanCONTROL30xx_25 && m_tscanCONTROLType <= scanCONTROL30xx_xxx)
            {
                cout << "The scanCONTROL is a scanCONTROL30xx\n\n";
            } 
            else
            {
                cout << "The scanCONTROL is a undefined type\nPlease contact Micro-Epsilon for a newer SDK\n\n";
            }

			cout << "Get all possible resolutions\n";
			if ((iRetValue = m_pLLT->GetResolutions(&vdwResolutions[0], (unsigned int)vdwResolutions.size())) < GENERAL_FUNCTION_OK)
			{
				OnError("Error during GetResolutions", iRetValue);
				bOK = false;
			}

			m_uiResolution = vdwResolutions[0];
		}

		if (bOK)
		{
			cout << "Set resolution to " << m_uiResolution << "\n";
			if ((iRetValue = m_pLLT->SetResolution(m_uiResolution)) < GENERAL_FUNCTION_OK)
			{
				OnError("Error during SetResolution", iRetValue);
				bOK = false;
			}


		}

		if (bOK)
		{
			cout << "Set trigger to internal\n";
			if ((iRetValue = m_pLLT->SetFeature(FEATURE_FUNCTION_TRIGGER, TRIG_INTERNAL)) < GENERAL_FUNCTION_OK)
			{
				OnError("Error during SetFeature(FEATURE_FUNCTION_TRIGGER)", iRetValue);
				bOK = false;
			}
		}

		if (bOK)
		{
			cout << "Profile config set to PROFILE\n";
			if ((iRetValue = m_pLLT->SetProfileConfig(PROFILE)) < GENERAL_FUNCTION_OK)
			{
				OnError("Error during SetProfileConfig", iRetValue);
				bOK = false;
			}
		}

		if (bOK)
		{
			cout << "Set shutter time to " << uiExposureTime << "\n";
			if ((iRetValue = m_pLLT->SetFeature(FEATURE_FUNCTION_EXPOSURE_TIME, uiExposureTime)) < GENERAL_FUNCTION_OK)
			{
				OnError("Error during SetFeature(FEATURE_FUNCTION_EXPOSURE_TIME)", iRetValue);
				bOK = false;
			}
		}

		if (bOK)
		{
			// 200HZ
			cout << "Set idle time to " << uiIdleTime << "\n";
			if ((iRetValue = m_pLLT->SetFeature(FEATURE_FUNCTION_IDLE_TIME, uiIdleTime)) < GENERAL_FUNCTION_OK)
			{
				OnError("Error during SetFeature(FEATURE_FUNCTION_IDLE_TIME)", iRetValue);
				bOK = false;
			}

			// uiExposureTime 单位是us
			m_frequency = 1000000 / ((uiExposureTime+uiIdleTime) * 10);
		}

		// Resize the profile buffer to the estimated profile size
		// one porfile : m_uiResolution * 64 		
		for (int i=0 ; i<PROFILE_BUFFER_SIZE; i++)
		{
			m_ProfileDataContext.m_vucProfileBuffer[i].resize(m_uiResolution * 64 * m_uiNeededProfileCount);
		}


		if (bOK)
		{			
			GetProfiles_Callback();
		}

		if (bConnected)
		{
			cout << "Disconnect the scanCONTROL\n";
			if ((iRetValue = m_pLLT->Disconnect()) < GENERAL_FUNCTION_OK)
			{
				OnError("Error during Disconnect", iRetValue);
			}
		}
	}

	// Deletes the LLT-object
	delete m_pLLT;

	CloseHandle(m_hProfileEvent);

	// Wait for a keyboard hit
	while (!_kbhit()) {}

	return 0;
}

void GetProfiles_Callback()
{
	//等一等 calculateRDThread python初始化。
	Sleep(1000);

	int iRetValue;
	vector<double> vdValueX(m_uiResolution); // position
	vector<double> vdValueZ(m_uiResolution); // distance

	// Resets the event
	ResetEvent(m_hProfileEvent);

	cout << "\nDemonstrate the profile transfer via callback function\n";

	cout << "Register the callback\n";
	if ((iRetValue = m_pLLT->RegisterCallback(STD_CALL, (void*)NewProfile, 0)) < GENERAL_FUNCTION_OK)
	{
		OnError("Error during RegisterCallback", iRetValue);
		return;
	}	

	cout << "Enable the measurement\n";
	if ((iRetValue = m_pLLT->TransferProfiles(NORMAL_TRANSFER, true)) < GENERAL_FUNCTION_OK)
	{
		OnError("Error during TransferProfiles", iRetValue);
		return;
	}

	//cout << "Wait for one profile\n";

	//if (WaitForSingleObject(m_hProfileEvent, 1000) != WAIT_OBJECT_0)
	//{
	//	cout << "Error getting profile over the callback \n\n";
	//	return;
	//}

	//cout << "Disable the measurement\n";
	//if ((iRetValue = m_pLLT->TransferProfiles(NORMAL_TRANSFER, false)) < GENERAL_FUNCTION_OK)
	//{
		//OnError("Error during TransferProfiles", iRetValue);
		//return;
	//}


	// Test the size from the profile
	//if (m_uiProfileDataSize == m_uiResolution * 64)
	//	cout << "Profile size is OK \n";
	//else
	//{
	//	cout << "Profile size is wrong \n\n";
	//	return;
	//}

	//cout << m_uiReceivedProfileCount << " profiles have been received\n";

	while(1){
		WaitForSingleObject(m_ProfileDataContext.full, 6000);

		int bufIdx = (m_ProfileDataContext.bufIdx + 1) % 2;

		for (int i = 0; i < m_uiNeededProfileCount; i ++)
		{
			//cout << "Converting of profile data from the first reflection\n";
			iRetValue = m_pLLT->ConvertProfile2Values(&m_ProfileDataContext.m_vucProfileBuffer[bufIdx][i*m_uiResolution * 64]
			, m_uiResolution, PROFILE, m_tscanCONTROLType, 0, true, NULL, NULL, NULL, &vdValueX[0], &vdValueZ[0], NULL, NULL);
			if (((iRetValue & CONVERT_X) == 0) || ((iRetValue & CONVERT_Z) == 0))
			{
				OnError("Error during Converting of profile data", iRetValue);
				continue;
			}

			DisplayProfile(&vdValueX[0], &vdValueZ[0], m_uiResolution);
		}		

		ReleaseSemaphore(m_ProfileDataContext.empty,1,NULL); 
	}
	//cout << "\n\nDisplay the timestamp from the profile:";
	//DisplayTimestamp(&m_vucProfileBuffer[m_uiResolution * 64 - 16]);
}

// Callback function
void __stdcall NewProfile(const unsigned char* pucData, unsigned int uiSize, void* pUserData)
{
	
	//一次应该只传输一个porfile，一个porfile为uiSize
	if (uiSize > 0)
	{
		if (m_ProfileDataContext.count < m_uiNeededProfileCount)
		{
			// If the needed profile count not arrived: copy the new Profile in the buffer and increase the recived buffer count
			m_uiProfileDataSize = uiSize;
			memcpy(&m_ProfileDataContext.m_vucProfileBuffer[m_ProfileDataContext.bufIdx]
			[m_ProfileDataContext.count * uiSize], pucData, uiSize);
			m_ProfileDataContext.count++;
		}

		if (m_ProfileDataContext.count >= m_uiNeededProfileCount)
		{
			// If the needed profile count is arived: set the event
			WaitForSingleObject(m_ProfileDataContext.empty,INFINITE);
			m_ProfileDataContext.bufIdx = ++m_ProfileDataContext.bufIdx%2;
			m_ProfileDataContext.count=0;
			
			ReleaseSemaphore(m_ProfileDataContext.full, 1, NULL); 
		}
	}
	UNREFERENCED_PARAMETER(pUserData);
}

// Displaying the error text
void OnError(const char* szErrorTxt, int iErrorValue)
{
	char acErrorString[200];

	cout << szErrorTxt << "\n";
	if (m_pLLT->TranslateErrorValue(iErrorValue, acErrorString, sizeof(acErrorString)) >= GENERAL_FUNCTION_OK)
		cout << acErrorString << "\n\n";
}

//根据车速计算频率和行车距离
void calculateRDDistance(double&  distance, bool isGPSInfoValid, double& v)
{
	double frequency = m_frequency; //HZ
	
	double vPerSecond;
	if (isGPSInfoValid)
	{
		vPerSecond = v;		
	}
	else
	{
		// GPS无效的情况下，默认行车速度为20km/h
		vPerSecond = 20. * 1000 / 3600; // m/s	
		v = vPerSecond;
	}

	distance = vPerSecond / frequency;
}



// Displays one profile
void DisplayProfile(double* pdValueX, double* pdValueZ, unsigned int uiResolution)
{
	int sampleCount = 17;
	int sampleInterval = (uiResolution-1) / (sampleCount -1);
	ProfileSample profileSample;
	profileSample.vdValueZ.resize(sampleCount);
	int count = 0;
	for (unsigned int i = 0; i < uiResolution; i+=sampleInterval)
	{		
		profileSample.vdValueZ[count] = pdValueZ[i];
		count++;
	}

	bool isGPSInfoValid  = false;
	getVelocityByGPS(profileSample.v, isGPSInfoValid);
	calculateRDDistance(profileSample.distance, isGPSInfoValid, profileSample.v);

	criticalSectionProfileSample.Lock();
	if (m_dqProfileSample.size() < MAX_PROFILE_SAMPLE_LENGTH)
	{
		m_dqProfileSample.push_back(profileSample);
		//cout <<"m_dqProfileSample.size:" << m_dqProfileSample.size() << "\n";
	}
	else
	{
		cout <<"m_dqProfileSample is full"  << "\n";
	}
	
	criticalSectionProfileSample.Unlock();

}

// Displays the timestamp
void DisplayTimestamp(unsigned char* pucTimestamp)
{
	double dShutterOpen, dShutterClose;
	unsigned int uiProfileCount;

	// Decode the timestamp
	m_pLLT->Timestamp2TimeAndCount(pucTimestamp, &dShutterOpen, &dShutterClose, &uiProfileCount);
	cout << "\nShutterOpen: " << dShutterOpen << " ShutterClose: " << dShutterClose << "\n";
	cout << "ProfileCount: " << uiProfileCount << "\n";
	cout << "\n";
}



// Convert a double value to a string
std::string Double2Str(double dValue)
{
	std::ostringstream NewStreamApp;
	NewStreamApp << dValue;

	return NewStreamApp.str();
}
