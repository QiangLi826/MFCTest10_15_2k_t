#include "stdafx.h"
#include "GPS_Info.h"
#include <cmath>

GPS_Info::GPS_Info()
{
}

GPS_Info::GPS_Info(CString tmp)
{
	CStringArray gpsStrArray;
	GPS_Info::SplitStr(tmp, ",", gpsStrArray);
	if (gpsStrArray.GetSize() != 17){
		isGPSInfoValid = false;
		isGPSFormatValid = false;
		x=0;
		y=0;
		z=0;
		time=0;
		return;
	}
	isGPSFormatValid = true;
		//无效解
	if (gpsStrArray.GetAt(15)=='0' ){
		isGPSInfoValid = false;
		return;
	}
	
	x = atof(gpsStrArray.GetAt(12));
	y = atof(gpsStrArray.GetAt(13));
	z = atof(gpsStrArray.GetAt(14));

	//utc时间转换成秒
	CString utc = gpsStrArray.GetAt(4);
	CString h;
	CString m;
	CString s;

	// 62925.00
	if(utc.GetLength()==8)
	{
		h = utc.Left(1);
		m = utc.Mid(1,2);
		s = utc.Right(5);
	}
	// 162925.00
	else
	{
		h = utc.Left(2);
		m = utc.Mid(2,2);
		s = utc.Right(5);
	}	

	time = atof(h)*3600 + atof(m)*60 + atof(s);
	isGPSInfoValid = true;
}

GPS_Info::~GPS_Info()
{
}

void GPS_Info::SplitStr(CString strSrc, CString strGap, CStringArray &strResult)
{
 int nPos = strSrc.Find(strGap);
 CString strLeft = _T("");

 while (0 <= nPos)
 {
  strLeft = strSrc.Left(nPos);
  if (!strLeft.IsEmpty())
  {
   strResult.Add(strLeft);
  }

  strSrc = strSrc.Right(strSrc.GetLength() - nPos - strGap.GetLength());
  nPos = strSrc.Find(strGap);
 }

 if (!strSrc.IsEmpty())
 {
  strResult.Add(strSrc);
 }
}

void GPS_Info::getDistance(double& distance, GPS_Info&lastGps, GPS_Info &currentGps)
{
	distance = sqrtf((lastGps.x - currentGps.x) * (lastGps.x - currentGps.x) +
		(lastGps.y - currentGps.y) * (lastGps.y - currentGps.y)
		+ (lastGps.z - currentGps.z) * (lastGps.z - currentGps.z));
}


//计算速度
void GPS_Info::getVelocity(double& v, double distance, GPS_Info& lastGps, GPS_Info &currentGps)
{
	double time = currentGps.time >=
		lastGps.time ? (currentGps.time - lastGps.time) : (currentGps.time + 5184000 - lastGps.time);

	v = (time == 0.0 )? 0.0 : distance / time;
	
}



