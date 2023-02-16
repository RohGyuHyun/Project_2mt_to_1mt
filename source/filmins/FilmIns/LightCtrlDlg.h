#pragma once


// CLightCtrlDlg ��ȭ �����Դϴ�.

class CLightCtrlDlg : public CDialog
{
	DECLARE_DYNAMIC(CLightCtrlDlg)

public:
	CLightCtrlDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CLightCtrlDlg();

// ��ȭ ���� �������Դϴ�.
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
