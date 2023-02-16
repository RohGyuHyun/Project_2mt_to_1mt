#pragma once


// CSetUpChipDlg 대화 상자입니다.

class CSetUpChipDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetUpChipDlg)

public:
	CSetUpChipDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSetUpChipDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SETUP_CHIP_DIALOG };

public:
	long	m_out_tol;
	int		m_out_trsh;
	long	m_mold_tol;
	int		m_map0;
	int		m_map1;
	int		m_map2;
	int		m_map3;
	int		m_map4;
	int		m_map5;
	int		m_map6;
	long	m_pot_tol;
	int		m_pot_trsh;
	long	m_npot_tol;
	long	m_potp_tol;
	int		m_potp_trsh;
	int		m_potp_trshhigh;
	int		m_mold_trsh_high;
	long	m_pot_area;
	int		m_potrange_tol;
	int		m_broken_thrsh;
	int		m_potp2_trsh;
	long	m_potp_tol_2;
	long	m_out_count;
	long	m_out_tol_m;
	int		m_out_trsh_m;

	HTuple m_hPunchOffset;		
	HTuple m_hFilmCenter;	
	HTuple m_hChipCenter;			
	long   m_lTemp;
	CString m_lstrTemp;
	int		m_iRectIdx;
	Hobject m_PotRegion;
	BOOL	m_bShow;
	BOOL	m_bAdvanced;

public:
	void ChageStateInspParamInputBox();
	void Update(BOOL flag);
	BOOL InspectPotPart();
	BOOL InspectChipMold();
	BOOL InspectChipOut(int type);
	BOOL InspectFindChip();
	BOOL InspectPot();

	void ClickBtnenhctrlChipFindchip();
	void ClickBtnenhctrlChipChipout();
	void ClickBtnenhctrlChipChipout2();
	void ClickBtnenhctrlChipMold();
	void ClickBtnenhctrlChipPot();
	void ClickBtnenhctrlChipPotpart();
	void ClickBtnenhctrlChipPotTeach();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()

	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	int m_map7;
	int m_out_edgesize;
};
