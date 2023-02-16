#pragma once


// CLightCtrlDlg 대화 상자입니다.

class CLightCtrlDlg : public CDialog
{
	DECLARE_DYNAMIC(CLightCtrlDlg)

public:
	CLightCtrlDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLightCtrlDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LIGHT_CONTROL_DIALOG };

public:
	CSliderCtrl	m_cCh1SliderCtrl;
	CSliderCtrl	m_cCh2SliderCtrl;
	CSliderCtrl	m_cCh3SliderCtrl;
	CSliderCtrl	m_cCh4SliderCtrl;
	UINT	m_img0_ch0;
	UINT	m_img0_ch1;
	UINT	m_img0_ch2;
	UINT	m_img0_ch3;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlider3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlider4(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlLightOk();
	void ClickBtnenhctrlLightCancel();
};
