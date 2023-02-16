#pragma once


// CDialogModelMenu 대화 상자입니다.

class CDialogModelMenu : public CDialog
{
	DECLARE_DYNAMIC(CDialogModelMenu)

public:
	CDialogModelMenu(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogModelMenu();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MODEL_MENU };

public:
	CRect m_Rect;
	int m_Ret; 	//20100712 lee

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnPaint();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlModelNew();
	void ClickBtnenhctrlModelDelete();
	void ClickBtnenhctrlModelSelect();
};
