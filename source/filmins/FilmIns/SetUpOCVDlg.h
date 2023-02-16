#pragma once


// CSetUpOCVDlg ��ȭ �����Դϴ�.

class CSetUpOCVDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetUpOCVDlg)

public:
	CSetUpOCVDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CSetUpOCVDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SETUP_OCV_DIALOG };
	int		m_map0;
	int		m_pat_score;
	int		m_pat_thrsh;
	int		m_var_thrsh;
	long	m_defect_area;
	long	m_ppunch_minarea;
	long	m_ppunch_maxarea;
	long	m_ppunchbur_tol;
	int		m_iRectIdx;
	BOOL	m_bShow;

	void ChageStateInspParamInputBox();
	BOOL OCVTeach(int map_idx, int thresh);
	BOOL ExtOCVTeach();
	void Update(BOOL flag);
	void RegionSeperate();
	void RegionMerge();
	void ExRegionMerge();
	void RegionSet();
	BOOL ExtRegionSet();
	BOOL RegionSet(int map_idx, BOOL auto_mode);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlRegionSet();
	void ClickBtnenhctrlOcvTeach();
	void ClickBtnenhctrlOcvTest1();
	void ClickBtnenhctrlOcvTest2();
//	void AnimDLLDoneBtnenhctrlOcvTeach(long iPicture, BOOL bResult);
};
