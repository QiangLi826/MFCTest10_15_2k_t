#pragma once
#include "afxwin.h"


// RDDialog 对话框

class RDDialog : public CDialogEx
{
	DECLARE_DYNAMIC(RDDialog)

public:
	RDDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~RDDialog();

// 对话框数据
	enum { IDD = IDD_DIALOG_RD };
	CString edit_rd;
	CEdit   m_edit;
	UINT showRdData(LPVOID lpParamter);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
};
