#pragma once


// CSetUpSysDlg 대화 상자입니다.

class CSetUpSysDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetUpSysDlg)

public:
	CSetUpSysDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSetUpSysDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SETUP_SYS_DIALOG };

	BOOL m_bShow;

	void Update(BOOL flag);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int		m_iDeleteTerm;
	double  m_dPixelSize;
	BOOL	m_0;
	BOOL	m_1;
	BOOL	m_2;
	BOOL	m_3;
	BOOL	m_4;
	BOOL	m_5;
	BOOL	m_6;
	BOOL	m_7;
	BOOL	m_8;
	BOOL	m_9;
	BOOL	m_10;
	BOOL	m_11;
	BOOL	m_12;
	BOOL	m_13;
	BOOL	m_14;
	BOOL	m_15;
	BOOL	m_16;
	BOOL	m_17;
	BOOL	m_18;
	BOOL	m_19;
	BOOL	m_20;
	BOOL	m_21;
	BOOL	m_22;
	BOOL	m_23;
	BOOL	m_24;
	BOOL	m_25;
	BOOL	m_26;
	BOOL	m_27;
	BOOL	m_28;
	BOOL	m_29;
	BOOL	m_chk_goodsave;
	BOOL	m_chk_ngsave;
	BOOL	m_chk_ngoverlay;
	BOOL	m_chk_NgRstDisp;
	BOOL	m_chk_InspParamLock;

	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
