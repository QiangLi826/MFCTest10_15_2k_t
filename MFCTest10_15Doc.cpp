// MFCTest10_15Doc.cpp : CMFCTest10_15Doc 类的实现
//

#include "stdafx.h"
#include "MFCTest10_15.h"

#include "MFCTest10_15Doc.h"
#include "MFCTest10_15View.h"
#include "ControlView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCTest10_15Doc

IMPLEMENT_DYNCREATE(CMFCTest10_15Doc, CDocument)

BEGIN_MESSAGE_MAP(CMFCTest10_15Doc, CDocument)
END_MESSAGE_MAP()


// CMFCTest10_15Doc 构造/析构

CMFCTest10_15Doc::CMFCTest10_15Doc()
{
	// TODO: 在此添加一次性构造代码

}

CMFCTest10_15Doc::~CMFCTest10_15Doc()
{
}

BOOL CMFCTest10_15Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CMFCTest10_15Doc 序列化

void CMFCTest10_15Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CMFCTest10_15Doc 诊断

#ifdef _DEBUG
void CMFCTest10_15Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMFCTest10_15Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMFCTest10_15Doc 命令

BOOL CMFCTest10_15Doc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  在此添加您专用的创建代码
	/*
	CMFCTest10_15View *pMainView;
	CControlView *pControlView;
	POSITION pos; 
	CView* pView; 
	while(pos!=NULL) 
	{ 
		pView=GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CMFCTest10_15View))==NULL) 
			pMainView=(CMFCTest10_15View*)pView; 
		else 
			pControlView=(CControlView*)pView; 
	} 
	*/

	return TRUE;
}
