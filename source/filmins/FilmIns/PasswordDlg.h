#pragma once


// CPasswordDlg 대화 상자입니다.

class CPasswordDlg : public CDialog
{
	DECLARE_DYNAMIC(CPasswordDlg)

public:
	CPasswordDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CPasswordDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DILALOG_PASSWORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_ctrlPassword;
	CString m_ctrlIdNumber;
	DECLARE_EVENTSINK_MAP()
	//void ClickBtnenhctrlPasswordOk();
	void ClickBtnenhctrlPasswordCancel();
};
