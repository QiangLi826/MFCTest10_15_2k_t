// MFCTest10_15Doc.h : CMFCTest10_15Doc ��Ľӿ�
//


#pragma once


class CMFCTest10_15Doc : public CDocument
{
protected: // �������л�����
	CMFCTest10_15Doc();
	DECLARE_DYNCREATE(CMFCTest10_15Doc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CMFCTest10_15Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
};


