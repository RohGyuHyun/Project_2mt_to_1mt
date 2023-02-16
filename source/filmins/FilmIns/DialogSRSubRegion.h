#pragma once
#include "afxcmn.h"


// CDialogSRSubRegion 대화 상자입니다.

class CDialogSRSubRegion : public CDialog
{
	DECLARE_DYNAMIC(CDialogSRSubRegion)

public:
	CDialogSRSubRegion(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogSRSubRegion();

	//20150119 lss SR sub region
	int m_iSmallSubRgnIdx;													// 생성된 영역 개수
	int m_iSmallSubRgnPointIdx[SR_SUBREGION_MAX_NUM];						// 생성된 영역 마다 가지고 있는 포인트 개수
	int m_iSmallSubRgnX[SR_SUBREGION_MAX_NUM][SR_SUBREGION_POINT_MAX_NUM];	// 포인트 x (최대 20개)
	int m_iSmallSubRgnY[SR_SUBREGION_MAX_NUM][SR_SUBREGION_POINT_MAX_NUM];	// 포인트 y (최대 20개)
	int m_iSmallSubRgnType[SR_SUBREGION_MAX_NUM];
	
	int m_iCurListCtrlIndex;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_SR_SUB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
