#pragma once


// CDialogCharRgnSet ��ȭ �����Դϴ�.

class CDialogCharRgnSet : public CDialog
{
	DECLARE_DYNAMIC(CDialogCharRgnSet)

public:
	CDialogCharRgnSet(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogCharRgnSet();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_CHAR_REGION_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlCharRegionSet();
	void ClickBtnenhctrlCharRegionMerge();
	void ClickBtnenhctrlCharRegionSeperate();
	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
