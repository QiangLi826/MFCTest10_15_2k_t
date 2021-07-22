// SockDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SockDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSockDlg dialog

CSockDlg::CSockDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSockDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSockDlg)
	m_strMessage = _T("");
	m_strName = _T("");
	m_iPort = 0;
	m_strEdit3 = _T("");
	m_iFch = 0;
	m_iChn = 0;
	m_iGain = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSockDlg)
	DDX_Control(pDX, IDC_LSENT, m_ctlSent);
	DDX_Control(pDX, IDC_LRECVD, m_ctlRecvd);
	DDX_Control(pDX, IDC_BCONNECT, m_ctlConnect);
	DDX_Text(pDX, IDC_EMSG, m_strMessage);
	DDX_Text(pDX, IDC_ESERVNAME, m_strName);
	DDX_Text(pDX, IDC_ESERVPORT, m_iPort);
//	DDX_Text(pDX, IDC_EDIT3, m_strEdit3);
	DDX_Text(pDX, IDC_EFCH, m_iFch);
	DDV_MinMaxInt(pDX, m_iFch, 0, 255);
	DDX_Text(pDX, IDC_ECHN, m_iChn);
	DDV_MinMaxInt(pDX, m_iChn, 1, 256);
	DDX_Text(pDX, IDC_EGAIN, m_iGain);
	DDV_MinMaxInt(pDX, m_iGain, 1, 256);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSockDlg, CDialog)
	//{{AFX_MSG_MAP(CSockDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BCONNECT, OnBconnect)
	ON_BN_CLICKED(IDC_BCLOSE, OnBclose)
	ON_BN_CLICKED(IDC_BSEND, OnBsend)
	ON_BN_CLICKED(IDC_BCJ, OnBcj)
	ON_BN_CLICKED(IDC_BCLS, OnBcls)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSockDlg message handlers

BOOL CSockDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_strName="169.254.168.148";
	m_iPort=3333;
	m_strEdit3="";
	m_iFch=0;
	m_iChn=4;
	m_iGain=1;

	UpdateData(FALSE);
	m_sConnectSocket.SetParent (this);
	m_sListenSocket.SetParent (this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}



// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSockDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSockDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSockDlg::OnBconnect() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	GetDlgItem(IDC_BCONNECT)->EnableWindow(FALSE);

	m_sListenSocket.Create (m_iPort);
	m_sListenSocket.Listen ();
	
}

void CSockDlg::OnAccept()
{
	m_sListenSocket.Accept(m_sConnectSocket);
	GetDlgItem(IDC_EMSG)->EnableWindow(TRUE);
	GetDlgItem(IDC_BSEND)->EnableWindow(TRUE);
	GetDlgItem(IDC_BCLOSE)->EnableWindow(TRUE);
}

void CSockDlg::OnConnect()
{
	GetDlgItem(IDC_EMSG)->EnableWindow(TRUE);
	GetDlgItem(IDC_BCLOSE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BSEND)->EnableWindow(TRUE);

}

void CSockDlg::OnSend()
{

}

void CSockDlg::OnReceive()
{
	char *pBuf=new char[1025];
	int *pw=(int *)pBuf;
	int iBufSize=1024;
	int iRcvd,i;
	CString strRecvd;
	iRcvd=m_sConnectSocket.Receive (pBuf,iBufSize);
	if(iRcvd==SOCKET_ERROR)
	{
	}
	else
	{
		if(iRcvd>20){
//			pBuf[iRcvd]=NULL;
//			strRecvd=pBuf;
//			m_ctlRecvd.AddString (strRecvd);
//			UpdateData(FALSE);

	CString   output,strData;   
    
//	UpdateData(TRUE);

	for(int j=0;j<8;j++){
		for(i=0;i<m_iChn;i++){

			strData.Format("  %+2.6f ",(double)(pw[j*m_iChn+i])/65536.*.00030518/3.33333333);
		//	strData.Format("  %8X ",pw[j*m_iChn+i]);
		
			output.Insert(i*11,strData);
		}
		m_ctlRecvd.AddString(output);  //m_strMessage);

		UpdateData(FALSE);
		output.Empty();
	}
	
		}
		else
		{
			pBuf[iRcvd]=NULL;
			strRecvd=pBuf;
			m_ctlSent.AddString (strRecvd);
			UpdateData(FALSE);			
		}
	}
}

void CSockDlg::OnClose()
{
	m_sConnectSocket.Close ();
	GetDlgItem(IDC_EMSG)->EnableWindow(FALSE);
	GetDlgItem(IDC_BSEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATICMSG)->EnableWindow(FALSE);
	GetDlgItem(IDC_BCLOSE)->EnableWindow(FALSE);
}

void CSockDlg::OnBclose() 
{
	// TODO: Add your control notification handler code here
UCHAR c1[19];
int iSent;

	c1[0]=0x39;

		iSent=m_sConnectSocket.Send(LPCTSTR(c1),sizeof(c1));
		if(iSent==SOCKET_ERROR)
		{
			MessageBox("Send error");
		}
//	OnClose();
	
}

void CSockDlg::OnBsend() 
{
	// TODO: Add your control notification handler code here
	int iLen;
	int iSent;

	UpdateData(TRUE);
	if(m_strMessage !="")
	{
		iLen=m_strMessage.GetLength();
		iSent=m_sConnectSocket.Send(LPCTSTR(m_strMessage),iLen);
		if(iSent==SOCKET_ERROR)
		{
		}
		else
		{
			m_ctlSent.AddString(m_strMessage);
		
			UpdateData(FALSE);
		}

	}
	
}

void CSockDlg::OnBcj() 
{
	// TODO: Add your control notification handler code here
int iSent;
//int i;
int sum=2;
int count=3;
UCHAR c1[20];

int fr,blkn;

//short d1[512];
	UpdateData(TRUE);

fr=1000;   // 频率 1000Hz
blkn=2;     // 采样块数= 4

	c1[0]=149;  //
	c1[1]=0;
	c1[2]=0x0f;  // 4个通道同时采，见说明书  
	c1[3]=0;
	c1[4]=0;
	c1[5]=1;
	c1[6]=(unsigned char)(fr%256);
	c1[7]=(unsigned char)(fr/256);
    c1[8]=(unsigned char)(blkn%256);
	c1[9]=(unsigned char)(blkn/256);
	c1[10]=0;
    c1[11]=0;
	c1[12]=0;

		iSent=m_sConnectSocket.Send(LPCTSTR(c1),sizeof(c1));
		if(iSent==SOCKET_ERROR)
		{
			MessageBox("Send error");
		}



UINT idx=m_ctlRecvd.GetCount();
if(idx==0){
	CString   output,strData;   

	for(int j=0;j<8;j++){

			strData.Format("    CH%2d   ",j+m_iFch);
			output.Insert(j*11,strData);
		}
		m_ctlRecvd.AddString(output);  //m_strMessage);

		UpdateData(FALSE);
		output.Empty();
	}

}

void CSockDlg::OnBcls() 
{
	// TODO: Add your control notification handler code here
UINT indx =m_ctlRecvd.GetCount();
for(UINT i=0;i<indx;i++){
	m_ctlRecvd.DeleteString(1);
}
}
