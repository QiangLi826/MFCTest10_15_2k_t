
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
	

	//显示
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
	

	// 绘图代码区
	int DrawStripScene(void);

	GLfloat translateX,translateY,translateZ;

	double PI;
	double tFovy;                   // 在gluPerspective中使用(在OnMouseWheel中使用)
	CPoint prePt, nowPt;                                        // 在OnMouseMove中使用
	double tEyeX, tEyeY, tEyeZ;                                    // 在gluLookAt中使用的3个向量
	double tCenterX, tCenterY, tCenterZ;
	double tUpX, tUpY, tUpZ;
	double tVerticalAng, tHorizonAng, tRadius, tAngInc; // 水平方向、垂直方向的角、半径
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);   // 重载OnMouseWheel函数
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);        // 重载OnMouseMove函数
	
	afx_msg LRESULT AddMessageEx(WPARAM wParam, LPARAM lParam);

//**********************DefineByCamera*******************************
public:
	float m_Gain;
	float m_BlackLevel;
	float m_LineRate;
	GLuint texName;
	GLuint texNameR;
	GLuint *ImageIDs;//第一个相机纹理的ID指针	
	GLuint *ImageID1;//第二个相机纹理的ID指针	
	GLuint TexPerLine;//每个相机在每一帧横向的纹理块数	
	GLuint TextureBlockNum;//每个相机在每一帧的纹理块数
	
	GLubyte SmallImage[512][512];
	void InitImage();
	void ZoomImage();

	void UseSelfDefineTexure(void); //使用自定义的纹理映射
	void UseBMPTexure(void);//使用位图的纹理映射
	void UseOneImageTeture(void);
	void TriangleStripFunc(float Xstart, int istart, int iend, float dx, float Ystart, int jstart, int jend, float dy);
	void TriangleStripFullFunc(float Xstart, int istart, int iend, float dx, float Ystart, int jstart, int jend, float dy);
	POINT3 cross(POINT3 a, POINT3 b, POINT3 c, POINT3 d);
	void Lighting();
//**********************DefineByCamera*******************************
	
protected: // 仅从序列化创建
	CMFCTest10_15View();
	DECLARE_DYNCREATE(CMFCTest10_15View)
	

// 属性
public:
	CMFCTest10_15Doc* GetDocument() const;

// 操作
public:
	bool CreateViewGLContext(HDC hDC);

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CMFCTest10_15View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
	bool SetWindowPixelFormat(HDC hDC);
	// 创建绘制环境(RC)并使之成为当前绘制环境    bool CreateViewGLContext(HDC hDC);
	// 初始化openGL
	bool InitGL(void);
	
	//>像素格式的索引值
	int m_GLPixelIndex;
	// 绘制环境，HGLRC是一个指向rendering context的句柄
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

#ifndef _DEBUG  // MFCTest10_15View.cpp 中的调试版本
inline CMFCTest10_15Doc* CMFCTest10_15View::GetDocument() const
   { return reinterpret_cast<CMFCTest10_15Doc*>(m_pDocument); }
#endif

