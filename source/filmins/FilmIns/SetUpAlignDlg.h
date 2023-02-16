#pragma once


// CSetUpAlignDlg ��ȭ �����Դϴ�.

class CSetUpAlignDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetUpAlignDlg)

public:
	CSetUpAlignDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CSetUpAlignDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SETUP_ALIGN_DIALOG };

public:
	int		m_thrsh;
	int		m_score;
	int		m_poslimit;
	int		m_map0;
	int		m_map1;
	int		m_map2;
	int		m_map3;


	int m_iRectIdx;
	Hlong m_lMWindowID;
	Hlong m_lMWindowID_SR;
	Hlong m_lMWindowID_Lead;
	Hlong m_lMWindowID_SROut;
	Hlong m_lMWindowID_Chip;
	int m_iIndex;

	long m_lSrTeachThr;
	double m_dSrTeachThrDark;
	double m_dSrTeachThrWhite;

	BOOL m_bShow;

	void MasterImageCoverttoHalImageBuffer();
	void SetRegion(int idx);
	void Update(BOOL flag);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlAlignSetdefualt();
	void ClickBtnenhctrlAlignSetmaster();
	void ClickBtnenhctrlAlignLoadmaster2();
	void ClickBtnenhctrlAlignTeach();
	void ClickBtnenhctrlAlignPositionset();
	void ClickBtnenhctrlAlignTest();
	void ClickBtnenhctrlAlignInsRegionSrStart();
	void ClickBtnenhctrlAlignInsRegionSroStart();
	void ClickBtnenhctrlAlignInsSr();
	void ClickBtnenhctrlAlignInsRegionSrout();
	void ClickBtnenhctrlAlignInsRegionSrMinus();
	void ClickBtnenhctrlAlignInsRegionSroutMinus();
	void ClickBtnenhctrlAlignInsRegionPfStart();
	void ClickBtnenhctrlAlignInsRegionPf();
	void ClickBtnenhctrlAlignInsRegionPfMinus();
	void ClickBtnenhctrlAlignInsRegionLead();
	void ClickBtnenhctrlAlignInsRegionLeadIn();
	void ClickBtnenhctrlAlignFindchip();
	void ClickBtnenhctrlAlignPotTeach();
	int m_map4;
	void ClickBtnenhctrlAlignInsRegionAm();
	void ClickBtnenhctrlAlignInsRegionAmStart();
	void ClickBtnenhctrlAlignInsRegionMMinus();
	void ClickBtnenhctrlAlignInsRegionMMinusV();
	void ClickBtnenhctrlAlignInsRegionSrMinusV();
	void ClickBtnenhctrlAlignInsRegionSroutMinusV();
	void ClickBtnenhctrlAlignInsRegionPfMinusV();
	void ClickBtnenhctrlAlignInsRegionLeadV();
	void ClickBtnenhctrlAlignInsRegionLeadInV();
	void ClickBtnenhctrlAlignAutoregion();
	void ClickBtnenhctrlAlignMakeSr();
	void ClickBtnenhctrlAlignInsSrDarkP();
//	void AnimDLLDoneBtnenhctrlAlignInsSrDarkM(long iPicture, BOOL bResult);
	void ClickBtnenhctrlAlignInsSrDarkM();
	void ClickBtnenhctrlAlignInsSrWhiteM();
//	void AnimDLLDoneBtnenhctrlAlignInsSrWhiteP(long iPicture, BOOL bResult);
	void MakeSRRegion(void);
	void ClickBtnenhctrlAlignInsSrWhiteP();
};
