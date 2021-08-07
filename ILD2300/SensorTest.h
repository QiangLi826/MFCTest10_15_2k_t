UINT enter_ild2300 (LPVOID lpParamter);
#define INVALID_VELOCITY_FRESH_CYCLE 50000//随便设置的一个大数，gps开启的情况下，不使用默认速度。

class IRI_Info
{
	public:
		IRI_Info() ;
		double IRI_length;  //IRI统计长度
		double IRI_result;  //IRI值
		double dx;
		double v;
		double IRI_result_100m_average;
		double IRI_result_320m_average;
		double SMTD_result;
		double SMTD_result_200m_average;
		~IRI_Info();
		CString toString();		
};







