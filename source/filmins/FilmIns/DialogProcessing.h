#pragma once
#include "afxcmn.h"


// CDialogProcessing ��ȭ �����Դϴ�.

class CDialogProcessing : public CDialog
{
	DECLARE_DYNAMIC(CDialogProcessing)

public:
	CDialogProcessing(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogProcessing();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_PROCESSING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_progress;
	int m_iStep;
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
