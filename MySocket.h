#if !defined(AFX_MYSOKET_H__CF6DC064_7D4D_4F89_824F_7B5B668F2B94__INCLUDED_)
#define AFX_MYSOKET_H__CF6DC064_7D4D_4F89_824F_7B5B668F2B94__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MySoket.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMySocket command target

class CMySocket : public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	CMySocket();
	virtual ~CMySocket();

// Overrides
public:
	void SetParent(CDialog* pWnd);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMySocket)
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CMySocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
	virtual void OnAccept(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
private:
	CDialog* m_pWnd;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYSOKET_H__CF6DC064_7D4D_4F89_824F_7B5B668F2B94__INCLUDED_)
