#pragma once
#include <windows.h>   
#include "ContrlDlg.h"
#include "Device.h"
#include "Texture.h"
#include "MFCTest10_15View.h"
#include "MainFrm.h"
#include <pylon/PylonIncludes.h>
using namespace Pylon;

#include "DataProcess.h"
#include "afxwin.h"
#pragma warning(disable:4996)


// CControlView 窗体视图
//**********************DefineByCamera*******************************
//抓取图像的线程中传递的参数结构
class PARAM{
public:
	int c_countOfImagesToGrab;
	int c_maxCamerasToUse;
};

UINT GrabImageThread(LPVOID p);//抓取图像的线程函数
void Configure();//配置参数后抓取图像
UINT GrabImageThreadC(LPVOID p);//抓取图像的线程函数

const int TextureNumPerFrame = 10;//每一帧纵向的纹理数量

//**********************DefineByCamera*******************************
class CControlView : public CFormView
{
	DECLARE_DYNCREATE(CControlView)

protected:
	CControlView();           // 动态创建所使用的受保护的构造函数
	virtual ~CControlView();

public:
	enum { IDD = IDD_CONTROLVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
//	float m_color;
	afx_msg void OnBnClickedButton2();
//	float m_color2;
//	float m_color3;
	afx_msg void OnBnClickedStart();
private:
	
public:
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedStop();
	afx_msg void OnSelchangeCombo();
	CComboBox m_typeCombo;
	
	CButton stopBtn;
	CButton startBtn;
	afx_msg void OnBnClickedStartTexture();

//**********************DefineByCamera*******************************
public:
	//一次抓取的图片数量
	uint32_t c_countOfImagesToGrab;
	
	int Second;
	
	//使用的相机数量
	static const size_t c_maxCamerasToUse = 2;
	PARAM A;
	
protected:
	CWinThread* pThread;
//**********************DefineByCamera*******************************
public:
	afx_msg void OnBnClickedCloseTexture();
	int m_gain;
	int m_BlackLevel;
	int m_LineRate;
//	int m_AutoExposure;
	CButton m_ConnectCamera;
	CButton m_CutCamera;
	int m_ShowStyle;
	afx_msg void OnClickedRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio2();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	
	afx_msg void OnBnSetfocusRadio1();
	afx_msg void OnBnSetfocusRadio3();
	afx_msg void OnBnSetfocusRadio2();
	afx_msg void OnBnSetfocusButton3();
	afx_msg void OnBnSetfocusButton4();
	afx_msg void OnBnSetfocusButton5();
	afx_msg void OnBnSetfocusButton6();
	CButton m_ParameterSetOK;
	afx_msg void OnBnClickedButsave();

	CButton btnSave;
	

	int save_flag_radio;
	afx_msg void OnBnClickedRadio6();
	afx_msg void OnBnClickedRadio7();

	
	int character_data;
	afx_msg void OnEnChangeEdit1();
	


	static int save_character;
	static int save_config;
	static int chk1;
	static int chk2;
	static int chk3;
	static int chk4;

	int xStep;
	int yStep;

	bool save_data_flag;
	
	afx_msg void OnChangeEdit2();
	afx_msg void OnChangeEdit3();
};


