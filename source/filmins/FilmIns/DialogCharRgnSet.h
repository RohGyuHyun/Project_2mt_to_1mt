#pragma once


// CDialogCharRgnSet 대화 상자입니다.

class CDialogCharRgnSet : public CDialog
{
	DECLARE_DYNAMIC(CDialogCharRgnSet)

public:
	CDialogCharRgnSet(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogCharRgnSet();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CHAR_REGION_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlCharRegionSet();
	void ClickBtnenhctrlCharRegionMerge();
	void ClickBtnenhctrlCharRegionSeperate();
	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
