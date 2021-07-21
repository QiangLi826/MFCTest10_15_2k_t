// MFCTest10_15View.cpp : CMFCTest10_15View ���ʵ��
//

#include "stdafx.h"
#include "MFCTest10_15.h"

#include "MFCTest10_15Doc.h"
#include "MFCTest10_15View.h"
#include "Go2.h"
#include <glaux.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "glaux")

// CMFCTest10_15View

IMPLEMENT_DYNCREATE(CMFCTest10_15View, CView)

BEGIN_MESSAGE_MAP(CMFCTest10_15View, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()

	ON_MESSAGE(WM_MY_MESSAGE, AddMessageEx)

END_MESSAGE_MAP()



double Triang_Strip[ROW_COUNT][POINT_MAX_WIDTH];

//**********************DefineByCamera*******************************
extern GLubyte Image[TexImageHeight][TexImageHeight];
extern CTexture TexBlock[TextureNumPerFrame][TexImageWidth/TexImageHeight];
extern CTexture TexBlock1[TextureNumPerFrame][TexImageWidth/TexImageHeight];
extern int imagecount;
extern int imagecount1;
extern int ShowStyle;
int ic = 0;
int zMin=-30;
int zMax=450;
void zColorCompute(float z, float &r, float &g, float &b);
GLubyte BigImage[512][512];
GLubyte BigImageR[512][512];
//**********************DefineByCamera*******************************
// CMFCTest10_15View ����/����
extern int typeIndex;

CMFCTest10_15View::CMFCTest10_15View()
{
	// TODO: �ڴ˴���ӹ������
	this->m_GLPixelIndex = 0;
	this->m_hGLContext = NULL;
	m_Gain = 800;
    m_BlackLevel = 50;
	m_LineRate = 2000;

	PI = 3.1415926535897;
	tAngInc = PI / 90;                      // ÿ�δ�������¼��任�ĽǶ�ֵ
	tFovy = 45.0;
	prePt.SetPoint(-1, -1);                 // ��ʼ��prePt & nowPt(��OnMouseMove��ʹ��)
	nowPt.SetPoint(-1, -1);
	tVerticalAng = 0.;
	tHorizonAng = PI / 2;
	tRadius = 2500.0;
	tEyeX =0;
	tEyeY = 0;
	tEyeZ = 5000;
	tCenterX = 0.;
	tCenterY = 0.;
	tCenterZ = 0.;
	tUpX = 0.;
	tUpY = 1.0;
	tUpZ = 0.;

	devices=new Device();
	show_Type_Index=1;
	translateX=0;
	translateY=0;
	translateZ=0;

//**********************DefineByHaiyan*******************************
	TexPerLine = TexImageWidth / TexImageHeight;
	TextureBlockNum = TextureNumPerFrame * TexPerLine;
	//ָ�����һ��Ҫ�ڳ�ʼ����ʱ������ڴ�ռ�
	ImageIDs = new GLuint[TextureBlockNum];
	ImageID1 = new GLuint[TextureBlockNum];
	InitImage();
//*******************************************************************
	

	

}
bool CMFCTest10_15View::first=true;
CMFCTest10_15View::~CMFCTest10_15View()
{
	/*delete devices;*/
	//�ͷŰ󶨵�����ID���ͷŵĶ���һ��Ҫ��Ӧ��ȷ�����ͷ����ͷŵĶ��������ж�����
	if (ImageIDs != NULL)
	{
		delete []ImageIDs;
	}
	if (ImageID1 != NULL)
	{
		delete []ImageID1;
	}
}

BOOL CMFCTest10_15View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ
	//An OpenGL window must be created with the following flag   
    // and must not include CS_PARENTIDC for the class style.
    cs.style|=WS_CLIPSIBLINGS|WS_CLIPCHILDREN;
	return CView::PreCreateWindow(cs);
}

// CMFCTest10_15View ����

void CMFCTest10_15View::OnDraw(CDC* /*pDC*/)
{
	CMFCTest10_15Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	
	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
	
}


// CMFCTest10_15View ��ӡ

BOOL CMFCTest10_15View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CMFCTest10_15View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CMFCTest10_15View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}


// CMFCTest10_15View ���

#ifdef _DEBUG
void CMFCTest10_15View::AssertValid() const
{
	CView::AssertValid();
}

void CMFCTest10_15View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCTest10_15Doc* CMFCTest10_15View::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCTest10_15Doc)));
	return (CMFCTest10_15Doc*)m_pDocument;
}
#endif //_DEBUG


// CMFCTest10_15View ��Ϣ�������

bool CMFCTest10_15View::SetWindowPixelFormat(HDC hDC)
{
	PIXELFORMATDESCRIPTOR pixelDesc=
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0,0,0,0,0,0,
		0,
		0,
		0,
		0,0,0,0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0,0,0
	};

	this->m_GLPixelIndex = ChoosePixelFormat(hDC,&pixelDesc);
	if(this->m_GLPixelIndex==0)
	{
		this->m_GLPixelIndex = 1;
		if(DescribePixelFormat(hDC,this->m_GLPixelIndex,sizeof(PIXELFORMATDESCRIPTOR),&pixelDesc)==0)
		{
			return FALSE;
		}
	}

	if(SetPixelFormat(hDC,this->m_GLPixelIndex,&pixelDesc)==FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

int CMFCTest10_15View::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here
	SetTimer(1,40, NULL);

	HWND hWnd = this->GetSafeHwnd();    
	HDC hDC = ::GetDC(hWnd);
	if(this->SetWindowPixelFormat(hDC)==FALSE)
	{
		return 0;
	}
	if(this->CreateViewGLContext(hDC)==FALSE)
	{
		return 0;
	}
	return 0;
}

bool CMFCTest10_15View::CreateViewGLContext(HDC hDC)
{
	this->m_hGLContext = wglCreateContext(hDC);
	if(this->m_hGLContext==NULL)
	{//����ʧ��
		return FALSE;
	}

	if(wglMakeCurrent(hDC,this->m_hGLContext)==FALSE)
	{//ѡΪ��ǰRCʧ��
		return FALSE;
	}

	return TRUE;	

}
afx_msg BOOL CMFCTest10_15View::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
	int tWheelCount = zDelta / 120;
	if(tWheelCount > 0) {
		tFovy += 1.0;
	} else if(tWheelCount < 0) {
		tFovy -= 1.0;
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();                                            // �˴����䲻����glLoadIdentity()
	gluPerspective(tFovy, 1, 0.1, 10000.0);                        // ע��zNear,zFar��ȡֵ
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	return TRUE;
}


// �ڽ�����ʾ��Ļ��ͨ���������任�����ӽ�
afx_msg void CMFCTest10_15View::OnMouseMove(UINT nFlags, CPoint point) {
	if(nFlags && MK_LBUTTON == TRUE) {

		//MessageBox("mouse move function triggered!", "attentino", MB_OK);

		nowPt.x = point.x;
		nowPt.y = point.y;
		if(prePt.x!=-1 && prePt.y!=-1 && nowPt.x!=-1 && nowPt.y!=-1) {
			double tDx = nowPt.x - prePt.x;
			double tDy = nowPt.y - prePt.y;
			double tDis = sqrt(tDx*tDx+tDy*tDy);
			if(tDx > 0.) {
				tHorizonAng += tAngInc * tDx / tDis;
				if(tHorizonAng < 0.) { tHorizonAng += 2*PI; }
				if(tHorizonAng > 2*PI) { tHorizonAng -= 2*PI; }
			} else if(tDx < 0.) {
				tHorizonAng += tAngInc * tDx / tDis;
				if(tHorizonAng < 0.) { tHorizonAng += 2*PI; }
				if(tHorizonAng > 2*PI) { tHorizonAng -= 2*PI; }
			}
			if(tDy > 0.) {
				tVerticalAng = tVerticalAng + tAngInc * tDy / tDis;
				if(tVerticalAng > PI/2) { tVerticalAng = PI/2; }
			} else if(tDy < 0.) {
				tVerticalAng = tVerticalAng + tAngInc * tDy / tDis;
				if(tVerticalAng < -PI/2) { tVerticalAng = -PI/2; }
			}

			tEyeX = tRadius * cos(tVerticalAng) * cos(tHorizonAng);
			tEyeY = tRadius * sin(tVerticalAng);
			tEyeZ = tRadius * cos(tVerticalAng) * sin(tHorizonAng);
		}
		prePt.x = nowPt.x;
		prePt.y = nowPt.y;
	}
}

bool CMFCTest10_15View::InitGL(void)
{
	glShadeModel(GL_SMOOTH);                              // Enable Smooth Shading
	glClearColor(0.6,0.6,0.6,0.0);// Black Background
	glClearDepth(1.0f);                                    // Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);                            // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);                                // The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);    // Really Nice Perspective Calculations

	
	return TRUE;                                        // Initialization Went OK
}

void CMFCTest10_15View::OnDestroy()
{
	CView::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	if(wglGetCurrentContext()!=NULL)
	{
		wglMakeCurrent(NULL,NULL);
	}
	if(this->m_hGLContext!=NULL)//����ָ��Ҳ�ͷţ�
	{
		wglDeleteContext(this->m_hGLContext);
		this->m_hGLContext = NULL;
	}
}

void CMFCTest10_15View::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	/*
    OnSizeͨ��glViewport(0, 0, width, height)�������ӿں��ӿ����ꡣ
    glViewport�ĵ�һ�������������ӿ����½ǵ��������꣬�������ĸ��������ӿڵĿ�Ⱥ͸߶ȡ�

    OnSize�е�glMatrixMode���������þ���ģʽ�ģ���������ѡ�GL_MODELVIEW��GL_PROJECTION�� GL_TEXTURE��
    GL_MODELVIEW��ʾ��ʵ������ϵת����������ϵ��
    GL_PROJECTION��ʾ����������ϵת����������ϵ��
    GL_TEXTURE��ʾ�Ӷ������������ϵ��ճ�����������ϵ�ı任��

    glLoadIdentity��ʼ�����̾���(project matrix)
    gluOrtho2D�ѹ��̾������ó���ʾһ����άֱ����ʾ����
    */
    GLsizei width,height;
    width = cx;
    height = cy;
    if (height==0)                                        // Prevent A Divide By Zero By
    {
        height=1;                                        // Making Height Equal One
    }

    glViewport(0,0,width,height);                        // Reset The Current Viewport

    glMatrixMode(GL_PROJECTION);                        // Select The Projection Matrix
    glLoadIdentity();                                    // Reset The Projection Matrix
    // Calculate The Aspect Ratio Of The Window
    //gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);//͸��ͶӰ
	//glOrtho(-100,2000,-2000,500,-4000,4000);
	gluPerspective(45,width/height,0.1,10000);
    glMatrixMode(GL_MODELVIEW);                            // Select The Modelview Matrix
    glLoadIdentity();                                    // Reset The Modelview Matrix

}

LRESULT CMFCTest10_15View::AddMessageEx(WPARAM wParam, LPARAM lParam)
{
	int  newMsg = (int )wParam;
	if(newMsg == NULL)
		return -1;
	if (newMsg==1)
	{
		this->first=false;
	}
	
	return 0;
}
#include <stdio.h>
#include <direct.h>
#include <stdlib.h>
#include <memory>
int CMFCTest10_15View::CheckDir(char* Dir)
{
	FILE *fp = NULL;
	char TempDir[200];
	memset(TempDir,'\0',sizeof(TempDir));
	sprintf(TempDir,Dir);
	//strcat(TempDir,"\\");
	//strcat(TempDir,".temp.fortest");
	
	fp = fopen(TempDir,"w");
	if (!fp)
	{
		if(_mkdir(Dir)==0)
		{
			return 1;//�ļ��д����ɹ�
		}
		else
		{
			return -1;//can not make a dir;
		}
	}
	else
	{
		fclose(fp);
	}
	return 0;
}
void CMFCTest10_15View::laser(bool save_data_flag,int save_character,int character_data,int save_config,int chk1,int chk2,int chk3,int chk4,int xStep,int yStep){
	MyPARAM *myParam = new MyPARAM;
	myParam->hWnd=m_hWnd;
	
	myParam->save_character=save_character;
	myParam->character_data=character_data;

	CheckDir("data/");
	this->devices->init( save_data_flag,save_config,chk1,chk2,chk3,chk4,xStep,yStep);
	int Status=this->devices->startDevices( );
	if(Status==Device::APP_OK)
	{
		AfxMessageBox(_T("�豸���ӳɹ�,��ʼ���ݴ��䣡"),0,NULL);

	}else if (Status==Device::ConnectError)
	{
		AfxMessageBox(_T("�豸δ�����رգ����ȹر��豸��"),0,NULL);
		return;
	}
	
	this->devices->ScanData();


	Sleep(2000);
// 	CButton *pBtn= (CButton *)GetDlgItem(IDC_BUTTON2); //IDC_BUTTON2�����ť 
// 	if(pBtn!=NULL) 
// 	{ 
// 		pBtn->EnableWindow(FALSE); // True or False 
// 	}
// 	GetDlgItem(IDC_EDIT1)->EnableWindow(FALSE);
// 	GetDlgItem(IDC_EDIT2)->EnableWindow(FALSE);
// 	GetDlgItem(IDC_EDIT3)->EnableWindow(FALSE);
//	GetDlgItem(IDC_RADIO4)->EnableWindow(FALSE);
	processDataThread = AfxBeginThread(&ProcessData,myParam);
}

void CMFCTest10_15View::stopLaser()
{
	this->devices->stopDevices();

	AfxMessageBox(_T("�豸�رճɹ���ֹͣ�������ݣ�"),0,NULL);
	DWORD code;

	/*cout<<"�������رգ�"<<endl;*/
	if (!processDataThread)
	{
		GetExitCodeThread(processDataThread->m_hThread, &code);
		if(code == STILL_ACTIVE)
		{   
			Sleep(1000);
			TerminateThread(processDataThread->m_hThread, 0);
		}
		cout<<"Stop Process Thread"<<endl;
		
	}
	
}
void CMFCTest10_15View::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CView::OnPaint()
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    // Clear Screen And Depth Buffer
	glLoadIdentity();                    
	
	glClearColor(0,0,0,1);

	tEyeX = tRadius * cos(tVerticalAng) * cos(tHorizonAng);
	tEyeY = tRadius * sin(tVerticalAng);
	tEyeZ = tRadius * cos(tVerticalAng) * sin(tHorizonAng);
	gluLookAt(tEyeX, tEyeY, tEyeZ, tCenterX, tCenterY, tCenterZ, tUpX, tUpY, tUpZ);
	glTranslatef(translateX,translateY,translateZ);
	glRotatef(-55, 1, 0, 0);
	glTranslatef(0, 160, 0);



		DrawStripScene();
	

	SwapBuffers(dc.m_hDC);
	
	
}


//**********************DefineByCamera*******************************
int CMFCTest10_15View::DrawStripScene(void)
{
	glEnable(GL_DEPTH_TEST);	
// 	glCullFace(GL_BACK);
// 	glFrontFace(GL_CCW);
	if(typeIndex==0)
	{
		glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);
	}else
		glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);

	glShadeModel(GL_SMOOTH);
	float red = 0.65, green = 0.65, blue = 0.65;
	switch(ShowStyle)
	{
	case 0:  //��ʼ���ѡ��λͼӳ��
		UseBMPTexure();
		break ;  
	case 1:  //���������ѡ���Զ��������ӳ��
		UseOneImageTeture();
		break ;
	default: 
		break ;
	}

	int Width = 1700;
	int Heigth = 2400;
	float xZoom = 1.0;
	float yZoom = 1.0;	

	if (WholePointCount>0)
	{
		float startX=-850;
		float startY=1300;
		float dx=2*fabs(startX)/pc;
		float dy=2*fabs(startY)/ROW_COUNT;
		glBindTexture(GL_TEXTURE_2D, ImageIDs[0]);
			switch(ShowStyle)
			{
			case 0:  //��ʼ���ѡ��λͼӳ��
				glBindTexture(GL_TEXTURE_2D, ImageIDs[0]);
				break ;  
			case 1:  //���������ѡ���Զ��������ӳ��
				glBindTexture(GL_TEXTURE_2D, texName);
				break ;
			default: 
				break ;
			}
			criticalSectionOne.Lock();

			for (int j=0;j<ROW_COUNT-1;j++)
			{

				int j0=(j+0);
				int j1=j+1;
				float y0=startY-j*dy;
				float y1=y0-dy;

				glBegin(GL_TRIANGLE_STRIP);	
				for(int i=0;i<(pc/*/2 */- 1) ;i=i+1)
				{

					double x0=startX+i*dx;	
					double z00=Triang_Strip[j0][i];
					double z10=Triang_Strip[j1][i];

					if (ShowStyle == 2)
					{
						zColorCompute(z00, red, green, blue);
						
					}
					glColor3f(red, green,blue);
//					glTexCoord2f((x0 + 850) * xZoom/(850 - 1), (1200 - y0) * yZoom/(Heigth - 1));
					glTexCoord2f((x0 + 850) * xZoom/(1700 - 1), (1200 - y0) * yZoom/(Heigth - 1));
					glVertex3f(x0,y0,z00);

					if (ShowStyle == 2)
					{
						zColorCompute(z10, red, green, blue);
						
					}
					glColor3f(red, green,blue);
//					glTexCoord2f((x0 + 850) * xZoom/(850 - 1), (1200 - y1) * yZoom/(Heigth - 1));
					glTexCoord2f((x0 + 850) * xZoom/(1700 - 1), (1200 - y1) * yZoom/(Heigth - 1));
					glVertex3f(x0,y1,z10);

				}

				glEnd();
			}
			criticalSectionOne.Unlock();
// 			switch(ShowStyle)
// 			{
// 			case 0:  //��ʼ���ѡ��λͼӳ��
// 				glBindTexture(GL_TEXTURE_2D, ImageIDs[0]);
// 				break ;  
// 			case 1:  //���������ѡ���Զ��������ӳ��
// 				glBindTexture(GL_TEXTURE_2D, texNameR);
// 				break ;
// 			default: 
// 				break ;
// 			}
// 			
// 			for (int j=0;j<ROW_COUNT-1;j++)
// 			{
// 
// 				int j0=(Strip_Indx+j+0)%ROW_COUNT;
// 				int j1=(Strip_Indx+j+1)%ROW_COUNT;
// 				float y0=startY-j*dy;
// 				float y1=y0-dy;
// 
// 				glBegin(GL_TRIANGLE_STRIP);	
// 				for(int i=(pc/2 - 2);i<pc-2 ;i=i+1)
// 				{
// 
// 					double x0=startX+i*dx;	
// 					double z00=Triang_Strip[j0][i];
// 					double z10=Triang_Strip[j1][i];
// 
// 					if (ShowStyle == 2)
// 					{
// 						zColorCompute(z00, red, green, blue);
// 						
// 					}
// 					glColor3f(red, green,blue);
// 					glTexCoord2f((x0) * xZoom/(850 - 1), (1200 - y0) * yZoom/(Heigth - 1));
// 					glVertex3f(x0,y0,z00);
// 
// 					if (ShowStyle == 2)
// 					{
// 						zColorCompute(z10, red, green, blue);
// 						
// 					}
// 					glColor3f(red, green,blue);
// 					glTexCoord2f((x0) * xZoom/(850 - 1), (1200 - y1) * yZoom/(Heigth - 1));
// 					glVertex3f(x0,y1,z10);
// 
// 				}
// 
// 				glEnd();
// 			}
		
		}

	
	                  
	glDeleteTextures(1, &ImageIDs[0]);
// 	glDeleteTextures(TextureBlockNum, ImageIDs);
// 	glDeleteTextures(TextureBlockNum, ImageID1);
	glDeleteTextures(1, &texNameR);
	glDeleteTextures(1, &texName);
	return TRUE;                                                         
}
//**********************DefineByCamera*******************************


void CMFCTest10_15View::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	this->Invalidate(FALSE);
	CView::OnTimer(nIDEvent);

}
	

BOOL CMFCTest10_15View::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	return TRUE;
	//return CView::OnEraseBkgnd(pDC);
}
void CMFCTest10_15View::setTranslateX(GLfloat x)
{
	this->translateX=x;
	
}
void CMFCTest10_15View::setTranslateY(GLfloat y)
{
	this->translateY=y;

}
void CMFCTest10_15View::reset()
{
	this->translateZ=0;
	this->translateX=0;
	this->translateY=0;
	this->tEyeX=0;
	this->tEyeY=0;
	this->tEyeZ=5000;

}
GLfloat CMFCTest10_15View::getTranslateX()
{
	return translateX;
}
GLfloat CMFCTest10_15View::getTranslateY()
{
	return translateY;
}
//**********************DefineByCamera*******************************
void CMFCTest10_15View::TriangleStripFunc(float Xstart, int istart, int iend, float dx, float Ystart, int jstart, int jend, float dy)
{
	int i, j;

	float xZoom = 1.0;
	float yZoom = 1.0;

	POINT3 a;
	POINT3 b;
	POINT3 c;
	POINT3 d;
	POINT3 n;

	for (j = jstart; j < jend; j++)
	{
		float y0 = (-Ystart) + j*dy;
		float y1 = y0 + dy;

		glBegin(GL_TRIANGLE_STRIP);
		for(i = istart; i < iend; i++)
		{
			float x0 = Xstart + i*dx;
			float x1 = x0 + dx;	
			float z00 = Triang_Strip[j][i];
			float z10 = Triang_Strip[j+1][i];
			float z01 = Triang_Strip[j][i+1];
			float z11 = Triang_Strip[j+1][i+1];

			a.x = x0; a.y = y0; a.z = z00;
			b.x = x0; b.y = y1; b.z = z10;
			c.x = x1; c.y = y0; c.z = z01;
			d.x = x1; d.y = y1; d.z = z11;

			n = cross(a, b, c, n);
			glNormal3f(n.x, n.y, n.z);

			glColor3f(0.7,0.7,0.7);
			glTexCoord2f((x0 - (Xstart+istart*dx)) * xZoom/((iend-istart)*dx - 1), ((-Ystart+jend*dy) - 1 - y0) * yZoom/((jend-jstart)*dy - 1));
			glVertex3f(x0, y0, z00);
			


			n = cross(d, c, b, n);
			glNormal3f(n.x, n.y, n.z);

			glColor3f(0.7,0.7,0.7);
			glTexCoord2f((x0 - (Xstart+istart*dx)) * xZoom/((iend-istart)*dx - 1), ((-Ystart+jend*dy) - 1 - y1) * yZoom/((jend-jstart)*dy - 1));
			glVertex3f(x0, y1, z10);
			
		}
		glEnd();
	}	
}
void CMFCTest10_15View::TriangleStripFullFunc(float Xstart, int istart, int iend, float dx, float Ystart, int jstart, int jend, float dy)
{
	int i, j;
	float xZoom = 1.0;
	float yZoom = 1.0;

	POINT3 a;
	POINT3 b;
	POINT3 c;
	POINT3 d;
	POINT3 n;
	float red=0.7, green=0.7, blue=0.7;

	for (int j=jstart; j<jend;j++)
	{

		int j0=(Strip_Indx+j+0)%ROW_COUNT;
		int j1=(Strip_Indx+j+1)%ROW_COUNT;
		float y0=Ystart-j*dy;
		float y1=y0-dy;
		glBegin(GL_TRIANGLE_STRIP); 
		for(int i=istart; i<iend; i++)
		{
			double x0=Xstart+i*dx; 
			float x1 = x0 + dx;
			double z00=Triang_Strip[j0][i];
			double z10=Triang_Strip[j1][i];
			float z01 = Triang_Strip[j0][i+1];
			float z11 = Triang_Strip[j1][i+1];

			a.x = x0; a.y = y0; a.z = z00;
			b.x = x0; b.y = y1; b.z = z10;
			c.x = x1; c.y = y0; c.z = z01;	
			d.x = x1; d.y = y1; d.z = z11;

			n = cross(a, b, c, n);
			glNormal3f(n.x, n.y, n.z);
			if ( ShowStyle == 2)
			{
				zColorCompute(z00, red, green, blue);
			}
			
			glColor3f(red,green,blue);
//			glTexCoord2f((x0 - (Xstart+istart*dx)) * xZoom/((iend-istart)*dx - 1), ((-Ystart+jend*dy) - 1 - y0) * yZoom/((jend-jstart)*dy - 1));
			glTexCoord2f((x0 - (Xstart+istart*dx)) * xZoom/((iend-istart)*dx - 1), (y0 - (Ystart+jstart*dy)) * yZoom/((jend-jstart)*dy - 1));
			glVertex3f(x0,y0,z00);
			
			
			n = cross(d, c, b, n);
			glNormal3f(n.x, n.y, n.z);
			if ( ShowStyle == 2)
			{
				zColorCompute(z10, red, green, blue);
			}
			glColor3f(red,green,blue);
//			glTexCoord2f((x0 - (Xstart+istart*dx)) * xZoom/((iend-istart)*dx - 1), ((-Ystart+jend*dy) - 1 - y1) * yZoom/((jend-jstart)*dy - 1));
			glTexCoord2f((x0 - (Xstart+istart*dx)) * xZoom/((iend-istart)*dx - 1), (y1 - (Ystart+jstart*dy)) * yZoom/((jend-jstart)*dy - 1));
			glVertex3f(x0,y1,z10);
			

		}

		glEnd();
	}
}
POINT3 CMFCTest10_15View::cross(POINT3 a, POINT3 b, POINT3 c, POINT3 d)
{
	d.x = (b.y - a.y)*(c.z - a.z) - (b.z - a.z)*(c.y - a.y);
	d.y = (b.z - a.z)*(c.x - a.x) - (b.x - a.x)*(c.z - a.z);
	d.z = (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);

	d.normal();
	return d;
}
//���Զ��������������������
void CMFCTest10_15View::UseSelfDefineTexure(void)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);	
	glGenTextures(TextureBlockNum, ImageIDs);
	glGenTextures(TextureBlockNum, ImageID1);

	for (int i = 0; i<TextureNumPerFrame; i++)

	{
		for (int j = 0; j<TexPerLine; j++)
		{
			TexBlock[i][j].MakeMipImages();
			glBindTexture(GL_TEXTURE_2D, ImageIDs[i * TexPerLine + j]);
//		    glTexImage2D(GL_TEXTURE_2D, 0, 3, TexImagWidth, TexImagHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, Tex[i].TextureImage);
// 			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, TexImagWidth, TexImagHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, Tex[i].TextureImageLight);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, TexImageHeight, TexImageHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, TexBlock[i][j].TextureImageBlock);
			glTexImage2D(GL_TEXTURE_2D, 1, GL_LUMINANCE, 128, 128, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, TexBlock[i][j].MipmapImage128);
			glTexImage2D(GL_TEXTURE_2D, 2, GL_LUMINANCE, 64, 64, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, TexBlock[i][j].MipmapImage64);
			glTexImage2D(GL_TEXTURE_2D, 3, GL_LUMINANCE, 32, 32, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, TexBlock[i][j].MipmapImage32);
			glTexImage2D(GL_TEXTURE_2D, 4, GL_LUMINANCE, 16, 16, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, TexBlock[i][j].MipmapImage16);
			glTexImage2D(GL_TEXTURE_2D, 5, GL_LUMINANCE, 8, 8, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, TexBlock[i][j].MipmapImage8);
			glTexImage2D(GL_TEXTURE_2D, 6, GL_LUMINANCE, 4, 4, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, TexBlock[i][j].MipmapImage4);
			glTexImage2D(GL_TEXTURE_2D, 7, GL_LUMINANCE, 2, 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, TexBlock[i][j].MipmapImage2);
			glTexImage2D(GL_TEXTURE_2D, 8, GL_LUMINANCE, 1, 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, TexBlock[i][j].MipmapImage1);

//			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_LUMINANCE, TexImagHeight, TexImagHeight, GL_LUMINANCE, GL_UNSIGNED_BYTE, TexBlock[i][j].TextureImageBlock);
/*			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, TexImagHeight, TexImagHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, TexBlock[i][j].TextureImageBlock);
*/
			glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
			
		}

	}

	for (int i = 0; i<TextureNumPerFrame; i++)

	{
		for (int j = 0; j<TexPerLine; j++)
		{
			TexBlock1[i][j].MakeMipImages();
			glBindTexture(GL_TEXTURE_2D, ImageID1[i * TexPerLine + j]);
//		    glTexImage2D(GL_TEXTURE_2D, 0, 3, TexImagWidth, TexImagHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, Tex[i].TextureImage);
// 			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, TexImagWidth, TexImagHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, Tex[i].TextureImageLight);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, TexImageHeight, TexImageHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, TexBlock1[i][j].TextureImageBlock);
			glTexImage2D(GL_TEXTURE_2D, 1, GL_LUMINANCE, 128, 128, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, TexBlock1[i][j].MipmapImage128);
			glTexImage2D(GL_TEXTURE_2D, 2, GL_LUMINANCE, 64, 64, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, TexBlock1[i][j].MipmapImage64);
			glTexImage2D(GL_TEXTURE_2D, 3, GL_LUMINANCE, 32, 32, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, TexBlock1[i][j].MipmapImage32);
			glTexImage2D(GL_TEXTURE_2D, 4, GL_LUMINANCE, 16, 16, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, TexBlock1[i][j].MipmapImage16);
			glTexImage2D(GL_TEXTURE_2D, 5, GL_LUMINANCE, 8, 8, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, TexBlock1[i][j].MipmapImage8);
			glTexImage2D(GL_TEXTURE_2D, 6, GL_LUMINANCE, 4, 4, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, TexBlock1[i][j].MipmapImage4);
			glTexImage2D(GL_TEXTURE_2D, 7, GL_LUMINANCE, 2, 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, TexBlock1[i][j].MipmapImage2);
			glTexImage2D(GL_TEXTURE_2D, 8, GL_LUMINANCE, 1, 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, TexBlock1[i][j].MipmapImage1);

//			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_LUMINANCE, TexImagHeight, TexImagHeight, GL_LUMINANCE, GL_UNSIGNED_BYTE, TexBlock[i][j].TextureImageBlock);
/*			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, TexImagHeight, TexImagHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, TexBlock[i][j].TextureImageBlock);
*/
			glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
		}

	}

	
	
	glEnable(GL_TEXTURE_2D);
}
void CMFCTest10_15View::UseBMPTexure(void)
{
	AUX_RGBImageRec *Image;
	WCHAR *Filename=L"texture.bmp";
	Image = auxDIBImageLoadW(Filename);
	GLuint iWidth = Image->sizeX;
	GLuint iHeight = Image->sizeY;
	GLubyte *pImage = Image->data;

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);	
	glGenTextures(1, &ImageIDs[0]);
	glBindTexture(GL_TEXTURE_2D, ImageIDs[0]);			
	glTexImage2D(GL_TEXTURE_2D, 0, 3, iWidth, iHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pImage);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
//	glGenTextures(TextureBlockNum, ImageIDs);
// 	glGenTextures(TextureBlockNum, ImageID1);
// 	for (int i = 0; i<TextureNumPerFrame; i++)
// 
// 	{
// 		for (int j = 0; j<TexPerLine; j++)
// 		{
// 			glBindTexture(GL_TEXTURE_2D, ImageIDs[i * TexPerLine + j]);			
// 			glTexImage2D(GL_TEXTURE_2D, 0, 3, iWidth, iHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pImage);
// 			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
// 			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
// 			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
// 			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
// 			glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
// 		}
// 
// 	}
// 
// 	for (int i = 0; i<TextureNumPerFrame; i++)
// 
// 	{
// 		for (int j = 0; j<TexPerLine; j++)
// 		{
// 			glBindTexture(GL_TEXTURE_2D, ImageID1[i * TexPerLine + j]);			
// 			glTexImage2D(GL_TEXTURE_2D, 0, 3, iWidth, iHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pImage);
// 			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
// 			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
// 			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
// 			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
// 			glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
// 		}
// 
// 	}
	if (Image)
	{
		if (Image->data)
		{
			free(Image->data);
		}
		free(Image);
	}
	glEnable(GL_TEXTURE_2D);
}
void CMFCTest10_15View::UseOneImageTeture(void)
{	
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);	
	glGenTextures(1, &texName);

	glBindTexture(GL_TEXTURE_2D, texName);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 512, 512, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, BigImage);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

	glGenTextures(1, &texNameR);
	glBindTexture(GL_TEXTURE_2D, texNameR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 512, 512, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, BigImageR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

	glEnable(GL_TEXTURE_2D);
}
void CMFCTest10_15View::Lighting()
{
	GLfloat  ambient[] = {1, 1, 1, 1.0};
	GLfloat  diffuse[] = {1, 1, 1, 1.0};
	GLfloat  specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light_position[] = { 500.0 , 500.0 , 500.0 , 1.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glMaterialf(GL_FRONT, GL_SHININESS, 300.0);


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}
void zColorCompute(float z, float &r, float &g, float &b)
{
	int R, G, B;
	int Ratio = 256 * (z - zMin)/(zMax - zMin);
	if (Ratio < 128/4)
	{
		R = 0;
		G = Ratio & 0xFF;
		B = 255;
		r = (1.0 * R) / 255;
		g = (1.0 * G) / 255;
		b = (1.0 * B) / 255;
	}
	else if (Ratio < 256/4)
	{
		R = 0;
		G = 255;
		B = 255 - Ratio & 0xFF;
		r = (1.0 * R) / 255;
		g = (1.0 * G) / 255;
		b = (1.0 * B) / 255;
	}
	else if (Ratio < 512/4)
	{
		R = Ratio & 0xFF;
		G = 255;
		B = 0;
		r = (1.0 * R) / 255;
		g = (1.0 * G) / 255;
		b = (1.0 * B) / 255;
	}
	else
	{
		R = 255;
		G = 255 - Ratio & 0xFF;
		B = 0;
		r = (1.0 * R) / 255;
		g = (1.0 * G) / 255;
		b = (1.0 * B) / 255;
	}
}
void CMFCTest10_15View::InitImage()
{
	for(int i = 0; i < 512; i++)
	{
		for(int j = 0; j < 512; j++)
		{
// 			if ( i > 0 && i < 128 && j > 0 && j <128 )
// 			{
// 				BigImage[i][j] = 0;
// 				BigImageR[i][j] = 0;
// 			}
// 			else
// 			{
				BigImage[i][j] = 150;
				BigImageR[i][j] = 150;
/*			}*/
		}
	}
// 	for(int i = 0; i < 512; i++)
// 	{
// 		for(int j = 0; j < 512; j++)
// 		{
// 			if ( i > 0 && i < 128 && j > 0 && j <128 )
// 			{
// 				SmallImage[i][j] = 0;
// 			}
// 			else
// 			{
// 				SmallImage[i][j] = 255;
// 			}
// 						
// 		}
// 	}
//	ZoomImage();
}
void CMFCTest10_15View::ZoomImage()
{
	int size;
	int i, j;
	int t;

	size = 2048;
	GLubyte **Image1;
	Image1 = new GLubyte *[size];
	for (i = 0; i < size; i++)
	{
		Image1[i] = new GLubyte[size];
	}	
	
	for (i=0; i<size; i++)
	{
		for (j=0; j<size; j++)
		{

			//			MipmapImage128[i][j]=size;
			Image1[i][j]=(BigImage[2*i][2*j] + BigImage[2*i][2*j+1] 
			+ BigImage[2*i+1][2*j] + BigImage[2*i+1][2*j+1])/4;

		}
	}
	

	size = 1024;
	GLubyte **Image2;
	Image2 = new GLubyte *[size];
	for (i = 0; i < size; i++)
	{
		Image2[i] = new GLubyte[size];
	}
	
	for (i=0; i<size; i++)
	{
		for (j=0; j<size; j++)
		{

			//			MipmapImage128[i][j]=size;
			Image2[i][j]=(Image1[2*i][2*j] + Image1[2*i][2*j+1] 
			+ Image1[2*i+1][2*j] + Image1[2*i+1][2*j+1])/4;

		}
	}
	size = 512;
	for (i=0; i<size; i++)
	{
		for (j=0; j<size; j++)
		{

			//			MipmapImage128[i][j]=size;
			SmallImage[i][j]=(Image2[2*i][2*j] + Image2[2*i][2*j+1] 
			+ Image2[2*i+1][2*j] + Image2[2*i+1][2*j+1])/4;

		}
	}

	for( i = 0; i < 1024; i++)
	{
		delete[] Image2[i];
	}
	delete[] Image2;
	for( i = 0; i < 2048; i++)
	{
		delete[] Image1[i];
	}
	delete[] Image1;
}
//**********************DefineByCamera*******************************