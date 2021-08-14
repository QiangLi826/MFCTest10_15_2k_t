// MFCTest10_15.h : MFCTest10_15 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号
#include "serial/CnCommDlg.h"
#include "RDDialog.h"
#include "IRIDialog.h"

// CMFCTest10_15App:
// 有关此类的实现，请参阅 MFCTest10_15.cpp
//

class CMFCTest10_15App : public CWinApp
{
public:
	CMFCTest10_15App();
	CnCommDlg *cnCommDlg;
	RDDialog *rdDialog;
	IRIDialog *iriDialog;
	
// 重写
public:
	virtual BOOL InitInstance();

// 实现
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
	afx_msg void OnAppAcceleration();
	afx_msg void OnGps();
	afx_msg void Onmenuiri();	
	afx_msg void On32777();
};

extern CMFCTest10_15App theApp;

