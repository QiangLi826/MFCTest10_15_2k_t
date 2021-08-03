#pragma once
#include "afxwin.h"


// IRIDialog �Ի���

class IRIDialog : public CDialogEx
{
	DECLARE_DYNAMIC(IRIDialog)

public:
	IRIDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~IRIDialog();


// �Ի�������
	enum { IDD = IDD_DIALOG_IRI };	
	CString edit_iri;
	CEdit   m_edit;
	UINT IRIDialog::showIriData(LPVOID lpParamter);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
};
