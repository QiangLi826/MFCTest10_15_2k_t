// MFCTest10_15.h : MFCTest10_15 Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������
#include "serial/CnCommDlg.h"
#include "RDDialog.h"
#include "IRIDialog.h"

// CMFCTest10_15App:
// �йش����ʵ�֣������ MFCTest10_15.cpp
//

class CMFCTest10_15App : public CWinApp
{
public:
	CMFCTest10_15App();
	CnCommDlg *cnCommDlg;
	RDDialog *rdDialog;
	IRIDialog *iriDialog;
	
// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
	afx_msg void OnAppAcceleration();
	afx_msg void OnGps();
	afx_msg void Onmenuiri();	
	afx_msg void On32777();
};

extern CMFCTest10_15App theApp;

