#include "StdAfx.h"
#include "DataProcess.h"


DataProcess::DataProcess(void)
{
	this->zResolution=0.035;
	this->xResolution=1.93;
	INVALID_RANGE_16BIT= 0x8000;
	
	int count=0;
	l=-1;
	r=-1;
}
DataProcess::~DataProcess(void)
{
}


void DataProcess::simplify(vector<short> &dataItem,int pointWithStep)
{

	int j=0;
	this->count=dataItem.size();
	for(int i=0;i<this->count;i++)
	{
		if (i%pointWithStep==0)
		{
			dataItem[j++]=dataItem[i];
		}

	}
	
}

bool DataProcess::matchPrecise(vector<short> & preDataItem,vector<short> &curDataItem){
	
	
	int overlap=0;
	
	int preCount = preDataItem.size();
	int curCount = curDataItem.size();
	int tag1=0,tag2=0;
	
	double overlapDistence =(this->xResolution*5*(preCount)/2)+(this->xResolution*5*curCount/2)-INPUT_SENSOR_DISTANCE;

	overlap=(int)(overlapDistence/this->xResolution/5);
	int i=0,j=0;
	for( i=preCount-1,j=0;j<curCount&&i>0;i--,j++)
	{
		if (preDataItem[i]!=-INVALID_RANGE_16BIT&&tag1==0)
		{
			tag1=preCount-(i+1);  //左传感器的尾部有tag1个点
		}
 		if (curDataItem[j]!=-INVALID_RANGE_16BIT&&tag2==0)
		{
			tag2=j;//有tag2个点
		}
		if (tag1!=0&&tag2!=0)
		{
			break;
		}

	}
	if (tag1<overlap&&tag2<overlap)
	{
		int index = preCount- fabs((double)overlap -tag2);//左边传感器有效点个数index
		i=--index;//左边传感器重叠点 索引开始位置
		j=tag2;  //后面右面传感器重叠点开始位置索引开始位置
		int olRIndex =preCount-tag1-1;  //重叠结束索引位置
		for ( ;i<=olRIndex;i++,j++)
		{
			preDataItem[index]=curDataItem[j]>preDataItem[i]?curDataItem[j]:preDataItem[i];
			index++;
		}
		vector<short>::iterator it= curDataItem.begin();
		
		it = curDataItem.erase(it,it+j);
		
		vector<short>::iterator beg=preDataItem.begin();
		vector<short>::iterator end=preDataItem.end();

		it = preDataItem.erase(beg+index,end);
		
	}
	else 
	{
		vector<short>::iterator it=curDataItem.begin();
		for (int l=0;l<tag2;l++)
		{
			it = curDataItem.erase(it);
		}
		it=preDataItem.begin()+preCount-tag1;
		for (int l=0;l<tag1;l++)
		{
			it = preDataItem.erase(it);
		}
		
	}

	
	return true;
}




void DataProcess::findHole()
{

	if (count>0)
	{
		for (int i=0;i<count;i++)
		{

			if ((zValue[i]==-INVALID_RANGE_16BIT)&&l==-1)
			{
				l=i;
			}else if (zValue[i]!=-INVALID_RANGE_16BIT&&l!=-1)
			{
				r=i-1;
				hole[l]=r;


				l=-1;
			}
		}
		if (zValue[count-1]==-INVALID_RANGE_16BIT)
		{
			hole[l]=count-1;

		}

		l=-1;r=-1;
	}
	


}
void DataProcess::mendDataCloud(vector<short> &dataItem)
{

	this->count = dataItem.size();

	vector<short> zValue = dataItem;

	this->zValue=zValue;
	

	this->findHole();
	map<int,int>::iterator it;
	if (!hole.empty())
	{
		for (it= hole.begin();it!=hole.end();it++)
		{

			this->mendPerHole(it->first,it->second);
		}
		hole.clear();
	}

	dataItem=this->zValue;
	this->zValue.clear();
}
void DataProcess::mendPerHole(int l,int r)
{
	if (l==0)
	{
		for (int i=l;i<=r;i++)
		{
			zValue[i]=zValue[r+1];
		}
	}else if (r==zValue.size()-1)
	{
		for (int i=l;i<=r;i++)
		{
			zValue[i]=zValue[l-1];
		}
	}else
	{
		if (l==r)
		{
			zValue[l]=zValue[l-1];
		}else
		{
			int k=(zValue[l-1]-zValue[r+1])/(l-r);
			int b=(l*zValue[r+1]-r*zValue[l-1])/(l-r);
			for (int i=l;i<=r;i++)
			{

				zValue[i]=k*i+b;
			}
		}
		
	}
	
}

// void DataProcess::getProfileDataValues(Device &d ,double *profile,int ItemCount){
// 	this->mendDataCloud();
// 	
// 	for (int i=0;i<ItemCount;i++){
// 		double blockId=(i+0.0)/(ItemCount-1);
// 		int dataXNumber= blockId*(d.index-1);
// 		double zValue = Device::matchedData[dataXNumber];
// 		profile[i]=zValue;
// 	}
// 
// 
// 	
// }