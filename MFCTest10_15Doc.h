// MFCTest10_15Doc.h : CMFCTest10_15Doc 类的接口
//


#pragma once


class CMFCTest10_15Doc : public CDocument
{
protected: // 仅从序列化创建
	CMFCTest10_15Doc();
	DECLARE_DYNCREATE(CMFCTest10_15Doc)

// 属性
public:

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 实现
public:
	virtual ~CMFCTest10_15Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
};


