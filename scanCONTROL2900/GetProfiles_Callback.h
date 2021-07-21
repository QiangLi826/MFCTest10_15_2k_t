//   GetProfiles_Callback.h: demo-application for using the LLT.dll
//
//   Version 3.9.0
//
//   Copyright 2019
//
//   MICRO-EPSILON GmbH & Co. KG
//   K�nigbacher Str. 15
//   94496 Ortenburg
//   Germany
//---------------------------------------------------------------------------

#ifndef LLTGetProfilesCallbackH
#define LLTGetProfilesCallbackH

#define MAX_INTERFACE_COUNT    5
#define MAX_RESOULUTIONS       6

void GetProfiles_Callback();
void OnError(const char* szErrorTxt, int iErrorValue);
void DisplayProfile(double* pdValueX, double* pdValueZ, unsigned int uiResolution);
void DisplayTimestamp(unsigned char* pucTimestamp);
std::string Double2Str(double dValue);

extern void __stdcall NewProfile(const unsigned char* pucData, unsigned int uiSize, void* pUserData);

UINT main_scan(LPVOID lpParamter);

#endif
