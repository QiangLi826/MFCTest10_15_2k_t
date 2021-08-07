

class GPS_Info
{

	public:
		GPS_Info() ;
		GPS_Info(CString tmp);
		double x; 
		double y;
		double z;
		double time; //UTCʱ�䣬ת������
		bool isGPSInfoValid; //GPS�����Ƿ���Ч
		bool isGPSFormatValid; //GPS��ʽ�Ƿ���Ч
		~GPS_Info(); 
		static void SplitStr(CString strSrc, CString strGap, CStringArray &strResult);
		static void getDistance(double& distance, GPS_Info&lastGps, GPS_Info &currentGps);
		static void getVelocity(double& v, double distance, GPS_Info& lastGps, GPS_Info &currentGps);
		static double GPS_Info::getTimeDifference(double& timeDiff, GPS_Info& lastGps, GPS_Info &currentGps);
};




