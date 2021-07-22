// SockDlg.h : header file
//

#if !defined(AFX_SOCKDLG_H__1D66753D_0749_4989_85AF_852AD41CDF2B__INCLUDED_)
#define AFX_SOCKDLG_H__1D66753D_0749_4989_85AF_852AD41CDF2B__INCLUDED_

#include "Resource.h"
#include "MySocket.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSockDlg dialog

class CSockDlg : public CDialog
{
// Construction
public:
	void OnClose();
	void OnReceive();
	void OnSend();
	void OnConnect();
	void OnAccept();
	CSockDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSockDlg)
	enum { IDD = IDD_SOCK_DIALOG };
	CListBox	m_ctlSent;
	CListBox	m_ctlRecvd;
	CButton	m_ctlConnect;
	CString	m_strMessage;
	CString	m_strName;
	int		m_iPort;
	CString	m_strEdit3;
	int		m_iFch;
	int		m_iChn;
	int		m_iGain;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSockDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSockDlg)
	virtual BOOL OnInitDialog();
	//afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBconnect();
	afx_msg void OnBclose();
	afx_msg void OnBsend();
	afx_msg void OnBcj();
	afx_msg void OnBcls();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CMySocket m_sConnectSocket;
	CMySocket m_sListenSocket;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOCKDLG_H__1D66753D_0749_4989_85AF_852AD41CDF2B__INCLUDED_)
