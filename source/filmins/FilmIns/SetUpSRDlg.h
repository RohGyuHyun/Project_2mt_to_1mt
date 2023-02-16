#pragma once


// CSetUpSRDlg 대화 상자입니다.

class CSetUpSRDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetUpSRDlg)

public:
	CSetUpSRDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSetUpSRDlg();

	void Update(BOOL flag);
	BOOL InspectSRBub();
	void ChageStateInspParamInputBox();

	int m_iRectIdx;

	BOOL m_bShow;

	float	m_sr_fact;
	int		m_sr_trsh;
	long	m_sr_tol;
	int		m_map0;
	int		m_map1;
	int		m_map2;
	int		m_map3;
	int		m_map4;
	int		m_map5;
	int		m_sr_cnt;
	int		m_sr_tolmax;
	int		m_hall_thresh;
	int		m_hall_tol;
	int		m_fold_trsh;
	int		m_fold_tol;
	int		m_hall_thresh2;


// 대화 상자 데이터입니다.
	enum { IDD = IDD_SETUP_SR_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlSrTest();

};
