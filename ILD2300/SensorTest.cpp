#include "stdafx.h"
#include <stdio.h> // printf, scanf, file IO
#include <string.h> // _stricmp
#include <float.h> // DBL_MAX
#include <cctype> // isspace
#if defined(_WIN32)
#include <stdlib.h> // _countof
#include <conio.h> // _kbhit
#else // _WIN32
#include <sys/select.h> // FD_SET, select
#include <unistd.h> // STDIN_FILENO
#define _stricmp strcasecmp
#define _countof(a) (sizeof(a)/sizeof(*(a)))
#endif //ifdef WIN32
#include "MEDAQLib.h" // MEDAQLib.h must be included to know it's functions
#if !defined (MAX_PATH)
#define MAX_PATH	260
#endif

#define MEAS_FREQUENCY // Measure real datarate of received signal
//#define CHECK_MONOTONY // Enable monotony check for counter values
#include <deque>
#include <vector>
#include <afxmt.h>
#include "algorithm/IRI.h"

static bool sensorIsMEBUS= false;
static bool sensorIsMultiSensor= false;
static bool videoStreamActive= false;
static int32_t valsPerFrame= 0;
int g_SubSampleRate = 200; // 采样率是g_SubSampleRate/20 000
int g_IRIMinSize = 2; //每次计算iri需要队列大小 ILD2300_infos
int g_IRIMinLength = 30; //每次计算iri需要的最小里程
double g_RIRMinDX=0.25; //采样间隔dx

#if defined (CHECK_MONOTONY)
static int32_t counterIndex= -1;
#endif

class ILD2300_Info
{

	public:
		ILD2300_Info() {
		}
		uint32_t rawData; // Distance1 
		double scaledData; // Distance1 scaledData
		double timestamp;
		double frequency;
		~ILD2300_Info() {
		}
};

#define MAX_ILD2300_Info	200
uint32_t indexDistance1 = 0;
std::deque<std::vector<ILD2300_Info>> ILD2300_infos;
CCriticalSection criticalSectionILD2300;


/** Helper function to set cursor at start of current line in console.
*/
void ClearLine()
	{
	printf ("\r"); // Jump at start of line, next printf will overwrite old content
	}


/** Show current progress in console.
*/
void ShowProgress (double progress)
	{
	printf ("Progress: %5.1f%%", progress);
#if !defined(_WIN32)
	fflush(stdout);
#endif
	}

/** Check if key was pressed, read character in case of.
*/
bool KeyPressed()
	{
#if defined(_WIN32)
	bool keyPressed= false;
	while (_kbhit())
		keyPressed|= _getch()!=0; // Function keys (F..., Ctrl-C, ...) writer two characters into keyboard buffer (first may be 0), so call getch() multiple times
	return keyPressed;
#else // _WIN32
	bool KeyIsPressed = false;
	int* pKeyCode = nullptr;
	struct timeval tv;
	fd_set rdfs;

	tv.tv_sec = tv.tv_usec = 0;

	FD_ZERO(&rdfs);
	FD_SET(STDIN_FILENO, &rdfs);

	select(STDIN_FILENO+1, &rdfs, nullptr, nullptr, &tv);

	if(FD_ISSET(STDIN_FILENO, &rdfs))
	{
		int KeyCode = getchar();
		if(pKeyCode != nullptr)
			*pKeyCode = KeyCode;
		KeyIsPressed = true;
	}

	return KeyIsPressed;
#endif //ifdef WIN32
	}

/** Close and release sensor instance, wait for a key pressed
*/
int32_t Cleanup (uint32_t sensorInstance, int32_t retCode= 0)
	{
	if (sensorInstance)
		{
		::CloseSensor (sensorInstance);
		::ReleaseSensorInstance (sensorInstance);
		}
	printf ("Press any key to exit");
#if !defined(_WIN32)
	fflush(stdout);
#endif // !_WIN32
	//while (!KeyPressed())
		//; // busy waiting
	return retCode;
	}

/** Retrieve error message after any MEDAQLib function returned an error
*/
void ShowLastError (uint32_t sensorInstance, const char *location)
	{
	char lastError[2048]= "\0";
	::GetError (sensorInstance, lastError, _countof (lastError));
	printf ("\nError at '%s'\n", location);
	printf ("%s\n", lastError);

	int32_t errNumber;
	if (::GetParameterInt (sensorInstance, "SA_ErrorNumber", &errNumber)==ERR_NOERROR)
		printf ("Sensor error number: %d\n", errNumber);

	char errText[1024]= "\0";
	uint32_t tmp= _countof (errText);
	if (::GetParameterString (sensorInstance, "SA_ErrorText", errText, &tmp)==ERR_NOERROR)
		printf ("Sensor error text: '%s'\n", errText);
	}

/** Macro to encapsulate error handling when calling MEDAQLib functions.
* Can only be used if variable sensorInstance exists.
*/
#define CHECK_ERROR(expr) \
	{ \
	const ERR_CODE ceRet= expr; \
	if (ceRet!=ERR_NOERROR) \
		{ \
		ShowLastError (sensorInstance, #expr); \
		if (ceRet!=ERR_WARNING && ceRet!=ERR_SENSOR_ANSWER_WARNING) \
			{ \
			Cleanup (sensorInstance); \
			return false; \
			} \
		} \
	}

/** Print short help on console
*/
int32_t Usage()
	{
	printf ("Usage:\nSensorTest <Sensor name> [InterfaceParam=Value] ...\n");
	printf ("Value can be an integer number (without dot) or a double number (with dot) or a string.\n");
	printf ("If a parameter contains a space, it must be quoted by \"...\".\n");
	printf ("If a command file should be processed after connecting, set parameter Commands=\"<File name>\"\n\n");
	printf ("Examples:\n");
	printf ("SensorTest ILD2300 (will use SensorFinder to detect ILD2300)\n");
	printf ("SensorTest IFD2451 IP_Interface=TCP/IP IP_RemoteAddr=192.168.1.10 (connect to IFD2451 via TCP/IP on a fixed address)\n\n");
	printf ("Format of command file:\n");
	printf ("Each command is at an own line. Command parameters are separated by spaces.\n");
	printf ("A command line is limited to 2048 characters.\n");
	printf ("If parameter S_Command is missing the whole line is skipped.\n");
	printf ("e.g.: S_Command=Set_DataOutInterface SP_DataOutInterface=2\n");
	Cleanup (0);
	return 0;
	}

/** Extracts and processes command line parameters
*/
bool ParseParameters (uint32_t sensorInstance, int32_t argc, const char *const argv[])
	{
	if (!argv)
		return false;
	for (int32_t i=2 ; i<argc ; i++)
		CHECK_ERROR (::SetParameters  (sensorInstance, argv[i]));
	return true;
	}

/** Convert a parity enum to a string
*/
const char *ParityToString (uint32_t parity)
	{
	const char *const strings[]= { "No parity", "Odd parity", "Even parity", "Mark parity", "Space parity", "Invalid parity" };
	return strings[parity<_countof (strings) ? parity : _countof (strings)-1];
	}

/** Convert a stop bit enum to a string
*/
const char *StopbitsToString (uint32_t stopbits)
	{
	const char *const strings[]= { "One stop bit", "One point five stop bits", "Two stop bits", "Invalid stop bits" };
	return strings[stopbits<_countof (strings) ? stopbits : _countof (strings)-1];
	}

/** Print RS232 interface parameters at console
*/
bool PrintRS232 (uint32_t sensorInstance)
	{
	char port[64]= {};
	uint32_t len= _countof (port);
	int32_t baudrate= 0, byteSize= 0, parity= 0, stopbits= 0, sensorAddress= 0;
	CHECK_ERROR (::GetParameterString (sensorInstance, "IP_Port", port, &len));
	CHECK_ERROR (::GetParameterInt    (sensorInstance, "IP_Baudrate", &baudrate));
	CHECK_ERROR (::GetParameterInt    (sensorInstance, "IP_ByteSize", &byteSize));
	CHECK_ERROR (::GetParameterInt    (sensorInstance, "IP_Parity", &parity));
	CHECK_ERROR (::GetParameterInt    (sensorInstance, "IP_Stopbits", &stopbits));
	const ERR_CODE ret= ::GetParameterInt (sensorInstance, "IP_SensorAddress", &sensorAddress);
	if (ret!=ERR_NOERROR && ret!=ERR_NOT_FOUND)
		CHECK_ERROR (::GetParameterInt (sensorInstance, "IP_SensorAddress", &sensorAddress));
	printf ("%s, %d Baud, %d data bits, %s, %s", port, baudrate, byteSize, ParityToString (parity), StopbitsToString (stopbits));
	if (ret==ERR_NOERROR)
		printf (", RS485 addr: %d", sensorAddress);
	return true;
	}

/** Print IF2004 interface parameters at console
*/
bool PrintIF2004 (uint32_t sensorInstance)
	{
	int32_t cardInstance= 0, channelNumber= 0, baudrate= 0;
	CHECK_ERROR (::GetParameterInt  (sensorInstance, "IP_CardInstance", &cardInstance));
	CHECK_ERROR (::GetParameterInt  (sensorInstance, "IP_ChannelNumber", &channelNumber));
	CHECK_ERROR (::GetParameterInt  (sensorInstance, "IP_Baudrate", &baudrate));
	printf ("Card %d, Channel %d, %d Baud", cardInstance, channelNumber, baudrate);
	return true;
	}

/** Print TCP/IP interface parameters at console
*/
bool PrintTCPIP (uint32_t sensorInstance)
	{
	char ipAddress[64]= {};
	uint32_t len= _countof (ipAddress);
	CHECK_ERROR (::GetParameterString (sensorInstance, "IP_RemoteAddr", ipAddress, &len));
	printf ("%s", ipAddress);
	return true;
	}

/** Print USB interface parameters at console
*/
bool PrintUSB (uint32_t sensorInstance)
	{
	int32_t deviceInstance;
	CHECK_ERROR (::GetParameterInt  (sensorInstance, "IP_DeviceInstance", &deviceInstance));
	printf ("Instance %d", deviceInstance);
	return true;
	}

/** Print IF2004_USB parameters at console
*/
bool PrintIF2004_USB (uint32_t sensorInstance)
	{
	int32_t deviceInstance= 0, channelNumber= 0, baudrate= 0;
	char serialNumber[64]= {}, port[64]= {};
	uint32_t len= _countof (port);
	CHECK_ERROR (::GetParameterInt    (sensorInstance, "IP_DeviceInstance", &deviceInstance));
	CHECK_ERROR (::GetParameterInt    (sensorInstance, "IP_ChannelNumber", &channelNumber));
	CHECK_ERROR (::GetParameterString (sensorInstance, "IP_Port", port, &len));
	len= _countof (serialNumber);
	CHECK_ERROR (::GetParameterString (sensorInstance, "IP_SerialNumber", serialNumber, &len));
	CHECK_ERROR (::GetParameterInt    (sensorInstance, "IP_Baudrate", &baudrate));
	printf ("Device %d (SN%s, %s), Channel %d, %d Baud", deviceInstance, serialNumber, port, channelNumber, baudrate);
	return true;
	}

/** Print IF2008 interface parameters at console
*/
bool PrintIF2008 (uint32_t sensorInstance)
	{
	int32_t cardInstance= 0, channelNumber= 0, baudrate= 0, parity= 0;
	CHECK_ERROR (::GetParameterInt  (sensorInstance, "IP_CardInstance", &cardInstance));
	CHECK_ERROR (::GetParameterInt  (sensorInstance, "IP_ChannelNumber", &channelNumber));
	CHECK_ERROR (::GetParameterInt  (sensorInstance, "IP_Baudrate", &baudrate));
	CHECK_ERROR (::GetParameterInt  (sensorInstance, "IP_Parity", &parity));
	printf ("Card %d, Channel %d, %d Baud, %s", cardInstance, channelNumber, baudrate, ParityToString (parity));
	return true;
	}

/** Print IF2008_ETH interface parameters at console
*/
bool PrintIF2008_ETH (uint32_t sensorInstance, bool showSensorParam)
	{
	char ipAddress[64]= {};
	int32_t channelNumber= 0, baudrate= 0;
	uint32_t len= _countof (ipAddress);
	CHECK_ERROR (::GetParameterString (sensorInstance, "IP_RemoteAddr", ipAddress, &len));
	CHECK_ERROR (::GetParameterInt (sensorInstance, "IP_ChannelNumber", &channelNumber));
	printf ("%s, Channel %d", ipAddress, channelNumber);
	if (showSensorParam)
		{
		CHECK_ERROR (::GetParameterInt (sensorInstance, "IP_Baudrate", &baudrate));
		printf (", %d Baud", baudrate);
		}
	return true;
	}

/** Show one result of SensorFinder at console
*/
bool ShowFoundSensor (uint32_t sensorInstance, int32_t sensorIndex)
	{
	int32_t hardwareInterface= 0, sensorType= 0;
	char sensorTypeName[64]= {}, hwInterfaceName[64]= {}, serialNumber[64]= {};
	uint32_t len= 0;
	CHECK_ERROR (::SetIntExecSCmd     (sensorInstance, "Get_FoundSensor", "IP_Index", sensorIndex));
	len= _countof (sensorTypeName);
	CHECK_ERROR (::GetParameterInt    (sensorInstance, "IP_SensorType", &sensorType));
	CHECK_ERROR (::GetParameterString (sensorInstance, "IP_SensorTypeName", sensorTypeName, &len));
	CHECK_ERROR (::GetParameterInt    (sensorInstance, "IP_InterfaceIdx", &hardwareInterface));
	len= _countof (hwInterfaceName);
	CHECK_ERROR (::GetParameterString (sensorInstance, "IP_Interface", hwInterfaceName, &len));
	printf ("%d: %s at %s (", sensorIndex, sensorTypeName, hwInterfaceName);
	switch (hardwareInterface)
		{
		case  1: if (!PrintRS232      (sensorInstance)) return false; break;
		case  2: if (!PrintIF2004     (sensorInstance)) return false; break;
		case  3: if (!PrintTCPIP      (sensorInstance)) return false; break;
		case  4: if (!PrintUSB        (sensorInstance)) return false; break;
		case  5: if (!PrintUSB        (sensorInstance)) return false; break;
		case  6: if (!PrintIF2008     (sensorInstance)) return false; break;
		case  7: if (!PrintUSB        (sensorInstance)) return false; break;
		case  8: if (!PrintIF2004_USB (sensorInstance)) return false; break;
		case 10: if (!PrintIF2008_ETH (sensorInstance, sensorType!=ETH_ADAPTER_IF2008)) return false; break;
		}
	printf (")");
	len= _countof (serialNumber);
	if (::GetParameterString (sensorInstance, "SA_SerialNumber", serialNumber, &len)==ERR_NOERROR)
		printf (", Serial number %s", serialNumber);
	printf ("\n");
	return true;
	}

/** Seach for specified sensor at any interface and selects one sensor
*/
bool SensorFinder (uint32_t &sensorInstance)
	{
	int32_t enableLogging= 0;
	::GetParameterInt (sensorInstance, "IP_EnableLogging", &enableLogging);
	if (enableLogging)
		CHECK_ERROR (::SetIntExecSCmd (sensorInstance, "Enable_Logging", "CP_ClearSendParameters", 0));

	printf ("Scanning for sensors (press any key to abort)\n");
	double progress= 0;
	int32_t found= 0, processed= 0;
	ShowProgress (progress);
	CHECK_ERROR (::ExecSCmd (sensorInstance, "Start_FindSensor"));
	do
		{
		CHECK_ERROR (::SetIntExecSCmd (sensorInstance, "Exec_SleepTime", "IP_SleepTime", 50/*ms*/));
		ClearLine();
		CHECK_ERROR (::ExecSCmdGetDouble (sensorInstance, "Get_FindSensorProgress", "IA_Progress", &progress));
		CHECK_ERROR (::GetParameterInt   (sensorInstance, "IA_Found", &found));
		while (found>processed)
			if (!ShowFoundSensor  (sensorInstance, processed++))
				return false;
		if (KeyPressed())
			CHECK_ERROR (::ExecSCmd (sensorInstance, "Abort_FindSensor"));
		ShowProgress (progress);
		} while (progress<100.0);
	ClearLine();

	int32_t sensorIndex= -1;
	if (found<=0)
		{
		printf ("Specified sensor is not found\n");
		Cleanup (sensorInstance);
		return false;
		}
	else if (found==1)
		sensorIndex= 0;
	else // more sensors found
		{
		printf ("Please type sensor index <CR> to open sensor: ");
		const int32_t cnt= scanf ("%d", &sensorIndex);
		if (cnt!=1 || sensorIndex<0 || sensorIndex>=found)
			{
			printf ("No valid sensor selected\n");
			Cleanup (sensorInstance);
			return false;
			}
		}
	// Following function requests all interface parameters for selected sensor. Now it can be used at OpenSensor.
	CHECK_ERROR (::SetIntExecSCmd (sensorInstance, "Get_FoundSensor", "IP_Index", sensorIndex));
	int32_t similarSensor= false;
	::GetParameterInt (sensorInstance, "IP_SimilarSensor", &similarSensor); // Ignore error if parameter does not exist
	if (similarSensor) // Other sensor as expected was found (if IP_FindSimilarSensors is set)
		{
		char parameterList[4096]= {}; // 4K should be enough
		uint32_t maxLen= _countof (parameterList);
		ME_SENSOR sensorType= NO_SENSOR;
		CHECK_ERROR (::GetParameterInt (sensorInstance, "IP_SensorType", reinterpret_cast<int32_t *>(&sensorType)));
		CHECK_ERROR (::GetParameters (sensorInstance, parameterList, &maxLen));

		const uint32_t oldInstance= sensorInstance;
		sensorInstance= CreateSensorInstance (sensorType); // so create a new one using correct sensor type
		CHECK_ERROR (::SetParameters (sensorInstance, parameterList));
		::ReleaseSensorInstance (oldInstance); // and release old instance
		}
	return true;
	}

/** Connect to sensor
*/
bool Open (uint32_t sensorInstance)
	{
	printf ("Connecting to sensor\n");
	CHECK_ERROR (::OpenSensor (sensorInstance));
	return true;
	}

bool ProcessSensorCommands (uint32_t sensorInstance, const char *cmdFileName)
	{
	enum { CommSize= 65536 };
	FILE *f= fopen (cmdFileName, "rt");
	if (!f)
		{
#if defined (_WIN32)
		char fullPathName[MAX_PATH];
		cmdFileName= _fullpath (fullPathName, cmdFileName, MAX_PATH);
#endif
		printf ("Could not open command file '%s', skip it\n", cmdFileName);
		return true;
		}
	char *cmdLine= new char[CommSize], *answer= new char[CommSize];
	if (!cmdLine || !answer)
		{
		fclose (f);
		if (cmdLine) delete []cmdLine;
		if (answer)  delete []answer;
		printf ("ProcessSensorCommands: Could not allocate buffer\n");
		Cleanup (sensorInstance);
		return false;
		}
	while (fgets (cmdLine, CommSize, f))
		{
		char *cmd= cmdLine;
		while (isspace (*cmd))
			cmd++;
		if (!*cmd) // empty?
			continue;
		if (strstr (cmd, "S_Command")!=cmd) // Must start with S_Command
			{
			printf ("Ignoring line: %s", cmd);
			continue;
			}
		ERR_CODE ret= ::ClearAllParameters (sensorInstance);
		if (ret!=ERR_NOERROR)
			{
			fclose (f);
			delete[]cmdLine;
			delete[]answer;
			ShowLastError (sensorInstance, "ClearAllParameters");
			Cleanup (sensorInstance);
			return false;
			}
		ret= ::SetParameters (sensorInstance, cmd);
		if (ret!=ERR_NOERROR)
			{
			fclose (f);
			delete[]cmdLine;
			delete[]answer;
			ShowLastError (sensorInstance, "SetParameters");
			Cleanup (sensorInstance);
			return false;
			}
		uint32_t tmp;
		if (::GetParameterString (sensorInstance, "S_Command", nullptr, &tmp)==ERR_NOT_FOUND) // S_Command not specified
			continue;
		printf ("Executing command line: %s", cmdLine);
		tmp= CommSize;
		ret= ::SensorCommand (sensorInstance);
		if (ret!=ERR_NOERROR)
			ShowLastError (sensorInstance, "SensorCommand");
		else
			printf ("\nSuccess!\n");
		if (ret==ERR_NOERROR || ret==ERR_WARNING || ret==ERR_SENSOR_ANSWER_WARNING)
			{
			ret= ::GetParameters (sensorInstance, answer, &tmp);
			if (ret!=ERR_NOERROR)
				{
				fclose (f);
				delete[]cmdLine;
				delete[]answer;
				ShowLastError (sensorInstance, "SetParameters");
				Cleanup (sensorInstance);
				return false;
				}
			printf ("\nAnswer parameters: %s\n", answer);
			}
		}
	fclose (f);
	delete[]cmdLine;
	delete[]answer;
	return true;
	}

/** Retrieve information about transmitted data from sensor and show it at console
*/
bool GetTransmittedDataInfo (uint32_t sensorInstance)
	{
	uint32_t maxLen= 0;
	int32_t maxValsPerFrame= 0, index= 0;
	char sName[64]= {}, rawName[256]= {}, scaledName[256]= {}, rawUnit[256]= {}, scaledUnit[256]= {};
#if defined (CHECK_MONOTONY)
	char sMonotony[512]= {};
#endif
	double rawRangeMin= 0.0, rawRangeMax= 0.0, scaledRangeMin= 0.0, scaledRangeMax= 0.0, rawDatarate= 0.0, scaledDatarate= 0.0;
	CHECK_ERROR (::ExecSCmdGetInt (sensorInstance, "Get_TransmittedDataInfo", "IA_ValuesPerFrame", &valsPerFrame));
	CHECK_ERROR (::GetParameterInt (sensorInstance, "IA_MaxValuesPerFrame", &maxValsPerFrame));

	printf ("\nSensor transmits %d of %d possible values\n", valsPerFrame, maxValsPerFrame);
	for (int32_t i=0 ; i<valsPerFrame ; i++)
		{
		sprintf (sName, "IA_Index%d", i+1);
		CHECK_ERROR (::GetParameterInt (sensorInstance, sName, &index));

		sprintf (sName, "IA_Raw_Name%d", i+1);
		maxLen= _countof (rawName);
		CHECK_ERROR (::GetParameterString (sensorInstance, sName, rawName, &maxLen));

		sprintf (sName, "IA_Scaled_Name%d", i+1);
		maxLen= _countof (scaledName);
		CHECK_ERROR (::GetParameterString (sensorInstance, sName, scaledName, &maxLen));

		sprintf (sName, "IA_Raw_RangeMin%d", i+1);
		CHECK_ERROR (::GetParameterDouble (sensorInstance, sName, &rawRangeMin));

		sprintf (sName, "IA_Scaled_RangeMin%d", i+1);
		CHECK_ERROR (::GetParameterDouble (sensorInstance, sName, &scaledRangeMin));

		sprintf (sName, "IA_Raw_RangeMax%d", i+1);
		CHECK_ERROR (::GetParameterDouble (sensorInstance, sName, &rawRangeMax));

		sprintf (sName, "IA_Scaled_RangeMax%d", i+1);
		CHECK_ERROR (::GetParameterDouble (sensorInstance, sName, &scaledRangeMax));

		sprintf (sName, "IA_Raw_Unit%d", i+1);
		maxLen= _countof (rawUnit);
		CHECK_ERROR (::GetParameterString (sensorInstance, sName, rawUnit, &maxLen));

		sprintf (sName, "IA_Scaled_Unit%d", i+1);
		maxLen= _countof (scaledUnit);
		CHECK_ERROR (::GetParameterString (sensorInstance, sName, scaledUnit, &maxLen));

		sprintf (sName, "IA_Raw_Datarate%d", i+1);
		CHECK_ERROR (::GetParameterDouble (sensorInstance, sName, &rawDatarate));

		sprintf (sName, "IA_Scaled_Datarate%d", i+1);
		CHECK_ERROR (::GetParameterDouble (sensorInstance, sName, &scaledDatarate));

		printf (" %2d: %s [%.1f .. %.1f %s] @ %.1f Hz, %s [%g .. %g %s] @ %.1f Hz\n", index, rawName, rawRangeMin, rawRangeMax, rawUnit, rawDatarate, scaledName, scaledRangeMin, scaledRangeMax, scaledUnit, scaledDatarate);

		if (strcmp(rawName, "Distance1") == 0)
		{
			indexDistance1 = index;
			printf("indexDistance1");
		}

#if defined (CHECK_MONOTONY)
		if (counterIndex==-1 && strstr (scaledName, "Counter"))
			{
			counterIndex= i;
			snprintf (sMonotony, _countof (sMonotony), "Enable monotony check for signal %2d: %s\n", index, scaledName);
			}
#endif
		}

	int32_t videoSignalsPerFrame= 0, videoSignalPixelCount= 0, videoSignalPixelByteSize= 0;
	char videoSignalName[256]= {};
	::GetParameterInt (sensorInstance, "IA_VideoSignalsPerFrame", &videoSignalsPerFrame); // This parameter may not exist, so ignore errors
	if (videoSignalsPerFrame>0)
		{
		videoStreamActive= true;
		printf ("\nSensor transmits %d video signals\n", videoSignalsPerFrame);
		for (int32_t i=0; i<videoSignalsPerFrame; i++)
			{
			sprintf (sName, "IA_VideoSignalName%d", i+1);
			maxLen= _countof (videoSignalName);
			CHECK_ERROR (::GetParameterString (sensorInstance, sName, videoSignalName, &maxLen));

			sprintf (sName, "IA_VideoSignalPixelCount%d", i+1);
			CHECK_ERROR (::GetParameterInt (sensorInstance, sName, &videoSignalPixelCount));

			sprintf (sName, "IA_VideoSignalPixelByteSize%d", i+1);
			CHECK_ERROR (::GetParameterInt (sensorInstance, sName, &videoSignalPixelByteSize));

			printf (" %s (%d pixel each %d bytes) with length %d bytes\n", videoSignalName, videoSignalPixelCount, videoSignalPixelByteSize, videoSignalPixelCount*videoSignalPixelByteSize);
			}
		}
#if defined (CHECK_MONOTONY)
	if (counterIndex!=-1)
		printf ("%s", sMonotony);
#endif
	return true;
	}

#if defined (CHECK_MONOTONY)
void CheckMonotony (int32_t count, const double *values, int32_t overflow)
	{
	if (counterIndex<0)
		return;
	static double sLastVal= -1;
	if (overflow!=0)
		printf ("\nToo much data, %d values (%d frames) discarded, unwarranted monotony error may occur!\n", overflow, overflow/valsPerFrame);
	for (int32_t i=0; i<count; i+= valsPerFrame)
		{
		if (sLastVal!=-1/*at Start*/ && values[counterIndex+i]!=0/*wrap around after overflow*/ && values[counterIndex+i]-sLastVal!=1)
			printf ("\nMonotony error: %.0f .. %.0f -> missing %d frames\n", sLastVal, values[counterIndex+i], static_cast<int32_t>(values[counterIndex+i]-sLastVal-1));
		sLastVal= values[counterIndex+i];
		}
	if (overflow!=0)
		sLastVal+= overflow/valsPerFrame;
	}
#endif

#if defined (MEAS_FREQUENCY)
double MeasureFrequency (uint32_t sensorInstance, int32_t actValues)
	{
	static double sValues= 0, sStart= 0;
	double actual;
	ExecSCmdGetDouble (sensorInstance, "Get_HighResolutionTime", "IA_HighResolutionTime", &actual);
	if (sStart==0) // First time
		{
		sStart= actual;
		return 0;
		}
	sValues+= actValues;
	return sValues / (actual-sStart) * 1000.0 /*[Hz]*/;
	}
#endif

bool OutputTimeReached (uint32_t sensorInstance)
	{
	enum { OutputIntervall= 100/*ms*/ };
	static double sLastTime= 0;
	double actual;
	ExecSCmdGetDouble (sensorInstance, "Get_HighResolutionTime", "IA_HighResolutionTime", &actual);
	if (actual-sLastTime>=OutputIntervall)
		{
		sLastTime= actual;
		return true;
		}
	else
		return false;
	}
/**
* 向缓冲区队列添加设备传来的数据
*/
void pushILD2300Info(std::vector<ILD2300_Info>& ILD2300_infos_tmp)
{
	// 不加锁，数量有一点点不准确没有关系
	if (ILD2300_infos.size()< MAX_ILD2300_Info)
	{
		criticalSectionILD2300.Lock();	
		//printf("pushILD2300Info \n\r");
		ILD2300_infos.push_back(ILD2300_infos_tmp); //互斥访问当缓存队列	
		criticalSectionILD2300.Unlock();
	}
	else
	{
		// 缓存队列满，不再添加数据
		printf("ILD2300_infos queue is full\r");
		
		ILD2300_infos_tmp.clear();
		
	}
}

/** Read continuous data from sensor and show each first frame at console
*/
bool GetData (uint32_t sensorInstance)
	{
	enum { BufSize= 15000 };
	int32_t sleepTime= 50;
	printf ("\nReading data continuous (press any key to abort)\n");
	int32_t avail= 0;
	int32_t *rawData= new int32_t[BufSize];
	double  *scaledData= new double[BufSize];
	double timestamp= 0.0;
	if (!rawData || !scaledData)
		{
		if (rawData)    { delete []rawData;    rawData=    nullptr;	}
		if (scaledData) { delete []scaledData; scaledData= nullptr;	}
		printf ("GetData: Could not allocate buffer\n");
		}
	while (1)
		{
		if (sensorIsMEBUS)
			{
			if (sensorIsMultiSensor)
				CHECK_ERROR (::SetIntExecSCmd (sensorInstance, "Get_Measure", "IP_SensorIndex", -1))
			else
				CHECK_ERROR (::ExecSCmd (sensorInstance, "Get_Measure"));
			}
		else if (sleepTime>0)
			CHECK_ERROR (::SetIntExecSCmd (sensorInstance, "Exec_SleepTime", "IP_SleepTime", sleepTime/*ms*/));
		if (videoStreamActive)
			{
			::SetParameterInt (sensorInstance, "SP_ReadMode", 2/*Automatic*/);
			::SetParameterInt (sensorInstance, "SP_WaitVideoTimeout", 500/*ms*/);
			::ExecSCmd (sensorInstance, "Get_VideoStreamSignal");
			}
		CHECK_ERROR (::DataAvail (sensorInstance, &avail));
		if (avail<valsPerFrame)
			continue;
		int32_t read= avail;
		if (read>BufSize)
			read= BufSize;
		CHECK_ERROR (::TransferDataTS (sensorInstance, rawData, scaledData, read, &read, &timestamp)); // Read up to [buffer size] values
		if (read<valsPerFrame)
			{
			if (rawData)    { delete[]rawData;    rawData=    nullptr; }
			if (scaledData) { delete[]scaledData; scaledData= nullptr; }
			printf ("Error at '::TransferDataTS (sensorInstance, rawData, scaledData, _countof (rawData), &read)'\nTo less values (%d) was read.\n", read);
			return false;
			}
		if (avail>read) // More data avail?
			CHECK_ERROR (::TransferData (sensorInstance, nullptr, nullptr, avail-read, nullptr)); // Read rest of values without processing to avoid buffer overflows
#if defined (MEAS_FREQUENCY)
		const double frequency= valsPerFrame ? MeasureFrequency (sensorInstance, avail)/valsPerFrame : 0;
#endif
#if defined (CHECK_MONOTONY)
		CheckMonotony (read, scaledData, avail-read);
#endif	
		if (OutputTimeReached (sensorInstance))
			{
			//ClearLine();
			//printf ("[%5d/%5d] \n\r", read, avail);

			std::vector<ILD2300_Info> ILD2300_infos_tmp; //或者使用BufSize
			ILD2300_infos_tmp.reserve(ceil(double(read/valsPerFrame)));

			//每隔g_SubSampleRate个数据采样一次
			for (int32_t i = indexDistance1; i < read && (i+ valsPerFrame*g_SubSampleRate) < read; i+= (valsPerFrame*g_SubSampleRate))
				{
				//ClearLine();
				ILD2300_Info tmp_info;
				if (scaledData[i] == -DBL_MAX) // MEDAQLib default error value
				{
					printf("%u (error value)", static_cast<uint32_t>(rawData[i]));
					//tmp_info.rawData = static_cast<uint32_t>(rawData[i]);
					//tmp_info.scaledData = scaledData[i];
					continue;
				}
				else
				{
					//printf("%u (%-.3f)", static_cast<uint32_t>(rawData[i]), scaledData[i]);
					tmp_info.rawData = static_cast<uint32_t>(rawData[i]);
					tmp_info.scaledData = scaledData[i];				
				}

				//printf(", TS %.3f", timestamp);
				tmp_info.timestamp = timestamp; // TODO: 这个是最老的那个点的时间。暂时每个点都记录下时间。
#if defined (MEAS_FREQUENCY)
				//printf(" @ %.3f Hz", frequency);
				tmp_info.frequency = frequency; 
#endif
				ILD2300_infos_tmp.push_back(tmp_info);
				}


			pushILD2300Info(ILD2300_infos_tmp);

#if !defined(_WIN32)
			fflush( stdout );
#endif //if not def Win32
			} // OutputTimeReached()
		} // while (1);
	if (rawData)    { delete[]rawData;    rawData=    nullptr; }
	if (scaledData) { delete[]scaledData; scaledData= nullptr; }
	printf ("\n");
	return true;
	}

//从缓存队列中提取distances数据
void getDistances(std::vector<double>* distances, int limit)
{
	int size = ILD2300_infos.size();
	for(int i=0; i < size; i++)
	{
		std::vector<ILD2300_Info> tmp_v = ILD2300_infos.front(); 
		ILD2300_infos.pop_front();

		int count = tmp_v.size();
		for (int i = 0; i < count;i++)
		{
			distances->push_back(double(tmp_v[i].scaledData/1000)); //mm转换成m
		}

		if (distances->size() >= limit){
			// 剩余队列过多继续
			if (size - i > 20)
			{
				continue;
			}
			break;
		}
	}
}

//判断计算iri的源数据是否充足
boolean isDataEnough(int limit)
{
	int count = 0;
	for (std::deque<std::vector<ILD2300_Info>>::iterator it = ILD2300_infos.begin(); it != ILD2300_infos.end(); ++it)
	{
		count += it->size();
		if (count > limit)
			return true;			
	}

	return false;
}


/**
* 异步处理点激光数据
*/
UINT processILD2300InfosThread(LPVOID lparam)
{
	double v=80.0; //1/4车辆行驶速度（km/h）
			
	double vPerSecond = v * 1000 / 3600; // m/s
			
	int frequency = 20000; //目前点激光采集频率是20KHz		

	//计算iri需要最少的数据量 30m / vPerSecond * frequency / g_SubSampleRate
	int dataNumLimit  = g_IRIMinLength * frequency / g_SubSampleRate / vPerSecond ; 

	double dx = g_SubSampleRate * vPerSecond / frequency; // 采样间隔（m） 这个采样间隔要大于0.25
	if(dx < g_RIRMinDX)
	{
		//根据采样间隔调整采样频率。
		g_SubSampleRate = ceil(g_RIRMinDX * frequency / vPerSecond);
	}

	dx = g_SubSampleRate * vPerSecond / frequency;
		

	while (1) {		
		Sleep(1000); // 暂停

		criticalSectionILD2300.Lock();
		int size = ILD2300_infos.size();
		
			
		if (!isDataEnough(dataNumLimit)){
			criticalSectionILD2300.Unlock();
			continue;
		}
			
		std::vector<double> distances;
		
		getDistances(&distances, dataNumLimit);

		printf("ild2300 queue size %d,  vPerSecond:(%-.3f) dx: (%-.3f)  distances.size: %d dataNumLimit:%d \n\r"
			, size, vPerSecond, dx,distances.size(), dataNumLimit);
		
		iri(distances, dx, v);
			
		criticalSectionILD2300.Unlock();		
	}

	return 0;
}


/** Main SensorTest program
*/
UINT main_program (LPVOID lpParamter)
	{	

	char *argv[] = {"SensorTest", "ILD2300"};
	int argc =2;

	if (argc<2 || !argv)
		return Usage();

	printf ("SensorTest %s\n", argv[1]);
	uint32_t sensorInstance= ::CreateSensorInstByName (argv[1]);
	if (!sensorInstance)
		{
		printf ("Cannot create driver instance of sensor '%s'!\n", argv[1]);
		return Cleanup (sensorInstance, -1);
		}

	if ((_stricmp(argv[1], "MEbus") == 0) || (_stricmp(argv[1], "ME-Bus") == 0) || (_stricmp(argv[1], "DT6120") == 0) || (_stricmp(argv[1], "capaNCDT 6120") == 0))
		sensorIsMEBUS = true;
	else if ((_stricmp (argv[1], "MultiSensor") == 0))
		sensorIsMultiSensor= true;

	if (!ParseParameters (sensorInstance, argc, argv))
		return Cleanup (sensorInstance, -2);
	char cmdFileName[MAX_PATH]= "\0";
	uint32_t tmp= _countof (cmdFileName);
	::GetParameterString (sensorInstance, "Commands", cmdFileName, &tmp);
	if (::GetParameterString (sensorInstance, "IP_Interface", nullptr, &tmp)==ERR_NOT_FOUND) // Interface not specified, 
		{
		if (!SensorFinder (sensorInstance)) // so start SensorFinder
			return Cleanup (sensorInstance, -3);
		if (!ParseParameters (sensorInstance, argc, argv))
			return Cleanup (sensorInstance, -4);
		}

	if (!Open (sensorInstance))
		return Cleanup (sensorInstance, -5);
	if (sensorIsMultiSensor)
		{
		int32_t sensorType= NO_SENSOR;
		if (::ExecSCmdGetInt (sensorInstance, "Get_DrvSetting", "IA_SensorType#1", &sensorType)==ERR_NOERROR && (sensorType==SENSOR_ON_MEBUS || sensorType==SENSOR_DT6120))
			sensorIsMEBUS= true; // If first sensor is MEBus assume any sensor is MEBus
		}

	if (*cmdFileName && !ProcessSensorCommands (sensorInstance, cmdFileName))
		return Cleanup (sensorInstance, -6);

	if (!GetTransmittedDataInfo (sensorInstance))
		return Cleanup (sensorInstance, -7);
	if (!GetData (sensorInstance))
		return Cleanup (sensorInstance, -8);
	return Cleanup (sensorInstance);
	}



UINT enter_ild2300 (LPVOID lpParamter)
{
	AfxBeginThread(&processILD2300InfosThread, NULL);

	while(1)
	{
		//失败后等待5s
		Sleep(5000);
		
		main_program(lpParamter);
	}	
}




