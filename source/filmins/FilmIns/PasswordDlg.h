#pragma once


// CPasswordDlg ��ȭ �����Դϴ�.

class CPasswordDlg : public CDialog
{
	DECLARE_DYNAMIC(CPasswordDlg)

public:
	CPasswordDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CPasswordDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DILALOG_PASSWORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CString m_ctrlPassword;
	CString m_ctrlIdNumber;
	DECLARE_EVENTSINK_MAP()
	//void ClickBtnenhctrlPasswordOk();
	void ClickBtnenhctrlPasswordCancel();
};
