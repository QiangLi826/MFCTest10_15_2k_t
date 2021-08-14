
class RD_Mean_Info
{
	public:
		RD_Mean_Info();
		double RD1;  //RD1
		double RD2;  //RD2
		double RD;   //
		double length;   //统计长度
		double v;
		double RD_result_100m_average;
		double RD_result_200m_average;	
		~RD_Mean_Info();
		CString toString();		
};


class RD_Info
{

	public:
		RD_Info() ;
		int kind; 
		double RD1;		
		double RD2;	
		~RD_Info();
};


#define MAX_RD_INFO_LENGTH 100




