// MFCTest10_15.h : MFCTest10_15 Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������



// CMFCTest10_15App:
// �йش����ʵ�֣������ MFCTest10_15.cpp
//

class CMFCTest10_15App : public CWinApp
{
public:
	CMFCTest10_15App();
	
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
};

extern CMFCTest10_15App theApp;