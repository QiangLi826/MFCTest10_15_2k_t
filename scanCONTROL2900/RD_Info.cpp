#include "stdafx.h"
#include "RD_Info.h"


RD_Info::RD_Info(){}

RD_Info::~RD_Info(){}

RD_Mean_Info::RD_Mean_Info()
{ 
	RD1 = 0;
	RD2 = 0;
	RD = 0;   	 
	length = 0;   
	v = 0;		 
	RD_result_100m_average = 0;
	RD_result_200m_average = 0;	

}



RD_Mean_Info::~RD_Mean_Info()
{ }


CString RD_Mean_Info::toString()
{
	CString tmp;
	CString output;
	tmp.Format(_T("RD1 %-.3f "), RD1); 
	output += tmp;
	tmp.Format(_T("RD2 %-.3f "), RD2); 
	output += tmp;
	tmp.Format(_T("RD %-.3f "), RD); 
	output += tmp;
	
	tmp.Format(_T("，车速: %-.3f km/h"), v); 
	output += tmp;
	
	tmp.Format(_T("，统计长度: %-.3f m"), length); 
	output += tmp;

	if (RD_result_100m_average > 0.0)
	{
		tmp.Format(_T("，100m平均IRI: %-.3f "), RD_result_100m_average); 
		output += tmp;
	}

	if (RD_result_200m_average > 0.0)
	{
		tmp.Format(_T("，320m平均IRI: %-.3f "), RD_result_200m_average); 
		output += tmp;
	}
	
	return output;
}

ProfileSample::ProfileSample(){};

ProfileSample::~ProfileSample(){};







