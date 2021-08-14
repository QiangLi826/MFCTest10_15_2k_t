// RDDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCTest10_15.h"
#include "RDDialog.h"
#include "afxdialogex.h"
#include "scanCONTROL2900/RD_Info.h"

#include <deque>
extern std::deque<RD_Mean_Info> m_dqRD_show;  
extern CCriticalSection criticalSectionRD_show;

// RDDialog 对话框

IMPLEMENT_DYNAMIC(RDDialog, CDialogEx)

RDDialog::RDDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(RDDialog::IDD, pParent)
{
	edit_rd = _T("");		
}

RDDialog::~RDDialog()
{
}

void RDDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_RD, edit_rd);
	DDX_Control(pDX, IDC_EDIT_RD, m_edit);
	
}


BEGIN_MESSAGE_MAP(RDDialog, CDialogEx)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


 UINT RDDialog::showRdData(LPVOID lpParamter)
{
	if (edit_rd.GetLength()>10000)
	{
		edit_rd = '\0';
	}

	criticalSectionRD_show.Lock();
	int size = m_dqRD_show.size();
	for (int i = 0 ; i< size ; i++)
	{
		RD_Mean_Info rd = m_dqRD_show.front();
		edit_rd += rd.toString();
		edit_rd += "\r\n";
		m_dqRD_show.pop_front();
	}
	
	criticalSectionRD_show.Unlock();

	UpdateData(FALSE);
	UpdateWindow();

	
	m_edit.LineScroll(m_edit.GetLineCount()-1,0);
	return 0;
}


 void RDDialog::OnTimer(UINT_PTR nIDEvent)
 {
	 // TODO: 在此添加消息处理程序代码和/或调用默认值

	 showRdData(NULL);
 }

 

 int RDDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
 {
	 if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		 return -1;

	 SetTimer(1,1000,NULL); 
	 return 0;
 }


 void RDDialog::OnClose()
 {
	 // TODO: 在此添加消息处理程序代码和/或调用默认值
	 KillTimer(1);

	 CDialogEx::OnClose();
 }
