#pragma once


// ChangeDlg �Ի���

class ChangeDlg : public CDialog
{
	DECLARE_DYNAMIC(ChangeDlg)

public:
	ChangeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ChangeDlg();

// �Ի�������
	enum { IDD = IDD_CHANGEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
