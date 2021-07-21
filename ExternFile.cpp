#pragma once 
#include "StdAfx.h"
#include <iostream>
#include <queue>
#include <stdlib.h>
#include <fstream>
#include "DataProcess.h"
#include "constant.h"
#include "ControlView.cpp"
using namespace std;
#pragma warning(disable:4006)

queue<vector<short>> Q0,Q1,Q2;

HANDLE mutex;
CCriticalSection criticalSectionA;
CCriticalSection criticalSectionB;
CCriticalSection criticalSectionC;

CCriticalSection criticalSectionOne;
CCriticalSection criticalSectionTwo;

int first=1;

int WholePointCount=0;
int Tri_Row_Index=0;

extern double Triang_Strip[ROW_COUNT][POINT_MAX_WIDTH];
int Strip_Indx=0;
int pc;

void outFromDeviceQueue();
void TrangStrip(double profile[],int pointCount,HWND hWnd);
UINT ProcessData(LPVOID lpParamter)   
{
	MyPARAM * myParam = (MyPARAM *)lpParamter;
	HWND hWnd = myParam->hWnd;
	int show_Type_Index=myParam->show_Type_Index;
	int save_character	= myParam->save_character;
	int feature_threshold = myParam->character_data;
	first=1;
	delete myParam;

	int XTemp=0;
	int x1=MAXDATASHRE,x2=MINDATASHRE,y1=MAXDATASHRE,y2=MINDATASHRE;
	FILE *fp=NULL;
	if (save_character)
	{
		
		time_t rawtime; 
		time(&rawtime);
		char tmp[80];
		strftime( tmp, sizeof(tmp), "data/feature%Y年%m月%d日%H时%M分.txt",localtime(&rawtime) );

		if ((fp=fopen(tmp,"wb"))==NULL)
		{
			AfxMessageBox(_T("数据文件创建失败！"),0,NULL);
		}

	}

	while(!Device::start)
		Sleep(100);
	cout<<"++++++++++++++++++++++++++begin processing +++++++++++++++++++++++"<<endl;
	while(Q0.empty()||Q1.empty()||Q2.empty())
	{
		Sleep(10);	 
	}
	int INDEX=0;

	vector<short> v=Q0.front();


	int width1=v.size();


	v=Q1.front();


	int width2=v.size();

	v=Q2.front();


	int width3=v.size();
	memset(Triang_Strip,0,ROW_COUNT*WholePointCount);
	WholePointCount=width1+width2+width3;
	cout<<"every scan point count: "<<WholePointCount<<endl;

	int i=0;
	DWORD t1,t2;
	
	t1 = GetTickCount();
	int test=0;int test2=0;

	double **profile =new double*[ROW_COUNT];
	for (int i=0;i<ROW_COUNT;i++)
	{
		profile[i]=new double[WholePointCount];
	}
	while (Device::start)
	{

		XTemp=0;
		int l=0;
		DataProcess d;
		
		for (int in=0;in<ROW_COUNT;in++)
		{
			if(!(Q0.empty())&&!(Q1.empty())&&!(Q2.empty()))
			{
				vector<short> v0;
				vector<short> v1 ;
				vector<short> v2 ;
				criticalSectionA.Lock();
				v0 = Q0.front();
				Q0.pop();
				criticalSectionA.Unlock();

				criticalSectionB.Lock();
				v1= Q1.front();
				Q1.pop();
				criticalSectionB.Unlock();


				criticalSectionC.Lock();
				v2= Q2.front();
				Q2.pop();
				criticalSectionC.Unlock();

// 				if (!d.matchPrecise(v0,v1)||!d.matchPrecise(v1,v2))
// 				{
// 					break;
// 				}
				
				pc = v0.size()+v1.size()+v2.size();

				d.mendDataCloud(v0);
				d.mendDataCloud(v1);
				d.mendDataCloud(v2);

				int indx=0;
			
				for (int j=0;j<v0.size();j++)
				{

					profile[in][indx]= v0[j]*0.035;
					

					if (profile[in][indx]>feature_threshold)
					{
						if (indx<x1)
						{
							x1=indx;
							
						}else if (in<y1)
						{
							y1=in;
						}else if (indx>x2)
						{
							x2=indx;
						}else if (in>y2)
						{
							y2=in;
						}
					}


					indx++;
				} 
				v0.clear();
				for (int j=0;j<v1.size();j++)
				{

					profile[in][indx]= v1[j]*0.035;
					if (profile[in][indx]>feature_threshold)
					{
						if (indx<x1)
						{
							x1=indx;

						}else if (in<y1)
						{
							y1=in;
						}else if (indx>x2)
						{
							x2=indx;
						}else if (in>y2)
						{
							y2=in;
						}
					}
					indx++;
				} 
				v1.clear();
				for (int j=0;j<v2.size();j++)
				{

					profile[in][indx]= v2[j]*0.035;
					if (profile[in][indx]>feature_threshold)
					{
						if (indx<x1)
						{
							x1=indx;

						}else if (in<y1)
						{
							y1=in;
						}else if (indx>x2)
						{
							x2=indx;
						}else if (in>y2)
						{
							y2=in;
						}
					}
					indx++;
				} 
				v2.clear();
		}
		
		}
		if (save_character)
		{
			if(x1!=MAXDATASHRE&&y1!=MAXDATASHRE&&x2!=MINDATASHRE&&y2!=MINDATASHRE)
			{
				time_t rawtime; 
				time(&rawtime);
				char tmp[40];
				strftime( tmp, sizeof(tmp), "%Y年%m月%d日%H时%M分%S秒特征:",localtime(&rawtime) );
				fprintf(fp, "%s%d %d %d %d\n", tmp,x1,y1,x2,y2);
				x1=MAXDATASHRE;x2=MINDATASHRE;y1=MAXDATASHRE;y2=MINDATASHRE;
			}
		}
		
		criticalSectionOne.Lock();
		for (int in=0;in<ROW_COUNT;in++)
		{
			for (int j=0;j<pc;j++)
			{
				Triang_Strip[in][j]=profile[in][j];
			}
		}
		criticalSectionOne.Unlock();


	}
	if (fp!=NULL)
	{
		fclose(fp);
	}
	 t2 = GetTickCount();
	 for (int i=0;i<ROW_COUNT;i++)
	 {
		delete [] profile[i];
	 }
	 delete [] profile;
	cout<<"\n now the three queue 1号： "<<Q0.size()<<";   2号： "<<Q1.size()<<" ;    3号："<<Q2.size()<<endl;

	outFromDeviceQueue();
	cout<<"\n now the three queue 1号： "<<Q0.size()<<";   2号： "<<Q1.size()<<" ;    3号："<<Q2.size()<<endl;

	cout<<"++++++++++++++++++++++++++finish processing +++++++++++++++++++++++"<<endl;
	return 0;

}
void outFromDeviceQueue()
{
	
	while((Q0).size()>0)
	{
		Q0.pop();
	}
	while((Q1).size())
	{
		Q1.pop();
	}
	while((Q2).size()>0)
	{
		Q2.pop();
	}
		
	
}
void TrangStrip(double profile[],int pointCount,HWND hWnd){

	criticalSectionOne.Lock();
	for (int i=0;i<pointCount;i++)
	{
		
		Triang_Strip[Strip_Indx][i]=profile[i];
		
	}
	criticalSectionOne.Unlock();
	Strip_Indx++;
	if (Strip_Indx==(ROW_COUNT-1)&&first)
	{
		::PostMessage(hWnd, WM_MY_MESSAGE, (WPARAM)first, NULL); 
		first=0;
	}
	Strip_Indx=Strip_Indx==ROW_COUNT?0:Strip_Indx;
}
short temp1[BUFFER_SIZE][EVERY_SCANLINE_BUFFER_SIZE];
UINT  dataChildTh1(LPVOID lpParamter)  
{
	MyStoreParam * myParam = (MyStoreParam *)lpParamter;
	DataContext * context = myParam->context;
	bool save_flag=myParam->save_data_flag; 
	int STEP_X = myParam->xStep;
	int STEP_Y = myParam->yStep;
	Sleep(2000);
	HWND hWnd = myParam->hWnd;
	
	time_t rawtime; 
	time(&rawtime);
	char tmp[64];
	strftime( tmp, sizeof(tmp), "data/传感器1%Y年%m月%d日%H时%M分%S秒.dat",localtime(&rawtime) );
	FILE *fp=NULL;
	if ((fp=fopen(tmp,"wb"))==NULL)
	{
		AfxMessageBox(_T("数据文件创建失败！"),0,NULL);
	}
	//ofstream ofs("1.txt");
	while (!Device::isStop)
	{
		while (Device::start)
		{

				bool goon=true;

					WaitForSingleObject(context[0].full,WAIT);
					int pointWidth =context[0].profileWidth;
					//cout<<" 50条："	<<context[0].memory[(context[0].bufIdx+1)%2].data[50][80]<<" 500条: "	<<context[0].memory[(context[0].bufIdx+1)%2].data[500][220]<<endl;
					for (int i=0;i<BUFFER_SIZE;i++)
					{
						for (int j=0;j<pointWidth;j++)
						{
							temp1[i][j]= context[0].memory[(context[0].bufIdx+1)%2].data[i][j];

						}

					}
					ReleaseSemaphore(context[0].empty,1,NULL); 
					//cout<<" temp150："<<temp1[50][80]<<" temp1500："<<temp1[500][220];

					short a[SENSOR_ROW_STAY_POINT];

					for (int i=0;i<BUFFER_SIZE;i=i+STEP_Y)
					{
						int index=0;

						for (int j=0;j<pointWidth;j=j+STEP_X)
						{
							a[index++]=temp1[i][j];
						}
						vector<short> v(pointWidth/STEP_X);
						memcpy((short*)&v[0], a, v.size()*2);
						criticalSectionA.Lock();
						Q0.push(v);
						criticalSectionA.Unlock();
					}

					if (save_flag)
					{
						fseek( fp,0,SEEK_END);
						for (int i=0;i<BUFFER_SIZE;i=i++)
						{
							fwrite(temp1[i],sizeof(short),pointWidth,fp);
						}
					

					}
					

			}
	}
	cout<<"队列1剩余数据条数："<<Q0.size()<<endl;
	if (fp!=NULL)
	{
		fclose(fp);
	}
	
	Sleep(2000);
	while((Q0).size()>0)
	{
		Q0.pop();
	}
	cout<<"1处理完毕！";
	return 0;
}
short temp2[BUFFER_SIZE][EVERY_SCANLINE_BUFFER_SIZE];
UINT  dataChildTh2(LPVOID lpParamter)  
{
	MyStoreParam * myParam = (MyStoreParam *)lpParamter;
	DataContext * context = myParam->context;
	bool save_flag=myParam->save_data_flag; 
	int STEP_X = myParam->xStep;
	int STEP_Y = myParam->yStep;
	Sleep(2000);
	time_t rawtime; 
	time(&rawtime);
	char tmp[64];
	strftime( tmp, sizeof(tmp), "data/传感器2%Y年%m月%d日%H时%M分%S秒.dat",localtime(&rawtime) );
	FILE *fp=NULL;
	if ((fp=fopen(tmp,"wb"))==NULL)
	{
		AfxMessageBox(_T("数据文件创建失败！"),0,NULL);
	}
	while (!Device::isStop)
	{
		while(Device::start)
		{
			WaitForSingleObject(context[1].full,WAIT);
			int pointWidth =context[1].profileWidth;
			for (int i=0;i<BUFFER_SIZE;i++)
			{
				for (int j=0;j<pointWidth;j++)
				{
					temp2[i][j]= context[1].memory[(context[1].bufIdx+1)%2].data[i][j];

				}

			}
			ReleaseSemaphore(context[1].empty,1,NULL); 

			short a[SENSOR_ROW_STAY_POINT];
		
			for (int i=0;i<BUFFER_SIZE;i=i+STEP_Y)
			{
				int index=0;
				for (int j=0;j<pointWidth;j=j+STEP_X)
				{
					a[index++]=temp2[i][j];
				}
				vector<short> v(pointWidth/STEP_X);
				memcpy((short*)&v[0], a, v.size()*2);
				criticalSectionB.Lock();
				Q1.push(v);
				criticalSectionB.Unlock();
			}
			if (save_flag)
			{
				fseek( fp,0,SEEK_END);
				for (int i=0;i<BUFFER_SIZE;i=i++)
				{
					fwrite(temp2[i],sizeof(short),pointWidth,fp);
				}

			}
		}
	}
	if (fp!=NULL)
	{
		fclose(fp);
	}
/*	cout<<"队列2剩余数据条数："<<Q1.size()<<endl;*/
	Sleep(2000);
	while((Q1).size()>0)
	{
		Q1.pop();
	}
	cout<<"2处理完毕！";
	return 0;
}
short temp3[BUFFER_SIZE][EVERY_SCANLINE_BUFFER_SIZE];
UINT  dataChildTh3(LPVOID lpParamter)  
{

	MyStoreParam * myParam = (MyStoreParam *)lpParamter;
	DataContext * context = myParam->context;
	bool save_flag=myParam->save_data_flag; 
	int STEP_X = myParam->xStep;
	int STEP_Y = myParam->yStep;
	Sleep(2000);
	time_t rawtime; 
	time(&rawtime);
	char tmp[64];
	strftime( tmp, sizeof(tmp), "data/传感器3%Y年%m月%d日%H时%M分%S秒.dat",localtime(&rawtime) );
	FILE *fp=NULL;
	if ((fp=fopen(tmp,"wb"))==NULL)
	{
		AfxMessageBox(_T("数据文件创建失败！"),0,NULL);
	}
	while (!Device::isStop)
	{
		while(Device::start)
		{
			WaitForSingleObject(context[2].full,WAIT);
			int pointWidth =context[2].profileWidth;
			for (int i=0;i<BUFFER_SIZE;i++)
			{
				for (int j=0;j<pointWidth;j++)
				{
					temp3[i][j]= context[2].memory[(context[2].bufIdx+1)%2].data[i][j];

				}

			}
			ReleaseSemaphore(context[2].empty,1,NULL); 
			short a[SENSOR_ROW_STAY_POINT];	
			for (int i=0;i<BUFFER_SIZE;i=i+STEP_Y)
			{
				int index=0;
				for (int j=0;j<pointWidth;j=j+STEP_X)
				{
					a[index++]=temp3[i][j];
				}
				vector<short> v(pointWidth/STEP_X);
				memcpy((short*)&v[0], a, v.size()*2);
				criticalSectionC.Lock();
				Q2.push(v);
				criticalSectionC.Unlock();
			}
			if (save_flag)
			{
				fseek( fp,0,SEEK_END);
				for (int i=0;i<BUFFER_SIZE;i=i++)
				{
					fwrite(temp3[i],sizeof(short),pointWidth,fp);
				}


			}

		}
	}
	if (fp!=NULL)
	{
		fclose(fp);
	}
/*	cout<<"队列3剩余数据条数："<<Q2.size()<<endl;*/
	Sleep(2000);
	while((Q2).size()>0)
	{
		Q2.pop();
	}
	cout<<"3处理完毕！";
	return 0;
}
UINT  receiveData(LPVOID lpParamter)   
{ 
	
	AfxBeginThread(&dataChildTh1,lpParamter);
	AfxBeginThread(&dataChildTh2,lpParamter);
	AfxBeginThread(&dataChildTh3,lpParamter);

	return 0;

}

