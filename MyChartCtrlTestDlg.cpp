
// MyChartCtrlTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyChartCtrlTest.h"
#include "MyChartCtrlTestDlg.h"
#include "afxdialogex.h"
#include "ChartCtrl/ChartAxis.h"
#include "ChartCtrl/ChartLineSerie.h"
#include <deque>
#include "ILD2300/SensorTest.h"

#define TIMER_TEMP_SHOW	1//绘图控件计时器1

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern std::deque<ILD2300_Infos_Buffer> g_ILD2300_infos_buffer_chart;
extern CCriticalSection criticalSectionILD2300Chart;




// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
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
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMyChartCtrlTestDlg 对话框



CMyChartCtrlTestDlg::CMyChartCtrlTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMyChartCtrlTestDlg::IDD, pParent)
	, m_c_arrayLength(2096)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyChartCtrlTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHART, m_Chart);
}

BEGIN_MESSAGE_MAP(CMyChartCtrlTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMyChartCtrlTestDlg 消息处理程序

BOOL CMyChartCtrlTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  桩代码,模拟数据。记得删除。
	///*for (int j = 0; j<10;j++)
	//{
	//	ILD2300_Infos_Buffer buffer;
	//	std::vector<ILD2300_Info>& infos_v = buffer.infos_v;
	//	for (int i = 0; i< 10000; i++)
	//	{
	//		ILD2300_Info tmp;
	//		tmp.scaledData = randf(56, 58);
	//		infos_v.push_back(tmp);
	//	}

	//	g_ILD2300_infos_buffer_chart.push_back(buffer);
	//}*/
	

	
	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	InitMain_Chart();
	AutoDraw();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMyChartCtrlTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	
	CDialogEx::OnSysCommand(nID, lParam);
	
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMyChartCtrlTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMyChartCtrlTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMyChartCtrlTestDlg::InitMain_Chart()
{
	CChartAxis *pAxis = NULL;
	//初始化坐标
	pAxis = m_Chart.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetAutomatic(true);
	
	pAxis = m_Chart.CreateStandardAxis(CChartCtrl::LeftAxis);
	//pAxis->SetAutomatic(true);
	pAxis->SetMinMax(0, 101);
	m_pLineSerie = m_Chart.CreateLineSerie();
	UpdateData(FALSE);
}

void CMyChartCtrlTestDlg::drawMoving()
{
	m_pLineSerie->ClearSerie();
	RandArray(m_HightSpeedChartArray, m_c_arrayLength);
	LeftMoveArray(m_X, m_c_arrayLength, m_count);
	m_pLineSerie->AddPoints(m_X, m_HightSpeedChartArray, m_c_arrayLength);
}
/// 
/// \brief 左移数组
/// \param ptr 数组指针
/// \param data 新数值
///
//通过遍历将数组中后一位数据放到前一位 
//将新进来的数据放到数组末尾
void CMyChartCtrlTestDlg::LeftMoveArray(double* ptr, size_t length, double data)
{
	for (size_t i = 1; i < length; ++i)
	{
		ptr[i - 1] = ptr[i];
	}
	ptr[length - 1] = data;
}

void CMyChartCtrlTestDlg::RandArray(double* ptr, size_t length)
{
	for (size_t i = 0; i < length; ++i)
	{
		ptr[i] = randf(-1, 1);
	}
}
//动态绘图，绘图速度较慢，可通过更改计时器间隔时间调整	（已设置为最快）
void CMyChartCtrlTestDlg::AutoDraw()
{
	KillTimer(0);
	//初始化函数：
	//指向一块准备用0来填充的内存区域的开始地址
	//准备用0来填充的内存区域的大小，按字节来计算
	ZeroMemory(&m_HightSpeedChartArray, sizeof(double)*m_c_arrayLength);
	for (size_t i = 0; i < m_c_arrayLength; ++i)
	{
		m_X[i] = i;
	}
	m_count = m_c_arrayLength;
	m_pLineSerie->ClearSerie();
	SetTimer(TIMER_TEMP_SHOW, 0, NULL);
}
//随机数生成
double CMyChartCtrlTestDlg::randf(double min, double max)
{
	int minInteger = (int)(min * 10000);
	int maxInteger = (int)(max * 10000);
	int randInteger = rand()*rand();
	int diffInteger = maxInteger - minInteger;
	int resultInteger = randInteger % diffInteger + minInteger;
	return resultInteger / 10000.0;
}


ILD2300_Infos_Buffer g_buffer;
int g_bufferIndex=-1;
int g_bufferSize=0;

void CMyChartCtrlTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (TIMER_TEMP_SHOW == nIDEvent)
	{
		if (g_bufferIndex < 0)
		{
			criticalSectionILD2300Chart.Lock();
			if (g_ILD2300_infos_buffer_chart.size() <=0)
			{
				criticalSectionILD2300Chart.Unlock();
				return;
			}
			g_buffer = g_ILD2300_infos_buffer_chart.front();
			criticalSectionILD2300Chart.Unlock();

			g_bufferIndex = 0;
			g_bufferSize = g_buffer.infos_v.size();
		}

		double data =  g_buffer.infos_v.at(g_bufferIndex).scaledData;

		++m_count;
		m_pLineSerie->ClearSerie();
		LeftMoveArray(m_HightSpeedChartArray, m_c_arrayLength, data);
		//LeftMoveArray(m_X, m_c_arrayLength, m_count);//X轴每次增长并且移动1
		m_pLineSerie->AddPoints(m_X, m_HightSpeedChartArray, m_c_arrayLength);

		//调整下一次的索引
		g_bufferIndex = g_bufferIndex + 100; //采样显示，每隔X个显示一个。
		if (g_bufferIndex >= g_bufferSize)
		{
			criticalSectionILD2300Chart.Lock();			
			g_ILD2300_infos_buffer_chart.pop_front();
			if (g_ILD2300_infos_buffer_chart.size() <=0)
			{
				g_bufferIndex = -1;
				criticalSectionILD2300Chart.Unlock();
				return;
			}
			g_buffer = g_ILD2300_infos_buffer_chart.front();
			g_bufferIndex = 0;
			g_bufferSize = g_buffer.infos_v.size();
			criticalSectionILD2300Chart.Unlock();
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}



