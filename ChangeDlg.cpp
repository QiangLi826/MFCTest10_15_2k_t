// ChangeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCTest10_15.h"
#include "ChangeDlg.h"
#include "afxdialogex.h"


// ChangeDlg 对话框

IMPLEMENT_DYNAMIC(ChangeDlg, CDialog)

ChangeDlg::ChangeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ChangeDlg::IDD, pParent)
{

}

ChangeDlg::~ChangeDlg()
{
}

void ChangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ChangeDlg, CDialog)
END_MESSAGE_MAP()


// ChangeDlg 消息处理程序
