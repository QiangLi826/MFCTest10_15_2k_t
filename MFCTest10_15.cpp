// MFCTest10_15.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "MFCTest10_15.h"
#include "MainFrm.h"

#include "MFCTest10_15Doc.h"
#include "MFCTest10_15View.h"
#include "serial/CnCommDlg.h"
#include "IRIDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCTest10_15App

BEGIN_MESSAGE_MAP(CMFCTest10_15App, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CMFCTest10_15App::OnAppAbout)
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// ��׼��ӡ��������
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_APP_ACCELERATION, &CMFCTest10_15App::OnAppAcceleration)
	ON_COMMAND(ID_GPS, &CMFCTest10_15App::OnGps)
	ON_COMMAND(ID_menu_iri, &CMFCTest10_15App::Onmenuiri)
END_MESSAGE_MAP()


// CMFCTest10_15App ����

CMFCTest10_15App::CMFCTest10_15App()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CMFCTest10_15App ����

CMFCTest10_15App theApp;


// CMFCTest10_15App ��ʼ��

BOOL CMFCTest10_15App::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
// 	CHAR *str="blue.ssk";
// 	skinppLoadSkin(str);
	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	LoadStdProfileSettings(4);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)
	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMFCTest10_15Doc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ��ܴ���
		RUNTIME_CLASS(CMFCTest10_15View));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);



	// ������׼������DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// ��������������ָ����������
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// �������к�׺ʱ�ŵ��� DragAcceptFiles
	//  �� SDI Ӧ�ó����У���Ӧ�� ProcessShellCommand  ֮����
	return TRUE;
}



// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

#include "SockDlg.h"
// �������жԻ����Ӧ�ó�������
void CMFCTest10_15App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();	
}




int CMFCTest10_15App::ExitInstance()
{
	// TODO: �ڴ����ר�ô����/����û���
	/*skinppExitSkin();*/
	return CWinApp::ExitInstance();
}


void CMFCTest10_15App::OnAppAcceleration()
{
	CSockDlg dlg;
	dlg.DoModal();
}


void CMFCTest10_15App::OnGps()
{
	// TODO: �ڴ���������������
	//dlg.DoModal();
	
    // ������ģ̬�Ի���ʵ��   
	if (dlg == NULL)
	{
		dlg = new CnCommDlg();		
		dlg->Create(CnCommDlg::IDD);   
	}
  
    
    // ��ʾ��ģ̬�Ի���   
    dlg->ShowWindow(SW_SHOW);   
	
}


void CMFCTest10_15App::Onmenuiri()
{
	IRIDialog dlg;
	dlg.DoModal();
}




