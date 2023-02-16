#pragma once

#include "afxcmn.h"
#include "afxwin.h"

#include "include/_FuncModuleHal.h"
#include "SetUpLeadDlg.h"
#include "SetUpFilmDlg.h"
#include "SetUpChipDlg.h"
#include "SetUpAlignDlg.h"
#include "SetUpSysDlg.h"
#include "SetUpOCVDlg.h"
#include "SetUpSRDlg.h"



// CSetUpBsaeDlg 대화 상자입니다.
class CSetUpBsaeDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetUpBsaeDlg)

public:
	CSetUpBsaeDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSetUpBsaeDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SETUP_BASE_DIALOG };
public:
	//Inspection parameter
	typeInspectPara m_InsPara;
	//System parameter
	typeSystemPara m_SysPara;

	//Inspect region
	Hobject m_HalInsRgn[SUB_RGN_MAX_NUM][INS_RGN_MAX_NUM];//0 : SR inspect region, 1 : PF hall region, 
														  //2 : up lead region, 3 : dn lead region
														  //4 : user area region 5 : SR out region
	                                                      //6 : vaccum region, 7 : chip region
														  //8 : A/M 0번(위쪽)  9 : A/M 0번(아래쪽)
														  //10 : A/M 0번(위쪽)  11 : A/M 0번(아래쪽): 
	int		m_iRectIdx;
	int		m_iSubIdx;
	int		m_cof_num;
	double	m_cof_pfpitch;
	BOOL	m_bDlgCreated;

	CRect	m_SubRectPos;

	CTreeCtrl m_Tree;

	CComboBox m_ctrlCOFWidth;
	CComboBox m_ctrlPFCnt;

	CSetUpLeadDlg m_SetUpLeadDlg;
	CSetUpFilmDlg m_SetUpFilmDlg;
	CSetUpChipDlg m_SetUpChipDlg;
	CSetUpAlignDlg m_SetUpAlignDlg;
	CSetUpSysDlg m_SetUpSysDlg;
	CSetUpOCVDlg m_SetUpOCVDlg;
	CSetUpSRDlg m_SetUpSRDlg;


	void Update(BOOL flag);
	void UpdateComBox(int COFWidth, int PFCnt);
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	void ClickBtnenhctrlSetupBaseOk();
	void ClickBtnenhctrlSetupBaseApplyall();
	void ClickBtnenhctrlSetupBaseApplyallSave();
	afx_msg void OnClose();
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangingTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangePfCnt();
	afx_msg void OnCbnSelchangeCofWidth();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	
	
	
};
