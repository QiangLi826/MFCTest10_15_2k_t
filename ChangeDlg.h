#pragma once


// ChangeDlg 对话框

class ChangeDlg : public CDialog
{
	DECLARE_DYNAMIC(ChangeDlg)

public:
	ChangeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ChangeDlg();

// 对话框数据
	enum { IDD = IDD_CHANGEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
