#pragma once
#include <windows.h> 
#include "DataContext.h"
#define TIN 0
#define GRID 1

#define TexImageWidth 4096
#define TexImageHeight 256

typedef struct MyPARAM
{
	HWND hWnd;
	int show_Type_Index;
	int save_character;
	int character_data;

}MyPARAM;
typedef struct MyStoreParam
{
	HWND hWnd;
	DataContext * context;
	bool save_data_flag;
	int xStep;
	int yStep;
}MyStoreParam;
const int SCREENLINEBLOCK=13;
const int POINT_MAX_WIDTH=2000;
const int ROW_COUNT=300;
const int SIMPLIFY_ROW_COUNT=5;
const int GRIDROW=13;

const int BUFFER_COUNT=2;
const int BUFFER_SIZE=1000;
const int INVALID_SENSOR_DATA=-32768 ;
const int EVERY_SCANLINE_BUFFER_SIZE=800;
const int INPUT_SENSOR_DISTANCE=1000;
const int SENSOR_ROW_STAY_POINT=300;
const int MAXDATASHRE=2000;
const int MINDATASHRE=-1;
const int WAIT=6000;