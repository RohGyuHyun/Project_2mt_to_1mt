#pragma once


// CSetUpFilmDlg 대화 상자입니다.

class CSetUpFilmDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetUpFilmDlg)

public:
	CSetUpFilmDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSetUpFilmDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SETUP_FILM_DIALOG };

public:
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
	float	m_mold_fact;
	long	m_mold_tol;
	int		m_mold_bright_tol;
	int		m_mold_bright_tol_white;
	int		m_map0;
	int		m_map1;
	int		m_map2;
	long	m_pf_tol;
	int		m_pf_trsh;
	int		m_pf_trshhigh;
	long	m_mold_tol_light;
	int		m_mold_thrs_user;
	long	m_mold_tol_user;
	int		m_map3;
	int		m_map4;
	int		m_map5;
	long	m_pf_tol_nopf;
	int		m_metal_tol;
	int		m_metal_trsh;
	int		m_map6;
	long	m_punchburr_tol;
	long	m_punch_maxtol;
	long	m_punch_mintol;
	long	m_punch_error_pos;
	int		m_sr_screach_thesh;
	int		m_sr_screach_hi_thesh;
	int		m_sr_screach_tol;
	int		m_pf_thsh_nopf;
	long	m_punch_error_pos2;
	int		m_thresh_dark;
	int		m_thresh_white;

	int		 m_iRectIdx;
	BOOL	 m_bShow;
	long	 m_lSrTeachThr;
	Hobject *m_Image;
	Hobject	 m_Region;
	Hobject  m_HallRegion;
	RECT	 m_RectRegion;
	RECT	 m_PFRectRegion;
	RECT	 m_DontCareRegion;

public:
	BOOL InspectFilmMold();
	BOOL InspectFilmPF();
	void SRTest(int type, int direct);
	void ChageStateInspParamInputBox();
	void Update(BOOL flag);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlFilmSrDark();
	void ClickBtnenhctrlFilmSrDarkMinus();
	void ClickBtnenhctrlFilmSrDarkTest2();
	void ClickBtnenhctrlFilmSrDark2();
	void ClickBtnenhctrlFilmSrDarkMinus2();
	void ClickBtnenhctrlFilmSrDarkTest3();
	void ClickBtnenhctrlFilmTest2();
	void ClickBtnenhctrlFilmTest7();
	void ClickBtnenhctrlFilmTestPunch();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	int m_edit_edit_size_tol;
	void ClickBtnenhctrlFilmEditTest();
	int m_edit_edit_thresh;
};
