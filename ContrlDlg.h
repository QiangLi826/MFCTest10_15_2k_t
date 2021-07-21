#pragma once
#include <windows.h> 
#include <iostream>
#include "resource.h"
#include "MyButton.h"

using namespace std;
// ContrlDlg 对话框

class ContrlDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ContrlDlg)
public:
	CMyButton m_Btn1, m_Btn2, m_Btn3, m_Btn4, m_Btn5;
	enum {UNI_BTN1 = 1001, UNI_BTN2 = 1002, UNI_BTN3 = 1003, UNI_BTN4 = 1004, UNI_BTN5 = 1005};
public:
	ContrlDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ContrlDlg();

// 对话框数据
	enum { IDD = IDD_CONTRLDLG};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void SetRegion(CDC* pDC, UINT BackBitmapID, COLORREF TransColor);  
	BOOL  OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnBn5Clicked();
	afx_msg void OnBn1Clicked();
	afx_msg void OnBn2Clicked();
	afx_msg void OnBn3Clicked();
	afx_msg void OnBn4Clicked();

};
