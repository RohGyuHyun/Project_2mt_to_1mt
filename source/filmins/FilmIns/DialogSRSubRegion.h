#pragma once
#include "afxcmn.h"


// CDialogSRSubRegion ��ȭ �����Դϴ�.

class CDialogSRSubRegion : public CDialog
{
	DECLARE_DYNAMIC(CDialogSRSubRegion)

public:
	CDialogSRSubRegion(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogSRSubRegion();

	//20150119 lss SR sub region
	int m_iSmallSubRgnIdx;													// ������ ���� ����
	int m_iSmallSubRgnPointIdx[SR_SUBREGION_MAX_NUM];						// ������ ���� ���� ������ �ִ� ����Ʈ ����
	int m_iSmallSubRgnX[SR_SUBREGION_MAX_NUM][SR_SUBREGION_POINT_MAX_NUM];	// ����Ʈ x (�ִ� 20��)
	int m_iSmallSubRgnY[SR_SUBREGION_MAX_NUM][SR_SUBREGION_POINT_MAX_NUM];	// ����Ʈ y (�ִ� 20��)
	int m_iSmallSubRgnType[SR_SUBREGION_MAX_NUM];
	
	int m_iCurListCtrlIndex;

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_SR_SUB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	void ClickBtnenhctrlStartSubsrSmall();
	void ClickBtnenhctrlPSubsrSmall();
	void ClickBtnenhctrlMSubsrSmall();
	void ClickBtnenhctrlVSubsrSmall();
	CListCtrl m_list_sr_small;
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	void ClickBtnenhctrlSubsrOk();
	void ClickBtnenhctrlAddrgnSmall();
	afx_msg void OnNMClickListRegionSmall(NMHDR *pNMHDR, LRESULT *pResult);
	void ClickBtnenhctrlCompSubsrSmall();
	void ClickBtnenhctrlSubsrCancel();
	void ClickBtnenhctrlDelrgnSmall();
	afx_msg void OnBnClickedCheckLarge();
	BOOL m_check_large;
};
