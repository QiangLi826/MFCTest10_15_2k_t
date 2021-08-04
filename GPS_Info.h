

class GPS_Info
{

	public:
		GPS_Info() ;
		GPS_Info(CString tmp);
		double x; 
		double y;
		double z;
		double time; //UTCʱ�䣬ת������
		~GPS_Info(); 
		void SplitStr(CString strSrc, CString strGap, CStringArray &strResult);
		static void getDistance(double& distance, GPS_Info&lastGps, GPS_Info &currentGps);
		static void getVelocity(double& v, double distance, GPS_Info& lastGps, GPS_Info &currentGps);
};




