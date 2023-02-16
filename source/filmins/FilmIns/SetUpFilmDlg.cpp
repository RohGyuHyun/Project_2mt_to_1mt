// SetUpFilmDlg.cpp : 구현 파일입니다.
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

#include "SetUpFilmDlg.h"


// CSetUpFilmDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSetUpFilmDlg, CDialog)

CSetUpFilmDlg::CSetUpFilmDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetUpFilmDlg::IDD, pParent)
	, m_edit_edit_size_tol(0)
	, m_edit_edit_thresh(0)
{
	m_trsh0 = 3.0f;
	m_trsh1 = 0.8f;
	m_trsh2 = 0.8f;
	m_trsh3 = 0.8f;
	m_area0 = 2;
	m_area1 = 2;
	m_area2 = 2;
	m_area3 = 2;
	m_ss0 = 21;
	m_ss1 = 21;
	m_ss2 = 21;
	m_ss3 = 21;
	m_se0 = 21;
	m_se1 = 21;
	m_se2 = 21;
	m_se3 = 21;
	m_rat0 = 0.8f;
	m_rat1 = 0.8f;
	m_rat2 = 0.8f;
	m_rat3 = 0.8f;
	m_cand0 = 10;
	m_cand1 = 10;
	m_cand2 = 10;
	m_cand3 = 10;
	m_cont0 = 255;
	m_cont1 = 5;
	m_cont2 = 5;
	m_cont3 = 5;
	m_mold_fact = 15.0f;
	m_mold_tol = 100;
	m_mold_bright_tol = 25;
	m_mold_bright_tol_white = 230;
	m_map0 = 0;
	m_map1 = 0;
	m_map2 = 3;
	m_pf_tol = 0;
	m_pf_trsh = 0;
	m_pf_trshhigh = 0;
	m_mold_tol_light = 0;
	m_mold_thrs_user = 0;
	m_mold_tol_user = 0;
	m_map3 = 0;
	m_map4 = 0;
	m_map5 = 0;
	m_pf_tol_nopf = 0;
	m_metal_tol = 0;
	m_metal_trsh = 0;
	m_map6 = 0;
	m_punchburr_tol = 10;
	m_punch_maxtol = 100000;
	m_punch_mintol = 400000;
	m_punch_error_pos = 200;
	m_sr_screach_thesh = 110;
	m_sr_screach_hi_thesh = 125;
	m_sr_screach_tol = 12;
	m_pf_thsh_nopf = 0;
	m_punch_error_pos2 = 200;
	m_thresh_dark = 0;
	m_punch_error_pos2 = 0;
	m_thresh_white = 0;
	//}}AFX_DATA_INIT
	
	m_iRectIdx = 0;
	
	m_bShow = FALSE;
	
	
	m_lSrTeachThr = 0;
}

CSetUpFilmDlg::~CSetUpFilmDlg()
{
}

void CSetUpFilmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TRSH0_FILM, m_trsh0);
	DDX_Text(pDX, IDC_EDIT_TRSH1_FILM, m_trsh1);
	DDX_Text(pDX, IDC_EDIT_TRSH2_FILM, m_trsh2);
	DDX_Text(pDX, IDC_EDIT_TRSH3_FILM, m_trsh3);
	DDX_Text(pDX, IDC_EDIT_AREA0_FILM, m_area0);
	DDX_Text(pDX, IDC_EDIT_AREA1_FILM, m_area1);
	DDX_Text(pDX, IDC_EDIT_AREA2_FILM, m_area2);
	DDX_Text(pDX, IDC_EDIT_AREA3_FILM, m_area3);
	DDX_Text(pDX, IDC_EDIT_SS0_FILM, m_ss0);
	DDX_Text(pDX, IDC_EDIT_SS1_FILM, m_ss1);
	DDX_Text(pDX, IDC_EDIT_SS2_FILM, m_ss2);
	DDX_Text(pDX, IDC_EDIT_SS3_FILM, m_ss3);
	DDX_Text(pDX, IDC_EDIT_SE0_FILM, m_se0);
	DDX_Text(pDX, IDC_EDIT_SE1_FILM, m_se1);
	DDX_Text(pDX, IDC_EDIT_SE2_FILM, m_se2);
	DDX_Text(pDX, IDC_EDIT_SE3_FILM, m_se3);
	DDX_Text(pDX, IDC_EDIT_SRAT0_FILM, m_rat0);
	DDX_Text(pDX, IDC_EDIT_SRAT1_FILM, m_rat1);
	DDX_Text(pDX, IDC_EDIT_SRAT2_FILM, m_rat2);
	DDX_Text(pDX, IDC_EDIT_SRAT3_FILM, m_rat3);
	DDX_Text(pDX, IDC_EDIT_CANDI0_FILM, m_cand0);
	DDX_Text(pDX, IDC_EDIT_CANDI1_FILM, m_cand1);
	DDX_Text(pDX, IDC_EDIT_CANDI2_FILM, m_cand2);
	DDX_Text(pDX, IDC_EDIT_CANDI3_FILM, m_cand3);
	DDX_Text(pDX, IDC_EDIT_CONT0_FILM, m_cont0);
	DDX_Text(pDX, IDC_EDIT_CONT1_FILM, m_cont1);
	DDX_Text(pDX, IDC_EDIT_CONT2_FILM, m_cont2);
	DDX_Text(pDX, IDC_EDIT_CONT3_FILM, m_cont3);
	DDX_Text(pDX, IDC_EDIT_MOLD_FACT_FILM, m_mold_fact);
	DDX_Text(pDX, IDC_EDIT_MOLD_TOL_FILM, m_mold_tol);
	DDX_Text(pDX, IDC_EDIT_MOLD_TRSH_FILM, m_mold_bright_tol);
	DDX_Text(pDX, IDC_EDIT_MOLD_TRSH_FILM_WHITE, m_mold_bright_tol_white);
	DDX_Text(pDX, IDC_EDIT_MAPNUM0_FILM, m_map0);
	DDX_Text(pDX, IDC_EDIT_MAPNUM1_FILM, m_map1);
	DDX_Text(pDX, IDC_EDIT_MAPNUM6_FILM, m_map2);
	DDX_Text(pDX, IDC_EDIT_PF_TOL_FILM, m_pf_tol);
	DDX_Text(pDX, IDC_EDIT_PF_TRSH_FILM, m_pf_trsh);
	DDX_Text(pDX, IDC_EDIT_PF_TRSHHIGH_FILM, m_pf_trshhigh);
	DDX_Text(pDX, IDC_EDIT_MOLD_TOL_FILM2, m_mold_tol_light);
	DDX_Text(pDX, IDC_EDIT_MOLD_TRSH_FILM_USER, m_mold_thrs_user);
	DDX_Text(pDX, IDC_EDIT_MOLD_TOL_FILM_USER, m_mold_tol_user);
	DDX_Text(pDX, IDC_EDIT_MAPNUM3_FILM, m_map3);
	DDX_Text(pDX, IDC_EDIT_MAPNUM4_FILM, m_map4);
	DDX_Text(pDX, IDC_EDIT_MAPNUM5_FILM, m_map5);
	DDX_Text(pDX, IDC_EDIT_PFEDIT_TOL_FILM, m_pf_tol_nopf);
	DDX_Text(pDX, IDC_EDIT_PF_TOL_METALPART, m_metal_tol);
	DDX_Text(pDX, IDC_EDIT_PF_TRSH_METALPART, m_metal_trsh);
	DDX_Text(pDX, IDC_EDIT_MAPNUM7_FILM, m_map6);
	DDX_Text(pDX, IDC_EDIT_PUNCHBURR_TOL_FILM, m_punchburr_tol);
	DDX_Text(pDX, IDC_EDIT_PUNCH_MAXTOL_FILM, m_punch_maxtol);
	DDX_Text(pDX, IDC_EDIT_PUNCH_MINTOL_FILM, m_punch_mintol);
	DDX_Text(pDX, IDC_EDIT_PUNCH_ERROR_POS, m_punch_error_pos);
	DDX_Text(pDX, IDC_EDIT_MOLD_TRSH_FILM2, m_sr_screach_thesh);
	DDX_Text(pDX, IDC_EDIT_MOLD_TRSH_FILM3, m_sr_screach_hi_thesh);
	DDX_Text(pDX, IDC_EDIT_MOLD_TOL_FILM3, m_sr_screach_tol);
	DDX_Text(pDX, IDC_EDIT_PF_TRSHHIGH_FILM2, m_pf_thsh_nopf);
	DDX_Text(pDX, IDC_EDIT_PUNCH_ERROR_POS2, m_punch_error_pos2);
	DDX_Text(pDX, IDC_EDIT_MOLD_TRSH_DARK, m_thresh_dark);
	DDX_Text(pDX, IDC_EDIT_MOLD_TRSH_WHITE, m_thresh_white);
	DDX_Text(pDX, IDC_EDIT_EDIT_WIDTH_TOL, m_edit_edit_size_tol);
	DDX_Text(pDX, IDC_EDIT_EDIT_THRESH, m_edit_edit_thresh);
}


BEGIN_MESSAGE_MAP(CSetUpFilmDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CSetUpFilmDlg 메시지 처리기입니다.

BOOL CSetUpFilmDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

HBRUSH CSetUpFilmDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void CSetUpFilmDlg::ChageStateInspParamInputBox()
{
	BOOL state;
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	
	state = pdlg->m_dlgSetup->m_SysPara.m_bPara[34];

	GetDlgItem(IDC_EDIT_MOLD_TRSH_FILM)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_MOLD_FACT_FILM)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_MOLD_TOL_FILM)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_MOLD_TOL_FILM2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_MOLD_TRSH_FILM2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_MOLD_TRSH_FILM3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_MOLD_TOL_FILM3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_MOLD_TRSH_FILM_USER)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_MOLD_TOL_FILM_USER)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_PF_TRSHHIGH_FILM)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_PF_TOL_FILM)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_PFEDIT_TOL_FILM)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_PF_TRSHHIGH_FILM2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_PUNCH_MINTOL_FILM)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_PUNCH_MAXTOL_FILM)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_PUNCHBURR_TOL_FILM)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_PUNCH_ERROR_POS)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_PF_TRSH_METALPART)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_PF_TOL_METALPART)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_PUNCH_ERROR_POS2)->EnableWindow(state);

	GetDlgItem(IDC_EDIT_MOLD_TRSH_DARK)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_MOLD_TRSH_WHITE)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_MOLD_TRSH_FILM_WHITE)->EnableWindow(state);
}


BOOL CSetUpFilmDlg::InspectFilmMold()
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	BOOL rslt = TRUE;
	CRect ins_rect, chip_ins_rect, ocv_ins_rect;
	Hlong rslt_num = 0;
	Hobject rslt_rgn, rslt_sr_rgn, ocv_chars_rgn, rslt_notsr_rng, rslt_user_rgn, sr_out_region, rslt_sr_out_rgn;
	
	disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	

	for(int i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		//if(pdlg->m_dlgSetup->m_InsPara.m_bCreatFilmVarID[i])
		if(1)
		{
			ins_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[i][0];
			chip_ins_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0];
			ocv_ins_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[i][0];

			rslt = pdlg->m_VisMod->FilmMold(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(),
				   //Mold image
				   pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0),
				   //Ink image
				   pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map2),
				   //User image
				   pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map1),
				   //SR-OUT Scratch image
				   pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map3),
				   pdlg->m_VisMod->GetHalTaughtImage(0, 2),//SR
				   ins_rect,
				   chip_ins_rect,
				   ocv_ins_rect,
				   ocv_chars_rgn,
				   
				   *pdlg->m_VisMod->GetHalInspectRgn(0, i, 0),//SR region
				   *pdlg->m_VisMod->GetHalInspectRgn(0, i, 4),//user region
				   *pdlg->m_VisMod->GetHalInspectRgn(0, i, 5),//SR out region

				   pdlg->m_dlgSetup->m_InsPara.m_FilmVarID[i],
				   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				   m_thresh_dark, m_mold_bright_tol, m_thresh_white, m_mold_bright_tol_white, m_mold_fact, m_mold_tol, m_mold_tol_light,
				   m_mold_thrs_user, m_mold_tol_user,
				   m_sr_screach_thesh, m_sr_screach_hi_thesh, m_sr_screach_tol,
				   
				   pdlg->m_dlgSetup->m_InsPara.m_iSmallSubRgnIdx,
				   pdlg->m_dlgSetup->m_InsPara.m_iSmallSubRgnX,
				   pdlg->m_dlgSetup->m_InsPara.m_iSmallSubRgnY,
				   pdlg->m_dlgSetup->m_InsPara.m_iSmallSubRgnType,
				   &rslt_num,
				   &rslt_rgn,
				   &rslt_sr_rgn,
				   &rslt_notsr_rng,
				   &rslt_user_rgn,
				   &rslt_sr_out_rgn);

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
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "NG : FILM MOLD");
			}
		}
		else
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "red");
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(HTuple("FAIL : no pattern!")));
		}

	
	}

	return TRUE;
}


BOOL CSetUpFilmDlg::InspectFilmPF()
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	BOOL rslt = TRUE;
	CRect ins_rect, chip_ins_rect, ocv_ins_rect, dontcare_rect;
	Hlong rslt_num = 0;
	Hobject rslt_rgn, rslt_pf_rgn;

	m_pf_trsh = m_pf_trshhigh - 10;
	if(1 > m_pf_trsh)
	{
		m_pf_trsh = 1;
	}

	disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map1), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	
	for(int i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		ins_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[i][0];
		chip_ins_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0];
		dontcare_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0];
		
		rslt = pdlg->m_VisMod->FilmPF(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(),
			pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map4),
			pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map5),
			ins_rect, chip_ins_rect, dontcare_rect, *pdlg->m_VisMod->GetHalInspectRgn(0, i, 1),//PF region
			*pdlg->m_VisMod->GetHalInspectRgn(0, i, 6),//vaccume region
			0, 0, 0, 0, 0, 0, 0, 0,
			m_pf_trsh, m_pf_tol, m_pf_thsh_nopf, m_pf_tol_nopf,
			&rslt_num,
			&rslt_rgn,
			&rslt_pf_rgn);

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
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "NG : PF 찢김");
		}
	}
	
	return rslt;
}


void CSetUpFilmDlg::SRTest(int type, int direct)
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);
	
	BOOL rslt = TRUE;
	CRect ins_rect, chip_ins_rect, ocv_ins_rect;
	Hlong rslt_num = 0;
	Hobject rslt_rgn, rslt_sr_rgn, ocv_chars_rgn, rslt_notsr_rng, rslt_user_rgn, sr_out_region, rslt_sr_out_rgn;
	
	disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	
	int thresh, thresh_tol, thresh_w, thresh_w_tol;
	int mold_tol, mold_tol_light;
	int sr_screach_thesh, sr_screach_hi_thesh, sr_screach_tol;

	switch(type)
	{
	case 0:
		switch(direct)
		{
		case 0:
			m_thresh_dark = m_thresh_dark + 2;
			if(m_thresh_dark > 100) m_thresh_dark = 0;	
			break;
		case 1:
			m_thresh_dark = m_thresh_dark - 2;
			if(m_thresh_dark < 0) m_thresh_dark = 0;	
			break;
		}

		//검은계열 인식 
		//m_thresh_dark = m_lSrTeachThr;
		thresh = m_thresh_dark;
		thresh_w = 0;

		thresh_tol = 255;
		thresh_w_tol = 255;
		mold_tol = 0;
		mold_tol_light = 10000;

		sr_screach_thesh = 0;
		sr_screach_hi_thesh = 0;
		sr_screach_tol = 10000;
		break;
	case 1:
		switch(direct)
		{
		case 0:
			m_thresh_white = m_thresh_white + 2;
			if(m_thresh_white > 100) m_thresh_white = 0;	
			break;
		case 1:
			m_thresh_white = m_thresh_white - 2;
			if(m_thresh_white < 0) m_thresh_white = 0;	
			break;
		}

		//밝은계열 인식
		//m_thresh_white = m_lSrTeachThr;
		thresh_w = m_thresh_white;
		thresh = 0;

		thresh_tol = 255;
		thresh_w_tol = 255;
		mold_tol = 10000;
		mold_tol_light = 0;

		sr_screach_thesh = 0;
		sr_screach_hi_thesh = 0;
		sr_screach_tol = 10000;
		break;
	case 2:

		//어두운계열 테스트
		thresh = m_thresh_dark;
		thresh_w = 0;
		
		thresh_tol = m_mold_bright_tol;
		thresh_w_tol = 255;
		mold_tol = m_mold_tol;
		mold_tol_light = 10000;
		
		sr_screach_thesh = 0;
		sr_screach_hi_thesh = 0;
		sr_screach_tol = 10000;
		break;
	case 3:

		//밝은계열 테스트
		thresh_w = m_thresh_white;
		thresh = 0;
		
		thresh_tol = 255;
		thresh_w_tol = m_mold_bright_tol_white;
		mold_tol = 10000;
		mold_tol_light = m_mold_tol_light;
		
		sr_screach_thesh = 0;
		sr_screach_hi_thesh = 0;
		sr_screach_tol = 10000;
		break;	
	}

	/*
	thresh, thresh_tol, thresh_w, thresh_w_tol, m_mold_fact, mold_tol, mold_tol_light,
				m_mold_thrs_user, m_mold_tol_user,
	*/
	for(int i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		//if(pdlg->m_dlgSetup->m_InsPara.m_bCreatFilmVarID[i])
		if(1)
		{
			ins_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[i][0];
			chip_ins_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0];
			ocv_ins_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[i][0];
			
			rslt = pdlg->m_VisMod->FilmMold(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(),
				//Mold image
				pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0),
				//Ink image
				pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map2),
				//User image
				pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map1),
				//SR-OUT Scratch image
				pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map3),
				pdlg->m_VisMod->GetHalTaughtImage(0, 2),//SR
				ins_rect,
				chip_ins_rect,
				ocv_ins_rect,
				ocv_chars_rgn,
				
				*pdlg->m_VisMod->GetHalInspectRgn(0, i, 0),//SR region
				*pdlg->m_VisMod->GetHalInspectRgn(0, i, 4),//user region
				*pdlg->m_VisMod->GetHalInspectRgn(0, i, 5),//SR out region
				
				pdlg->m_dlgSetup->m_InsPara.m_FilmVarID[i],
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				thresh, thresh_tol, thresh_w, thresh_w_tol, m_mold_fact, mold_tol, mold_tol_light,
				m_mold_thrs_user, m_mold_tol_user,
				sr_screach_thesh, sr_screach_hi_thesh, sr_screach_tol,
				
				pdlg->m_dlgSetup->m_InsPara.m_iSmallSubRgnIdx,
			   pdlg->m_dlgSetup->m_InsPara.m_iSmallSubRgnX,
			   pdlg->m_dlgSetup->m_InsPara.m_iSmallSubRgnY,
			   pdlg->m_dlgSetup->m_InsPara.m_iSmallSubRgnType,
				&rslt_num,
				&rslt_rgn,
				&rslt_sr_rgn,
				&rslt_notsr_rng,
				&rslt_user_rgn,
				&rslt_sr_out_rgn);
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
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "NG : FILM MOLD");
			}
		}
		else
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "red");
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(HTuple("FAIL : no pattern!")));
		}
	}

	UpdateData(FALSE);
}

void CSetUpFilmDlg::Update(BOOL flag)
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	if(flag)
	{
		m_mold_fact = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[0];
		m_mold_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[1];
		m_mold_tol_light = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[6];
		m_mold_bright_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[2];
		m_mold_bright_tol_white = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[21];

		m_mold_tol_user = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[7];
		m_mold_thrs_user = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[8];

		m_pf_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[3];
		m_pf_tol_nopf = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[9];

		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[4] = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[5] - 10;
		if(1 > pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[4])
		{
			pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[4] = 1;
		}
		m_pf_trsh = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[4];
		m_pf_trshhigh = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[5];
		m_pf_thsh_nopf = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[19];

		m_metal_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[10];
		m_metal_trsh = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[11];

		m_punchburr_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[12];
		m_punch_mintol = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[13];
		m_punch_maxtol = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[14];
		m_punch_error_pos = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[15];
		m_punch_error_pos2 = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[20];


		m_sr_screach_thesh = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[16];
		m_sr_screach_hi_thesh = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[17];
		m_sr_screach_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[18];

		m_thresh_dark = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[22];
		m_thresh_white = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[23];

		m_edit_edit_size_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[24];
		m_edit_edit_thresh = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[25];
		/*
		m_map0 = pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[0];
		m_map1 = pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[1];

		pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[2] = pdlg->m_dlgSetup->m_InsPara.m_iParaMapOCV[0];
		m_map2 = pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[2];

		m_map3 = pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[3];
		m_map4 = pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[4];
		m_map5 = pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[5];
		m_map6 = pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[6];
		*/

		//20141111 lby 비젼 인덱스에 따른 맵 디파인
		//디버그 필요
		//20150223 by
		switch(pdlg->m_VisMod->GetVisIndex())
		{
		case VISION_CAM_TOP1:
			m_map0 = 2;
			m_map1 = 2;
			m_map2 = 2;
			m_map3 = 2;
			m_map4 = 0;
			m_map5 = 0;
			m_map6 = 1;
			break;
		case VISION_CAM_TOP2:
			m_map0 = 2;
			m_map1 = 2;
			m_map2 = 2;
			m_map3 = 2;
			m_map4 = 0;
			m_map5 = 0;
			m_map6 = 1;
			break;
		case VISION_CAM_BOTTOM:

			break;
		default:

			break;
		}
		/*
		switch(pdlg->m_VisMod->GetVisIndex())
		{
		case VISION_CAM_TOP1:
			m_map0 = 2;
			m_map1 = 2;
			m_map2 = 2;
			m_map3 = 2;
			m_map4 = 0;
			m_map5 = 0;
			m_map6 = 1;
			break;
		case VISION_CAM_BOTTOM1:
			m_map0 = 2;
			m_map1 = 2;
			m_map2 = 2;
			m_map3 = 2;
			m_map4 = 0;
			m_map5 = 0;
			m_map6 = 1;
			break;
		case VISION_CAM_TOP2:
		
			break;
		case VISION_CAM_BOTTOM2:

			break;
		default:

			break;
		}
		*/
		
		
		/*
		map0 수지묻음 : 4
		?map1 PUNCH/USER수지: ?
		map2 잉크묻음 : 2
		map3 SR OUT PAT. 긁힘 : 4
		map4 찢김 : 5
		map5 막힘 : 0
		map6 SR면 금속성 이물 : 1
		*/

		UpdateData(FALSE);
	}
	else
	{
		UpdateData(TRUE);

		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[0] = m_mold_fact;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[1] = m_mold_tol;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[6] = m_mold_tol_light;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[2] = m_mold_bright_tol;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[21] = m_mold_bright_tol_white;

		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[7] = m_mold_tol_user;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[8] = m_mold_thrs_user;

		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[3] = m_pf_tol;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[9] = m_pf_tol_nopf;
		//pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[4] = m_pf_trsh;

		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[5] = m_pf_trshhigh;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[19] = m_pf_thsh_nopf;

		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[4] = pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[5] - 10;
		if(1 > pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[4])
		{
			pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[4] = 1;
		}

		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[10] = m_metal_tol;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[11] = m_metal_trsh;

		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[12] = m_punchburr_tol;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[13] = m_punch_mintol;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[14] = m_punch_maxtol;

		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[15] = m_punch_error_pos;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[20] = m_punch_error_pos2;

		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[16] = m_sr_screach_thesh;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[17] = m_sr_screach_hi_thesh;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[18] = m_sr_screach_tol;

		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[22] = m_thresh_dark;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[23] = m_thresh_white;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[24] = m_edit_edit_size_tol;
		pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[25] = m_edit_edit_thresh;

		pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[0] = m_map0;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[1] = m_map1;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[2] = m_map2;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[3] = m_map3;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[4] = m_map4;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[5] = m_map5;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[6] = m_map6;


	}
}
BEGIN_EVENTSINK_MAP(CSetUpFilmDlg, CDialog)
	ON_EVENT(CSetUpFilmDlg, IDC_BTNENHCTRL_FILM_SR_DARK, DISPID_CLICK, CSetUpFilmDlg::ClickBtnenhctrlFilmSrDark, VTS_NONE)
	ON_EVENT(CSetUpFilmDlg, IDC_BTNENHCTRL_FILM_SR_DARK_MINUS, DISPID_CLICK, CSetUpFilmDlg::ClickBtnenhctrlFilmSrDarkMinus, VTS_NONE)
	ON_EVENT(CSetUpFilmDlg, IDC_BTNENHCTRL_FILM_SR_DARK_TEST2, DISPID_CLICK, CSetUpFilmDlg::ClickBtnenhctrlFilmSrDarkTest2, VTS_NONE)
	ON_EVENT(CSetUpFilmDlg, IDC_BTNENHCTRL_FILM_SR_DARK2, DISPID_CLICK, CSetUpFilmDlg::ClickBtnenhctrlFilmSrDark2, VTS_NONE)
	ON_EVENT(CSetUpFilmDlg, IDC_BTNENHCTRL_FILM_SR_DARK_MINUS2, DISPID_CLICK, CSetUpFilmDlg::ClickBtnenhctrlFilmSrDarkMinus2, VTS_NONE)
	ON_EVENT(CSetUpFilmDlg, IDC_BTNENHCTRL_FILM_SR_DARK_TEST3, DISPID_CLICK, CSetUpFilmDlg::ClickBtnenhctrlFilmSrDarkTest3, VTS_NONE)
	ON_EVENT(CSetUpFilmDlg, IDC_BTNENHCTRL_FILM_TEST2, DISPID_CLICK, CSetUpFilmDlg::ClickBtnenhctrlFilmTest2, VTS_NONE)
	ON_EVENT(CSetUpFilmDlg, IDC_BTNENHCTRL_FILM_TEST7, DISPID_CLICK, CSetUpFilmDlg::ClickBtnenhctrlFilmTest7, VTS_NONE)
	ON_EVENT(CSetUpFilmDlg, IDC_BTNENHCTRL_FILM_TEST_PUNCH, DISPID_CLICK, CSetUpFilmDlg::ClickBtnenhctrlFilmTestPunch, VTS_NONE)
	ON_EVENT(CSetUpFilmDlg, IDC_BTNENHCTRL_FILM_EDIT_TEST, DISPID_CLICK, CSetUpFilmDlg::ClickBtnenhctrlFilmEditTest, VTS_NONE)
END_EVENTSINK_MAP()

void CSetUpFilmDlg::ClickBtnenhctrlFilmSrDark()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SRTest(0, 0);
}

void CSetUpFilmDlg::ClickBtnenhctrlFilmSrDarkMinus()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SRTest(0, 1);
}

void CSetUpFilmDlg::ClickBtnenhctrlFilmSrDarkTest2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SRTest(2, 2);
}

void CSetUpFilmDlg::ClickBtnenhctrlFilmSrDark2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SRTest(1, 0);
}

void CSetUpFilmDlg::ClickBtnenhctrlFilmSrDarkMinus2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SRTest(1, 1);
}

void CSetUpFilmDlg::ClickBtnenhctrlFilmSrDarkTest3()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SRTest(3, 2);	
}

void CSetUpFilmDlg::ClickBtnenhctrlFilmTest2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);
	disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	InspectFilmMold();
}

void CSetUpFilmDlg::ClickBtnenhctrlFilmTest7()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);
	disp_obj(*((CFilmInsDlg *)AfxGetApp()->m_pMainWnd)->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map2), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	InspectFilmPF();	
}

void CSetUpFilmDlg::ClickBtnenhctrlFilmTestPunch()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	Hlong rslt_area = 0;

	UpdateData(TRUE);
	
	BOOL rslt = TRUE;
	CRect ins_rect, chip_ins_rect, ocv_ins_rect;
	Hlong rslt_num = 0;
	Hobject rslt_rgn, rslt_sr_rgn, ocv_chars_rgn, rslt_notsr_rng, rslt_user_rgn, sr_out_region, rslt_sr_out_rgn;

	BOOL PuchErr;
	Hobject rslt_region;

	disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	
	int	pf_pitch = (int)((pdlg->m_dlgSetup->m_InsPara.m_dParaBase[1] * 1000.) / pdlg->m_dlgSetup->m_SysPara.m_dPara[0]);

	int edit_thresh = m_pf_trshhigh - 10;
	if(1 > edit_thresh)
	{
		edit_thresh = 1;
	}

	for(int i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{


		ins_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[i][0];
		chip_ins_rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0];

		rslt = 	pdlg->m_VisMod->FindPuchingChip(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 
													pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map4), 
													*pdlg->m_VisMod->GetHalInspectRgn(0, i, 0), 
													ins_rect, 
													chip_ins_rect, 
													m_pf_thsh_nopf, m_pf_tol_nopf, pf_pitch,
													m_punch_mintol, m_punch_maxtol, m_punchburr_tol, m_punch_error_pos, m_punch_error_pos2, 
													pdlg->m_dlgSetup->m_InsPara.m_iParaBase[2], 
													m_edit_edit_thresh,
													m_edit_edit_size_tol,
													&PuchErr, &rslt_region, &rslt_area); 
		
		if(rslt)
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("PUNCH, AREA : ") + HTuple(rslt_area));

			if(PuchErr)
			{
				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 150 + (i * 200), 10);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("PUNCH ERR"));
			}
		}
		else
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(),110 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "NG : NO PUNCH");
		}
			
			
	}
	
	UpdateData(FALSE);
}

void CSetUpFilmDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	UpdateData(TRUE);	
	// TODO: Add your message handler code here
	if(bShow)
	{
		//20150223 by
		/*
		switch(pdlg->m_VisMod->GetVisIndex())
		{
		case VISION_CAM_BOTTOM1:
			GetDlgItem(IDC_STATIC_PUNCH)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STATIC_PF_NG)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STATIC_SR_METAL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_EDIT_MAPNUM4_FILM)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_EDIT_MAPNUM5_FILM)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_EDIT_MAPNUM7_FILM)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_TEST_PUNCH)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BTNENHCTRL_FILM_TEST7)->ShowWindow(SW_HIDE);
			break;
		}
		*/
		
		m_bShow = TRUE;
		disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], pdlg->m_iMapIdx[0]), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
		ChageStateInspParamInputBox(); //20110610 lee
		Update(TRUE);
	}
	else
	{
		m_bShow = FALSE;
		Update(FALSE);
	}
}

void CSetUpFilmDlg::ClickBtnenhctrlFilmEditTest()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	ClickBtnenhctrlFilmTestPunch();
}
