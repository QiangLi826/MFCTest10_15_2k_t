// MySoket.cpp : implementation file
//

#include "stdafx.h"
#include "MySocket.h"
#include "SockDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMySocket

CMySocket::CMySocket()
{
}

CMySocket::~CMySocket()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CMySocket, CAsyncSocket)
	//{{AFX_MSG_MAP(CMySocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CMySocket member functions

void CMySocket::SetParent(CDialog *pWnd)
{
	m_pWnd=pWnd;
}

void CMySocket::OnAccept(int nErrorCode)
{
	if(nErrorCode==0)
		((CSockDlg*)m_pWnd)->OnAccept();
}


void CMySocket::OnConnect(int nErrorCode)
{
	if(nErrorCode==0)
		((CSockDlg*)m_pWnd)->OnConnect();	
}

void CMySocket::OnClose(int nErrorCode)
{
	if(nErrorCode==0)
		((CSockDlg*)m_pWnd)->OnClose();
}

void CMySocket::OnReceive(int nErrorCode)
{
	if(nErrorCode==0)
		((CSockDlg*)m_pWnd)->OnReceive();
}

void CMySocket::OnSend(int nErrorCode)
{
	if(nErrorCode==0)
		((CSockDlg*)m_pWnd)->OnSend();
}
