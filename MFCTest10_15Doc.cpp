// MFCTest10_15Doc.cpp : CMFCTest10_15Doc ���ʵ��
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


// CMFCTest10_15Doc ����/����

CMFCTest10_15Doc::CMFCTest10_15Doc()
{
	// TODO: �ڴ����һ���Թ������

}

CMFCTest10_15Doc::~CMFCTest10_15Doc()
{
}

BOOL CMFCTest10_15Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CMFCTest10_15Doc ���л�

void CMFCTest10_15Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}


// CMFCTest10_15Doc ���

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


// CMFCTest10_15Doc ����

BOOL CMFCTest10_15Doc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  �ڴ������ר�õĴ�������
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
