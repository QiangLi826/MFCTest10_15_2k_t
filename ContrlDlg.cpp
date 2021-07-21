// ContrlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCTest10_15View.h"
#include "MainFrm.h"
#include "ContrlDlg.h"
#include "afxdialogex.h"


// ContrlDlg 对话框

IMPLEMENT_DYNAMIC(ContrlDlg, CDialogEx)

ContrlDlg::ContrlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(ContrlDlg::IDD, pParent)
{

}

ContrlDlg::~ContrlDlg()
{
}

CBitmapButton m_btnX1;
BOOL ContrlDlg::OnInitDialog(){
	CDialog::OnInitDialog();
	
// 	SetIcon(m_hIcon, TRUE);			// Set big icon
// 	SetIcon(m_hIcon, FALSE);		// Set small icon

	//根据双色图片设置窗口形状（非黑色区域有效）



	SetRegion(GetDC(),IDD_BACK_RING,RGB(0,0,0));



	//居中
	/*CenterWindow();*/

	//播放背景音乐
	//sndPlaySound(TEXT("cf_bgm.wav"),SND_ASYNC);

 
	HDC hDC = CreateCompatibleDC(GetDC() -> GetSafeHdc());
	HRGN rl; 
	BeginPath(hDC);
	MoveToEx(hDC, 0, 32, NULL);
	LineTo(hDC, 16, 8);
	LineTo(hDC, 16, 16);
	LineTo(hDC, 48, 16);
	LineTo(hDC, 48, 48);
	LineTo(hDC, 16, 48);
	LineTo(hDC, 16, 56);
	LineTo(hDC, 0, 32);
	EndPath(hDC);
	rl = CreateRectRgn(0, 0, 63, 63);
	rl = PathToRegion(hDC);
	m_Btn1.Create(L"", WS_CHILD | WS_VISIBLE, CPoint(67, 112), rl, this, UNI_BTN1, 2, RGB(245, 240,183), RGB(255, 255, 0), RGB(255, 255, 0), GetSysColor(COLOR_BTNFACE)); 

	DeleteObject(rl);

	
	HRGN rt; 
	BeginPath(hDC);
	MoveToEx(hDC, 32, 0, NULL);
	LineTo(hDC, 8, 16);
	LineTo(hDC, 16, 16);
	LineTo(hDC, 16, 50);
	LineTo(hDC, 50, 50);
	LineTo(hDC, 50, 16);
	LineTo(hDC, 58, 16);
	LineTo(hDC, 32, 0);
	EndPath(hDC);
	rt = CreateRectRgn(0, 0, 63, 63);
	rt = PathToRegion(hDC);
	m_Btn2.Create(L"", WS_CHILD | WS_VISIBLE, CPoint(107.5, 68), rt, this, UNI_BTN2, 2,RGB(196,226,158), RGB(0, 255, 0), RGB(0, 255, 0), GetSysColor(COLOR_BTNFACE)); 
	DeleteObject(rt);

	HRGN cr;
	BeginPath(hDC);
	MoveToEx(hDC, 0, 16, NULL);
	LineTo(hDC, 32, 16);
	LineTo(hDC, 32, 8);
	LineTo(hDC, 48, 32);
	LineTo(hDC, 32, 56);
	LineTo(hDC, 32, 48);
	LineTo(hDC, 0, 48);
	LineTo(hDC, 0, 16);
	EndPath(hDC);
	cr = CreateRectRgn(0, 0, 63, 63);
	cr = PathToRegion(hDC);
	m_Btn3.Create(L"", WS_CHILD | WS_VISIBLE, CPoint(148, 112), cr, this, UNI_BTN3, 2, RGB(250, 207, 194), RGB(242, 64, 50),RGB(242, 64, 50), GetSysColor(COLOR_BTNFACE)); 

	HRGN rdd; 
	BeginPath(hDC);
	MoveToEx(hDC, 16, 0, NULL);
	LineTo(hDC, 16, 32);
	LineTo(hDC, 8, 32);
	LineTo(hDC, 32, 48);
	LineTo(hDC, 56, 32);
	LineTo(hDC, 48, 32);
	LineTo(hDC, 48, 0);
	LineTo(hDC, 6, 0);
	EndPath(hDC);
	rdd = CreateRectRgn(0, 0, 63, 63);
	rdd = PathToRegion(hDC);
	m_Btn4.Create(L"", WS_CHILD | WS_VISIBLE, CPoint(107, 150), rdd, this, UNI_BTN4, 2,RGB(151, 244, 219), RGB(6, 236, 228), RGB(6, 236, 228), GetSysColor(COLOR_BTNFACE));  
	DeleteObject(rdd);
	HRGN center;
	BeginPath(hDC);
	MoveToEx(hDC, 0, 0, NULL);
	LineTo(hDC, 28, 0);
	LineTo(hDC, 28, 28);
	LineTo(hDC, 0, 28);
	LineTo(hDC, 0, 0);
	EndPath(hDC);
	center = CreateRectRgn(0, 0, 28, 28);
	center = PathToRegion(hDC);

	m_Btn5.Create(L"复位", WS_CHILD | WS_VISIBLE, CPoint(117, 120), center, this, UNI_BTN5, 2,  RGB(208, 208, 208),RGB(174,174,174),RGB(174,174,174), GetSysColor(COLOR_BTNFACE)); 
	DeleteObject(center);


	return TRUE;  // return TRUE  unless you set the focus to a control
}
void ContrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	
}
void ContrlDlg::SetRegion(CDC* pDC, UINT bg, COLORREF TransColor)
{
	CDC dcMem;
	if(!dcMem.CreateCompatibleDC(pDC))
	{
		MessageBox(_T("创建兼容DC失败！"));
	}

	CBitmap bitmap;
	if(!bitmap.LoadBitmap(bg))
	{
		MessageBox(_T("加载位图失败！"));
	}

	if(!dcMem.SelectObject(&bitmap))
	{
		MessageBox(_T("选进设备描述表失败！"));
	}

	BITMAP bitmapinfo;
	bitmap.GetBitmap(&bitmapinfo);
	//把窗口设为图片的大小,去掉这个的话，那么程序分割会不正确
	MoveWindow(5,40,bitmapinfo.bmWidth,bitmapinfo.bmHeight,true); 
	//整体区域
	CRgn rgn;
	CRgn tmpRgn;
	rgn.CreateRectRgn(0,0,bitmapinfo.bmWidth,bitmapinfo.bmHeight);
	//从整体区域中剔除所有黑色像素区域
	for(int i=0;i<bitmapinfo.bmWidth;i++)
	{
		for(int j=0;j<bitmapinfo.bmHeight;j++)
		{
			COLORREF cl=dcMem.GetPixel(i,j);
			if(cl==986895)
			{
				tmpRgn.CreateRectRgn(i,j,i+1,j+1);
				rgn.CombineRgn(&rgn,&tmpRgn,RGN_XOR);
				tmpRgn.DeleteObject();
			}
		}
	}
	//设置窗口显示区域
	SetWindowRgn(rgn,true);
}

BEGIN_MESSAGE_MAP(ContrlDlg, CDialogEx)
	ON_WM_PAINT()
//	ON_WM_NCLBUTTONDOWN()
ON_WM_LBUTTONDOWN()
ON_BN_CLICKED(UNI_BTN5, &ContrlDlg::OnBn5Clicked)
ON_BN_CLICKED(UNI_BTN1, &ContrlDlg::OnBn1Clicked)
ON_BN_CLICKED(UNI_BTN2, &ContrlDlg::OnBn2Clicked)
ON_BN_CLICKED(UNI_BTN3, &ContrlDlg::OnBn3Clicked)
ON_BN_CLICKED(UNI_BTN4, &ContrlDlg::OnBn4Clicked)
END_MESSAGE_MAP()


// ContrlDlg 消息处理程序



void ContrlDlg::OnPaint(){

	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		//	dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC  dc(this);   
		CRect  rect;   
		GetWindowRect(&rect);   
		CDC  dcMem;   
		dcMem.CreateCompatibleDC(&dc);   
		CBitmap  bmpBackground;   
		bmpBackground.LoadBitmap(IDD_BACK_RING);  //背景图
		BITMAP  bitmap;   
		bmpBackground.GetBitmap(&bitmap);   
		CBitmap  *pbmpOld=dcMem.SelectObject(&bmpBackground); 
		dc.StretchBlt(0,0,rect.Width(),rect.Height(),&dcMem,0,0,bitmap.bmWidth,bitmap.bmHeight,SRCCOPY);
		dcMem.SelectObject(pbmpOld);
		dcMem.DeleteDC();
		bmpBackground.DeleteObject();
	}
}



void ContrlDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnLButtonDown(nFlags, point);
	 PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x, point.y));   
}


void ContrlDlg::OnBn5Clicked()
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame* pMainFrame=(CMainFrame*)AfxGetMainWnd(); //定位主窗口
	CMFCTest10_15View* pMainView=(CMFCTest10_15View*)pMainFrame->splitter1.GetPane(0,0);
	pMainView->reset();
	pMainFrame=NULL;
	delete pMainFrame;
	pMainView=NULL;
	delete pMainView;
}
void ContrlDlg::OnBn1Clicked()
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame* pMainFrame=(CMainFrame*)AfxGetMainWnd(); //定位主窗口
	CMFCTest10_15View* pMainView=(CMFCTest10_15View*)pMainFrame->splitter1.GetPane(0,0);
	pMainView->setTranslateX(pMainView->getTranslateX()+100);
	pMainFrame=NULL;
	delete pMainFrame;
	pMainView=NULL;
	delete pMainView;
}
void ContrlDlg::OnBn2Clicked()
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame* pMainFrame=(CMainFrame*)AfxGetMainWnd(); //定位主窗口
	CMFCTest10_15View* pMainView=(CMFCTest10_15View*)pMainFrame->splitter1.GetPane(0,0);
	pMainView->setTranslateY(pMainView->getTranslateY()-100);
	pMainFrame=NULL;
	delete pMainFrame;
	pMainView=NULL;
	delete pMainView;
}
void ContrlDlg::OnBn3Clicked()
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame* pMainFrame=(CMainFrame*)AfxGetMainWnd(); //定位主窗口
	CMFCTest10_15View* pMainView=(CMFCTest10_15View*)pMainFrame->splitter1.GetPane(0,0);
	pMainView->setTranslateX(pMainView->getTranslateX()-100);
	pMainFrame=NULL;
	delete pMainFrame;
	pMainView=NULL;
	delete pMainView;
}
void ContrlDlg::OnBn4Clicked()
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame* pMainFrame=(CMainFrame*)AfxGetMainWnd(); //定位主窗口
	CMFCTest10_15View* pMainView=(CMFCTest10_15View*)pMainFrame->splitter1.GetPane(0,0);
	pMainView->setTranslateY(pMainView->getTranslateY()+100);
	pMainFrame=NULL;
	delete pMainFrame;
	pMainView=NULL;
	delete pMainView;

}

