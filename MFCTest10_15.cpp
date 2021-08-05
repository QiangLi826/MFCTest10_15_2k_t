// MFCTest10_15.cpp : 定义应用程序的类行为。
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
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_APP_ACCELERATION, &CMFCTest10_15App::OnAppAcceleration)
	ON_COMMAND(ID_GPS, &CMFCTest10_15App::OnGps)
	ON_COMMAND(ID_menu_iri, &CMFCTest10_15App::Onmenuiri)
END_MESSAGE_MAP()


// CMFCTest10_15App 构造

CMFCTest10_15App::CMFCTest10_15App()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CMFCTest10_15App 对象

CMFCTest10_15App theApp;


// CMFCTest10_15App 初始化

BOOL CMFCTest10_15App::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
// 	CHAR *str="blue.ssk";
// 	skinppLoadSkin(str);
	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)
	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMFCTest10_15Doc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CMFCTest10_15View));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);



	// 分析标准外壳命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 SDI 应用程序中，这应在 ProcessShellCommand  之后发生
	return TRUE;
}



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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
// 用于运行对话框的应用程序命令
void CMFCTest10_15App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();	
}




int CMFCTest10_15App::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
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
	// TODO: 在此添加命令处理程序代码
	//dlg.DoModal();
	
    // 创建非模态对话框实例   
	if (dlg == NULL)
	{
		dlg = new CnCommDlg();		
		dlg->Create(CnCommDlg::IDD);   
	}
  
    
    // 显示非模态对话框   
    dlg->ShowWindow(SW_SHOW);   
	
}


void CMFCTest10_15App::Onmenuiri()
{
	IRIDialog dlg;
	dlg.DoModal();
}




