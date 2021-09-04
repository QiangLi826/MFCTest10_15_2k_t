
// MyChartCtrlTestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyChartCtrlTest.h"
#include "MyChartCtrlTestDlg.h"
#include "afxdialogex.h"
#include "ChartCtrl/ChartAxis.h"
#include "ChartCtrl/ChartLineSerie.h"
#include <deque>
#include "ILD2300/SensorTest.h"

#define TIMER_TEMP_SHOW	1//��ͼ�ؼ���ʱ��1

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern std::deque<ILD2300_Infos_Buffer> g_ILD2300_infos_buffer_chart;
extern CCriticalSection criticalSectionILD2300Chart;




// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
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


// CMyChartCtrlTestDlg �Ի���



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


// CMyChartCtrlTestDlg ��Ϣ��������

BOOL CMyChartCtrlTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ׮����,ģ�����ݡ��ǵ�ɾ����
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
	

	
	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ����Ӷ���ĳ�ʼ������
	InitMain_Chart();
	AutoDraw();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMyChartCtrlTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	
	CDialogEx::OnSysCommand(nID, lParam);
	
}

// �����Ի���������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMyChartCtrlTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMyChartCtrlTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMyChartCtrlTestDlg::InitMain_Chart()
{
	CChartAxis *pAxis = NULL;
	//��ʼ������
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
/// \brief ��������
/// \param ptr ����ָ��
/// \param data ����ֵ
///
//ͨ�������������к�һλ���ݷŵ�ǰһλ 
//���½��������ݷŵ�����ĩβ
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
//��̬��ͼ����ͼ�ٶȽ�������ͨ�����ļ�ʱ�����ʱ�����	��������Ϊ��죩
void CMyChartCtrlTestDlg::AutoDraw()
{
	KillTimer(0);
	//��ʼ��������
	//ָ��һ��׼����0�������ڴ�����Ŀ�ʼ��ַ
	//׼����0�������ڴ�����Ĵ�С�����ֽ�������
	ZeroMemory(&m_HightSpeedChartArray, sizeof(double)*m_c_arrayLength);
	for (size_t i = 0; i < m_c_arrayLength; ++i)
	{
		m_X[i] = i;
	}
	m_count = m_c_arrayLength;
	m_pLineSerie->ClearSerie();
	SetTimer(TIMER_TEMP_SHOW, 0, NULL);
}
//���������
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
	// TODO:  �ڴ�������Ϣ������������/�����Ĭ��ֵ
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
		//LeftMoveArray(m_X, m_c_arrayLength, m_count);//X��ÿ�����������ƶ�1
		m_pLineSerie->AddPoints(m_X, m_HightSpeedChartArray, m_c_arrayLength);

		//������һ�ε�����
		g_bufferIndex = g_bufferIndex + 100; //������ʾ��ÿ��X����ʾһ����
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


