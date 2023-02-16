// SetUpChipDlg.cpp : 구현 파일입니다.
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

#include "SetUpChipDlg.h"


// CSetUpChipDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSetUpChipDlg, CDialog)

CSetUpChipDlg::CSetUpChipDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetUpChipDlg::IDD, pParent)
	, m_map7(0)
	, m_out_edgesize(0)
{
	m_out_tol = 2;
	m_out_trsh = 100;
	m_mold_tol = 4;
	m_map0 = 0;
	m_map1 = 0;
	m_map2 = 0;
	m_map3 = 0;
	m_pot_tol = 30;
	m_pot_trsh = 25;
	m_map4 = 0;
	m_npot_tol = 0;
	m_potp_tol = 0;
	m_potp_trsh = 0;
	m_potp_trshhigh = 0;
	m_map5 = 0;
	m_pot_area = 0;
	m_potrange_tol = 0;
	m_broken_thrsh = 0;
	m_map6 = 0;
	m_potp2_trsh = 0;
	m_potp_tol_2 = 0;
	m_out_count = 3;
	m_out_tol_m = 2;
	m_out_trsh_m = 100;

	m_bAdvanced = FALSE;

	m_iRectIdx = 0;

	m_bShow = FALSE;
}

CSetUpChipDlg::~CSetUpChipDlg()
{
}

void CSetUpChipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MAPNUM0_CHIP, m_map0);
	DDX_Text(pDX, IDC_EDIT_MAPNUM1_CHIP, m_map1);
	DDX_Text(pDX, IDC_EDIT_MAPNUM2_CHIP, m_map2);
	DDX_Text(pDX, IDC_EDIT_MAPNUM3_CHIP, m_map3);
	DDX_Text(pDX, IDC_EDIT_MAPNUM5_CHIP, m_map4);
	DDX_Text(pDX, IDC_EDIT_MAPNUM6_CHIP, m_map5);
	DDX_Text(pDX, IDC_EDIT_MAPNUM7_CHIP, m_map6);
	DDX_Text(pDX, IDC_EDIT_OUT_TRSH_CHIP, m_out_trsh);
	DDX_Text(pDX, IDC_EDIT_OUT_TOL_CHIP, m_out_tol);
	DDX_Text(pDX, IDC_EDIT_OUT_TRSH_CHIP_M, m_out_trsh_m);
	DDX_Text(pDX, IDC_EDIT_OUT_TOL_CHIP_M, m_out_tol_m);
	DDX_Text(pDX, IDC_EDIT_OUT_COUNT_CHIP_M2, m_out_count);
	DDX_Text(pDX, IDC_EDIT_POTPART_TRSHHIG_CHIP, m_potp_trshhigh);
	DDX_Text(pDX, IDC_EDIT_MOLD_TRSHHIG_BROKEN, m_broken_thrsh);
	DDX_Text(pDX, IDC_EDIT_MOLD_TRSHHIG_CHIP, m_mold_trsh_high);
	DDX_Text(pDX, IDC_EDIT_MOLD_TOL_CHIP, m_mold_tol);
	DDX_Text(pDX, IDC_EDIT_POT_TRSH_CHIP, m_pot_trsh);
	DDX_Text(pDX, IDC_EDIT_POT_AREA, m_pot_area);
	DDX_Text(pDX, IDC_EDIT_POT_TOL_CHIP, m_pot_tol);
	DDX_Text(pDX, IDC_EDIT_NPOT_TOL_CHIP, m_npot_tol);
	DDX_Text(pDX, IDC_EDIT_POTPART_TRSH_CHIP, m_potp_trsh);
	DDX_Text(pDX, IDC_EDIT_POTPART2_TRSH_CHIP, m_potp2_trsh);
	DDX_Text(pDX, IDC_EDIT_POTPART_TOL_CHIP, m_potp_tol);
	DDX_Text(pDX, IDC_EDIT_POTPART_TOL_CHIP2, m_potp_tol_2);
	DDX_Text(pDX, IDC_EDIT_POTRANGE_TOL_CHIP, m_potrange_tol);

	DDX_Text(pDX, IDC_EDIT_MAPNUM8_CHIP, m_map7);
	DDX_Text(pDX, IDC_EDIT_OUT_TRSHHIGH_CHIP, m_out_edgesize);
}


BEGIN_MESSAGE_MAP(CSetUpChipDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CSetUpChipDlg 메시지 처리기입니다.
BEGIN_EVENTSINK_MAP(CSetUpChipDlg, CDialog)
	ON_EVENT(CSetUpChipDlg, IDC_BTNENHCTRL_CHIP_FINDCHIP, DISPID_CLICK, CSetUpChipDlg::ClickBtnenhctrlChipFindchip, VTS_NONE)
	ON_EVENT(CSetUpChipDlg, IDC_BTNENHCTRL_CHIP_CHIPOUT, DISPID_CLICK, CSetUpChipDlg::ClickBtnenhctrlChipChipout, VTS_NONE)
	ON_EVENT(CSetUpChipDlg, IDC_BTNENHCTRL_CHIP_CHIPOUT2, DISPID_CLICK, CSetUpChipDlg::ClickBtnenhctrlChipChipout2, VTS_NONE)
	ON_EVENT(CSetUpChipDlg, IDC_BTNENHCTRL_CHIP_MOLD, DISPID_CLICK, CSetUpChipDlg::ClickBtnenhctrlChipMold, VTS_NONE)
	ON_EVENT(CSetUpChipDlg, IDC_BTNENHCTRL_CHIP_POT, DISPID_CLICK, CSetUpChipDlg::ClickBtnenhctrlChipPot, VTS_NONE)
	ON_EVENT(CSetUpChipDlg, IDC_BTNENHCTRL_CHIP_POTPART, DISPID_CLICK, CSetUpChipDlg::ClickBtnenhctrlChipPotpart, VTS_NONE)
	ON_EVENT(CSetUpChipDlg, IDC_BTNENHCTRL_CHIP_POT_TEACH, DISPID_CLICK, CSetUpChipDlg::ClickBtnenhctrlChipPotTeach, VTS_NONE)
END_EVENTSINK_MAP()

void CSetUpChipDlg::ClickBtnenhctrlChipFindchip()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);

	disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	
	InspectFindChip();
}


BOOL CSetUpChipDlg::InspectFindChip()
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	BOOL rslt = TRUE;
	Hobject rslt_region;
	int i;

	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		rslt = pdlg->m_VisMod->FindChip(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 
						pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0), // GetHalImage->GetHalRedImage
						pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0],
						&rslt_region);

		if(i == 0)
		{
			HTuple hChipCenterx, hFilmCenterx;
			area_center(rslt_region, NULL, &m_hChipCenter, &hChipCenterx);	
			disp_cross(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), m_hChipCenter, hChipCenterx, 20, 0);
			m_hFilmCenter = (pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[0][0].bottom + pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[0][0].top) * 0.5;
			hFilmCenterx = (pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[0][0].left + pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[0][0].right) * 0.5;
			disp_cross(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), m_hFilmCenter, hFilmCenterx, 40, 0);
			m_hPunchOffset = m_hFilmCenter - m_hChipCenter;
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), m_hChipCenter + 75, 1000);	
			m_lTemp = (double)m_hPunchOffset[0];
			m_lstrTemp.Format("%.2f", m_lTemp * PIXEL_SIZE);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(HTuple("Punch Offset : ") + HTuple(m_lstrTemp)));	
		}

		if(rslt)
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "Chip searching success...");
		}
		else
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "Chip searching fail... NO CHIP!");
		}
	}

	return rslt;
}

void CSetUpChipDlg::ClickBtnenhctrlChipChipout()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);

	disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map1), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	InspectChipOut(0);
}

void CSetUpChipDlg::ClickBtnenhctrlChipChipout2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);

	disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map1), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	InspectChipOut(1);	
}


BOOL CSetUpChipDlg::InspectChipOut(int type)
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);

	BOOL rslt = TRUE;
	Hlong rslt_num = 0;
	Hobject rslt_chip_region, rslt_out_region;
	int i;

	int count = 0;
	int tol = 0;
	switch(type)
	{
	case 0:
		count = 10000;
		tol = m_out_tol;
		break;
	case 1:
		count = m_out_count;
		tol = 10000;
		break;
	}

	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		rslt = pdlg->m_VisMod->FindChip(0, 
						pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0),
						pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0],
						&rslt_chip_region);
		if(rslt)
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
			disp_obj(rslt_chip_region, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("blue"));
			disp_obj(pdlg->m_dlgSetup->m_HalInsRgn[i][7], pdlg->m_dlgImgRect[m_iRectIdx]->GetView());

			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "Chip searching success...");

			rslt = pdlg->m_VisMod->ChipOut(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 
											pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map1),		//Chipout
											pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map1),		//Chipout
											pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0),		//Chip인식
											pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map2),		//Chip이 약간 어두워 레진이 보이는 상태, Broken
											pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0),	    //Chip인식
											rslt_chip_region, pdlg->m_dlgSetup->m_HalInsRgn[i][7],          //chip region
											0,0,0,0,0,0,0,0,
											m_out_trsh, tol,
											m_out_trsh_m, m_out_tol_m, count, m_out_edgesize,
											&rslt_num, &rslt_out_region);
			if(rslt)
			{
				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 40 + (i * 200), 10);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "GOOD");
				
			}
			else
			{
				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 40 + (i * 200), 10);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "NG : CHIP OUT");

				disp_obj(rslt_out_region, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
			}
		}
		else
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 40 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "Chip searching fail... NO CHIP!");
		}
	}
		
	return rslt;
}


void CSetUpChipDlg::ClickBtnenhctrlChipMold()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);

	disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map3), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	InspectChipMold();
}


BOOL CSetUpChipDlg::InspectChipMold()
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);

	BOOL rslt = TRUE;
	Hlong rslt_num = 0;
	Hlong rslt_broken_num = 0;
	Hobject rslt_chip_region, rslt_mold_region, rslt_broken_region;
	int i;

	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		rslt = pdlg->m_VisMod->FindChip(0, 
						pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0),
						pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0],
						&rslt_chip_region);
		if(rslt)
		{

			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "Chip searching success...");

			rslt = pdlg->m_VisMod->ChipMold(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 
							pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map3),   //칩이면결합(Chip인식과 같은 이미지 사용)
							pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map2),   //Broken
							rslt_chip_region,0,0,0,0,0,0,0,0,
						    m_mold_trsh_high, m_mold_tol, m_broken_thrsh,
							&rslt_num, &rslt_mold_region,
							&rslt_broken_num, &rslt_broken_region);

			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
			disp_obj(rslt_chip_region, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("blue"));
			disp_obj(pdlg->m_dlgSetup->m_HalInsRgn[i][7], pdlg->m_dlgImgRect[m_iRectIdx]->GetView());

			if(rslt)
			{
				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 40 + (i * 200), 10);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "GOOD");
			}
			else
			{
				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 40 + (i * 200), 10);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "NG : CHIP BROKEN/MOLD");

				disp_obj(rslt_mold_region, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
				disp_obj(rslt_broken_region, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
			}
		}
		else
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 40 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "Chip searching fail... NO CHIP!");

		
		}

	}
		
	return rslt;
}

void CSetUpChipDlg::ClickBtnenhctrlChipPot()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);

	disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map4), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	InspectPot();
}

BOOL CSetUpChipDlg::InspectPot()
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);

	BOOL rslt = TRUE;
	Hlong rslt_num = 0;
	Hlong area = 0;
	Hobject rslt_chip_region, rslt_mold_region;
	int i;

	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		rslt = pdlg->m_VisMod->FindChip(0, 
						pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0),
						pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0],
						&rslt_chip_region);
		if(rslt)
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "Chip searching success...");

			rslt = pdlg->m_VisMod->ChipPot(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map4),
						   pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0],
						   m_pot_area,
						   m_pot_trsh, m_npot_tol, m_pot_tol, &rslt_num, &area, &m_PotRegion);
			if(rslt)
			{
				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 40 + (i * 200), 10);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple( HTuple("CHIP POT AREA : ") + HTuple(area) ) );
			}
			else
			{
				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 40 + (i * 200), 10);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple( HTuple("NG : CHIP POT AREA : ") + HTuple(area) ) );
			}
		}
		else
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 40 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "Chip searching fail... NO CHIP!");
		}
	}
		
	return rslt;
}


void CSetUpChipDlg::ClickBtnenhctrlChipPotpart()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);

	disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map5), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	if(InspectPot())
	{
		InspectPotPart();	
	}
}

BOOL CSetUpChipDlg::InspectPotPart()
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);

	BOOL rslt = TRUE;
	Hlong rslt_num = 0;
	Hlong rslt_num_range = 0;
	Hobject rslt_chip_region, rslt_potp_region, rslt_potr_region, rslt_recog_region;
	int i;

	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		rslt = pdlg->m_VisMod->FindChip(0, 
						pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0),
						pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0],
						&rslt_chip_region);
		if(rslt)
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "Chip searching success...");

			BOOL rslt;
			rslt = TRUE;

			/*
			밝기1 m_potp_thresh
			밝기2 m_potp2_thresh
			허용직경1 m_potp_tol
			허용직경2 m_potp_tol_2
			허용도포범위 m_potrange_tol
			*/

			rslt = pdlg->m_VisMod->PotPart(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 
						   pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map7), 
						   pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map6), 
						   pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map7), 
						   rslt_chip_region, m_PotRegion, 
						   *pdlg->m_VisMod->GetHalInspectRgn(0, i, 0),//SR region
						   0,0,0,0,0,0,0,0,
						   m_potp_trsh, m_potp2_trsh, m_potp_trshhigh, m_potp_tol, m_potrange_tol, pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[1], pdlg->m_dlgSetup->m_InsPara.m_iParaFilm[2],
						   &rslt_num, &rslt_potp_region, &rslt_num_range, &rslt_potr_region, &rslt_recog_region);

			
			if(rslt == TRUE)
			{
				rslt=TRUE;
			}
			else
			{
				rslt=FALSE;
			}

			if(rslt)
			{
				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 70 + (i * 200), 10);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "GOOD");
			}
			else
			{
				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 40 + (i * 200), 10);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "NG : POT PARTICLE");
				//write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "NG : POT PARTICLE/주변수지묻음");
			}
		}
		else
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 70 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "Chip searching fail... NO CHIP!");
		}
	}
		
	return rslt;
}

void CSetUpChipDlg::ClickBtnenhctrlChipPotTeach()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	BOOL rslt = TRUE;
	Hlong rslt_num = 0;
	Hlong area = 0;
	Hobject rslt_chip_region, rslt_mold_region, ChipRgn;
	int i;
	HTuple Min;
	Hlong min;

	UpdateData(TRUE);

	//첫번째것만 가지고
	//Find suitable threshold
	CRect rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[0][0];
	
	gen_rectangle1(&ChipRgn, rect.top, rect.left, rect.bottom, rect.right);

	min_max_gray(ChipRgn, *pdlg->m_VisMod->GetHalChipMasterImage(0), HTuple(0), &Min, _, _);
	min = (Hlong)Min[0];
	min = min + 35;
	if( (0 > min) || (255 < min) )
	{
		min = 70;
	}

	//m_pot_trsh = min; // imsi

	disp_obj(*pdlg->m_VisMod->GetHalChipMasterImage(0), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());

	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		rslt = pdlg->m_VisMod->FindChip(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 
						pdlg->m_VisMod->GetHalChipMasterImage(0), 
						pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0],
						&rslt_chip_region);

		//HTuple Temp1, Temp2, Temp3, Temp4;
		//smallest_rectangle1(rslt_chip_region,&Temp1,&Temp2,&Temp3,&Temp4);

		if(i == 0)
		{
			HTuple hChipCenterx, hFilmCenterx;
			area_center(rslt_chip_region, NULL, &m_hChipCenter, &hChipCenterx);	
			disp_cross(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), m_hChipCenter, hChipCenterx, 20, 0);
			m_hFilmCenter = (pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[0][0].bottom + pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[0][0].top) * 0.5;
			hFilmCenterx = (pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[0][0].left + pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[0][0].right) * 0.5;
			disp_cross(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), m_hFilmCenter, hFilmCenterx, 40, 0);
			m_hPunchOffset = m_hFilmCenter - m_hChipCenter;
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), m_hChipCenter + 75, 1000);	
			m_lTemp = (double)m_hPunchOffset[0];
			m_lstrTemp.Format("%.2f", m_lTemp * PIXEL_SIZE);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(HTuple("Punch Offset : ") + HTuple(m_lstrTemp)));	
		}

		if(rslt)
		{
			//Set chip region
			copy_obj(rslt_chip_region, &pdlg->m_dlgSetup->m_HalInsRgn[i][7], 1, -1);

			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "Chip searching success...");

			rslt = pdlg->m_VisMod->ChipPot(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map4),
						   pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0],
						   m_pot_area,
						   m_pot_trsh, m_npot_tol, m_pot_tol, &rslt_num, &area, &m_PotRegion);

			HTuple Temp1, Temp2, Temp3, Temp4;
			smallest_rectangle1(rslt_chip_region,&Temp1,&Temp2,&Temp3,&Temp4);

			if(rslt)
			{
				m_pot_area = area;
			}
			else
			{
				m_pot_area = area;
			}
		}
		else
		{
			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));
			set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 40 + (i * 200), 10);
			write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "Chip searching fail... NO CHIP!");
		}
	}


	m_potp_trsh = m_pot_trsh + 5;

	if(255 < m_potp_trsh)
	{
		m_potp_trsh = 230;
	}
	if(255 < m_potp_trshhigh)
	{
		m_potp_trsh = 230;
	}
		
	UpdateData(FALSE);
}


void CSetUpChipDlg::ChageStateInspParamInputBox()
{
	BOOL state;
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	state = pdlg->m_dlgSetup->m_SysPara.m_bPara[34];
	
	GetDlgItem(IDC_EDIT_OUT_TRSH_CHIP)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_OUT_TRSHHIGH_CHIP)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_OUT_TOL_CHIP)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_OUT_TRSH_CHIP_M)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_OUT_TRSHHIGH_CHIP_M)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_OUT_TOL_CHIP_M)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_OUT_COUNT_CHIP_M2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_MOLD_TRSHHIG_BROKEN)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_MOLD_TRSHHIG_CHIP)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_MOLD_TOL_CHIP)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_POT_TRSH_CHIP)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_POT_AREA)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_POT_TOL_CHIP)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_NPOT_TOL_CHIP)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_POTPART_TRSH_CHIP)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_POTPART2_TRSH_CHIP)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_POTPART_TOL_CHIP)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_POTRANGE_TOL_CHIP)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_POTPART_TOL_CHIP2)->EnableWindow(state);
}


BOOL CSetUpChipDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	set_draw(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("margin"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

HBRUSH CSetUpChipDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void CSetUpChipDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	UpdateData(TRUE);	

	CRect rect;

	if(bShow)
	{
		m_bShow = TRUE;
		disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], pdlg->m_iMapIdx[0]), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
		ChageStateInspParamInputBox();

		//20150223 by
		switch(pdlg->m_VisMod->GetVisIndex())
		{
			case VISION_CAM_TOP1:
				GetDlgItem(IDC_STATIC_CHIP1)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM1_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_CHIP2)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM2_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_CHIP3)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM3_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_CHIP_CHIPOUT)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_CHIP_CHIPOUT2)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_CHIP_MOLD)->ShowWindow(SW_HIDE);
				break;
			case VISION_CAM_TOP2:
				GetDlgItem(IDC_STATIC_CHIP4)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_CHIP5)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM5_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM6_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_CHIP6)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM7_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_CHIP7)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM8_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_CHIP_POT)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_CHIP_POTPART)->ShowWindow(SW_HIDE);
				break;
			case VISION_CAM_BOTTOM:

				break;
			default:
				AfxMessageBox("Error : invalid index(CSetUpChipDlg::OnShowWindow)!");
				break;
		}
		/*
		switch(pdlg->m_VisMod->GetVisIndex())
		{
			case VISION_CAM_TOP1:
				GetDlgItem(IDC_STATIC_CHIP1)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM1_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_CHIP2)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM2_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_CHIP3)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM3_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_CHIP_CHIPOUT)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_CHIP_CHIPOUT2)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_CHIP_MOLD)->ShowWindow(SW_HIDE);
				break;
			case VISION_CAM_BOTTOM1:
				GetDlgItem(IDC_STATIC_CHIP1)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM1_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_CHIP2)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM2_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_CHIP3)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM3_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_CHIP_CHIPOUT)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_CHIP_CHIPOUT2)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_CHIP_MOLD)->ShowWindow(SW_HIDE);

				GetDlgItem(IDC_STATIC_CHIP4)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_CHIP5)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM5_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM6_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_CHIP6)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM7_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_CHIP7)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM8_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_CHIP_POT)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_CHIP_POTPART)->ShowWindow(SW_HIDE);
				break;
			case VISION_CAM_TOP2:
				GetDlgItem(IDC_STATIC_CHIP0)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM0_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_CHIP4)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_CHIP5)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM5_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM6_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_CHIP6)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM7_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_CHIP7)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM8_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_CHIP_POT)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_CHIP_POTPART)->ShowWindow(SW_HIDE);
				break;
			case VISION_CAM_BOTTOM2:
				GetDlgItem(IDC_STATIC_CHIP1)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM1_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_CHIP2)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM2_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_CHIP3)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM3_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_CHIP_CHIPOUT)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_CHIP_CHIPOUT2)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_CHIP_MOLD)->ShowWindow(SW_HIDE);

				GetDlgItem(IDC_STATIC_CHIP4)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_CHIP5)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM5_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM6_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_CHIP6)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM7_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_CHIP7)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_EDIT_MAPNUM8_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_CHIP_POT)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_CHIP_POTPART)->ShowWindow(SW_HIDE);
				break;
			default:
				AfxMessageBox("Error : invalid index(CSetUpChipDlg::OnShowWindow)!");
				break;
		}
		*/

		Update(TRUE);
	}
	else
	{
		m_bShow = FALSE;
		Update(FALSE);
	}
}


void CSetUpChipDlg::Update(BOOL flag)
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	if(flag)
	{
		//이중충진, 미충진///////////////////////
		m_pot_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[6];
		m_pot_trsh = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[7];
		m_npot_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[8];

		//Chip out///////////////////////////////
		m_out_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[0];
		m_out_trsh = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[1];
		//m_out_trsh_high = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[2];

		//Chip out micro///////////////////////////////
		m_out_tol_m = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[17];
		m_out_trsh_m = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[18];
		m_out_edgesize = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[19];
		m_out_count = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[20];


		//Chip 이면결함//////////////////////////////
		m_mold_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[3];
		//m_mold_trsh = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[4];
		//m_mold_trsh = 0;
		m_mold_trsh_high = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[5];

		//Pot particle///////////////////////////
		m_potp_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[9];
		m_potp_trsh = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[10];
		m_potp_trshhigh = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[11];
		
		m_pot_area = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[12];
		m_potrange_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[13];

		m_broken_thrsh = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[14];

		m_potp2_trsh = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[15];

		m_potp_tol_2 = pdlg->m_dlgSetup->m_InsPara.m_iParaChip[16];

		switch(pdlg->m_VisMod->GetVisIndex())
		{
		case VISION_CAM_TOP1:
			m_map0 = 1;//Chip인식/포팅
			m_map1 = 1;//Chipout(IC표면 밝게)
			m_map2 = 1;//Broken/(IC표면의 어두운것이 잘보이게)
			m_map3 = 1;//칩이면결함(톱니 모양 불량, 밝은 IC면에서 어두운 불량) = broken 조건일수도
			m_map4 = 1;//Potting
			m_map5 = 1;//Potting위 도포불량
			m_map6 = 1;//Potting위 어두운이물
			m_map7 = 3;//Potting위 밝은 이무
			break;
		case VISION_CAM_TOP2:
			m_map0 = 1;//Chip인식/포팅
			m_map1 = 0;//Chipout(IC표면 밝게)
			m_map2 = 1;//Broken/(IC표면의 어두운것이 잘보이게)
			m_map3 = 1;//칩이면결함(톱니 모양 불량, 밝은 IC면에서 어두운 불량) = broken 조건일수도
			m_map4 = 1;//Potting
			m_map5 = 1;//Potting위 도포불량
			m_map6 = 1;//Potting위 어두운이물
			m_map7 = 3;//Potting위 밝은 이무
			break;
		}

		UpdateData(FALSE);
	}
	else
	{
		UpdateData(TRUE);

		//이중충진, 미충진///////////////////////
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[6] = m_pot_tol;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[7] = m_pot_trsh; 
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[8] = m_npot_tol;

		//Chip out///////////////////////////////
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[0] = m_out_tol;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[1] = m_out_trsh;
		//pdlg->m_dlgSetup->m_InsPara.m_iParaChip[2] = m_out_trsh_high;

		//Chip out///////////////////////////////
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[17] = m_out_tol_m;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[18] = m_out_trsh_m;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[19] = m_out_edgesize;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[20] = m_out_count;


		//Chip 이면결함//////////////////////////////
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[3] = m_mold_tol;
		//pdlg->m_dlgSetup->m_InsPara.m_iParaChip[4] = m_mold_trsh;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[4] = 0;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[5] = m_mold_trsh_high;

		//Pot particle///////////////////////////

		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[9] = m_potp_tol;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[10] = m_potp_trsh;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[11] = m_potp_trshhigh;


		if(255 < pdlg->m_dlgSetup->m_InsPara.m_iParaChip[10])
		{
			pdlg->m_dlgSetup->m_InsPara.m_iParaChip[10] = 255;
		}
		if(255 < pdlg->m_dlgSetup->m_InsPara.m_iParaChip[11])
		{
			pdlg->m_dlgSetup->m_InsPara.m_iParaChip[11] = 255;
		}

		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[12] = m_pot_area;
		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[13] = m_potrange_tol;

		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[14] = m_broken_thrsh;

		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[15] = m_potp2_trsh;

		pdlg->m_dlgSetup->m_InsPara.m_iParaChip[16] = m_potp_tol_2;

		pdlg->m_dlgSetup->m_InsPara.m_iParaMapChip[0] = m_map0;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapChip[1] = m_map1;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapChip[2] = m_map2;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapChip[3] = m_map3;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapChip[4] = m_map4;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapChip[5] = m_map5;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapChip[6] = m_map6;
		pdlg->m_dlgSetup->m_InsPara.m_iParaMapChip[7] = m_map7;

	}
}
