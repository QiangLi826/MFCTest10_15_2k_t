// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "MFCTest10_15.h"

#include "MainFrm.h"
#include "MFCTest10_15View.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	isInit = FALSE;
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}

	// TODO: 如果不需要工具栏可停靠，则删除这三行
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	HICON m_hIcon;

	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);

	SetIcon(m_hIcon, TRUE); // Set big icon

	SetIcon(m_hIcon, FALSE); // Set small icon

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}



#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 消息处理程序




BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (!splitter1.CreateStatic(this, 1, 2))//参数分别表示父窗口，分成几行，几列；将窗体分割成两个面板

	{
		//若创建失败，可添加对话框提示
		return FALSE;
	}


    /*
	//四个分割窗口的创建，但是这段代码四个子窗口用的是同一个视图，显示的时候有问题
	if (!splitter1.CreateView(0, 0, RUNTIME_CLASS(CControlView), CSize(100, 100), pContext) || 
		!splitter2.CreateStatic(&splitter1,2,2,WS_CHILD|WS_VISIBLE,splitter1.IdFromRowCol(0, 1))||  !splitter2.CreateView(0, 0, RUNTIME_CLASS(CMFCTest10_15View), CSize(350, 240), pContext) || 
		!splitter2.CreateView(1, 0, RUNTIME_CLASS(CMFCTest10_15View), CSize(350, 240), pContext) || 
		!splitter2.CreateView(0, 1, RUNTIME_CLASS(CMFCTest10_15View), CSize(350, 240), pContext) || 
		!splitter2.CreateView(1, 1, RUNTIME_CLASS(CMFCTest10_15View), CSize(350, 240),  pContext)) 
	{ 
		splitter1.DestroyWindow(); 
		return FALSE; 
	}*/

    //2个分割窗口的创建
	CRect cr;
	GetClientRect(&cr);

	if (!splitter1.CreateView(0, 0, RUNTIME_CLASS(CMFCTest10_15View), CSize(cr.Width()*3/4, cr.Height()), pContext))
	{
		//若创建失败，可添加对话框提示
		splitter1.DestroyWindow(); 
		return FALSE;
	}
	if (
		!splitter1.CreateView(0, 1, RUNTIME_CLASS(CControlView), CSize(cr.Width()/4, cr.Height()), pContext)
		) 
	{ 
		splitter1.DestroyWindow(); 
		return FALSE; 
	}
	
	isInit = TRUE;

	//return CFrameWnd::OnCreateClient(lpcs, pContext);
	
    return TRUE;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	//响应窗体大小变化的信息，窗体放大时，窗口的分隔情况随着窗体而变化而重新布局
	CRect cr;
	GetClientRect(&cr);

	if (isInit && nType!=SIZE_MINIMIZED)
	{
		splitter1.SetRowInfo(0, cy, 0);
		splitter1.SetColumnInfo(0, cr.Width()*3/4, 50);
		splitter1.SetColumnInfo(1, cr.Width()/4, 50);
		splitter1.RecalcLayout();
	}
	
	

}


void CMainFrame::ActivateFrame(int nCmdShow)
{
	// TODO: 在此添加专用代码和/或调用基类
	nCmdShow=SW_SHOWMAXIMIZED;
	CFrameWnd::ActivateFrame(nCmdShow);
}
