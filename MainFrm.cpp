// MainFrm.cpp : CMainFrame ���ʵ��
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
	ID_SEPARATOR,           // ״̬��ָʾ��
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
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
		TRACE0("δ�ܴ���������\n");
		return -1;      // δ�ܴ���
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}

	// TODO: �������Ҫ��������ͣ������ɾ��������
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
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

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


// CMainFrame ��Ϣ�������




BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: �ڴ����ר�ô����/����û���

	if (!splitter1.CreateStatic(this, 1, 2))//�����ֱ��ʾ�����ڣ��ֳɼ��У����У�������ָ���������

	{
		//������ʧ�ܣ�����ӶԻ�����ʾ
		return FALSE;
	}


    /*
	//�ĸ��ָ�ڵĴ�����������δ����ĸ��Ӵ����õ���ͬһ����ͼ����ʾ��ʱ��������
	if (!splitter1.CreateView(0, 0, RUNTIME_CLASS(CControlView), CSize(100, 100), pContext) || 
		!splitter2.CreateStatic(&splitter1,2,2,WS_CHILD|WS_VISIBLE,splitter1.IdFromRowCol(0, 1))||  !splitter2.CreateView(0, 0, RUNTIME_CLASS(CMFCTest10_15View), CSize(350, 240), pContext) || 
		!splitter2.CreateView(1, 0, RUNTIME_CLASS(CMFCTest10_15View), CSize(350, 240), pContext) || 
		!splitter2.CreateView(0, 1, RUNTIME_CLASS(CMFCTest10_15View), CSize(350, 240), pContext) || 
		!splitter2.CreateView(1, 1, RUNTIME_CLASS(CMFCTest10_15View), CSize(350, 240),  pContext)) 
	{ 
		splitter1.DestroyWindow(); 
		return FALSE; 
	}*/

    //2���ָ�ڵĴ���
	CRect cr;
	GetClientRect(&cr);

	if (!splitter1.CreateView(0, 0, RUNTIME_CLASS(CMFCTest10_15View), CSize(cr.Width()*3/4, cr.Height()), pContext))
	{
		//������ʧ�ܣ�����ӶԻ�����ʾ
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

	// TODO: �ڴ˴������Ϣ����������
	//��Ӧ�����С�仯����Ϣ������Ŵ�ʱ�����ڵķָ�������Ŵ�����仯�����²���
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
	// TODO: �ڴ����ר�ô����/����û���
	nCmdShow=SW_SHOWMAXIMIZED;
	CFrameWnd::ActivateFrame(nCmdShow);
}
