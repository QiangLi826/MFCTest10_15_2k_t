UINT enter_ild2300 (LPVOID lpParamter);
#define INVALID_VELOCITY_FRESH_CYCLE 50000//������õ�һ��������gps����������£���ʹ��Ĭ���ٶȡ�
#include <vector>

class IRI_Info
{
	public:
		IRI_Info() ;
		double IRI_length;  //IRIͳ�Ƴ���
		double IRI_result;  //IRIֵ
		double dx;
		double v;
		double IRI_result_100m_average;
		double IRI_result_320m_average;
		double SMTD_result;
		double SMTD_result_200m_average;
		double deltaH;
		double MPD;
		~IRI_Info();
		CString toString();		
};

class ILD2300_Info
{

	public:
		ILD2300_Info() {
		}
		//uint32_t rawData; // Distance1 
		double scaledData; // Distance1 scaledData
		double timestamp;
		//double frequency;
		~ILD2300_Info() {
		}
};



class ILD2300_Infos_Buffer
{

	public:
		ILD2300_Infos_Buffer() {
		}
		std::vector<ILD2300_Info> infos_v;
		double distance;
		double v;	// m/s
		double frequency;
		bool isGPSInfoValid;		
		~ILD2300_Infos_Buffer() {
		}
};






