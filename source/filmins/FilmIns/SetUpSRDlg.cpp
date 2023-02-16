// SetUpSRDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "FilmIns.h"

#include "UserSockClient.h"
#include "include/_FuncModuleHal.h"
#include "include/_FuncModuleBasic.h"
#include "include/_VisionModuleFilmIns.h"
#include "include/_CtrlImageRect.h"
#include "include/_CtrlModelManage.h"
#include "DialogModelMenu.h"

#include "SetUpBsaeDlg.h"

#include "DialogProcessing.h"
#include "FilmInsDlg.h"

#include "SetUpSRDlg.h"


// CSetUpSRDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSetUpSRDlg, CDialog)

CSetUpSRDlg::CSetUpSRDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetUpSRDlg::IDD, pParent)
	, m_map2(0)
	, m_map3(0)
	, m_map4(0)
	, m_map5(0)
{
	m_sr_fact = 15.0f;
	m_sr_trsh = 245;
	m_sr_tol = 100;
	m_map0 = 1;
	m_map1 = 2;
	m_sr_cnt = 0;
	m_sr_tolmax = 0;
	m_hall_thresh = 0;
	m_hall_tol = 0;
	m_fold_trsh = 0;
	m_fold_tol = 0;
	m_hall_thresh = 1; 
	//}}AFX_DATA_INIT

	m_iRectIdx = 0;

	m_bShow = FALSE;
}

CSetUpSRDlg::~CSetUpSRDlg()
{
}

void CSetUpSRDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SR_FACT_FILM, m_sr_fact);
	DDX_Text(pDX, IDC_EDIT_SR_TRSH_FILM, m_sr_trsh);
	DDX_Text(pDX, IDC_EDIT_SR_TOL_FILM, m_sr_tol);
	DDX_Text(pDX, IDC_EDIT_MAPNUM0_SR, m_map0);
	DDX_Text(pDX, IDC_EDIT_MAPNUM1_SR, m_map1);
	DDX_Text(pDX, IDC_EDIT_SR_TOL_CNT, m_sr_cnt);
	DDX_Text(pDX, IDC_EDIT_SR_TOL_MAX, m_sr_tolmax);
	DDX_Text(pDX, IDC_EDIT_SR_TRSH_HALL, m_hall_thresh);
	DDX_Text(pDX, IDC_EDIT_SR_TOL_HALL, m_hall_tol);
	DDX_Text(pDX, IDC_EDIT_SR_TRSH_FOLD, m_fold_trsh);
	DDX_Text(pDX, IDC_EDIT_SR_TOL_FOLD, m_fold_tol);
	DDX_Text(pDX, IDC_EDIT_SR_TRSH_HALL2, m_hall_thresh2); 
	DDX_Text(pDX, IDC_EDIT_MAPNUM2_SR, m_map2);
	DDX_Text(pDX, IDC_EDIT_MAPNUM3_SR, m_map3);
	DDX_Text(pDX, IDC_EDIT_MAPNUM4_SR, m_map4);
	DDX_Text(pDX, IDC_EDIT_MAPNUM5_SR, m_map5);
}


BEGIN_MESSAGE_MAP(CSetUpSRDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CSetUpSRDlg 메시지 처리기입니다.

HBRUSH CSetUpSRDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

    pDC->SetTextColor(RGB(255, 255, 255));    
	pDC->SetBkColor(RGB(0, 0, 0));

	return (HBRUSH)pdlg->m_BrushBlack;	

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	//return hbr;
}

void CSetUpSRDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	UpdateData(TRUE);	

	if(bShow)
	{
		m_bShow = TRUE;
		disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], pdlg->m_iMapIdx[0]), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
		ChageStateInspParamInputBox(); //20110617 lee
		
		Update(TRUE);	
	}
	else
	{
		m_bShow = FALSE;
		Update(FALSE);
	}	
}
BEGIN_EVENTSINK_MAP(CSetUpSRDlg, CDialog)
	ON_EVENT(CSetUpSRDlg, IDC_BTNENHCTRL_SR_TEST, DISPID_CLICK, CSetUpSRDlg::ClickBtnenhctrlSrTest, VTS_NONE)
END_EVENTSINK_MAP()

void CSetUpSRDlg::ClickBtnenhctrlSrTest()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);

	disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	InspectSRBub();	
}

void CSetUpSRDlg::Update(BOOL flag)
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	if(flag)
	{
		m_sr_fact = pdlg->m_dlgSetup->m_InsPara.m_iParaSR[0];
		m_sr_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaSR[1];
		m_sr_trsh = pdlg->m_dlgSetup->m_InsPara.m_iParaSR[2]; 
		m_sr_cnt = pdlg->m_dlgSetup->m_InsPara.m_iParaSR[3];
		m_sr_tolmax = pdlg->m_dlgSetup->m_InsPara.m_iParaSR[4];

		m_hall_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaSR[5];
		m_hall_thresh = pdlg->m_dlgSetup->m_InsPara.m_iParaSR[6];

		m_fold_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaSR[7];
		m_fold_trsh = pdlg->m_dlgSetup->m_InsPara.m_iParaSR[8]; 

		m_map0 = 1;
		m_map1 = 1;
		m_map2 = 2;
		m_map3 = 1;
		m_map4 = 3;
		m_map5 = 3;

		m_hall_thresh2 = pdlg->m_dlgSetup->m_InsPara.m_iParaSR[9]; 
		UpdateData(FALSE);
	}
	else
	{
		UpdateData(TRUE);

		pdlg->m_dlgSetup->m_InsPara.m_iParaSR[0] = m_sr_fact;		// 기포 Factor
		pdlg->m_dlgSetup->m_InsPara.m_iParaSR[1] = m_sr_tol;		// 기포 허용직경(개수관리)
		pdlg->m_dlgSetup->m_InsPara.m_iParaSR[2] = m_sr_trsh;		// 기포
		pdlg->m_dlgSetup->m_InsPara.m_iParaSR[3] = m_sr_cnt;		// 기포 허용개수
		pdlg->m_dlgSetup->m_InsPara.m_iParaSR[4] = m_sr_tolmax;	 	// 기포 허용직경(개수=1)

		pdlg->m_dlgSetup->m_InsPara.m_iParaSR[5] = m_hall_tol;		// 찍힘 허용직경
		pdlg->m_dlgSetup->m_InsPara.m_iParaSR[6] = m_hall_thresh;	// 찍힘 밝기차

		pdlg->m_dlgSetup->m_InsPara.m_iParaSR[7] = m_fold_tol;		// 구겨짐 허용면적
		pdlg->m_dlgSetup->m_InsPara.m_iParaSR[8] = m_fold_trsh;		//구겨짐 밝기차
		pdlg->m_dlgSetup->m_InsPara.m_iParaSR[9] = m_hall_thresh2;	// 잉크칩


		pdlg->m_dlgSetup->m_InsPara.m_iParaMapSR[0] = m_map0;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapSR[1] = m_map1;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapSR[2] = m_map2;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapSR[3] = m_map3;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapSR[4] = m_map4;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapSR[5] = m_map5;
	}
}	


BOOL CSetUpSRDlg::InspectSRBub()
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	
	BOOL rslt = TRUE;
	CRect ins_rect, chip_ins_rect, ocv_ins_rect, chip_search_rect; //CHOKI20110506
	Hlong rslt_num = 0;
	Hlong rslt_hall_num = 0;
	Hobject rslt_rgn, rslt_sr_rgn, rslt_hall_rgn, rslt_hallins_rgn;
	
	disp_obj(*pdlg->m_VisMod->GetHalBlueImage(0, pdlg->m_iBufIdx[0], m_map0), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	
	for(int i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		
		ins_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0];
		chip_ins_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[i][0];
		ocv_ins_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[i][0];
		chip_search_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0]; 
		
		
		decompose3(*pdlg->m_VisMod->GetHalTaughtImage(0,0), 
			pdlg->m_VisMod->GetHalRefRedImage(0, pdlg->m_iBufIdx[0], 0),
			pdlg->m_VisMod->GetHalRefGreenImage(0, pdlg->m_iBufIdx[0], 0),
			pdlg->m_VisMod->GetHalRefBlueImage(0, pdlg->m_iBufIdx[0], 0));
		
		rslt = pdlg->m_VisMod->_Hal_FilmFold(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), pdlg->m_VisMod->GetHalBlueImage(0, pdlg->m_iBufIdx[0], m_map1), pdlg->m_VisMod->GetHalBlueImage(0, pdlg->m_iBufIdx[0], m_map2),
								*pdlg->m_VisMod->GetHalInspectRgn(0, i, 0),
								chip_ins_rect, chip_search_rect,

								pdlg->m_dlgSetup->m_InsPara.m_dPatternPosXAlign[0][0], pdlg->m_dlgSetup->m_InsPara.m_dPatternPosYAlign[0][0], 
								pdlg->m_dlgSetup->m_InsPara.m_dPatternPosXAlign[0][1], pdlg->m_dlgSetup->m_InsPara.m_dPatternPosYAlign[0][1], 

								pdlg->m_dlgSetup->m_InsPara.m_dPatternPosXAlign[i][0], pdlg->m_dlgSetup->m_InsPara.m_dPatternPosYAlign[i][0], 
								pdlg->m_dlgSetup->m_InsPara.m_dPatternPosXAlign[i][1], pdlg->m_dlgSetup->m_InsPara.m_dPatternPosYAlign[i][1], 
								pdlg->m_dlgSetup->m_InsPara.m_dPatternPosXAlign[i][0], pdlg->m_dlgSetup->m_InsPara.m_dPatternPosYAlign[i][0], 
								pdlg->m_dlgSetup->m_InsPara.m_dPatternPosXAlign[i][1], pdlg->m_dlgSetup->m_InsPara.m_dPatternPosYAlign[i][1], 

								m_hall_thresh, m_hall_tol,
								m_fold_trsh, m_fold_tol,  

								pdlg->m_dlgSetup->m_InsPara.m_iSmallSubRgnIdx,
								pdlg->m_dlgSetup->m_InsPara.m_iSmallSubRgnX,
								pdlg->m_dlgSetup->m_InsPara.m_iSmallSubRgnY,
								pdlg->m_dlgSetup->m_InsPara.m_iSmallSubRgnType,

								&rslt_num,
								&rslt_rgn,
								&rslt_sr_rgn,
								&rslt_hall_num,
								&rslt_hall_rgn,
								&rslt_hallins_rgn);

		//20150317 Func change. by
		/*
		rslt = pdlg->m_VisMod->SRBub(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(),
			pdlg->m_VisMod->GetHalBlueImage(0, pdlg->m_iBufIdx[0], m_map0),
			pdlg->m_VisMod->GetHalBlueImage(0, pdlg->m_iBufIdx[0], m_map1),
			pdlg->m_VisMod->GetHalGreenImage(0, pdlg->m_iBufIdx[0], m_map1),
			pdlg->m_VisMod->GetHalRefGreenImage(0, pdlg->m_iBufIdx[0], 0),
			pdlg->m_VisMod->GetHalRefBlueImage(0, pdlg->m_iBufIdx[0], 0), 
			pdlg->m_VisMod->GetHalRedImage(0, pdlg->m_iBufIdx[0], m_map0),
			ins_rect,
			chip_ins_rect,
			chip_search_rect, 
			0, 0, 0, 0,
			m_sr_trsh, m_sr_trsh, m_sr_fact, m_sr_tol,
			m_sr_tolmax, m_sr_cnt,
			m_hall_thresh, m_hall_tol,
			m_fold_trsh, m_fold_tol, m_hall_thresh2,   
			&rslt_num,
			&rslt_rgn,
			&rslt_sr_rgn,
			&rslt_hall_num,
			&rslt_hall_rgn,
			&rslt_hallins_rgn);
		*/
		if(rslt)
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "GOOD");
		}
		else
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "NG : SR(Bottom) 찍힘");
		}
	}
	
	return TRUE;
}

void CSetUpSRDlg::ChageStateInspParamInputBox()
{
	BOOL state;
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	
	state = pdlg->m_dlgSetup->m_SysPara.m_bPara[34];
	
	GetDlgItem(IDC_EDIT_SR_TRSH_FILM)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_SR_FACT_FILM)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_SR_TOL_FILM)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_SR_TOL_CNT)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_SR_TOL_MAX)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_SR_TRSH_HALL)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_SR_TOL_HALL)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_SR_TRSH_FOLD)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_SR_TOL_FOLD)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_SR_TRSH_HALL2)->EnableWindow(state); 
}
