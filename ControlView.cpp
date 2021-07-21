// ControlView.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCTest10_15View.h"
#include "ControlView.h"
#include "MainFrm.h"
#include <winbase.h>
#include <io.h>
#include <winsock.h>
#include <iostream>
using namespace std;

#ifdef PYLON_WIN_BUILD
#include <pylon/PylonGUI.h>
#endif

#define min(a,b) (((a) < (b)) ? (a) : (b))

#include <pylon/PylonIncludes.h>
#include <pylonc/PylonC.h>
#include <pylon/gige/BaslerGigEInstantCamera.h>
typedef Pylon::CBaslerGigEInstantCamera Camera_t;

using namespace Basler_GigECameraParams;
int CameraChooseFlag = 0;//相机是否开启的标志
CTexture TexBlock[TextureNumPerFrame][TexImageWidth/TexImageHeight]; //第一个相机采集的纹理缓冲结构
CTexture TexBlock1[TextureNumPerFrame][TexImageWidth/TexImageHeight];//第二个相机采集的纹理缓冲结构
GLubyte Image[TexImageHeight][TexImageHeight];

int ShowStyle = 0;

// CControlView

IMPLEMENT_DYNCREATE(CControlView, CFormView)
//**********************DefineByCamera*******************************
void IntToChar(int convert_int, char str[], int StrLen);
char *StringJoint(char str1[], char str2[]);

int imagecount = 11;//第一个相机抓取的图像计数
int imagecount1 = 11;//第二个相机抓取的图像计数
int gain = 320;
int blackLevel = 50;
int linerate = 20000;
int exposuretime = 50;
extern GLubyte BigImage[512][512];
extern GLubyte BigImageR[512][512];


int typeIndex;
//**********************DefineByCamera*******************************
CControlView::CControlView()
	: CFormView(CControlView::IDD)
	, m_gain(1000)
	, m_BlackLevel(500)
	, m_LineRate(20000)
	, save_flag_radio(0)
	
	, character_data(100)
	,xStep(8),yStep(2)
{

	AllocConsole();
	SetConsoleTitle(_T("传感器状态后台监控"));
	freopen("CON", "r", stdin );
	freopen("CON", "w", stdout);
	freopen("CON", "w", stderr);
	cout << "控制台启动成功：";

	typeIndex=0;
	ContrlDlg  *pDlg=new ContrlDlg ;
	pDlg->Create(IDD_CONTRLDLG,this);
	pDlg->ShowWindow(SW_SHOW);
	pDlg=NULL;
	delete pDlg;

	//**********************DefineByCamera*******************************
	int i,j;
	for (i=0;i<TexImageHeight;i++)
	{
		for (j=0;j<TexImageHeight;j++)
		{
			if (i>TexImageHeight/4 && i<TexImageHeight/2 && j>TexImageHeight/4 && j<TexImageHeight/2)
			{
				Image[i][j] =0;
			}
			else
			{
				Image[i][j] =125;
			}


		}
	}

	Second =300;
	c_countOfImagesToGrab = Second * (m_LineRate/TexImageHeight);
	//**********************DefineByCamera*******************************

	// 	m_gain = 0;
	// 	m_BlackLevel = 0;
	// 	m_LineRate = 0;
	m_ShowStyle = 0;
	save_data_flag=true;


}

CControlView::~CControlView()
{
	FreeConsole();
}
int CControlView::save_character=1;
int CControlView::save_config=1;
int CControlView::chk1=1;
int CControlView::chk2=1;
int CControlView::chk3=1;
int CControlView::chk4=1;

void CControlView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//  DDX_Text(pDX, IDC_EDIT1, m_color);
	//  DDX_Text(pDX, IDC_EDIT2, m_color2);
	//  DDX_Text(pDX, IDC_EDIT3, m_color3);
	//DDV_MinMaxFloat(pDX, m_color, 0.0, 1.0);
	//  DDV_MinMaxFloat(pDX, m_color2, 0.0, 1.0);
	//  DDV_MinMaxFloat(pDX, m_color3, 0.0, 1.0);

	DDX_Control(pDX, IDC_COMBO1, m_typeCombo);
	DDX_Control(pDX, IDC_BUTTON4, stopBtn);
	DDX_Control(pDX, IDC_BUTTON3, startBtn);

	//  DDX_Text(pDX, IDC_EDIT4, m_AutoExposure);
	//  DDV_MinMaxInt(pDX, m_AutoExposure, 20, 100000);
	DDX_Control(pDX, IDC_BUTTON5, m_ConnectCamera);
	DDX_Control(pDX, IDC_BUTTON6, m_CutCamera);
	DDX_Radio(pDX, IDC_RADIO1, m_ShowStyle);
	DDX_Control(pDX, IDC_BUTTON2, m_ParameterSetOK);
	DDX_Control(pDX, IDC_BUTSAVE, btnSave);
	DDX_Text(pDX, IDC_EDIT1, character_data);
	DDX_Text(pDX, IDC_EDIT2, xStep);
	DDX_Text(pDX, IDC_EDIT3, yStep);
	DDX_Radio(pDX, IDC_RADIO6, save_flag_radio);
	DDX_Radio(pDX, IDC_RADIO4, save_character);
	DDX_Radio(pDX, IDC_RADIO8, save_config);
	DDX_Check(pDX,IDC_CHK1,chk1);
	DDX_Check(pDX,IDC_CHK2,chk2);
	DDX_Check(pDX,IDC_CHK3,chk3);
	DDX_Check(pDX,IDC_CHK4,chk4);
}

BEGIN_MESSAGE_MAP(CControlView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON2, &CControlView::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CControlView::OnBnClickedStart)
	
	ON_BN_CLICKED(IDC_BUTTON4, &CControlView::OnBnClickedStop)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CControlView::OnSelchangeCombo)
	ON_BN_CLICKED(IDC_BUTTON5, &CControlView::OnBnClickedStartTexture)
	ON_BN_CLICKED(IDC_BUTTON6, &CControlView::OnBnClickedCloseTexture)
	ON_BN_CLICKED(IDC_RADIO1, &CControlView::OnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO3, &CControlView::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO2, &CControlView::OnBnClickedRadio2)
	ON_WM_CTLCOLOR()
	
	ON_BN_SETFOCUS(IDC_RADIO1, &CControlView::OnBnSetfocusRadio1)
	ON_BN_SETFOCUS(IDC_RADIO3, &CControlView::OnBnSetfocusRadio3)
	ON_BN_SETFOCUS(IDC_RADIO2, &CControlView::OnBnSetfocusRadio2)
	ON_BN_SETFOCUS(IDC_BUTTON3, &CControlView::OnBnSetfocusButton3)
	ON_BN_SETFOCUS(IDC_BUTTON4, &CControlView::OnBnSetfocusButton4)
	ON_BN_SETFOCUS(IDC_BUTTON5, &CControlView::OnBnSetfocusButton5)
	ON_BN_SETFOCUS(IDC_BUTTON6, &CControlView::OnBnSetfocusButton6)
	ON_BN_CLICKED(IDC_BUTSAVE, &CControlView::OnBnClickedButsave)
	
	ON_BN_CLICKED(IDC_RADIO6, &CControlView::OnBnClickedRadio6)
	ON_BN_CLICKED(IDC_RADIO7, &CControlView::OnBnClickedRadio7)
	ON_EN_CHANGE(IDC_EDIT1, &CControlView::OnEnChangeEdit1)
	
	ON_EN_CHANGE(IDC_EDIT2, &CControlView::OnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT3, &CControlView::OnChangeEdit3)
END_MESSAGE_MAP()


// CControlView 诊断

#ifdef _DEBUG
void CControlView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CControlView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CControlView 消息处理程序
extern int STEP_X;
extern int STEP_Y;

void CControlView::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	//::MessageBox(NULL,_T("列表为空"),_T("标题"),MB_OK);此句可运行

	UpdateData( TRUE); 
	CMainFrame* pMainFrame=(CMainFrame*)AfxGetMainWnd(); //定位主窗口
	CMFCTest10_15View* pMainView=(CMFCTest10_15View*)pMainFrame->splitter1.GetPane(0,0);
//	pMainView->m_Gain = this->m_gain;
	 
// 	exposuretime = 1000000/m_LineRate;;
// 	linerate = m_LineRate;
// 	gain = m_gain;
// 	blackLevel = m_BlackLevel;
// 	CString str;
// 	str.Format(_T("%d"), exposuretime);
// 	AfxMessageBox(_T("相机参数设置成功！"),0,NULL);
	
	AfxMessageBox(_T("传感器参数配置成功！"),0,NULL);
// 	GetDlgItem(IDC_STATICExposure)->SetWindowText(str);
// 	pMainView->OnPaint();
	//cout<<"设置成功"<<endl;这句不显示
}



//**********************DefineByCamera*******************************
#include <malloc.h>
#include <string.h>


#define NUM_DEVICES 2
#define NUM_BUFFERS 5         /* Number of buffers used for grabbing. */

#define GIGE_PACKET_SIZE       1500 /* Size of one Ethernet packet. */
#define GIGE_PROTOCOL_OVERHEAD 36   /* Total number of bytes of protocol overhead. */

#define CHECK( errc ) if ( GENAPI_E_OK != errc ) printErrorAndExit( errc )

/* This function can be used to wait for user input at the end of the sample program. */
void pressEnterToExit(void);

/* This method demonstrates how to retrieve the error message for the last failed function call. */
void printErrorAndExit( GENAPIC_RESULT errc );

/* Calculating the minimum and maximum gray value. */
void getMinMax(unsigned char* pImg, int32_t width, int32_t height, 
	unsigned char* pMin, unsigned char* pMax);
void CControlView::OnBnClickedStart()
{
	// TODO: 在此添加控件通知处理程序代码
	
	CMainFrame* pMainFrame=(CMainFrame*)AfxGetMainWnd(); //定位主窗口
	CMFCTest10_15View* pMainView=(CMFCTest10_15View*)pMainFrame->splitter1.GetPane(0,0);
	pMainView->laser(save_data_flag,save_character,character_data,save_config,chk1,chk2,chk3,chk4,xStep,yStep);
	this->stopBtn.EnableWindow(TRUE);
	this->startBtn.EnableWindow(FALSE);
	pMainFrame=NULL;
	delete pMainFrame;
	pMainView = NULL;
	delete pMainView;

}
//**********************DefineByCamera*******************************



void CControlView::OnBnClickedStop()
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame* pMainFrame=(CMainFrame*)AfxGetMainWnd(); //定位主窗口
	CMFCTest10_15View* pMainView=(CMFCTest10_15View*)pMainFrame->splitter1.GetPane(0,0);
	pMainView->stopLaser();
	this->startBtn.EnableWindow(TRUE);
}



void CControlView::OnSelchangeCombo()
{
	// TODO: 在此添加控件通知处理程序代码
	typeIndex=m_typeCombo.GetCurSel();
	CString strCBText;
	m_typeCombo.GetLBText( typeIndex, strCBText);
	
}

//**********************DefineByCamera*******************************
#define MaxLenth_Name 20
void CControlView::OnBnClickedStartTexture()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData( TRUE); 
	CameraChooseFlag = 1;
	this->m_ConnectCamera.EnableWindow(FALSE);
	this->m_ParameterSetOK.EnableWindow(FALSE);
	this->m_CutCamera.EnableWindow(TRUE);
//  CMainFrame* pMainFrame=(CMainFrame*)AfxGetMainWnd(); //定位主窗口
//  CMFCTest10_15View* pMainView=(CMFCTest10_15View*)pMainFrame->splitter1.GetPane(0,0);
// 	
// 	CPylonImage imageSaved;
// 	char *str = new char[MaxLenth_Name];
	//传递抓取图像线程参数
	A.c_countOfImagesToGrab = c_countOfImagesToGrab;
	A.c_maxCamerasToUse = c_maxCamerasToUse;

	Configure();
	//开启抓取图像线程函数
	pThread=AfxBeginThread(GrabImageThread, &A);
	



//	delete []str;
}
void CControlView::OnBnClickedCloseTexture()
{
	// TODO: 在此添加控件通知处理程序代码
	this->m_ConnectCamera.EnableWindow(TRUE);
	this->m_ParameterSetOK.EnableWindow(TRUE);
	this->m_CutCamera.EnableWindow(FALSE);
	CameraChooseFlag = 2;
}
//整型转化成字符型的函数
void IntToChar(int convert_int, char str[], int StrLen)
{
	int j = 0;
	int k = 0;
	char *temp = new char[StrLen];
	while(convert_int)
	{
		temp[j] = convert_int % 10 + '0';
		j++;
		convert_int = convert_int / 10;
	}
	temp[j] = '\0';
	j--;
	while(j >= 0)
	{
		str[k] = temp[j];
		j--;
		k++;
	}
	str[k] = '\0';
	char *str1 = ".bmp";

	//str = strcat(str,str1);     //系统已有的函数实现字符串连接
	str = StringJoint(str,str1);  //自定义的字符串连接函数
	delete []temp;
}
//字符串连接函数保存图像的bmp格式
char *StringJoint(char str1[], char str2[])
{
	char *p = str1;
	char *q = str2;
	while((*p) != '\0')p++;
	while((*q) != '\0')
	{
		(*p) = (*q);
		p++;
		q++;
	}
	(*p) = '\0';
	return str1;
}
//相机图像线程函数
void Configure()
{	
	CGrabResultPtr ptrGrabResult[2];
	Pylon::PylonAutoInitTerm autoInitTerm;
	DeviceInfoList_t devices;
	int count[2]={0,0};
	unsigned char min, max;
	
	try
	{
//		CDeviceInfo deviceInfo;
//		deviceInfo.SetDeviceClass( Camera_t::DeviceClass());
		CTlFactory& tlFactory = CTlFactory::GetInstance();
		if ( tlFactory.EnumerateDevices(devices) == 0 )
		{
			throw RUNTIME_EXCEPTION( "No camera present.");
		}
//		Camera_t camera( CTlFactory::GetInstance().CreateFirstDevice(deviceInfo));
		Camera_t camera[2];

		if (devices.size()==1)
		{
			camera[0].Attach(tlFactory.CreateDevice( devices[0]));
			cout << "Using device " << camera[0].GetDeviceInfo().GetModelName() << endl;
			camera[0].Open();
		}
		if (devices.size()==2)
		{
			for (int i = 0; i < 2; i++)
			{
				camera[i].Attach(tlFactory.CreateDevice( devices[i]));
//				cout << endl << "Using device " << camera[i].GetDeviceInfo().GetModelName() << endl;
				camera[i].Open();
				UserSetDefaultSelectorEnums oldDefaultUserSet = camera[i].UserSetDefaultSelector.GetValue();
//				cout << "Loading default settings" << endl;
				camera[i].UserSetSelector.SetValue(UserSetSelector_Default);
				camera[i].UserSetLoad.Execute();

//			    cout << "Turning off Gain Auto and Exposure Auto." << endl;
				camera[i].GainAuto.SetValue(GainAuto_Off);
				camera[i].GainRaw.SetValue(gain);
				camera[i].ExposureAuto.SetValue(ExposureAuto_Off);
				camera[i].ExposureTimeAbs.SetValue(exposuretime);
				camera[i].AcquisitionLineRateAbs.SetValue(linerate);
				camera[i].BlackLevelRaw.SetValue(blackLevel);
//				cout << "Saving currently active settings to user set 1." << endl;
				camera[i].UserSetSelector.SetValue(UserSetSelector_UserSet1);
				camera[i].Width.SetValue(4096);
				camera[i].Height.SetValue(128);
				camera[i].UserSetSave.Execute();

// 				cout << endl << "Loading default settings." << endl;
// 				camera[i].UserSetSelector.SetValue(UserSetSelector_Default);
// 				camera[i].UserSetLoad.Execute();
// 				cout << "Default settings" << endl;
// 				cout << "================" << endl;
// 				cout << "Gain          : " << camera[i].GainRaw.GetValue() << endl;
// 				cout << "Exposure time : " << camera[i].ExposureTimeRaw.GetValue() << endl;
// 				cout << "LineRate      : " << camera[i].AcquisitionLineRateAbs.GetValue() << endl;
				
				cout << endl << "Loading user set 1 settings." << endl;
				camera[i].UserSetSelector.SetValue(UserSetSelector_UserSet1);
				camera[i].UserSetLoad.Execute();
				cout << "User set 1 settings" << endl;
				cout << "===================" << endl;
				cout << "Gain          : " << camera[i].GainRaw.GetValue() << endl;
				cout << "Exposure time : " << camera[i].ExposureTimeRaw.GetValue() << endl;
				cout << "LineRate      : " << camera[i].AcquisitionLineRateAbs.GetValue() << endl;
				camera[i].UserSetDefaultSelector.SetValue(UserSetDefaultSelector_UserSet1);
				camera[i].UserSetDefaultSelector.SetValue(oldDefaultUserSet);
//			    camera[i].Close();
// 				camera[i].StartGrabbing();
// 				camera[i].RetrieveResult( 5000, ptrGrabResult[i], TimeoutHandling_ThrowException);
// 			    if (ptrGrabResult[i]->GrabSucceeded())
// 				{
// 					cout << "Camera " <<  i << ": " << camera[i].GetDeviceInfo().GetModelName() << endl;
// 				    uint8_t *pImageBuffer = (uint8_t *) ptrGrabResult[i]->GetBuffer();
// 					getMinMax( pImageBuffer, TexImageWidth, TexImageHeight, &min, &max );
// 					printf("-----------\nNo. %d----------Min. val=%d, Max. val=%d\n",count[i],min, max);
// 				}
			}			
		}
		
		
	}
	catch (GenICam::GenericException &e)
	{
		cerr << "An exception occurred." << endl
			<< e.GetDescription() << endl;
	}
	
//	return 0;

}
UINT GrabImageThread(LPVOID p)
{
	PARAM *info = (PARAM *)p;
	static int count = 0;
	

	CGrabResultPtr ptrGrabResult;
	Pylon::PylonAutoInitTerm autoInitTerm;//自动
	
	DeviceInfoList_t devices;
	uint8_t *pImageBuffer;
	uint8_t *pImageBuffer1;
// 	while (CameraChooseFlag == 1)
// 	{
		try
		{
			CTlFactory& tlFactory = CTlFactory::GetInstance();
			
			if ( tlFactory.EnumerateDevices(devices) == 0 )
			{
				throw RUNTIME_EXCEPTION( "No camera present.");
			}

			CInstantCameraArray cameras( min( devices.size(), info->c_maxCamerasToUse));
			for ( size_t i = 0; i < cameras.GetSize(); ++i)
			{
				cameras[ i ].Attach( tlFactory.CreateDevice( devices[ i ]));
				

				// Print the model name of the camera.
				cout << "Using device " << cameras[ i ].GetDeviceInfo().GetModelName() << endl;
			}
			//CInstantCamera camera( CTlFactory::GetInstance().CreateFirstDevice());
			//cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;
			
			cameras.StartGrabbing();
// 			for( int i = 0; i < 5 && cameras.IsGrabbing(); i++)
// 			{
			while(CameraChooseFlag == 1 && cameras.IsGrabbing())
			{
				cameras.RetrieveResult( 5000, ptrGrabResult, TimeoutHandling_ThrowException);
				if (ptrGrabResult->GrabSucceeded())
				{
				intptr_t cameraContextValue = ptrGrabResult->GetCameraContext();
				
				// Print the index and the model name of the camera.
//				cout << "Camera " <<  cameraContextValue << ": " << cameras[ cameraContextValue ].GetDeviceInfo().GetModelName() << endl;
				
				
				
				int i, j, k, m, n, sum = 0;	
				unsigned char min, max;
				int imageh=256;
				int imagew=4096;
				String_t Serial1 = "21398393";
				String_t Serial2 = "21398402";
				int BlockNum = 32;
				int BlockHeight = 512 / BlockNum;
				float a = 0, b = 0.03, c = 2;
				float temp1, temp2;
				
				if (cameras[ cameraContextValue ].GetDeviceInfo().GetSerialNumber() == Serial1)
				{
					pImageBuffer = (uint8_t *) ptrGrabResult->GetBuffer();
					for (i = (imagecount % BlockNum) * BlockHeight; i < (imagecount % BlockNum + 1) * BlockHeight; i ++)
					{
						for (j = 0; j < 256; j ++)
						{
							for(m = 8 * (i % BlockHeight); m < 8 * (i % BlockHeight + 1); m++)
								for (n = 16*j; n < 16*(j+1); n++)
								{
									sum = sum + (uint8_t)*(pImageBuffer+ m*4096 + n);
								}
							sum = sum / 128;
								 
							temp1 = log(1 + (float)sum) / log(2.0);
							temp2 = (b * log(c)) / log(2.0);
							sum = a + temp1 / temp2;
							
							BigImage[i][j] = sum;
							sum = 0;
						}
					}
					imagecount ++;
				}  
				
				if (cameras[ cameraContextValue ].GetDeviceInfo().GetSerialNumber() == Serial2)
				{
					pImageBuffer1 = (uint8_t *) ptrGrabResult->GetBuffer();
					for (i = (imagecount1 % BlockNum) * BlockHeight; i < (imagecount1 % BlockNum + 1) * BlockHeight; i ++)
					{
						for (j = 256; j < 512; j ++)
						{
							sum = 0;
							for(m = 8 * (i % BlockHeight); m < 8 * (i % BlockHeight + 1); m++)
							{
								for (n = 16*(j % 256); n < 16*(j % 256+1); n++)
								{
									sum = sum + (uint8_t)*(pImageBuffer1+ m*4096 + n);
								}
							}
							sum = sum / 128;

							temp1 = log(1 + (float)sum) / log(2.0);
							temp2 = (b * log(c)) / log(2.0);
							sum = a + temp1 / temp2;

							BigImage[i][j] = sum;
								
						}
					}
					imagecount1 ++;
				}			
				
				count++;			
				
				}
				else

				{
					cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << endl;
				}
		    }
			cameras.StopGrabbing();
			cameras.Close();
			cameras.DetachDevice();
			cameras.DestroyDevice();
			
			pImageBuffer = NULL;
			delete pImageBuffer;
			pImageBuffer1 = NULL;
			delete pImageBuffer1;
//			}
			
		}
		catch(GenICam::GenericException &e)
		{
			cerr << "An exception occurred." << endl
				<< e.GetDescription() << endl;
		}
//	}//while
	
	return 0;
}
UINT GrabImageThreadC(LPVOID p)
{
	
	PARAM *info = (PARAM *)p;
	 GENAPIC_RESULT              res;                      /* Return value of pylon methods. */ 
    size_t                      numDevicesAvail;          /* Number of available devices. */
    bool                        isAvail;                  /* Used for checking feature availability. */
    int                         deviceIndex;              /* Index of device used in following variables. */
    PYLON_WAITOBJECTS_HANDLE    wos;                      /* Wait objects. */
    HANDLE                      hTimer;                   /* Grab timer. */
    PYLON_WAITOBJECT_HANDLE     woTimer;                  /* Timer wait object. */

    /* These are camera specific variables */
    PYLON_DEVICE_HANDLE         hDev[NUM_DEVICES];        /* Handle for the pylon device. */
    PYLON_STREAMGRABBER_HANDLE  hGrabber[NUM_DEVICES];    /* Handle for the pylon stream grabber. */
    unsigned char              *buffers[NUM_DEVICES][NUM_BUFFERS];    /* Buffers used for grabbing. */
    PYLON_STREAMBUFFER_HANDLE   bufHandles[NUM_DEVICES][NUM_BUFFERS]; /* Handles for the buffers. */

    /* Before using any pylon methods, the pylon runtime must be initialized. */
    Pylon::PylonInitialize();

    /* Enumerate all devices. You must call 
    PylonEnumerateDevices() before creating a device. */
    res = PylonEnumerateDevices( &numDevicesAvail );
    CHECK(res);
    if ( numDevicesAvail < NUM_DEVICES)
    {
        fprintf( stderr, "Not enough devices found. Found %i devices. At least %i devices needed to run this sample.\n", numDevicesAvail, NUM_DEVICES );
        Pylon::PylonTerminate(true);
        pressEnterToExit();
        exit(EXIT_FAILURE);
    }

    /* Create wait objects (must be done outside of the loop). */
    res = PylonWaitObjectsCreate(&wos);
    CHECK(res);

    /* In this sample, we want to grab for a given amount of time, then stop.
    Create a Windows timer, wrap it in a pylon C wait object, and add it to
    the wait object set. */
    hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
    if (hTimer == NULL)
        printErrorAndExit(GENAPI_E_FAIL);
    res = PylonWaitObjectFromW32(hTimer, 0, &woTimer);
    CHECK(res);
    res = PylonWaitObjectsAdd(wos, woTimer, NULL);
    CHECK(res);

    /* Open cameras and set parameters. */
    for (deviceIndex = 0; deviceIndex < NUM_DEVICES; ++deviceIndex)
    {
        PylonDeviceInfo_t di;
        
        /* Get a handle for the device. */
        res = PylonCreateDeviceByIndex( deviceIndex, &hDev[deviceIndex] );
        CHECK(res);

        /* Before using the device, it must be opened. Open it for setting
        parameters and for grabbing images. */
        res = PylonDeviceOpen( hDev[deviceIndex], PYLONC_ACCESS_MODE_CONTROL | PYLONC_ACCESS_MODE_STREAM );
        CHECK(res);

        /* Print out the name of the camera we are using. */
        {
            char buf[256];
            size_t siz = sizeof(buf);
            bool isReadable;

            isReadable = PylonDeviceFeatureIsReadable(hDev[deviceIndex], "DeviceModelName");
            if ( isReadable )
            {
                res = PylonDeviceFeatureToString( hDev[deviceIndex], "DeviceModelName", buf, &siz );
                CHECK(res);
                printf("Using camera '%s'\n", buf);
            }
        }

        /* Set the pixel format to Mono8, where gray values will be output as 8 bit values for each pixel. */
        /* ... First check to see if the device supports the Mono8 format. */
        isAvail = PylonDeviceFeatureIsAvailable(hDev[deviceIndex], "EnumEntry_PixelFormat_Mono8");
        if ( ! isAvail )
        {
            /* Feature is not available. */
            fprintf(stderr, "Device doesn't support the Mono8 pixel format");
            Pylon::PylonTerminate(true);
            pressEnterToExit();
            exit (EXIT_FAILURE);
        }
        
        /* ... Set the pixel format to Mono8. */
        res = PylonDeviceFeatureFromString( hDev[deviceIndex], "PixelFormat", "Mono8" );
        CHECK(res);

        /* Disable acquisition start trigger if available. */
        isAvail = PylonDeviceFeatureIsAvailable( hDev[deviceIndex], "EnumEntry_TriggerSelector_AcquisitionStart");
        if (isAvail)
        {
            res = PylonDeviceFeatureFromString( hDev[deviceIndex], "TriggerSelector", "AcquisitionStart");
            CHECK(res);
            res = PylonDeviceFeatureFromString( hDev[deviceIndex], "TriggerMode", "Off");
            CHECK(res);
        }
    
        /* Disable frame burst start trigger if available. */
        isAvail = PylonDeviceFeatureIsAvailable( hDev[deviceIndex], "EnumEntry_TriggerSelector_FrameBurstStart");
        if (isAvail)
        {
            res = PylonDeviceFeatureFromString( hDev[deviceIndex], "TriggerSelector", "FrameBurstStart");
            CHECK(res);
            res = PylonDeviceFeatureFromString( hDev[deviceIndex], "TriggerMode", "Off");
            CHECK(res);
        }

        /* Disable frame start trigger if available. */
        isAvail = PylonDeviceFeatureIsAvailable( hDev[deviceIndex], "EnumEntry_TriggerSelector_FrameStart");
        if (isAvail)
        {
            res = PylonDeviceFeatureFromString( hDev[deviceIndex], "TriggerSelector", "FrameStart");
            CHECK(res);
            res = PylonDeviceFeatureFromString( hDev[deviceIndex], "TriggerMode", "Off");
            CHECK(res);
        }

        /* We will use the Continuous frame mode, i.e., the camera delivers
        images continuously. */
        res = PylonDeviceFeatureFromString( hDev[deviceIndex], "AcquisitionMode", "Continuous" );
        CHECK(res);

        
        
        res = PylonDeviceGetDeviceInfo( hDev[deviceIndex], &di);
        CHECK(res);
        if (strcmp(di.DeviceClass, "BaslerGigE") == 0)
        {
            /* For GigE cameras, we recommend increasing the packet size for better 
               performance. When the network adapter supports jumbo frames, set the packet 
               size to a value > 1500, e.g., to 8192. In this sample, we only set the packet size
               to 1500.
            
               Also we set the Inter-Packet and the Frame Transmission delay
               so the switch can line up packets better.
            */

            res = PylonDeviceSetIntegerFeature( hDev[deviceIndex], "GevSCPSPacketSize", GIGE_PACKET_SIZE );
            CHECK(res);
    
            res = PylonDeviceSetIntegerFeature( hDev[deviceIndex], "GevSCPD", (GIGE_PACKET_SIZE + GIGE_PROTOCOL_OVERHEAD)*(NUM_DEVICES-1) );
            CHECK(res);

            res = PylonDeviceSetIntegerFeature( hDev[deviceIndex], "GevSCFTD", (GIGE_PACKET_SIZE + GIGE_PROTOCOL_OVERHEAD) * deviceIndex );
            CHECK(res);
        } else if (strcmp(di.DeviceClass, "Basler1394") == 0)
        {
            /* For FireWire we just set the PacketSize node to limit the bandwidth we're using. */
            
            /* We first divide the available bandwidth (4915 for FW400, 9830 for FW800)
               by the number of devices we are using. */
            int64_t newPacketSize = 4915 / NUM_DEVICES;
            int64_t recommendedPacketSize = 0;

            /* Get the recommended packet size from the camera. */
            res = PylonDeviceGetIntegerFeature( hDev[deviceIndex], "RecommendedPacketSize", &recommendedPacketSize );
            CHECK(res);

            if (newPacketSize < recommendedPacketSize)
            {
                /* Get the increment value for the packet size.
                   We must make sure that the new value we're setting is dividable by the increment of that feature. */
                int64_t packetSizeInc = 0;
                res = PylonDeviceGetIntegerFeatureInc( hDev[deviceIndex], "PacketSize", &packetSizeInc);
                CHECK(res);

                /* Adjust the new packet size so is dividable by its increment. */
                newPacketSize -= newPacketSize % packetSizeInc;
            }
            else
            {
                /* The recommended packet size should always be valid. Accordingly, there will be no need to check against the increment. */
                newPacketSize = recommendedPacketSize;
            }


            /* Set the new packet size. */
            res = PylonDeviceSetIntegerFeature( hDev[deviceIndex], "PacketSize", newPacketSize);
            CHECK(res);

            printf("Using packetsize: %lld\n", newPacketSize);
        }
    }


    /* Allocate and register buffers for grab. */
    for (deviceIndex = 0; deviceIndex < NUM_DEVICES; ++deviceIndex)
    {
        size_t i;
        PYLON_WAITOBJECT_HANDLE hWait;
        int32_t payloadSize;


        /* Determine the required size of the grab buffer. */
        res = PylonDeviceGetIntegerFeatureInt32( hDev[deviceIndex], "PayloadSize", &payloadSize );
        CHECK(res);


        /* Image grabbing is done using a stream grabber.  
          A device may be able to provide different streams. A separate stream grabber must 
          be used for each stream. In this sample, we create a stream grabber for the default 
          stream, i.e., the first stream ( index == 0 ).
          */

        /* Get the number of streams supported by the device and the transport layer. */
        res = PylonDeviceGetNumStreamGrabberChannels( hDev[deviceIndex], &i );
        CHECK(res);
        if ( i < 1 )
        {
            fprintf( stderr, "The transport layer doesn't support image streams.\n");
            Pylon::PylonTerminate(true);
            pressEnterToExit();
            exit(EXIT_FAILURE);
        }

        /* Create and open a stream grabber for the first channel. */ 
        res = PylonDeviceGetStreamGrabber( hDev[deviceIndex], 0, &hGrabber[deviceIndex] );
        CHECK(res);
        res = PylonStreamGrabberOpen( hGrabber[deviceIndex] );
        CHECK(res);

        /* Get a handle for the stream grabber's wait object. The wait object
           allows waiting for buffers to be filled with grabbed data. */
        res = PylonStreamGrabberGetWaitObject( hGrabber[deviceIndex], &hWait );
        CHECK(res);

        /* Add the stream grabber's wait object to our wait objects.
           This is needed to be able to wait until at least one camera has 
           grabbed an image in the grab loop below. */
        res = PylonWaitObjectsAdd(wos, hWait, NULL);
        CHECK(res);


        /* Allocate memory for grabbing.  */
        for ( i = 0; i < NUM_BUFFERS; ++i )
        {
            buffers[deviceIndex][i] = (unsigned char *) malloc ( payloadSize );
            if ( NULL == buffers[deviceIndex][i] )
            {
                fprintf( stderr, "Out of memory.\n" );
                Pylon::PylonTerminate(true);
                pressEnterToExit();
                exit(EXIT_FAILURE);
            }
        }

        /* We must tell the stream grabber the number and size of the buffers 
            we are using. */
        /* .. We will not use more than NUM_BUFFERS for grabbing. */
        res = PylonStreamGrabberSetMaxNumBuffer( hGrabber[deviceIndex], NUM_BUFFERS );
        CHECK(res);
        /* .. We will not use buffers bigger than payloadSize bytes. */
        res = PylonStreamGrabberSetMaxBufferSize( hGrabber[deviceIndex], payloadSize );
        CHECK(res);


        /*  Allocate the resources required for grabbing. After this, critical parameters 
            that impact the payload size must not be changed until FinishGrab() is called. */
        res = PylonStreamGrabberPrepareGrab( hGrabber[deviceIndex] );
        CHECK(res);


        /* Before using the buffers for grabbing, they must be registered at
           the stream grabber. For each registered buffer, a buffer handle
           is returned. After registering, these handles are used instead of the
           raw pointers. */
        for ( i = 0; i < NUM_BUFFERS; ++i )
        {
            res = PylonStreamGrabberRegisterBuffer( hGrabber[deviceIndex], buffers[deviceIndex][i], payloadSize,  &bufHandles[deviceIndex][i] );
            CHECK(res);
        }

        /* Feed the buffers into the stream grabber's input queue. For each buffer, the API 
           allows passing in a pointer to additional context information. This pointer
           will be returned unchanged when the grab is finished. In our example, we use the index of the 
           buffer as context information. */
        for ( i = 0; i < NUM_BUFFERS; ++i )
        {
            res = PylonStreamGrabberQueueBuffer( hGrabber[deviceIndex], bufHandles[deviceIndex][i], (void *) i );
            CHECK(res);
        }
    }


    /* The stream grabber is now prepared. As soon the camera starts to acquire images,
       the image data will be grabbed into the provided buffers.  */
    for (deviceIndex = 0; deviceIndex < NUM_DEVICES; ++deviceIndex)
    {
        /* Let the camera acquire images. */
        res = PylonDeviceExecuteCommandFeature( hDev[deviceIndex], "AcquisitionStart");
        // do not call CHECK() here! Instead exit the loop
        if (res != GENAPI_E_OK)
        {
            break;
        }
    }

    
    /* Only start the grab loop if all cameras have been "started" */
    if (res == GENAPI_E_OK)
    {
        unsigned int nGrabs = 0;
        LARGE_INTEGER intv;
        
        /* Set the timer to 5 s and start it. */
        intv.QuadPart = -50000000I64;
        if (!SetWaitableTimer(hTimer, &intv, 0, NULL, NULL, FALSE))
            printErrorAndExit(GENAPI_E_FAIL);

        /* Grab until the timer expires. */
        for (;;)
        {
            bool isReady;
            size_t woidx;
            unsigned char min, max;
            PylonGrabResult_t grabResult;

            /* Wait for the next buffer to be filled. Wait up to 1000 ms. */
            res = PylonWaitObjectsWaitForAny(wos, 1000, &woidx, &isReady );
            CHECK(res);
            if ( !isReady )
            {
                /* Timeout occurred. */
                fputs("Grab timeout occurred.\n", stderr);
                break; // Stop grabbing.
            }

            /* If the timer has expired, exit the grab loop */
            if (woidx == 0) {
                fputs("Game over.\n", stderr);
                break;  /* timer expired */
            }

            /* Account for the timer. */
            --woidx;

            /* Retrieve the grab result. */
            res = PylonStreamGrabberRetrieveResult( hGrabber[woidx], &grabResult, &isReady );
            CHECK(res);
            if ( !isReady )
            {
                /* Oops. No grab result available? We should never have reached this point. 
                   Since the wait operation above returned without a timeout, a grab result 
                   should be available. */
                fprintf(stderr, "Failed to retrieve a grab result\n");
                break;
            }

            /* Check to see if the image was grabbed successfully. */
            if ( grabResult.Status == EPylonGrabStatus::Grabbed )
            {
                /*  Success. Perform image processing. Since we passed more than one buffer
                to the stream grabber, the remaining buffers are filled while
                we do the image processing. The processed buffer won't be touched by
                the stream grabber until we pass it back to the stream grabber. */

                /* Pointer to the buffer attached to the grab result
                   Get the buffer pointer from the result structure. Since we also got the buffer index, 
                   we could alternatively use buffers[bufferIndex]. */
                unsigned char* buffer = (unsigned char*) grabResult.pBuffer;

                /* Perform processing. */
				
				 
				{
					int i, j, k;
					for (k=0; k<TexImageWidth/TexImageHeight; k++)
					{
						for (i=0;i<TexImageHeight;i++)
						{
							for (j=0;j<TexImageHeight;j++)
							{
								TexBlock[(imagecount-1)% TextureNumPerFrame][k].TextureImageBlock[i][j]
								=(uint8_t)*(buffer+i*TexImageWidth+j);
							}

						}
					}	
					imagecount ++;
				}
				getMinMax( buffer, grabResult.SizeX, grabResult.SizeY, &min, &max );
				printf("Grabbed frame #%2u from camera %2u into buffer %2d. Min. val=%3u, Max. val=%3u\n", 
					nGrabs, woidx, (int) grabResult.Context, min, max);

                /* Display image */
                res = PylonImageWindowDisplayImageGrabResult(woidx, &grabResult);
                CHECK(res);
            }
            else if ( grabResult.Status == EPylonGrabStatus::Failed )
            {
                fprintf( stderr,  "Frame %u wasn't grabbed successfully.  Error code = 0x%08X\n",
                    nGrabs, grabResult.ErrorCode );
            }

            /* Once finished with the processing, requeue the buffer to be filled again. */
            res = PylonStreamGrabberQueueBuffer( hGrabber[woidx], grabResult.hBuffer, grabResult.Context );
            CHECK(res);

            nGrabs++;
        }
    }
    
    
    /* Clean up. */

    /* Stop the image acquisition on the cameras. */
    for (deviceIndex = 0; deviceIndex < NUM_DEVICES; ++deviceIndex)
    {
        /*  ... Stop the camera. */
        res = PylonDeviceExecuteCommandFeature(hDev[deviceIndex], "AcquisitionStop");
        CHECK(res);
    }

    /* Remove all wait objects from waitobjects. */
    res = PylonWaitObjectsRemoveAll(wos);
    CHECK(res);
    res = PylonWaitObjectDestroy(woTimer);
    CHECK(res);
    res = PylonWaitObjectsDestroy(wos);
    CHECK(res);

    
    for (deviceIndex = 0; deviceIndex < NUM_DEVICES; ++deviceIndex)
    {
        size_t i;
        bool rdy;
        PylonGrabResult_t grabResult;

        /* ... We must issue a cancel call to ensure that all pending buffers are put into the
           stream grabber's output queue. */
        res = PylonStreamGrabberCancelGrab( hGrabber[deviceIndex] );
        CHECK(res);

        /* ... The buffers can now be retrieved from the stream grabber. */
        do 
        {
            res = PylonStreamGrabberRetrieveResult( hGrabber[deviceIndex], &grabResult, &rdy );
            CHECK(res);
        } while ( rdy );

        /* ... When all buffers are retrieved from the stream grabber, they can be deregistered.
               After deregistering the buffers, it is safe to free the memory. */

        for ( i = 0; i < NUM_BUFFERS; ++i )   
        {
            res = PylonStreamGrabberDeregisterBuffer( hGrabber[deviceIndex], bufHandles[deviceIndex][i] );
            CHECK(res);
            free( buffers[deviceIndex][i] );
        }

        /* ... Release grabbing related resources. */
        res = PylonStreamGrabberFinishGrab( hGrabber[deviceIndex] );
        CHECK(res);

        /* After calling PylonStreamGrabberFinishGrab(), parameters that impact the payload size (e.g., 
        the AOI width and height parameters) are unlocked and can be modified again. */

        /* ... Close the stream grabber. */
        res = PylonStreamGrabberClose( hGrabber[deviceIndex] );
        CHECK(res);


        /* ... Close and release the pylon device. The stream grabber becomes invalid
           after closing the pylon device. Don't call stream grabber related methods after 
           closing or releasing the device. */
        res = PylonDeviceClose( hDev[deviceIndex] );
        CHECK(res);
        res = PylonDestroyDevice ( hDev[deviceIndex] );
        CHECK(res);
    }

    pressEnterToExit();

    /* ... Shut down the pylon runtime system. Don't call any pylon function after 
       calling PylonTerminate(). */
    Pylon::PylonTerminate(true); 

    return EXIT_SUCCESS;
	return 0;
}

void printErrorAndExit( GENAPIC_RESULT errc )
{
    char *errMsg;
    size_t length;

    /* Retrieve the error message. 
    ... First find out how big the buffer must be, */
    GenApiGetLastErrorMessage( NULL, &length );
    errMsg = (char*) alloca( length );
    /* ... and retrieve the message. */
    GenApiGetLastErrorMessage( errMsg, &length );

    fprintf( stderr, "%s (%#08x).\n", errMsg, errc);

    Pylon::PylonTerminate(true);  /* Releases all pylon resources. */
    pressEnterToExit();

    exit(EXIT_FAILURE);
}



/* Simple "image processing" function returning the minimum and maximum gray 
   value of an 8 bit gray value image. */
void getMinMax(unsigned char* pImg, int32_t width, int32_t height, 
               unsigned char* pMin, unsigned char* pMax)
{
    unsigned char min = 255;
    unsigned char max = 0;
    unsigned char val;
    const unsigned char *p;

    for ( p = pImg; p < pImg + width * height; p++ )
    {
        val = *p;
        if ( val > max ) 
           max = val;
        if ( val < min )
           min = val;
    }
    *pMin = min; 
    *pMax = max;
}

/* This function can be used to wait for user input at the end of the sample program. */
void pressEnterToExit(void)
{
    fprintf( stderr, "\nPress enter to exit.\n");
    while( getchar() != '\n');
}
//**********************DefineByCamera*******************************

void CControlView::OnClickedRadio1()
{
	// TODO: 在此添加控件通知处理程序代码
		
	AfxMessageBox(_T(" 即将切换到模拟显示模型！"),0,NULL);
	ShowStyle = 0;
}

void CControlView::OnBnClickedRadio2()
{
	// TODO: 在此添加控件通知处理程序代码
		
	AfxMessageBox(_T(" 即将切换到纹理显示模型！"),0,NULL);
	ShowStyle = 1;
}

void CControlView::OnBnClickedRadio3()
{
	// TODO: 在此添加控件通知处理程序代码
		
	AfxMessageBox(_T(" 即将切换到高程显示模型！"),0,NULL);
	ShowStyle = 2;
}





HBRUSH CControlView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (pWnd->GetDlgCtrlID() == IDC_STATICTip)
	{
		pDC -> SetTextColor(RGB(125, 125, 125));
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}




void CControlView::OnBnSetfocusRadio1()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_STATICTip)->SetWindowText(_T("(模拟模型)\n以模拟路面图像显示路面模型"));
}


void CControlView::OnBnSetfocusRadio3()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_STATICTip)->SetWindowText(_T("(高程模型)\n以高程模型显示路面\n红色为高海拔，蓝色为低海拔"));
}


void CControlView::OnBnSetfocusRadio2()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_STATICTip)->SetWindowText(_T("(纹理模型)\n以实时捕捉纹理图像显示路面模型"));
}


void CControlView::OnBnSetfocusButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_STATICTip)->SetWindowText(_T("(打开激光)\n打开激光扫描仪设备"));
}


void CControlView::OnBnSetfocusButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_STATICTip)->SetWindowText(_T("(关闭激光)\n关闭激光扫描仪设备"));
}


void CControlView::OnBnSetfocusButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_STATICTip)->SetWindowText(_T("(连接相机)\n连接Basler相机采集设备"));
}


void CControlView::OnBnSetfocusButton6()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_STATICTip)->SetWindowText(_T("(关闭相机)\n断开Basler相机采集设备"));
}


void CControlView::OnBnClickedButsave()
{
	// TODO: 在此添加控件通知处理程序代码


	
}





void CControlView::OnBnClickedRadio6()
{
	// TODO: 在此添加控件通知处理程序代码
	AfxMessageBox(_T(" 此模式下将开始存储激光数据！"),0,NULL);
	save_data_flag=true;

}


void CControlView::OnBnClickedRadio7()
{
	// TODO: 在此添加控件通知处理程序代码
	
	AfxMessageBox(_T(" 此模式下将不会存储激光数据！"),0,NULL);
	save_data_flag=false;
}


void CControlView::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CFormView::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData( TRUE); 
}




void CControlView::OnChangeEdit2()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CFormView::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData( TRUE); 
}


void CControlView::OnChangeEdit3()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CFormView::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData( TRUE); 
}
