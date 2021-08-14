#include "stdafx.h"
#include "RD_Info.h"


RD_Info::RD_Info(){}

RD_Info::~RD_Info(){}

RD_Mean_Info::RD_Mean_Info()
{ 
	double RD1 = 0;
	double RD2 = 0;
	double RD = 0;   	 
	double length = 0;   
	double v = 0;		 
	double RD_result_100m_average = 0;
	double RD2_result_200m_average = 0;	

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
	
	tmp.Format(_T("������: %-.3f km/h"), v); 
	output += tmp;
	
	tmp.Format(_T("��ͳ�Ƴ���: %-.3f m"), length); 
	output += tmp;

	if (RD_result_100m_average > 0.0)
	{
		tmp.Format(_T("��100mƽ��IRI: %-.3f "), RD_result_100m_average); 
		output += tmp;
	}

	if (RD_result_200m_average > 0.0)
	{
		tmp.Format(_T("��320mƽ��IRI: %-.3f "), RD_result_200m_average); 
		output += tmp;
	}
	
	return output;
}

