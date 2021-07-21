#pragma once

#include "afxext.h"
#if !defined(AFX_UNIBUTTON_H__43AA6312_68BB_11D4_BD73_000021479D21__INCLUDED_)
#define AFX_UNIBUTTON_H__43AA6312_68BB_11D4_BD73_000021479D21__INCLUDED_

#if _MSC_VER > 1000
#endif
class CMyButton : public CButton
{
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyButton)
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
protected:
	virtual void PreSubclassWindow();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CMyButton)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	UINT m_nBorder;
	LONG m_lfEscapement;
	COLORREF m_nColor, m_sColor, m_hColor, m_dColor;
	CBitmap * m_pNormal;
	CBitmap * m_pSelected;
	CBitmap * m_pHover;
	CBitmap * m_pDisabled;	
	CPoint m_CenterPoint;
	bool m_bMouseDown;
	bool m_bHover;
	bool m_bCapture;
	HRGN m_hRgn;
	bool m_bNeedBitmaps;
	void DrawButton(CDC * pDC, CRect * pRect, UINT state);
	void PrepareStateBitmaps(CDC * pDC, CRect * pRect);
	BOOL HitTest(CPoint point);
	void FrameRgn3D(HDC hDC, const HRGN hRgn, BOOL bSunken);
	void CheckHover(CPoint point);
protected:
	void PrepareNormalState(CDC * pDC, CDC * pMemDC, CRect * pRect);
	void PrepareSelectedState(CDC * pDC, CDC * pMemDC, CRect * pRect);
	void PrepareHoverState(CDC * pDC, CDC * pMemDC, CRect * pRect);
	void PrepareDisabledState(CDC * pDC, CDC * pMemDC, CRect * pRect);
	void DrawButtonCaption(HDC hDC, CRect * pRect, BOOL bEnabled, BOOL bSunken);
	void PaintRgn(CDC * pDC, CDC * pMemDC, CBitmap * pBitmap, COLORREF color, CRect * pRect, BOOL bEnabled, BOOL bSunken);
public:
	CMyButton();
	virtual ~CMyButton();
	BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const CPoint point, const HRGN hRgn, CWnd* pParentWnd, UINT nID);
	BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const CPoint point, const HRGN hRgn, CWnd* pParentWnd, UINT nID, COLORREF color);
	BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const CPoint point, const HRGN hRgn, CWnd* pParentWnd, UINT nID, UINT nBorder,
		COLORREF nColor, COLORREF sColor, COLORREF hColor, COLORREF dColor);
	BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const CPoint point, const HRGN hRgn, CWnd* pParentWnd, UINT nID, UINT nBorder,
		LONG lfEscapement, COLORREF nColor, COLORREF sColor, COLORREF hColor, COLORREF dColor);
};
#endif