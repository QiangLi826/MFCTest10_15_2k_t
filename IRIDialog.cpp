// IRIDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCTest10_15.h"
#include "IRIDialog.h"
#include "afxdialogex.h"
#include "ILD2300/SensorTest.h"

#include <deque>
extern std::deque<IRI_Info> IRI_infos;
extern CCriticalSection criticalSectionIRI;



// IRIDialog 对话框

IMPLEMENT_DYNAMIC(IRIDialog, CDialogEx)

IRIDialog::IRIDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IRIDialog::IDD, pParent)
{
	edit_iri = _T("");		
}

IRIDialog::~IRIDialog()
{
	
}

void IRIDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_IRI, edit_iri);
	DDX_Control(pDX, IDC_EDIT_IRI, m_edit);
	
}


BEGIN_MESSAGE_MAP(IRIDialog, CDialogEx)		
	ON_BN_CLICKED(IDC_BUTTON1, &IRIDialog::OnBnClickedButton1)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


 UINT IRIDialog::showIriData(LPVOID lpParamter)
{
	if (edit_iri.GetLength()>10000)
	{
		edit_iri = '\0';
	}

	criticalSectionIRI.Lock();
	int size = IRI_infos.size();
	for (int i = 0 ; i< size ; i++)
	{
		IRI_Info iriInfo = IRI_infos.front();
		edit_iri += iriInfo.toString();
		edit_iri += "\r\n";
		IRI_infos.pop_front();
	}
	
	criticalSectionIRI.Unlock();

	UpdateData(FALSE);
	UpdateWindow();

	
	m_edit.LineScroll(m_edit.GetLineCount()-1,0);
	return 0;
}



 void IRIDialog::OnBnClickedButton1()
 {
	 
 }


 void IRIDialog::OnTimer(UINT_PTR nIDEvent)
 {
	 // TODO: 在此添加消息处理程序代码和/或调用默认值

	 showIriData(NULL);
 }

 

 int IRIDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
 {
	 if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		 return -1;

	 SetTimer(1,1000,NULL); 
	 return 0;
 }


 void IRIDialog::OnClose()
 {
	 // TODO: 在此添加消息处理程序代码和/或调用默认值
	 KillTimer(1);

	 CDialogEx::OnClose();
 }
