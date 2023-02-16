#pragma once


// CSetUpLeadDlg 대화 상자입니다.

class CSetUpLeadDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetUpLeadDlg)

public:
	CSetUpLeadDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSetUpLeadDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SETUP_LEAD_DIALOG };

public:
	/*
	float	m_trsh0;
	float	m_trsh1;
	float	m_trsh2;
	float	m_trsh3;
	long	m_area0;
	long	m_area1;
	long	m_area2;
	long	m_area3;
	int		m_ss0;
	int		m_ss1;
	int		m_ss2;
	int		m_ss3;
	int		m_se0;
	int		m_se1;
	int		m_se2;
	int		m_se3;
	float	m_rat0;
	float	m_rat1;
	float	m_rat2;
	float	m_rat3;
	int		m_cand0;
	int		m_cand1;
	int		m_cand2;
	int		m_cand3;
	int		m_cont0;
	int		m_cont1;
	int		m_cont2;
	int		m_cont3;
	*/
	int		m_edge_trsh;
	float	m_edge_fact;
	long	m_edge_tol;
	float	m_edge_fact2;
	int		m_edge_trsh2;
	int		m_map0;
	int		m_map1;
	int		m_map2;
	int		m_map3;
	int		m_map4;
	long	m_edge_tol2;
	int		m_dirt_tol;
	int		m_dirt_thrsh;
	int		m_scratch_trsh;
	int		m_scratch_tol;

	Hobject *m_Image;
	Hobject m_Region;
	Hobject m_Region2;
	RECT	m_RectRegion;
	RECT	m_RectRegion2;
	int		m_iRectIdx;
	BOOL	m_bShow;

public:
	void Update(BOOL flag);
	void ChageStateInspParamInputBox();
	BOOL InspectDnLead();
	BOOL InspectUpLead();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlLeadTest2();
	void ClickBtnenhctrlLeadDnTest();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	int m_map5;
	void ClickBtnenhctrlLeadAmTest();
};
