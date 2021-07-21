
#pragma once

#include <afxwin.h>
#include <afxmt.h>
#include <gl/glut.h>
#include <gl\gl.h>
#include <list>
#include <iostream>

#include "MFCTest10_15Doc.h"
#include "ControlView.h"

#include "constant.h"
using namespace std;

#include "POINT3.h"

UINT ProcessData(LPVOID lpParamter) ; 
extern CCriticalSection criticalSectionTwo;
extern CCriticalSection criticalSectionOne;
extern	int Tri_Row_Index;

extern int WholePointCount;
extern int pc;

extern int Strip_Indx;
class CMFCTest10_15View : public CView
{
public:

	void laser(bool save_data_flag,int save_character,int character_data,int save_config,int chk1,int chk2,int chk3,int chk4,int xStep,int yStep);
	void stopLaser();
	
	Device *devices;
	

	//��ʾ
	static bool first;
	int show_Type_Index;

	void setTranslateX(GLfloat x);
	void setTranslateY(GLfloat x);
	void reset();
	GLfloat getTranslateX();
	GLfloat getTranslateY();
	int CheckDir(char* Dir);


private:

	CWinThread* processDataThread ;
	

	// ��ͼ������
	int DrawStripScene(void);

	GLfloat translateX,translateY,translateZ;

	double PI;
	double tFovy;                   // ��gluPerspective��ʹ��(��OnMouseWheel��ʹ��)
	CPoint prePt, nowPt;                                        // ��OnMouseMove��ʹ��
	double tEyeX, tEyeY, tEyeZ;                                    // ��gluLookAt��ʹ�õ�3������
	double tCenterX, tCenterY, tCenterZ;
	double tUpX, tUpY, tUpZ;
	double tVerticalAng, tHorizonAng, tRadius, tAngInc; // ˮƽ���򡢴�ֱ����Ľǡ��뾶
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);   // ����OnMouseWheel����
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);        // ����OnMouseMove����
	
	afx_msg LRESULT AddMessageEx(WPARAM wParam, LPARAM lParam);

//**********************DefineByCamera*******************************
public:
	float m_Gain;
	float m_BlackLevel;
	float m_LineRate;
	GLuint texName;
	GLuint texNameR;
	GLuint *ImageIDs;//��һ����������IDָ��	
	GLuint *ImageID1;//�ڶ�����������IDָ��	
	GLuint TexPerLine;//ÿ�������ÿһ֡������������	
	GLuint TextureBlockNum;//ÿ�������ÿһ֡���������
	
	GLubyte SmallImage[512][512];
	void InitImage();
	void ZoomImage();

	void UseSelfDefineTexure(void); //ʹ���Զ��������ӳ��
	void UseBMPTexure(void);//ʹ��λͼ������ӳ��
	void UseOneImageTeture(void);
	void TriangleStripFunc(float Xstart, int istart, int iend, float dx, float Ystart, int jstart, int jend, float dy);
	void TriangleStripFullFunc(float Xstart, int istart, int iend, float dx, float Ystart, int jstart, int jend, float dy);
	POINT3 cross(POINT3 a, POINT3 b, POINT3 c, POINT3 d);
	void Lighting();
//**********************DefineByCamera*******************************
	
protected: // �������л�����
	CMFCTest10_15View();
	DECLARE_DYNCREATE(CMFCTest10_15View)
	

// ����
public:
	CMFCTest10_15Doc* GetDocument() const;

// ����
public:
	bool CreateViewGLContext(HDC hDC);

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CMFCTest10_15View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
	bool SetWindowPixelFormat(HDC hDC);
	// �������ƻ���(RC)��ʹ֮��Ϊ��ǰ���ƻ���    bool CreateViewGLContext(HDC hDC);
	// ��ʼ��openGL
	bool InitGL(void);
	
	//>���ظ�ʽ������ֵ
	int m_GLPixelIndex;
	// ���ƻ�����HGLRC��һ��ָ��rendering context�ľ��
	HGLRC m_hGLContext;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	afx_msg void OnDestroy();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#ifndef _DEBUG  // MFCTest10_15View.cpp �еĵ��԰汾
inline CMFCTest10_15Doc* CMFCTest10_15View::GetDocument() const
   { return reinterpret_cast<CMFCTest10_15Doc*>(m_pDocument); }
#endif

