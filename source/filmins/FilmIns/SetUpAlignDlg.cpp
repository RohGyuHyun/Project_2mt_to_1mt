// SetUpAlignDlg.cpp : 구현 파일입니다.
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

#include "SetUpAlignDlg.h"

#include "DialogSRSubRegion.h"


#define SR_CRACK	0
#define PF_HALL		1
#define OUT_LEAD	2
#define IN_LEAD		3
#define USER_AREA	4
#define SR_OUT		5


// CSetUpAlignDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSetUpAlignDlg, CDialog)

CSetUpAlignDlg::CSetUpAlignDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetUpAlignDlg::IDD, pParent)
	, m_map4(0)
{
	m_thrsh = 30;
	m_score = 70;
	m_map0 = 0;
	m_poslimit = 0;
	m_map1 = 0;
	m_map2 = 0;
	m_map3 = 0;
	m_map4 = 0;
	m_iIndex = 0;
	//}}AFX_DATA_INIT

	m_iRectIdx = 0;

	m_bShow = FALSE;
}

CSetUpAlignDlg::~CSetUpAlignDlg()
{
}

void CSetUpAlignDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TRSH_ALIGN, m_thrsh);
	DDX_Text(pDX, IDC_EDIT_SCORE_ALIGN, m_score);
	DDX_Text(pDX, IDC_EDIT_LIMIT_ALIGN, m_poslimit);
	DDX_Text(pDX, IDC_EDIT_MAPNUM0_ALIGN, m_map0);
	DDX_Text(pDX, IDC_EDIT_MAPNUM1_ALIGN, m_map1);
	DDX_Text(pDX, IDC_EDIT_MAPNUM2_ALIGN, m_map2);
	DDX_Text(pDX, IDC_EDIT_MAPNUM3_ALIGN, m_map3);
	DDX_Text(pDX, IDC_EDIT_MAPNUM_CHIP, m_map4);
}


BEGIN_MESSAGE_MAP(CSetUpAlignDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CSetUpAlignDlg 메시지 처리기입니다.

HBRUSH CSetUpAlignDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void CSetUpAlignDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	if(bShow)
	{
		m_bShow = TRUE;
		disp_obj(*pdlg->m_VisMod->GetHalMasterImage(0), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
		pdlg->m_iMapIdx[0] = 0;
		//disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], pdlg->m_iMapIdx[0]), pdlg->m_dlgImgRect[0]->GetView());

		pdlg->UpdateMapButton(pdlg->m_iMapIdx[0]);    
		
		//20150223 by
		switch(pdlg->m_VisMod->GetVisIndex())
		{
			case VISION_CAM_TOP1:

				break;
			case VISION_CAM_TOP2:
				//GetDlgItem(IDC_STATIC_SR)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_PF)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_SROUT)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_STATIC_MASTER_RECT_SR)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_MASTER_RECT_LEAD)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_MASTER_RECT_SROUT)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SR_START)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_SR)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SR_MINUS)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SRO_START)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SROUT)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SROUT_MINUS)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_INS_REGION_DIR0)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_INS_REGION_DIR1)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_PF_START)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_PF)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_PF_MINUS)->ShowWindow(SW_HIDE);

				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SR_MINUS_V)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SROUT_MINUS_V)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_PF_MINUS_V)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_M_MINUS_V)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_LEAD_V)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_LEAD_IN_V)->ShowWindow(SW_HIDE);
				break;
			case VISION_CAM_BOTTOM:
				GetDlgItem(IDC_STATIC_SR)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_PF)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_SROUT)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_MASTER_RECT_SR)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_MASTER_RECT_LEAD)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_MASTER_RECT_SROUT)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SR_START)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_SR)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SR_MINUS)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SRO_START)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SROUT)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SROUT_MINUS)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_INS_REGION_DIR0)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_INS_REGION_DIR1)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_PF_START)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_PF)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_PF_MINUS)->ShowWindow(SW_HIDE);

				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SR_MINUS_V)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SROUT_MINUS_V)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_PF_MINUS_V)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_M_MINUS_V)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_LEAD_V)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_LEAD_IN_V)->ShowWindow(SW_HIDE);

				GetDlgItem(IDC_STATIC_4INSREGSET)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_AM_START)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_AM)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_M_MINUS)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_M_MINUS_V)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_LEAD)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_LEAD_IN)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_5CHIPSRCTEACH)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_FINDCHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_POT_TEACH)->ShowWindow(SW_HIDE);

				GetDlgItem(IDC_STATIC_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_MASTER_RECT_CHIP)->ShowWindow(SW_HIDE);
				break;
			default:
				AfxMessageBox("Error : invalid index(CSetUpChipDlg::OnShowWindow)!");
				break;
		}
		/*
		switch(pdlg->m_VisMod->GetVisIndex())
		{
			case VISION_CAM_TOP1:
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_AM_START)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_AM)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_M_MINUS)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_M_MINUS_V)->ShowWindow(SW_HIDE);
				break;
			case VISION_CAM_BOTTOM1:
				//GetDlgItem(IDC_STATIC_SR)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_PF)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_CHIP)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_STATIC_SROUT)->ShowWindow(SW_HIDE);

				//GetDlgItem(IDC_STATIC_MASTER_RECT_SR)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_MASTER_RECT_LEAD)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_MASTER_RECT_CHIP)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_STATIC_MASTER_RECT_SROUT)->ShowWindow(SW_HIDE);
				
				//GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SR_START)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_SR)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SR_MINUS)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SRO_START)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SROUT)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SROUT_MINUS)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_INS_REGION_DIR0)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_INS_REGION_DIR1)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_PF_START)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_PF)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_PF_MINUS)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_PF_MINUS_V)->ShowWindow(SW_HIDE);

				GetDlgItem(IDC_STATIC_5CHIPSRCTEACH)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_FINDCHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_POT_TEACH)->ShowWindow(SW_HIDE);

				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_AM_START)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_AM)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_M_MINUS)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_M_MINUS_V)->ShowWindow(SW_HIDE);

				//GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SR_MINUS_V)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SROUT_MINUS_V)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_M_MINUS_V)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_LEAD_V)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_LEAD_IN_V)->ShowWindow(SW_HIDE);
				break;
			case VISION_CAM_TOP2:
				
				GetDlgItem(IDC_STATIC_SR)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_PF)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_SROUT)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_MASTER_RECT_SR)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_MASTER_RECT_LEAD)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_MASTER_RECT_SROUT)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SR_START)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_SR)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SR_MINUS)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SRO_START)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SROUT)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SROUT_MINUS)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_INS_REGION_DIR0)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_INS_REGION_DIR1)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_PF_START)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_PF)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_PF_MINUS)->ShowWindow(SW_HIDE);

				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SR_MINUS_V)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SROUT_MINUS_V)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_PF_MINUS_V)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_M_MINUS_V)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_LEAD_V)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_LEAD_IN_V)->ShowWindow(SW_HIDE);

				//GetDlgItem(IDC_STATIC_4INSREGSET)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_AM_START)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_AM)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_M_MINUS)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_M_MINUS_V)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_LEAD)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_LEAD_IN)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_STATIC_5CHIPSRCTEACH)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_BTNENHCTRL_ALIGN_FINDCHIP)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_BTNENHCTRL_ALIGN_POT_TEACH)->ShowWindow(SW_HIDE);

				//GetDlgItem(IDC_STATIC_CHIP)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_STATIC_MASTER_RECT_CHIP)->ShowWindow(SW_HIDE);
				break;
			case VISION_CAM_BOTTOM2:
			
				//GetDlgItem(IDC_STATIC_SR)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_PF)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_SROUT)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_STATIC_MASTER_RECT_SR)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_MASTER_RECT_LEAD)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_MASTER_RECT_SROUT)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SR_START)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_SR)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SR_MINUS)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SRO_START)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SROUT)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SROUT_MINUS)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_INS_REGION_DIR0)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_INS_REGION_DIR1)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_PF_START)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_PF)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_PF_MINUS)->ShowWindow(SW_HIDE);

				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SR_MINUS_V)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_SROUT_MINUS_V)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_PF_MINUS_V)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_LEAD_V)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_LEAD_IN_V)->ShowWindow(SW_HIDE);

				//GetDlgItem(IDC_STATIC_4INSREGSET)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_AM_START)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_AM)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_M_MINUS)->ShowWindow(SW_HIDE);
				//GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_M_MINUS_V)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_LEAD)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_REGION_LEAD_IN)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_5CHIPSRCTEACH)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_FINDCHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_POT_TEACH)->ShowWindow(SW_HIDE);

				GetDlgItem(IDC_STATIC_CHIP)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_MASTER_RECT_CHIP)->ShowWindow(SW_HIDE);

				//GetDlgItem(IDC_BTNENHCTRL_ALIGN_MAKE_SR)->ShowWindow(SW_HIDE);

				GetDlgItem(IDC_STATIC_BLACK)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC_WHITE)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_SR_DARK_P)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_SR_DARK_M)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_SR_WHITE_P)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_BTNENHCTRL_ALIGN_INS_SR_WHITE_M)->ShowWindow(SW_HIDE);
				break;

			default:
				AfxMessageBox("Error : invalid index(CSetUpAlignDlg::OnShowWindow)!");
				break;
		}
		*/

		Update(TRUE);
	}
	else
	{
		m_bShow = FALSE;
		Update(FALSE);

		pdlg->m_dlgImgRect[m_iRectIdx]->DeleteAll();
	}

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

BOOL CSetUpAlignDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	set_draw(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("margin"));

	CRect rect;

	GetDlgItem(IDC_STATIC_MASTER_RECT)->GetClientRect(&rect);

	set_check("~father");
	open_window(0, 0, rect.Width(), rect.Height(), (Hlong)GetDlgItem(IDC_STATIC_MASTER_RECT)->m_hWnd, "transparent", "", &m_lMWindowID);
	set_check("father");
	set_part(m_lMWindowID, 0, 0, pdlg->m_lHeight[0], pdlg->m_lWidth[0]);

	set_check("~father");
	open_window(0, 0, rect.Width(), rect.Height(), (Hlong)GetDlgItem(IDC_STATIC_MASTER_RECT_SR)->m_hWnd, "transparent", "", &m_lMWindowID_SR);
	set_check("father");
	set_part(m_lMWindowID_SR, 0, 0, pdlg->m_lHeight[0], pdlg->m_lWidth[0]);

	set_check("~father");
	open_window(0, 0, rect.Width(), rect.Height(), (Hlong)GetDlgItem(IDC_STATIC_MASTER_RECT_LEAD)->m_hWnd, "transparent", "", &m_lMWindowID_Lead);
	set_check("father");
	set_part(m_lMWindowID_Lead, 0, 0, pdlg->m_lHeight[0], pdlg->m_lWidth[0]);

	set_check("~father");
	open_window(0, 0, rect.Width(), rect.Height(), (Hlong)GetDlgItem(IDC_STATIC_MASTER_RECT_SROUT)->m_hWnd, "transparent", "", &m_lMWindowID_SROut);
	set_check("father");
	set_part(m_lMWindowID_SROut, 0, 0, pdlg->m_lHeight[0], pdlg->m_lWidth[0]);

	set_check("~father");
	open_window(0, 0, rect.Width(), rect.Height(), (Hlong)GetDlgItem(IDC_STATIC_MASTER_RECT_CHIP)->m_hWnd, "transparent", "", &m_lMWindowID_Chip);
	set_check("father");
	set_part(m_lMWindowID_Chip, 0, 0, pdlg->m_lHeight[0], pdlg->m_lWidth[0]);
	
	MasterImageCoverttoHalImageBuffer(); //20110511 lee

	m_lSrTeachThr = 100;
	m_dSrTeachThrDark = 0.55;
	m_dSrTeachThrWhite = 1.2;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CSetUpAlignDlg::MasterImageCoverttoHalImageBuffer()
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	for(int i = 0; (i < pdlg->m_VisMod->GetMapNum(0)) && (i < IMAGE_MAP_MAX_NUM); i++)
	{
		//copy_image(*pdlg->m_VisMod->GetHalTaughtImage(0,i), pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], i));
	}

}

void CSetUpAlignDlg::SetRegion(int idx)		// 110829 ytlee		region 저장 관련 Tracker 유무에 따른 순서 꼬임 방지
{
	/* 20110601 Dongwhee BEGIN
	 *   Update()에서 C_CtrlImageRectDlg에 Tracker를 추가하는 순서대로 인덱스가 구성된다
		 원하는 Tracker에 접근하기 위해서는 추가한 순서에 맞춰 인덱스를 계산해야 한다
		 이를 위한 인덱스 계산 하는 내용은 다음과 같다

		입력순서 입력개수      입력 내용        인덱스 계산 
		1			1				align mark			0
		2			1C				align search		1
		3			1C				chip search			1 1C
		4			1C				film search			1 2C
		5			2C				in,out lead			1 3C
		6			1				character pattern	1 5C
		7			1C				character search	1 5C  1
		8			1C				SR area				1 6C  1
		9			1				2nd align mark		1 7C  1
		10			1C				2nd align search	1 7C  1 1
		11			2C				2nd, 3rd out lead	1 8C  1 1
		12          2C				A/M scratch #0      1 10C 1 1
		13          2C				A/M scratch #1      1 12C 1 1

		  # 1C = 1 Cycle = pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]
		  ex) 1 2C 1 = 1 + pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0] * 2 + 1
	20110601 Dongwhee END*/
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	
	CRect rect;
	long left, top, right, bottom;
	int i;
	int inc, index;

	if(SUB_RGN_MAX_NUM < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0])
	{
		pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0] = SUB_RGN_MAX_NUM;
	}

	switch(idx)
	{
	//Align search
	case 0:
		//Align search
 		m_iIndex = 0;		// In Lead, OCV가 없을 경우 저장 순서 index 밀림	
		if( ((!pdlg->m_dlgSetup->m_SysPara.m_bPara[22]) && 
			(!pdlg->m_dlgSetup->m_SysPara.m_bPara[23])) )
		{
			m_iIndex -= pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0];
		}
 		if(!pdlg->m_dlgSetup->m_SysPara.m_bPara[25])
		{
			m_iIndex -= pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0];
			m_iIndex -= 1;
		}
		inc = 1;
		for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
		{
			pdlg->m_dlgImgRect[m_iRectIdx]->GetTracker(i + 1, &left, &top, &right, &bottom); // 20110527 Dongwhe* 2nd Align Search Tracker: i + inc -> i + 1
			rect.left = left;
			rect.top = top;
			rect.right = right;
			rect.bottom = bottom;
						
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[i][0] = rect;

			// 20110527 Dongwhe* BEGIN 2nd Align Search Tracker
			// 2nd Align Search Tracker 저장
			
			// 2nd Align Search Tracker 순서 계산
			// (SR영역 계산 법에 1만 추가함, 추후 Tracker 관리방법 개선필요)
			inc = 1 + pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0] * 7 + 1 + 1; // 20110601 Dongwhee 인덱스 계산 변경
			pdlg->m_dlgImgRect[m_iRectIdx]->GetTracker(i + inc + m_iIndex, &left, &top, &right, &bottom);		// 110829 ytlee
			rect.left = left;
			rect.top = top;
			rect.right = right;
			rect.bottom = bottom;
			
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[i][2] = rect;
			//END
		}
		break;
	//Align mark
	case 1:
		//Align mark
		m_iIndex = 0;		// In Lead, OCV가 없을 경우 저장 순서 index 밀림
		if( 
			(!pdlg->m_dlgSetup->m_SysPara.m_bPara[22]) && 
			(!pdlg->m_dlgSetup->m_SysPara.m_bPara[23]) )
		{
			m_iIndex -= pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0];
		}
 		if(!pdlg->m_dlgSetup->m_SysPara.m_bPara[25])
		{
			m_iIndex -= pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0];
			m_iIndex -= 1;
		}
		pdlg->m_dlgImgRect[m_iRectIdx]->GetTracker(0, &left, &top, &right, &bottom);
		rect.left = left;
		rect.top = top;
		rect.right = right;
		rect.bottom = bottom;
					
		pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[0][1] = rect;//only one

		// 20110527 Dongwhe* BEGIN 2nd Align mark
		// 2nd Align mark Tracker 저장
		
		// 2nd Align mark Tracker 순서 계산
		inc = 1 + pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0] * 7 + 1; // 20110601 Dongwhee 인덱스 계산 변경
		pdlg->m_dlgImgRect[m_iRectIdx]->GetTracker(inc + m_iIndex, &left, &top, &right, &bottom);		// 110829 ytlee
		rect.left = left;
		rect.top = top;
		rect.right = right;
		rect.bottom = bottom;
		
		pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[0][3] = rect;
		// 20110527 Dongwhe* END
		break;
	case 2:
		//Chip search
		m_iIndex = 0;		
		inc = 1 + pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0] * 1;
		for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
		{
			pdlg->m_dlgImgRect[m_iRectIdx]->GetTracker(i + inc + m_iIndex, &left, &top, &right, &bottom);		// 110829 ytlee
			rect.left = left;
			rect.top = top;
			rect.right = right;
			rect.bottom = bottom;

			pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0] = rect;
		}
		break;
	case 3:
		//Film search
		m_iIndex = 0;		
		inc = 1 + pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0] * 2;
		for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
		{
			pdlg->m_dlgImgRect[m_iRectIdx]->GetTracker(i + inc + m_iIndex, &left, &top, &right, &bottom);		// 110829 ytlee
			rect.left = left;
			rect.top = top;
			rect.right = right;
			rect.bottom = bottom;

			pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[i][0] = rect;
		}
		break;
	case 6:
		//Lead search
		m_iIndex = 0;		
		inc = 1 + pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0] * 3;
		for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
		{
			//상부
			pdlg->m_dlgImgRect[m_iRectIdx]->GetTracker(i * 2 + inc + m_iIndex, &left, &top, &right, &bottom);		// 110829 ytlee
			rect.left = left;
			rect.top = top;
			rect.right = right;
			rect.bottom = bottom;

			pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][0] = rect;

			if( 
			(!pdlg->m_dlgSetup->m_SysPara.m_bPara[22]) && 
			(!pdlg->m_dlgSetup->m_SysPara.m_bPara[23]) )	// 110829 ytlee In Lead 사용 안할경우 Step안에서 Index 순서 임의 변경
			{
				m_iIndex -= 1;
			}

	  		if( 
			(pdlg->m_dlgSetup->m_SysPara.m_bPara[22]) || 
			(pdlg->m_dlgSetup->m_SysPara.m_bPara[23]) )	// 110826 ytlee 사용 하는 경우만 보이고 저장
 			{
			//하부
			pdlg->m_dlgImgRect[m_iRectIdx]->GetTracker(i * 2 + inc + 1 + m_iIndex, &left, &top, &right, &bottom);		// 110829 ytlee
			rect.left = left;
			rect.top = top;
			rect.right = right;
			rect.bottom = bottom;

			pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][1] = rect;
			}
		}

		m_iIndex = 0;		//OCV가 없을 경우 저장 순서 index 밀림
 		if( 
			(!pdlg->m_dlgSetup->m_SysPara.m_bPara[22]) && 
			(!pdlg->m_dlgSetup->m_SysPara.m_bPara[23]) )
		{
			m_iIndex -= pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0];
		}
 		if(!pdlg->m_dlgSetup->m_SysPara.m_bPara[25])
		{
			m_iIndex -= pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0];
			m_iIndex -= 1;
		}

		//BEGIN : 2nd, 3rd lead 가 사용하는 경우에만 보이고 저장되도록 수정
		if(pdlg->m_dlgSetup->m_SysPara.m_bPara[24]) // 사용하지 않는 경우 Update()에서 Tracker를 생성하지도 않았다 저장도 하지말자
		{
			inc = 1 + pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0] * 8 + 1 + 1;
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				//상부2
				pdlg->m_dlgImgRect[m_iRectIdx]->GetTracker(i * 2 + inc + m_iIndex, &left, &top, &right, &bottom);		// 110829 ytlee
				rect.left = left;
				rect.top = top;
				rect.right = right;
				rect.bottom = bottom;
				pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][2] = rect;
				//상부3
				pdlg->m_dlgImgRect[m_iRectIdx]->GetTracker(i * 2 + inc + 1 + m_iIndex, &left, &top, &right, &bottom);	// 110829 ytlee
				rect.left = left;
				rect.top = top;
				rect.right = right;
				rect.bottom = bottom;
				pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][3] = rect;
			}
		}
		// END

		break;
	case 4:
		//OCV search
		m_iIndex = 0;		//In Lead가 없을 경우 저장 순서 index 밀림
 		if( 
			(!pdlg->m_dlgSetup->m_SysPara.m_bPara[22]) && 
			(!pdlg->m_dlgSetup->m_SysPara.m_bPara[23]) )
		{
			m_iIndex -= pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0];
		}
		if(pdlg->m_dlgSetup->m_SysPara.m_bPara[25])		//사용 하는 경우만 보이고 저장
 		{
			inc = 1 + pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0] * 5 + 1; // 20110601 Dongwhee 인덱스 계산 변경
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				pdlg->m_dlgImgRect[m_iRectIdx]->GetTracker(i + inc + m_iIndex, &left, &top, &right, &bottom);
				rect.left = left;
				rect.top = top;
				rect.right = right;
				rect.bottom = bottom;
				
				pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[i][0] = rect;
			}
		}
		break;
	case 5:
		//OCV mark
		m_iIndex = 0;		// In Lead가 없을 경우 저장 순서 index 밀림
 		if( 
			(!pdlg->m_dlgSetup->m_SysPara.m_bPara[22]) && 
			(!pdlg->m_dlgSetup->m_SysPara.m_bPara[23]) )
		{
			m_iIndex -= pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0];
		}
		if(pdlg->m_dlgSetup->m_SysPara.m_bPara[25])		//사용 하는 경우만 보이고 저장
 		{
		inc = 1 + pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0] * 5; //인덱스 계산 변경
		pdlg->m_dlgImgRect[m_iRectIdx]->GetTracker(inc + m_iIndex, &left, &top, &right, &bottom);
		rect.left = left;
		rect.top = top;
		rect.right = right;
		rect.bottom = bottom;
					
		pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1] = rect;//only one
		}
		break;
	case 7:
		//SR search
		m_iIndex = 0;		//In Lead, OCV가 없을 경우 저장 순서 index 밀림
 		if( 
			(!pdlg->m_dlgSetup->m_SysPara.m_bPara[22]) && 
			(!pdlg->m_dlgSetup->m_SysPara.m_bPara[23]) )
		{
			m_iIndex -= pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0];
		}
 		if(!pdlg->m_dlgSetup->m_SysPara.m_bPara[25])
		{
			m_iIndex -= pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0];
			m_iIndex -= 1;
		}

		//20150223 by
		switch(pdlg->m_VisMod->GetVisIndex())
		{
			case VISION_CAM_TOP1:
			case VISION_CAM_TOP2:
			case VISION_CAM_BOTTOM:
				inc = 1 + pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0] * 6 + 1; //인덱스 계산 변경
				break;
		}
	

		for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
		{
			pdlg->m_dlgImgRect[m_iRectIdx]->GetTracker(i + inc + m_iIndex, &left, &top, &right, &bottom);
			rect.left = left;
			rect.top = top;
			rect.right = right;
			rect.bottom = bottom;
					
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0] = rect;
		}
		break;
	case 8:
		//20150223 by
		//SR search
		m_iIndex = 0;		//In Lead, OCV가 없을 경우 저장 순서 index 밀림
 		if( 
			(!pdlg->m_dlgSetup->m_SysPara.m_bPara[22]) && 
			(!pdlg->m_dlgSetup->m_SysPara.m_bPara[23]) )
		{
			m_iIndex -= pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0];
		}

 		if(!pdlg->m_dlgSetup->m_SysPara.m_bPara[25])
		{
			m_iIndex -= pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0];
			m_iIndex -= 1;
		}

		if(!pdlg->m_dlgSetup->m_SysPara.m_bPara[24])		//out2
 		{
			m_iIndex -= pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0];
			m_iIndex -= pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0];
		}


		if(pdlg->m_dlgSetup->m_SysPara.m_bPara[15])		//사용 하는 경우만 보이고 저장
 		{
			index = 0;
			inc = 1 + pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0] * 10 + 1 + 1;
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				pdlg->m_dlgImgRect[m_iRectIdx]->GetTracker(index + inc + m_iIndex, &left, &top, &right, &bottom);
				rect.left = left;
				rect.top = top;
				rect.right = right;
				rect.bottom = bottom;

				index++;
				
				pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][4] = rect;

				pdlg->m_dlgImgRect[m_iRectIdx]->GetTracker(index + inc + m_iIndex, &left, &top, &right, &bottom);
				rect.left = left;
				rect.top = top;
				rect.right = right;
				rect.bottom = bottom;
				
				pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][5] = rect;

				index++;
			}
		}
		else
		{
			m_iIndex -= pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0];
		}

		if(pdlg->m_dlgSetup->m_SysPara.m_bPara[16])		//사용 하는 경우만 보이고 저장
 		{
			index = 0;
			inc = 1 + pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0] * 12 + 1 + 1;
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				pdlg->m_dlgImgRect[m_iRectIdx]->GetTracker(index + inc + m_iIndex, &left, &top, &right, &bottom);
				rect.left = left;
				rect.top = top;
				rect.right = right;
				rect.bottom = bottom;
				
				pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][6] = rect;

				index++;

				pdlg->m_dlgImgRect[m_iRectIdx]->GetTracker(index + inc + m_iIndex, &left, &top, &right, &bottom);
				rect.left = left;
				rect.top = top;
				rect.right = right;
				rect.bottom = bottom;
				
				pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][7] = rect;

				index++;
			}
		}
		else
		{
			//마지막
		}
		break;
		/*
		//AM Scratch
		m_iIndex = 0;		//In Lead, OCV가 없을 경우 저장 순서 index 밀림
 		if( 
			(!pdlg->m_dlgSetup->m_SysPara.m_bPara[22]) && 
			(!pdlg->m_dlgSetup->m_SysPara.m_bPara[23]) )
		{
			m_iIndex -= pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0];	// 아웃리드2개, 인리드2개
		}

		//OCV 사용하지 않을경우
 		if(!pdlg->m_dlgSetup->m_SysPara.m_bPara[25])
		{
			m_iIndex -= pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0];
			m_iIndex -= 1;		
		}

		if(!pdlg->m_dlgSetup->m_SysPara.m_bPara[24])		//out2
 		{
			m_iIndex -= pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0];
			m_iIndex -= pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0];
		}

		//m_iIndex = -11;//20150128 by IMSI code

		//AM #1		
		if(pdlg->m_dlgSetup->m_SysPara.m_bPara[15])		//사용 하는 경우만 보이고 저장
 		{
			index = 0;
			inc = 1 + pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0] * 10 + 1 + 1;
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				pdlg->m_dlgImgRect[m_iRectIdx]->GetTracker(index + inc + m_iIndex, &left, &top, &right, &bottom);
				rect.left = left;
				rect.top = top;
				rect.right = right;
				rect.bottom = bottom;

				index++;
				
				pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][4] = rect;

				pdlg->m_dlgImgRect[m_iRectIdx]->GetTracker(index + inc + m_iIndex, &left, &top, &right, &bottom);
				rect.left = left;
				rect.top = top;
				rect.right = right;
				rect.bottom = bottom;
				
				pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][5] = rect;

				index++;
			}
		}
		else
		{
			m_iIndex -= pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0];
		}

		//AM #2
		if(pdlg->m_dlgSetup->m_SysPara.m_bPara[16])		//사용 하는 경우만 보이고 저장
 		{
			index = 0;
			inc = 1 + pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0] * 12 + 1 + 1;
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				pdlg->m_dlgImgRect[m_iRectIdx]->GetTracker(index + inc + m_iIndex, &left, &top, &right, &bottom);
				rect.left = left;
				rect.top = top;
				rect.right = right;
				rect.bottom = bottom;
				
				pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][6] = rect;

				index++;

				pdlg->m_dlgImgRect[m_iRectIdx]->GetTracker(index + inc + m_iIndex, &left, &top, &right, &bottom);
				rect.left = left;
				rect.top = top;
				rect.right = right;
				rect.bottom = bottom;
				
				pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][7] = rect;

				index++;
			}
		}
		else
		{
			//마지막
		}
		break;
		*/
	default:
		AfxMessageBox("Error : invalid index(CSetUpAlignDlg::SetRegion)!");
		break;
	}

}

//20150223 by

void CSetUpAlignDlg::Update(BOOL flag)
{
 	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	CRect rect;
	int i;
	char temp[100];

	//if((VISION_CAM_TOP1 == pdlg->m_VisMod->GetVisIndex()) || (VISION_CAM_TOP2 == pdlg->m_VisMod->GetVisIndex()))
	if(1)
	{
		if(flag)
		{
			pdlg->m_dlgImgRect[m_iRectIdx]->DeleteAll();
			pdlg->m_dlgImgRect[m_iRectIdx]->SetActiveTracker(TRUE);
		
			//Alignment//////////////////////////////
			//Align mark
			rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[0][1];
			pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(0, 0, 255), "ALIGN-MARK REGION");
			pdlg->m_dlgImgRect[m_iRectIdx]->Display();

			//Align search
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[i][0];
				sprintf(temp, "ALIGN-SEARCH #%d", i + 1);
				pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(0, 126, 255), temp);
				pdlg->m_dlgImgRect[m_iRectIdx]->Display();
			}

			//Chip///////////////////////////////////
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0];
				sprintf(temp, "CHIP-SEARCH #%d", i + 1);
				pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(0, 255, 255), temp);
				pdlg->m_dlgImgRect[m_iRectIdx]->Display();
			}

			//Film///////////////////////////////////
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[i][0];
				sprintf(temp, "FILM-SEARCH #%d", i + 1);
				pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(255, 0, 255), temp);
				pdlg->m_dlgImgRect[m_iRectIdx]->Display();
			}

			//Lead///////////////////////////////////
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{		
				//상부
				rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][0];
				sprintf(temp, "OUT LEAD-SEARCH #%d", i + 1);
				pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(0, 126, 126), temp);		// 110714 ytlee
				pdlg->m_dlgImgRect[m_iRectIdx]->Display();
			    if( 
				pdlg->m_dlgSetup->m_SysPara.m_bPara[22] ||
				pdlg->m_dlgSetup->m_SysPara.m_bPara[23] )		// 110826 ytlee 사용 하는 경우만 그린다
				{
					//하부
					rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][1];
					sprintf(temp, "IN LEAD-SEARCH #%d", i + 1);
					pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(126, 0, 255), temp);
					pdlg->m_dlgImgRect[m_iRectIdx]->Display();
				}
			}

			//OCV////////////////////////////////////
			//Align mark
			if(pdlg->m_dlgSetup->m_SysPara.m_bPara[25])		//사용 하는 경우만 그린다
			{
				rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1];
				pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(0, 255, 0), "CHARACTER PATTERN REGION");
				pdlg->m_dlgImgRect[m_iRectIdx]->Display();

				//Align search
				for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
				{
					rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[i][0];
					sprintf(temp, "CHARACTER-SEARCH #%d", i + 1);
					pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(255, 255, 0), temp);
					pdlg->m_dlgImgRect[m_iRectIdx]->Display();
				}
			}

			//SR/////////////////////////////////////
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0];

				sprintf(temp, "GENERAL TOOL #%d", i + 1);

				pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(255, 126, 0), temp);
				pdlg->m_dlgImgRect[m_iRectIdx]->Display();
			}
			//BEGIN 2nd Align Mark & Search Region Tracker
			// Tracker는 추가되는 순서대로 index를 가지고 나중에 index를 계산해서 Tracker를 불러오기 때문에
			// 새로운 Tracker를 추가하려면 제일 마지막에 추가 해야한다. (중간에 다른 Tracker를 집어 넣으면 꼬임)
			
			// Add 2nd Align mark Tracker
			rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[0][3];
			pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(255, 0, 0), "2nd ALIGN-MARK REGION");
			pdlg->m_dlgImgRect[m_iRectIdx]->Display();
			
			// Add 2nd Align Search Region Tracker
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[i][2];
				sprintf(temp, "2nd ALIGN-SEARCH #%d", i + 1);
				pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(255, 80, 24), temp);
				pdlg->m_dlgImgRect[m_iRectIdx]->Display();
			}
			
			//BEGIN : 2nd, 3rd lead 가 사용하는 경우에만 보이고 저장되도록 수정
			// Tracker가 추가되는 순서가 꼬이지 않도록 제일 마지막에 추가하도록 수정함 (새로 코드를 옮긴 곳)
			// >> SetRegion()에 있는 주석 참고
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				if(pdlg->m_dlgSetup->m_SysPara.m_bPara[24]) // 사용 하는 경우만 그린다
				{
					//상부2(out 굵은것 왼쪽)
					rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][2];
					sprintf(temp, "OUT LEAD2-SEARCH #%d", i + 1);
					pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(126, 0, 255), temp);
					pdlg->m_dlgImgRect[m_iRectIdx]->Display();
					//상부3(out 굵은것 오른쪽)
					rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][3];
					sprintf(temp, "OUT LEAD2-SEARCH #%d", i + 1);
					pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(126, 0, 255), temp);
					pdlg->m_dlgImgRect[m_iRectIdx]->Display();
				}
			}

	
			// A/M scratch
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				if(pdlg->m_dlgSetup->m_SysPara.m_bPara[15])
				{
					rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][4];
					sprintf(temp, "A/M #%d-1", i + 1);
					pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(255, 0, 0), temp);
					pdlg->m_dlgImgRect[m_iRectIdx]->Display();

					rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][5];
					sprintf(temp, "A/M #%d-1", i + 1);
					pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(255, 0, 0), temp);
					pdlg->m_dlgImgRect[m_iRectIdx]->Display();
				}
			}

			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				if(pdlg->m_dlgSetup->m_SysPara.m_bPara[16])
				{
					rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][6];
					sprintf(temp, "A/M #%d-2", i + 1);
					pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(255, 0, 0), temp);
					pdlg->m_dlgImgRect[m_iRectIdx]->Display();

					rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][7];
					sprintf(temp, "A/M #%d-2", i + 1);
					pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(255, 0, 0), temp);
					pdlg->m_dlgImgRect[m_iRectIdx]->Display();
				}
			}
	

			// END

			m_thrsh = pdlg->m_dlgSetup->m_InsPara.m_iParaAlign[0];
			m_score = pdlg->m_dlgSetup->m_InsPara.m_iParaAlign[1];
			m_poslimit = pdlg->m_dlgSetup->m_InsPara.m_iParaAlign[2]; 

			m_map0 = 0;
			m_map1 = 9;//SR
			m_map2 = 10;
			m_map3 = 11;
			m_map4 = 8;//Chip

			disp_obj(*pdlg->m_VisMod->GetHalMasterImage(0), m_lMWindowID);
			disp_obj(*pdlg->m_VisMod->GetHalSRMasterImage(0), m_lMWindowID_SR);
			disp_obj(*pdlg->m_VisMod->GetHalLeadMasterImage(0), m_lMWindowID_Lead);
			disp_obj(*pdlg->m_VisMod->GetHalSROutMasterImage(0), m_lMWindowID_SROut);
			disp_obj(*pdlg->m_VisMod->GetHalChipMasterImage(0), m_lMWindowID_Chip);

			UpdateData(FALSE);
		}
		else
		{
			UpdateData(TRUE);

			pdlg->m_dlgSetup->m_InsPara.m_iParaAlign[0] = m_thrsh;
			pdlg->m_dlgSetup->m_InsPara.m_iParaAlign[1] = m_score;
			pdlg->m_dlgSetup->m_InsPara.m_iParaAlign[2] = m_poslimit; 

			pdlg->m_dlgSetup->m_InsPara.m_iParaMapAlign[0] = 0;//m_map0;
			pdlg->m_dlgSetup->m_InsPara.m_iParaMapAlign[1] = m_map1;
			pdlg->m_dlgSetup->m_InsPara.m_iParaMapAlign[2] = m_map2;
			pdlg->m_dlgSetup->m_InsPara.m_iParaMapAlign[3] = m_map3;
			pdlg->m_dlgSetup->m_InsPara.m_iParaMapAlign[4] = m_map4;

			if(IsWindowVisible())	
			{
				//Align search
				SetRegion(0);
				//Chip search
				SetRegion(2);
				//Film search
				SetRegion(3);
				//Lead search(up/down)
				SetRegion(4);
				//OCV search
				SetRegion(5);
				//OCV pattern
				SetRegion(6);
				//SR search
				SetRegion(7);
				//A/M
				SetRegion(8);
			}
		}
	}
	/*
	else if(VISION_CAM_BOTTOM == pdlg->m_VisMod->GetVisIndex())
	{
		if(flag)
		{
			pdlg->m_dlgImgRect[m_iRectIdx]->DeleteAll();
			pdlg->m_dlgImgRect[m_iRectIdx]->SetActiveTracker(TRUE);

			//Alignment//////////////////////////////
			//Align mark
			rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[0][1];
			pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(0, 0, 255), "ALIGN-MARK REGION");
			pdlg->m_dlgImgRect[m_iRectIdx]->Display();

			//Align search
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[i][0];
				sprintf(temp, "ALIGN-SEARCH #%d", i + 1);
				pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(0, 126, 255), temp);
				pdlg->m_dlgImgRect[m_iRectIdx]->Display();
			}

			//Chip///////////////////////////////////
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0];
				sprintf(temp, "CHIP-SEARCH #%d", i + 1);
				pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(0, 255, 255), temp);
				pdlg->m_dlgImgRect[m_iRectIdx]->Display();
			}

			//Film///////////////////////////////////
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[i][0];
				sprintf(temp, "FILM-SEARCH #%d", i + 1);
				pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(255, 0, 255), temp);
				pdlg->m_dlgImgRect[m_iRectIdx]->Display();
			}

			//SR/////////////////////////////////////
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0];
				sprintf(temp, "SR-SEARCH #%d", i + 1);

				pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(255, 126, 0), temp);
				pdlg->m_dlgImgRect[m_iRectIdx]->Display();
			}

			m_thrsh = pdlg->m_dlgSetup->m_InsPara.m_iParaAlign[0];
			m_score = pdlg->m_dlgSetup->m_InsPara.m_iParaAlign[1];
			m_poslimit = pdlg->m_dlgSetup->m_InsPara.m_iParaAlign[2]; 

			m_map0 = 0;
			m_map1 = 0;
			m_map2 = 4;
			m_map3 = 6;

			disp_obj(*pdlg->m_VisMod->GetHalMasterImage(0), m_lMWindowID);
			disp_obj(*pdlg->m_VisMod->GetHalSRMasterImage(0), m_lMWindowID_SR);
			disp_obj(*pdlg->m_VisMod->GetHalLeadMasterImage(0), m_lMWindowID_Lead);
			disp_obj(*pdlg->m_VisMod->GetHalSROutMasterImage(0), m_lMWindowID_SROut);
			disp_obj(*pdlg->m_VisMod->GetHalChipMasterImage(0), m_lMWindowID_Chip);

			UpdateData(FALSE);
		}
		else
		{
			UpdateData(TRUE);

			pdlg->m_dlgSetup->m_InsPara.m_iParaAlign[0] = m_thrsh;
			pdlg->m_dlgSetup->m_InsPara.m_iParaAlign[1] = m_score;
			pdlg->m_dlgSetup->m_InsPara.m_iParaAlign[2] = m_poslimit; 

			pdlg->m_dlgSetup->m_InsPara.m_iParaMapAlign[0] = 0;
			
			pdlg->m_dlgSetup->m_InsPara.m_iParaMapAlign[1] = 0;
			pdlg->m_dlgSetup->m_InsPara.m_iParaMapAlign[2] = 4;
			pdlg->m_dlgSetup->m_InsPara.m_iParaMapAlign[3] = 6;

			if(IsWindowVisible())
			{
				//Align search
				SetRegion(0);
				//Chip search
				SetRegion(2);
				//Film search
				SetRegion(3);
				//SR search
				SetRegion(7);
			}
		}
	}
	*/
}

/*
void CSetUpAlignDlg::Update(BOOL flag)
{
 	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	CRect rect;
	int i;
	char temp[100];

	//디버그 필요
	if(1)
	{
		if(flag)
		{
			pdlg->m_dlgImgRect[m_iRectIdx]->DeleteAll();
			pdlg->m_dlgImgRect[m_iRectIdx]->SetActiveTracker(TRUE);
		
			//Alignment//////////////////////////////
			//Align mark
			rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[0][1];
			pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(0, 0, 255), "ALIGN-MARK REGION");
			pdlg->m_dlgImgRect[m_iRectIdx]->Display();

			//Align search
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[i][0];
				sprintf(temp, "ALIGN-SEARCH #%d", i + 1);
				pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(0, 126, 255), temp);
				pdlg->m_dlgImgRect[m_iRectIdx]->Display();
			}

			//Chip///////////////////////////////////
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0];
				sprintf(temp, "CHIP-SEARCH #%d", i + 1);
				pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(0, 255, 255), temp);
				pdlg->m_dlgImgRect[m_iRectIdx]->Display();
			}

			//Film///////////////////////////////////
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[i][0];
				sprintf(temp, "FILM-SEARCH #%d", i + 1);
				pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(255, 0, 255), temp);
				pdlg->m_dlgImgRect[m_iRectIdx]->Display();
			}

			//Lead///////////////////////////////////
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				//상부
				rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][0];
				sprintf(temp, "OUT LEAD-SEARCH #%d", i + 1);
				pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(0, 126, 126), temp);		// 110714 ytlee
				pdlg->m_dlgImgRect[m_iRectIdx]->Display();

			    if( 
				pdlg->m_dlgSetup->m_SysPara.m_bPara[22] ||
				pdlg->m_dlgSetup->m_SysPara.m_bPara[23] )		// 110826 ytlee 사용 하는 경우만 그린다
				{
					//하부
					rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][1];
					sprintf(temp, "IN LEAD-SEARCH #%d", i + 1);
					pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(126, 0, 255), temp);
					pdlg->m_dlgImgRect[m_iRectIdx]->Display();
				}
			}

			//OCV////////////////////////////////////
			//Align mark
			if(pdlg->m_dlgSetup->m_SysPara.m_bPara[25])		//사용 하는 경우만 그린다
			{
				rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1];
				pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(0, 255, 0), "CHARACTER PATTERN REGION");
				pdlg->m_dlgImgRect[m_iRectIdx]->Display();

				//Align search
				for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
				{
					rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[i][0];
					sprintf(temp, "CHARACTER-SEARCH #%d", i + 1);
					pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(255, 255, 0), temp);
					pdlg->m_dlgImgRect[m_iRectIdx]->Display();
				}
			}

			//SR/////////////////////////////////////
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0];

				sprintf(temp, "GENERAL TOOL #%d", i + 1);

				pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(255, 126, 0), temp);
				pdlg->m_dlgImgRect[m_iRectIdx]->Display();
			}
			//BEGIN 2nd Align Mark & Search Region Tracker
			// Tracker는 추가되는 순서대로 index를 가지고 나중에 index를 계산해서 Tracker를 불러오기 때문에
			// 새로운 Tracker를 추가하려면 제일 마지막에 추가 해야한다. (중간에 다른 Tracker를 집어 넣으면 꼬임)
			
			// Add 2nd Align mark Tracker
			rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[0][3];
			pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(255, 0, 0), "2nd ALIGN-MARK REGION");
			pdlg->m_dlgImgRect[m_iRectIdx]->Display();
			
			// Add 2nd Align Search Region Tracker
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[i][2];
				sprintf(temp, "2nd ALIGN-SEARCH #%d", i + 1);
				pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(255, 80, 24), temp);
				pdlg->m_dlgImgRect[m_iRectIdx]->Display();
			}
			
			//BEGIN : 2nd, 3rd lead 가 사용하는 경우에만 보이고 저장되도록 수정
			// Tracker가 추가되는 순서가 꼬이지 않도록 제일 마지막에 추가하도록 수정함 (새로 코드를 옮긴 곳)
			// >> SetRegion()에 있는 주석 참고
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				if(pdlg->m_dlgSetup->m_SysPara.m_bPara[24]) // 사용 하는 경우만 그린다
				{
					//상부2(out 굵은것 왼쪽)
					rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][2];
					sprintf(temp, "OUT LEAD2-SEARCH #%d", i + 1);
					pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(126, 0, 255), temp);
					pdlg->m_dlgImgRect[m_iRectIdx]->Display();
					//상부3(out 굵은것 오른쪽)
					rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][3];
					sprintf(temp, "OUT LEAD2-SEARCH #%d", i + 1);
					pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(126, 0, 255), temp);
					pdlg->m_dlgImgRect[m_iRectIdx]->Display();
				}
			}

	
			// A/M scratch
			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				if(pdlg->m_dlgSetup->m_SysPara.m_bPara[15])
				{
					rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][4];
					sprintf(temp, "A/M #%d-1", i + 1);
					pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(255, 0, 0), temp);
					pdlg->m_dlgImgRect[m_iRectIdx]->Display();

					rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][5];
					sprintf(temp, "A/M #%d-1", i + 1);
					pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(255, 0, 0), temp);
					pdlg->m_dlgImgRect[m_iRectIdx]->Display();
				}
			}

			for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
			{
				if(pdlg->m_dlgSetup->m_SysPara.m_bPara[16])
				{
					rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][6];
					sprintf(temp, "A/M #%d-2", i + 1);
					pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(255, 0, 0), temp);
					pdlg->m_dlgImgRect[m_iRectIdx]->Display();

					rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][7];
					sprintf(temp, "A/M #%d-2", i + 1);
					pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom, RGB(255, 0, 0), temp);
					pdlg->m_dlgImgRect[m_iRectIdx]->Display();
				}
			}
	

			// END

			m_thrsh = pdlg->m_dlgSetup->m_InsPara.m_iParaAlign[0];
			m_score = pdlg->m_dlgSetup->m_InsPara.m_iParaAlign[1];
			m_poslimit = pdlg->m_dlgSetup->m_InsPara.m_iParaAlign[2]; 

			m_map0 = 0;
			m_map1 = 9;//SR
			m_map2 = 10;
			m_map3 = 11;
			m_map4 = 8;//Chip

			disp_obj(*pdlg->m_VisMod->GetHalMasterImage(0), m_lMWindowID);
			disp_obj(*pdlg->m_VisMod->GetHalSRMasterImage(0), m_lMWindowID_SR);
			disp_obj(*pdlg->m_VisMod->GetHalLeadMasterImage(0), m_lMWindowID_Lead);
			disp_obj(*pdlg->m_VisMod->GetHalSROutMasterImage(0), m_lMWindowID_SROut);
			disp_obj(*pdlg->m_VisMod->GetHalChipMasterImage(0), m_lMWindowID_Chip);

			UpdateData(FALSE);
		}
		else
		{
			UpdateData(TRUE);

			pdlg->m_dlgSetup->m_InsPara.m_iParaAlign[0] = m_thrsh;
			pdlg->m_dlgSetup->m_InsPara.m_iParaAlign[1] = m_score;
			pdlg->m_dlgSetup->m_InsPara.m_iParaAlign[2] = m_poslimit; 

			pdlg->m_dlgSetup->m_InsPara.m_iParaMapAlign[0] = 0;//m_map0;
			pdlg->m_dlgSetup->m_InsPara.m_iParaMapAlign[1] = m_map1;
			pdlg->m_dlgSetup->m_InsPara.m_iParaMapAlign[2] = m_map2;
			pdlg->m_dlgSetup->m_InsPara.m_iParaMapAlign[3] = m_map3;
			pdlg->m_dlgSetup->m_InsPara.m_iParaMapAlign[4] = m_map4;

			if(IsWindowVisible())	
			{
				//Align search
				SetRegion(0);
				//Chip search
				SetRegion(2);
				//Film search
				SetRegion(3);
				//Lead search(up/down)
				SetRegion(4);
				//OCV search
				SetRegion(5);
				//OCV pattern
				SetRegion(6);
				//SR search
				SetRegion(7);
				//A/M
				SetRegion(8);
			}
		}
	}	
}
*/

BEGIN_EVENTSINK_MAP(CSetUpAlignDlg, CDialog)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_SETDEFUALT, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignSetdefualt, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_SETMASTER, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignSetmaster, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_LOADMASTER2, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignLoadmaster2, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_TEACH, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignTeach, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_POSITIONSET, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignPositionset, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_TEST, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignTest, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_REGION_SR_START, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionSrStart, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_REGION_SRO_START, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionSroStart, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_SR, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignInsSr, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_REGION_SROUT, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionSrout, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_REGION_SR_MINUS, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionSrMinus, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_REGION_SROUT_MINUS, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionSroutMinus, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_REGION_PF_START, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionPfStart, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_REGION_PF, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionPf, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_REGION_PF_MINUS, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionPfMinus, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_REGION_LEAD, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionLead, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_REGION_LEAD_IN, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionLeadIn, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_FINDCHIP, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignFindchip, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_POT_TEACH, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignPotTeach, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_REGION_AM, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionAm, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_REGION_AM_START, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionAmStart, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_REGION_M_MINUS, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionMMinus, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_REGION_M_MINUS_V, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionMMinusV, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_REGION_SR_MINUS_V, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionSrMinusV, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_REGION_SROUT_MINUS_V, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionSroutMinusV, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_REGION_PF_MINUS_V, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionPfMinusV, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_REGION_LEAD_V, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionLeadV, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_REGION_LEAD_IN_V, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionLeadInV, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_AUTOREGION, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignAutoregion, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_MAKE_SR, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignMakeSr, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_SR_DARK_P, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignInsSrDarkP, VTS_NONE)
//ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_SR_DARK_M, 6, CSetUpAlignDlg::AnimDLLDoneBtnenhctrlAlignInsSrDarkM, VTS_I4 VTS_BOOL)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_SR_DARK_M, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignInsSrDarkM, VTS_NONE)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_SR_WHITE_M, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignInsSrWhiteM, VTS_NONE)
//ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_SR_WHITE_P, 6, CSetUpAlignDlg::AnimDLLDoneBtnenhctrlAlignInsSrWhiteP, VTS_I4 VTS_BOOL)
ON_EVENT(CSetUpAlignDlg, IDC_BTNENHCTRL_ALIGN_INS_SR_WHITE_P, DISPID_CLICK, CSetUpAlignDlg::ClickBtnenhctrlAlignInsSrWhiteP, VTS_NONE)
END_EVENTSINK_MAP()

void CSetUpAlignDlg::ClickBtnenhctrlAlignSetdefualt()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.		
	Update(false);
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	int FILM_TOP, FILM_BOTTOM, FILM_LEFT, FILM_RIGHT;
	double dFilmLength_mm, dFilmLength;
	//ins_para.m_iParaBase[2]: PF 수량, ins_para.m_dParaBase[1]: PF 간격(4.75mm)
	dFilmLength_mm =pdlg->m_dlgSetup->m_InsPara.m_iParaBase[2] * pdlg->m_dlgSetup->m_InsPara.m_dParaBase[1]; 
	if(0 < pdlg->m_dlgSetup->m_SysPara.m_dPara[0])	// m_SysPara.m_dPara[0]: Pixel Size
		dFilmLength = ((dFilmLength_mm * 1000) / pdlg->m_dlgSetup->m_SysPara.m_dPara[0]); 
	else
		dFilmLength = 0;
	
	FILM_TOP = 100;	//m_InsPara.m_iParaBase[0]: COF 개수
	FILM_BOTTOM = HEIGHT - 100;
	FILM_LEFT = WIDTH - dFilmLength + 100;		
	FILM_RIGHT = FILM_LEFT + dFilmLength;


	for(int j = 0; j < SUB_RGN_MAX_NUM; j++)
	{
		for(int k = 0; k < RECT_PARA_MAX_NUM_ALIGN; k++)
		{
			//Align//////////////////////////////
			int iAlignRectLeft, iAlignRectTop, iAlignRectRight, iAlignRectBottom;		

			if(k == 0)				// Align Search 위
			{
				iAlignRectLeft = WIDTH - (dFilmLength * j) - 1000;	
				iAlignRectRight = WIDTH - (dFilmLength * j) - 100;
				iAlignRectTop = FILM_TOP + 10;
				iAlignRectBottom = iAlignRectTop + 160;
			}
			else if(k == 1)			// Align Mark Search 위
			{
				iAlignRectLeft = WIDTH - (dFilmLength * j) - 500;	
				iAlignRectRight = WIDTH - (dFilmLength * j) - 300;
				iAlignRectTop = FILM_TOP + 50;
				iAlignRectBottom = iAlignRectTop + 80;
			}
			else if(k == 2)			// Align Search 아래
			{
				iAlignRectLeft = WIDTH - (dFilmLength * j) - 1000;	
				iAlignRectRight = WIDTH - (dFilmLength * j) - 100;
				iAlignRectTop = FILM_BOTTOM - 170;
				iAlignRectBottom = iAlignRectTop + 160;
			}
			else					// Align Mark Search 아래
			{
				iAlignRectLeft = WIDTH - (dFilmLength * j) - 500;	
				iAlignRectRight = WIDTH - (dFilmLength * j) - 300;
				iAlignRectTop = FILM_BOTTOM - 130;
				iAlignRectBottom = iAlignRectTop + 80;
			}
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[j][k].left = iAlignRectLeft;
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[j][k].top = iAlignRectTop;
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[j][k].right = iAlignRectRight;
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[j][k].bottom = iAlignRectBottom;
			

			//Chip///////////////////////////////
			int iChipRectTop, iChipRectBottom, iChipRectLeft, iChipRectRight;
			iChipRectLeft = (FILM_LEFT + FILM_RIGHT) * 0.5 - (dFilmLength * j) - 200;
			iChipRectRight = iChipRectLeft + 200;
			iChipRectTop = (FILM_TOP + FILM_BOTTOM) * 0.5 - 800;
			iChipRectBottom = (FILM_TOP + FILM_BOTTOM) * 0.5 + 800;
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[j][k].left = iChipRectLeft;
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[j][k].top = iChipRectTop;
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[j][k].right = iChipRectRight;
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[j][k].bottom = iChipRectBottom;


			//Lead///////////////////////////////
			int iLeadRectTop, iLeadRectBottom, iLeadRectLeft, iLeadRectRight;
			
			if(k == 1)			// In Lead Search
			{
				iLeadRectLeft = FILM_LEFT - (dFilmLength * j);
				iLeadRectRight = iLeadRectLeft + 100;
				iLeadRectTop = (FILM_TOP + FILM_BOTTOM) * 0.5 - 1200;
				iLeadRectBottom = (FILM_TOP + FILM_BOTTOM) * 0.5 + 1200;
			}
			else				// Out Lead Search
			{
				iLeadRectLeft = FILM_RIGHT - (dFilmLength * j) - 300;
				iLeadRectRight = iLeadRectLeft + 100;
				iLeadRectTop = (FILM_TOP + FILM_BOTTOM) * 0.5 - 1200;
				iLeadRectBottom = (FILM_TOP + FILM_BOTTOM) * 0.5 + 1200;
				if(k == 2)		// Out Lead Search 2 좌측
				{
					iLeadRectTop = FILM_TOP + 110;	// 110804 ytlee
					iLeadRectBottom = FILM_TOP + 185;
				}
				else if(k == 3)	// Out Lead Search 2 우측
				{
					iLeadRectTop = FILM_BOTTOM - 185;
					iLeadRectBottom = FILM_BOTTOM - 110;
				}
				//A/M scratch
				else if(k == 4)			
				{
					iLeadRectLeft -= 200;
					iLeadRectRight -= 200;
					iLeadRectTop = FILM_TOP + 160;
					iLeadRectBottom = FILM_TOP + 235;
				}
				else if(k == 5)			
				{
					iLeadRectLeft -= 400;
					iLeadRectRight -= 400;
					iLeadRectTop = FILM_BOTTOM - 235;
					iLeadRectBottom = FILM_BOTTOM - 160;
				}
				else if(k == 6)
				{
					iLeadRectLeft -= 600;
					iLeadRectRight -= 600;
					iLeadRectTop = FILM_TOP + 160;
					iLeadRectBottom = FILM_TOP + 235;
				}
				else if(k == 7)
				{
					iLeadRectLeft -= 800;
					iLeadRectRight -= 800;
					iLeadRectTop = FILM_BOTTOM - 235;
					iLeadRectBottom = FILM_BOTTOM - 160;
				}
			}
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[j][k].left = iLeadRectLeft;
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[j][k].top = iLeadRectTop;
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[j][k].right = iLeadRectRight;
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[j][k].bottom = iLeadRectBottom;

			//Film///////////////////////////////
			int iFilmRectLeft, iFilmRectRight;
			iFilmRectLeft = FILM_LEFT - (dFilmLength * j) - 150;		
			iFilmRectRight = FILM_RIGHT - (dFilmLength * j) - 150;		
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[j][k].left = iFilmRectLeft;
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[j][k].top = FILM_TOP;
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[j][k].right = iFilmRectRight;
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[j][k].bottom = FILM_BOTTOM;

	
			//OCV////////////////////////////////
			int iOCVRectTop, iOCVRectBottom, iOCVRectLeft, iOCVRectRight;
			iOCVRectLeft = (FILM_LEFT + FILM_RIGHT) * 0.5 - (dFilmLength * j) - 800;
			iOCVRectRight = iOCVRectLeft + 400;
			iOCVRectTop = (FILM_TOP + FILM_BOTTOM) * 0.5 - 500;
			iOCVRectBottom = (FILM_TOP + FILM_BOTTOM) * 0.5 + 500;
			if(k == 1)			// Character Pattern Search 
			{
				iOCVRectLeft = (FILM_LEFT + FILM_RIGHT) * 0.5 - (dFilmLength * j) - 700;
				iOCVRectRight = iOCVRectLeft + 200;
				iOCVRectTop = (FILM_TOP + FILM_BOTTOM) * 0.5 - 400;
				iOCVRectBottom = (FILM_TOP + FILM_BOTTOM) * 0.5 + 400;
			}
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[j][k].left = iOCVRectLeft;
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[j][k].top = iOCVRectTop;
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[j][k].right = iOCVRectRight;
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[j][k].bottom = iOCVRectBottom;

			//General Tool///////////////////////////////////					// 110720 ytlee
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[j][k].left = 100;
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[j][k].top = 100;
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[j][k].right = 200;
			pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[j][k].bottom = 200;


		}
	}
	disp_obj(*pdlg->m_VisMod->GetHalImage(0, 0, 3), pdlg->m_lWindowID[0]);		// 3번 이미지 갱신
	Update(true);
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignSetmaster()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	if(pdlg->m_bCheckGrab == TRUE)
	{
		copy_image(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0), pdlg->m_VisMod->GetHalMasterImage(0));
		disp_obj(*pdlg->m_VisMod->GetHalMasterImage(0), m_lMWindowID);
		
		copy_image(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map1), pdlg->m_VisMod->GetHalSRMasterImage(0));
		disp_obj(*pdlg->m_VisMod->GetHalSRMasterImage(0), m_lMWindowID_SR);
		
		copy_image(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map2), pdlg->m_VisMod->GetHalLeadMasterImage(0));
		disp_obj(*pdlg->m_VisMod->GetHalLeadMasterImage(0), m_lMWindowID_Lead);
		
		copy_image(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map3), pdlg->m_VisMod->GetHalSROutMasterImage(0));
		disp_obj(*pdlg->m_VisMod->GetHalSROutMasterImage(0), m_lMWindowID_SROut);

		copy_image(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map4), pdlg->m_VisMod->GetHalChipMasterImage(0));
		disp_obj(*pdlg->m_VisMod->GetHalChipMasterImage(0), m_lMWindowID_Chip);
		
		disp_obj(*pdlg->m_VisMod->GetHalMasterImage(0), pdlg->m_dlgImgRect[0]->GetView());
		
		for(int i = 0; (i < pdlg->m_VisMod->GetMapNum(0)) && (i < IMAGE_MAP_MAX_NUM); i++)
		{
			copy_image(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], i), pdlg->m_VisMod->GetHalTaughtImage(0, i));
		}

		ClickBtnenhctrlAlignLoadmaster2();
	}
	else
	{
		AfxMessageBox("정상적인 이미지 획득 방법이 아님니다. Grab이후에 이미지를 등록해 주세요.");
	}
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignLoadmaster2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	pdlg->m_bCheckGrab = FALSE;

	//로딩은 BufID 0으로 무조건 한다. 
	for(int i = 0; (i < pdlg->m_VisMod->GetMapNum(0)) && (i < IMAGE_MAP_MAX_NUM); i++)
	{
		copy_image(*pdlg->m_VisMod->GetHalTaughtImage(0,i), pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], i)); // GetHalImage->GetHalColorImage
		disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], i), pdlg->m_lWindowID[i]);
	}

	disp_obj(*pdlg->m_VisMod->GetHalMasterImage(0), pdlg->m_dlgImgRect[0]->GetView());

	pdlg->m_bCheckLoadMaster = TRUE;
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignTeach()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);

	//Align search
	SetRegion(0);
	//Align mark
	SetRegion(1);



	Update(FALSE);

	Hobject image, modimage, shape;
	Hlong wd, ht;
	
	//Teach
	disp_obj(*pdlg->m_VisMod->GetHalMasterImage(0), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
		
	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*pdlg->m_VisMod->GetHalMasterImage(0), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_05!");
		pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
		return;
	}	

	if(pdlg->m_dlgSetup->m_InsPara.m_bCreatMdIDAlign)
	{
		//clear_shape_model(pdlg->m_dlgSetup->m_InsPara.m_PatIDAlign);
	}

	Hobject EmphaImage;
	emphasize(*pdlg->m_VisMod->GetHalMasterImage(0), &EmphaImage, 15, 15, 3);

	//BEGIN 두개의 Align Mark Search가 모두 성공해야 flag 설정하도록 수정
	if(
		Hal_ModelTeachingRect1(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 
		&EmphaImage, 
		&pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[0][1],
		wd,
		ht,
		pdlg->m_dlgSetup->m_InsPara.m_PatParaAlign, 
		m_thrsh, //threshold
		m_thrsh * 0.5, //min threshold
		&pdlg->m_dlgSetup->m_InsPara.m_dParaAlign[0],//pattern cx 
		&pdlg->m_dlgSetup->m_InsPara.m_dParaAlign[1],//pattern cy
		&modimage, &pdlg->m_dlgSetup->m_InsPara.m_PatIDAlign)
	
		&&
		Hal_ModelTeachingRect1(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 
		&EmphaImage, 
		&pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[0][3],
		wd,
		ht,
		pdlg->m_dlgSetup->m_InsPara.m_PatParaAlign, 
		m_thrsh, //threshold
		m_thrsh * 0.5, //min threshold
		&pdlg->m_dlgSetup->m_InsPara.m_dParaAlign[2],//pattern cx #2
		&pdlg->m_dlgSetup->m_InsPara.m_dParaAlign[3],//pattern cy #2
		&modimage, &pdlg->m_dlgSetup->m_InsPara.m_PatIDAlign2)
		
		)
	{
		pdlg->m_dlgSetup->m_InsPara.m_bCreatMdIDAlign = TRUE;
	}
	else
	{
		pdlg->m_dlgSetup->m_InsPara.m_bCreatMdIDAlign = FALSE;
	}
	//END
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignPositionset()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);
	Update(FALSE);

	Hobject modimage, shape, Region, ImageReduced;
	Hlong wd, ht;
	double rx, ry, ra, rs, rscore;   
	Hlong rn;
	int i, j;
	double pat_score;
	Hobject EmphaImage;
	CRect rect, rect2;

	pat_score = (((double)m_score) * 0.01);
	
	disp_obj(*pdlg->m_VisMod->GetHalMasterImage(0), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*pdlg->m_VisMod->GetHalMasterImage(0), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_06!");
		pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
		return;
	}	
	
	rn = 0;
	int idx = 0;

	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		for(j = 0; j < PATTERN_INS_NUM; j++)
		{
			/*
			if(VISION_CAM_BOTTOM2 == pdlg->m_VisMod->GetVisIndex())
			{
				if(1 == j)
				{
					continue;
				}
			}
			*/

			// BEGIN
			rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[i][(j==0?0:2)];
			gen_rectangle1(&Region, rect.top, rect.left, rect.bottom, rect.right);

			reduce_domain(*pdlg->m_VisMod->GetHalMasterImage(0),Region,&ImageReduced);
			emphasize(ImageReduced, &EmphaImage, 15, 15, 3);
			
			if(pdlg->m_dlgSetup->m_InsPara.m_bCreatMdIDAlign)
			{
				Hal_ShapePatternFind(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 
					&EmphaImage, 
					(j==0?pdlg->m_dlgSetup->m_InsPara.m_PatIDAlign:pdlg->m_dlgSetup->m_InsPara.m_PatIDAlign2), 
					pdlg->m_dlgSetup->m_InsPara.m_PatParaAlign, 
					//pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[i][1],
					pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[i][(j==0?1:3)],
					wd, ht,
					//pdlg->m_dlgSetup->m_InsPara.m_dParaAlign[0],//pattern cx 
					//pdlg->m_dlgSetup->m_InsPara.m_dParaAlign[1],//pattern cy
					pdlg->m_dlgSetup->m_InsPara.m_dParaAlign[(j==0?0:2)],// pattern cx #1 or #2
					pdlg->m_dlgSetup->m_InsPara.m_dParaAlign[(j==0?1:3)],// pattern cy #1 or #2
					pat_score,//Score
					&modimage, 
					&shape, 
					&rx, 
					&ry, 
					&ra, 
					&rs, 
					&rscore, 
					&rn);

				if(rn)
				{
					set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), (j==0?"green":"orange red"));
					set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (idx * 200), 10);
					write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(HTuple("SCORE : ") + HTuple(rscore * 100)));
					set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 40 + (idx * 200), 10);
					write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(HTuple("X : ") + HTuple(rx)));
					set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 70 + (idx * 200), 10);
					write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(HTuple("Y : ") + HTuple(ry)));
					set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 100 + (idx * 200), 10);
					write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(HTuple("ANGLE : ") + HTuple(ra)));

					//Position set
					pdlg->m_dlgSetup->m_InsPara.m_dPatternPosXAlign[i][j] = rx;
					pdlg->m_dlgSetup->m_InsPara.m_dPatternPosYAlign[i][j] = ry;
				}
				else
				{
					set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "red");
					set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (idx * 200), 10);
					write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(HTuple("FAIL : can't find model!")));
				}
			}
			else
			{
				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "red");
				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (idx * 200), 10);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(HTuple("FAIL : no pattern!")));
			}

			idx++;
		}
		// END
	}

}

void CSetUpAlignDlg::ClickBtnenhctrlAlignTest()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);
	Update(FALSE);

	Hobject modimage, shape, Region, ImageReduced, InvertHallImage;
	Hlong wd, ht;
	double rx, ry, ra, rs, rscore;   
	Hlong rn;
	int i, j;	
	double pat_score;

	CRect rect, rect2;

	Hobject EmphaImage;

	pat_score = (((double)m_score) * 0.01);
	
	disp_obj(*pdlg->m_VisMod->GetHalMasterImage(0), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*pdlg->m_VisMod->GetHalMasterImage(0), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_04!");
		pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
		return;
	}	
	
	rn = 0;
	int idx = 0;

	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		for(j = 0; j < PATTERN_INS_NUM; j++)
		{
			//BEGIN
			rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[i][(j==0?0:2)];
			gen_rectangle1(&Region, rect.top, rect.left, rect.bottom, rect.right);
			
			reduce_domain(*pdlg->m_VisMod->GetHalMasterImage(0),Region,&ImageReduced);
			//			reduce_domain(*pdlg->m_VisMod->GetHalMasterImage(0),Region,&ImageReduced);
			emphasize(ImageReduced, &EmphaImage, 5, 5, 3);
			
			if(pdlg->m_dlgSetup->m_InsPara.m_bCreatMdIDAlign)
			{
				Hal_ShapePatternFind(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 
					&EmphaImage, 
					(j==0?pdlg->m_dlgSetup->m_InsPara.m_PatIDAlign:pdlg->m_dlgSetup->m_InsPara.m_PatIDAlign2), 
					pdlg->m_dlgSetup->m_InsPara.m_PatParaAlign, 
					//					pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[i][1], 
					pdlg->m_dlgSetup->m_InsPara.m_InsRegionAlign[i][(j==0?1:3)], 
					wd, ht,
					//					pdlg->m_dlgSetup->m_InsPara.m_dParaAlign[0],//pattern cx 
					//					pdlg->m_dlgSetup->m_InsPara.m_dParaAlign[1],//pattern cy
					pdlg->m_dlgSetup->m_InsPara.m_dParaAlign[(j==0?0:2)],//pattern cx 
					pdlg->m_dlgSetup->m_InsPara.m_dParaAlign[(j==0?1:3)],//pattern cy
					pat_score,//Score
					&modimage, 
					&shape, 
					&rx, 
					&ry, 
					&ra, 
					&rs, 
					&rscore, 
					&rn);
			// END
				if(rn)
				{
					set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "green");
					set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (idx * 200), 10);
					write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(HTuple("SCORE : ") + HTuple(rscore * 100)));
					set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 40 + (idx * 200), 10);
					write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(HTuple("X : ") + HTuple(rx)));
					set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 70 + (idx * 200), 10);
					write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(HTuple("Y : ") + HTuple(ry)));
					set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 100 + (idx * 200), 10);
					write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(HTuple("ANGLE : ") + HTuple(ra)));
				}
				else
				{
					set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "red");
					set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (idx * 200), 10);
					write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(HTuple("FAIL : can't find model!")));
				}
			}
			else
			{
				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "red");
				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10 + (idx * 200), 10);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(HTuple("FAIL : no pattern!")));
			}

			idx++;
		}
	}
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionSrStart()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_lSrTeachThr = 100;
	m_dSrTeachThrDark = 0.55;
	m_dSrTeachThrWhite = 1.2;
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionSroStart()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_lSrTeachThr = 150;
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignInsSr()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	MakeSRRegion();

	m_lSrTeachThr = m_lSrTeachThr + 5;
	if(m_lSrTeachThr > 255) m_lSrTeachThr = 255;
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionSrout()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	Hobject ImageReduced, ImageReducedWhite, Region, RegionClosing, LeadTopRgn, LeadBotRgn;
	Hobject FillUp;
	HTuple Num, Wd, Ht, Max;
	CRect rect;
	Hlong num;
	int i;
	Hobject DontCare, OCVRgn, MeanImage, RegionWhite, ErosImage, DilRegion;

	UpdateData(TRUE);
	Update(FALSE);

	disp_obj(*pdlg->m_VisMod->GetHalSROutMasterImage(0), m_lMWindowID_SROut);

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*pdlg->m_VisMod->GetHalSROutMasterImage(0), NULL, NULL, &Wd, &Ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_11!");
		pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
		return;
	}	

	disp_obj(*pdlg->m_VisMod->GetHalSROutMasterImage(0), pdlg->m_dlgImgRect[0]->GetView());
	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		//Film
		rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[i][0];
		gen_rectangle1(&Region, rect.top, rect.left, rect.bottom, rect.right);

		//Don't care region
		gen_rectangle1(&DontCare,  pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0].top, 
								   pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0].left, 
								   pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0].bottom, 
								   pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0].right);

		difference(Region, DontCare, &Region);
		intersection(pdlg->m_dlgSetup->m_HalInsRgn[i][0], Region, &Region);

		reduce_domain(*pdlg->m_VisMod->GetHalSROutMasterImage(0), Region, &ImageReduced);
		//reduce_domain(*pdlg->m_VisMod->GetHalSROutMasterImage(0), RegionWhite, &ImageReducedWhite);

		set_check("~give_error");
		herr = threshold(ImageReduced, &Region, 0, 90);
		set_check("give_error");
		if(H_MSG_TRUE == herr)
		{
		}
		else
		{
			status.Format("Halcon Error Occured at threshold_05!");
			pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
			return;
		}

		connection(Region, &Region);

		int max = m_lSrTeachThr;
		
		if( (0 >= max) || (255 < max) ) // threshold error 관련
		{
			max = 150;
		}


		set_check("~give_error");
		herr = threshold(ImageReduced, &RegionWhite, max, 255);
		set_check("give_error");
		if(H_MSG_TRUE == herr)
		{
		}
		else
		{
			status.Format("Halcon Error Occured at threshold_06!");
			pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
			return;
		}
	
		connection(RegionWhite, &RegionWhite);
		select_shape(RegionWhite, &RegionWhite, HTuple("area"), HTuple("and"), HTuple(20),  HTuple("max"));
		select_shape(RegionWhite, &RegionWhite, HTuple("rb"), HTuple("and"), HTuple(5),  HTuple("max"));
		
		union1(Region, &Region);
		union1(RegionWhite, &RegionWhite);
		union2(Region, RegionWhite, &Region);
		fill_up(Region, &Region);
		closing_rectangle1(Region, &Region, 50.5, 50.5);
		closing_circle(Region, &Region, 10.5);
		fill_up(Region, &Region);


		connection(Region, &Region);
		select_shape(Region, &Region, HTuple("area"), HTuple("and"), HTuple(1000),  HTuple("max"));

		dilation_circle(Region, &DilRegion, 7.5);
		difference(pdlg->m_dlgSetup->m_HalInsRgn[i][0], DilRegion, &Region);

		erosion_circle(Region, &Region, 9.5);

		connection(Region, &Region);
		select_shape(Region, &Region, HTuple("area"), HTuple("and"), HTuple(1000),  HTuple("max"));

		count_obj(Region, &Num);
		num = (Hlong)Num[0];
		if(0 < num)
		{
			union1(Region, &Region);
			//fill_up(Region, &Region);

			//disp_obj(*pdlg->m_VisMod->GetHalSROutMasterImage(0), pdlg->m_dlgImgRect[0]->GetView());
			set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("blue"));
			set_draw(pdlg->m_dlgImgRect[0]->GetView(), HTuple("margin"));
			disp_obj(Region, pdlg->m_dlgImgRect[0]->GetView());

			copy_obj(Region, &pdlg->m_dlgSetup->m_HalInsRgn[i][5], 1, -1);//SR-out region
		}
		else
		{
			gen_empty_obj(&pdlg->m_dlgSetup->m_HalInsRgn[i][5]);
		}
	}
	
	//LSS
	m_lSrTeachThr = m_lSrTeachThr + 5;
	if(m_lSrTeachThr > 255) m_lSrTeachThr = 255;

}

void CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionSrMinus()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	MakeSRRegion();

	m_lSrTeachThr = m_lSrTeachThr - 5;
	if(m_lSrTeachThr > 255) m_lSrTeachThr = 255;
	if(m_lSrTeachThr < 0) m_lSrTeachThr = 0;
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionSroutMinus()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	Hobject ImageReduced, ImageReducedWhite, Region, RegionClosing, LeadTopRgn, LeadBotRgn;
	Hobject FillUp;
	HTuple Num, Wd, Ht, Max;
	CRect rect;
	Hlong num;
	int i;
	Hobject DontCare, OCVRgn, MeanImage, RegionWhite, ErosImage, DilRegion;

	UpdateData(TRUE);
	Update(FALSE);

	disp_obj(*pdlg->m_VisMod->GetHalSROutMasterImage(0), m_lMWindowID_SROut);

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*pdlg->m_VisMod->GetHalSROutMasterImage(0), NULL, NULL, &Wd, &Ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_11!");
		pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
		return;
	}	

	disp_obj(*pdlg->m_VisMod->GetHalSROutMasterImage(0), pdlg->m_dlgImgRect[0]->GetView());
	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		//Film
		rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[i][0];
		gen_rectangle1(&Region, rect.top, rect.left, rect.bottom, rect.right);

		//Don't care region
		gen_rectangle1(&DontCare,  pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0].top, 
								   pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0].left, 
								   pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0].bottom, 
								   pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0].right);

		difference(Region, DontCare, &Region);
		intersection(pdlg->m_dlgSetup->m_HalInsRgn[i][0], Region, &Region);

		reduce_domain(*pdlg->m_VisMod->GetHalSROutMasterImage(0), Region, &ImageReduced);
		//reduce_domain(*pdlg->m_VisMod->GetHalSROutMasterImage(0), RegionWhite, &ImageReducedWhite); //20101024 lee

		set_check("~give_error");
		herr = threshold(ImageReduced, &Region, 0, 90);
		set_check("give_error");
		if(H_MSG_TRUE == herr)
		{
		}
		else
		{
			status.Format("Halcon Error Occured at threshold_05!");
			pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
			return;
		}

		connection(Region, &Region);

		//LSS
		int max = m_lSrTeachThr;
		
		if( (0 >= max) || (255 < max) ) //threshold error 관련
		{
			max = 150;
		}


		set_check("~give_error");
		herr = threshold(ImageReduced, &RegionWhite, max, 255);
		set_check("give_error");
		if(H_MSG_TRUE == herr)
		{
		}
		else
		{
			status.Format("Halcon Error Occured at threshold_06!");
			pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
			return;
		}
	
		connection(RegionWhite, &RegionWhite);
		select_shape(RegionWhite, &RegionWhite, HTuple("area"), HTuple("and"), HTuple(20),  HTuple("max"));
		select_shape(RegionWhite, &RegionWhite, HTuple("rb"), HTuple("and"), HTuple(5),  HTuple("max"));
		
		union1(Region, &Region);
		union1(RegionWhite, &RegionWhite);
		union2(Region, RegionWhite, &Region);
		fill_up(Region, &Region);
		closing_rectangle1(Region, &Region, 50.5, 50.5);
		closing_circle(Region, &Region, 10.5);
		fill_up(Region, &Region);


		connection(Region, &Region);
		select_shape(Region, &Region, HTuple("area"), HTuple("and"), HTuple(1000),  HTuple("max"));

		dilation_circle(Region, &DilRegion, 7.5);
		difference(pdlg->m_dlgSetup->m_HalInsRgn[i][0], DilRegion, &Region);

		erosion_circle(Region, &Region, 9.5);

		connection(Region, &Region);
		select_shape(Region, &Region, HTuple("area"), HTuple("and"), HTuple(1000),  HTuple("max"));

		count_obj(Region, &Num);
		num = (Hlong)Num[0];
		if(0 < num)
		{
			union1(Region, &Region);
			//fill_up(Region, &Region);

			disp_obj(*pdlg->m_VisMod->GetHalSROutMasterImage(0), pdlg->m_dlgImgRect[0]->GetView());
			set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("blue"));
			set_draw(pdlg->m_dlgImgRect[0]->GetView(), HTuple("margin"));
			disp_obj(Region, pdlg->m_dlgImgRect[0]->GetView());

			copy_obj(Region, &pdlg->m_dlgSetup->m_HalInsRgn[i][5], 1, -1);//SR-out region
		}
		else
		{
			gen_empty_obj(&pdlg->m_dlgSetup->m_HalInsRgn[i][5]);
		}
	}
	
	m_lSrTeachThr = m_lSrTeachThr - 5;
	if(m_lSrTeachThr > 255) m_lSrTeachThr = 255;	
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionPfStart()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_lSrTeachThr = 150;
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionPf()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	Hobject ImageReduced, Region, RegionDilation;
	HTuple Num, Min, Max;
	CRect rect;
	Hlong num, min;
	int i;
	Hobject DontCare;

	UpdateData(TRUE);
	Update(FALSE);

	disp_obj(*pdlg->m_VisMod->GetHalLeadMasterImage(0), pdlg->m_dlgImgRect[0]->GetView());
	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		//Don't care region
		gen_rectangle1(&DontCare,  pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0].top, 
								   pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0].left, 
								   pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0].bottom, 
								   pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0].right);

		//Film
		rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[i][0];
		gen_rectangle1(&Region, rect.top, rect.left, rect.bottom, rect.right);

		dilation_circle(pdlg->m_dlgSetup->m_HalInsRgn[i][0], &RegionDilation, 5);//SR region
		difference(Region, RegionDilation, &Region);
		//difference(Region, DontCare, &Region);

		//reduce_domain(*pdlg->m_VisMod->GetHalLeadMasterImage(0), Region, &ImageReduced);
		reduce_domain(*pdlg->m_VisMod->GetHalLeadMasterImage(0), Region, &ImageReduced);

		//Find suitable threshold
		//min_max_gray(Region, *pdlg->m_VisMod->GetHalLeadMasterImage(0), HTuple(0), &Min, &Max, _);
		//min_max_gray(Region, *pdlg->m_VisMod->GetHalBlueImage(0, 0, 4), HTuple(0), &Min, &Max, _);
		//min = (long)Max[0];

		min = m_lSrTeachThr;

		if( (0 >= min) || (255 < min) ) // threshold error 관련
		{
			min = 150;
		}
		
		Herror herr;
		CString status;

		set_check("~give_error");
		herr = 	threshold(ImageReduced, &Region, min, 255); 
		set_check("give_error");
		if(H_MSG_TRUE == herr)
		{
		}
		else
		{
			status.Format("Halcon Error Occured at threshold_01!... at %ld",(long)min);
			pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
			return;
		}

		//Temp
		//disp_obj(Region, pdlg->m_dlgImgRect[0]->GetView());
		

		connection(Region, &Region);
		select_shape(Region, &Region, HTuple("area"), HTuple("and"), HTuple(100),  HTuple("max"));

		closing_circle(Region, &Region, 12.5);
		shape_trans(Region, &Region, "convex");
		dilation_circle(Region, &Region, 5.5);
		closing_circle(Region, &Region, 12.5);
		
		//difference(Region, DontCare, &Region);

		count_obj(Region, &Num);
		num = (Hlong)Num[0];
		if(0 < num)
		{
			union1(Region, &Region);
			fill_up(Region, &Region);

			//disp_obj(*pdlg->m_VisMod->GetHalLeadMasterImage(0), pdlg->m_dlgImgRect[0]->GetView());
			set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("blue"));
			set_draw(pdlg->m_dlgImgRect[0]->GetView(), HTuple("margin"));
			disp_obj(Region, pdlg->m_dlgImgRect[0]->GetView());

			copy_obj(Region, &pdlg->m_dlgSetup->m_HalInsRgn[i][1], 1, -1);//PF hall region
		}
		else
		{
			gen_empty_obj(&pdlg->m_dlgSetup->m_HalInsRgn[i][1]);
		}
	}	

	m_lSrTeachThr = m_lSrTeachThr + 5;
	if(m_lSrTeachThr > 255) m_lSrTeachThr = 255;
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionPfMinus()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	Hobject ImageReduced, Region, RegionDilation;
	HTuple Num, Min, Max;
	CRect rect;
	Hlong num, min;
	int i;
	Hobject DontCare;

	UpdateData(TRUE);
	Update(FALSE);

	disp_obj(*pdlg->m_VisMod->GetHalLeadMasterImage(0), pdlg->m_dlgImgRect[0]->GetView());
	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		//Don't care region
		gen_rectangle1(&DontCare,  pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0].top, 
								   pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0].left, 
								   pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0].bottom, 
								   pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0].right);

		//Film
		rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[i][0];
		gen_rectangle1(&Region, rect.top, rect.left, rect.bottom, rect.right);

		dilation_circle(pdlg->m_dlgSetup->m_HalInsRgn[i][0], &RegionDilation, 5);//SR region
		difference(Region, RegionDilation, &Region);
		//difference(Region, DontCare, &Region);

		//reduce_domain(*pdlg->m_VisMod->GetHalLeadMasterImage(0), Region, &ImageReduced);
		reduce_domain(*pdlg->m_VisMod->GetHalLeadMasterImage(0), Region, &ImageReduced);

		//Find suitable threshold
		//min_max_gray(Region, *pdlg->m_VisMod->GetHalLeadMasterImage(0), HTuple(0), &Min, &Max, _);
		//min_max_gray(Region, *pdlg->m_VisMod->GetHalBlueImage(0, 0, 4), HTuple(0), &Min, &Max, _);
		//min = (long)Max[0];

		min = m_lSrTeachThr;

		if( (0 >= min) || (255 < min) ) //threshold error 관련
		{
			min = 150;
		}
		
		Herror herr;
		CString status;

		set_check("~give_error");
		herr = 	threshold(ImageReduced, &Region, min, 255); 
		set_check("give_error");
		if(H_MSG_TRUE == herr)
		{
		}
		else
		{
			status.Format("Halcon Error Occured at threshold_01!... at %ld",(long)min);
			pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
			return;
		}

		//Temp
		//disp_obj(Region, pdlg->m_dlgImgRect[0]->GetView());
		

		connection(Region, &Region);
		select_shape(Region, &Region, HTuple("area"), HTuple("and"), HTuple(100),  HTuple("max"));

		closing_circle(Region, &Region, 12.5);
		shape_trans(Region, &Region, "convex");
		dilation_circle(Region, &Region, 5.5);
		closing_circle(Region, &Region, 12.5);
		
		//difference(Region, DontCare, &Region);

		count_obj(Region, &Num);
		num = (Hlong)Num[0];
		if(0 < num)
		{
			union1(Region, &Region);
			fill_up(Region, &Region);

			//disp_obj(*pdlg->m_VisMod->GetHalLeadMasterImage(0), pdlg->m_dlgImgRect[0]->GetView());
			set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("blue"));
			set_draw(pdlg->m_dlgImgRect[0]->GetView(), HTuple("margin"));
			disp_obj(Region, pdlg->m_dlgImgRect[0]->GetView());

			copy_obj(Region, &pdlg->m_dlgSetup->m_HalInsRgn[i][1], 1, -1);//PF hall region
		}
		else
		{
			gen_empty_obj(&pdlg->m_dlgSetup->m_HalInsRgn[i][1]);
		}
	}	

	m_lSrTeachThr = m_lSrTeachThr - 5;
	if(m_lSrTeachThr > 255) m_lSrTeachThr = 255;	
	if(m_lSrTeachThr < 0) m_lSrTeachThr = 0;
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionLead()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	Hobject Region;
	RECT rect;

	int i;
	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{	
		rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][0];
		gen_rectangle1(&Region, rect.top, rect.left, rect.bottom, rect.right);

		set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("blue"));
		set_draw(pdlg->m_dlgImgRect[0]->GetView(), HTuple("margin"));
		disp_obj(Region, pdlg->m_dlgImgRect[0]->GetView());
		
		copy_obj(Region, &pdlg->m_dlgSetup->m_HalInsRgn[i][2], 1, -1);
	}
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionLeadIn()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	Hobject Region;
	RECT rect;

	int i;
	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{	
		rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][1];
		gen_rectangle1(&Region, rect.top, rect.left, rect.bottom, rect.right);

		set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("yellow"));
		set_draw(pdlg->m_dlgImgRect[0]->GetView(), HTuple("margin"));
		disp_obj(Region, pdlg->m_dlgImgRect[0]->GetView());
		
		copy_obj(Region, &pdlg->m_dlgSetup->m_HalInsRgn[i][3], 1, -1);
	}
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignFindchip()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	pdlg->m_dlgSetup->m_SetUpChipDlg.ClickBtnenhctrlChipFindchip();
	
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignPotTeach()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	pdlg->m_dlgSetup->m_SetUpChipDlg.Update(TRUE);
	pdlg->m_dlgSetup->m_SetUpChipDlg.ClickBtnenhctrlChipPotTeach();
}


void CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionAm()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	Hobject ImageReduced, Region, RegionDilation;
	HTuple Num, Min, Max;
	CRect rect;
	Hlong num, min;
	int i;
	Hobject DontCare;

	UpdateData(TRUE);
	Update(FALSE);

	//Inspect region
	//0 : SR inspect region, 1 : PF hall region, 
	//2 : up lead region, 3 : dn lead region
	//4 : user area region 5 : SR out region
	//6 : vaccum region, 7 : chip region
	//8 : A/M 0번(위쪽)  9 : A/M 0번(아래쪽)
	//10 : A/M 0번(위쪽)  11 : A/M 0번(아래쪽): 

	disp_obj(*pdlg->m_VisMod->GetHalLeadMasterImage(0), pdlg->m_dlgImgRect[0]->GetView());
	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		//#0번
		if(pdlg->m_dlgSetup->m_SysPara.m_bPara[15])
		{
			//위/////////////////////////////////////////////////////////////////

			rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][4];
			gen_rectangle1(&Region, rect.top, rect.left, rect.bottom, rect.right);

			reduce_domain(*pdlg->m_VisMod->GetHalSRMasterImage(0), Region, &ImageReduced);

			min = m_lSrTeachThr;

			if( (150 >= min) || (255 < min) ) // threshold error 관련
			{
				min = 150;
			}

			Herror herr;
			CString status;

			set_check("~give_error");
			herr = 	threshold(ImageReduced, &Region, min, 255); 
			set_check("give_error");
			if(H_MSG_TRUE == herr)
			{
			}
			else
			{
				status.Format("Halcon Error Occured at threshold_01!... at %ld",(long)min);
				pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
				return;
			}

			connection(Region, &Region);
			select_shape(Region, &Region, HTuple("area"), HTuple("and"), HTuple(10),  HTuple("max"));

			closing_circle(Region, &Region, 1.5);
			erosion_circle(Region, &Region, 1.5);

			count_obj(Region, &Num);
			num = (Hlong)Num[0];
			if(0 < num)
			{
				union1(Region, &Region);
				fill_up(Region, &Region);

				set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("blue"));
				set_draw(pdlg->m_dlgImgRect[0]->GetView(), HTuple("margin"));
				disp_obj(Region, pdlg->m_dlgImgRect[0]->GetView());

				copy_obj(Region, &pdlg->m_dlgSetup->m_HalInsRgn[i][8], 1, -1);//PF hall region
			}
			else
			{
				gen_empty_obj(&pdlg->m_dlgSetup->m_HalInsRgn[i][8]);
			}

			//아래//////////////////////////////////////////////////////////////////
			rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][5];
			gen_rectangle1(&Region, rect.top, rect.left, rect.bottom, rect.right);

			reduce_domain(*pdlg->m_VisMod->GetHalSRMasterImage(0), Region, &ImageReduced);

			min = m_lSrTeachThr;

			if( (150 >= min) || (255 < min) ) // threshold error 관련
			{
				min = 150;
			}

			set_check("~give_error");
			herr = 	threshold(ImageReduced, &Region, min, 255); 
			set_check("give_error");
			if(H_MSG_TRUE == herr)
			{
			}
			else
			{
				status.Format("Halcon Error Occured at threshold_01!... at %ld",(long)min);
				pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
				return;
			}

			connection(Region, &Region);
			select_shape(Region, &Region, HTuple("area"), HTuple("and"), HTuple(10),  HTuple("max"));

			closing_circle(Region, &Region, 1.5);
			erosion_circle(Region, &Region, 1.5);

			count_obj(Region, &Num);
			num = (Hlong)Num[0];
			if(0 < num)
			{
				union1(Region, &Region);
				fill_up(Region, &Region);

				set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("blue"));
				set_draw(pdlg->m_dlgImgRect[0]->GetView(), HTuple("margin"));
				disp_obj(Region, pdlg->m_dlgImgRect[0]->GetView());

				copy_obj(Region, &pdlg->m_dlgSetup->m_HalInsRgn[i][9], 1, -1);//PF hall region
			}
			else
			{
				gen_empty_obj(&pdlg->m_dlgSetup->m_HalInsRgn[i][9]);
			}
		}
		//#1번
		if(pdlg->m_dlgSetup->m_SysPara.m_bPara[16])
		{
			//위/////////////////////////////////////////////////////////////////

			rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][6];
			gen_rectangle1(&Region, rect.top, rect.left, rect.bottom, rect.right);

			reduce_domain(*pdlg->m_VisMod->GetHalSRMasterImage(0), Region, &ImageReduced);

			min = m_lSrTeachThr;

			if( (150 >= min) || (255 < min) ) // threshold error 관련
			{
				min = 150;
			}

			Herror herr;
			CString status;

			set_check("~give_error");
			herr = 	threshold(ImageReduced, &Region, min, 255); 
			set_check("give_error");
			if(H_MSG_TRUE == herr)
			{
			}
			else
			{
				status.Format("Halcon Error Occured at threshold_01!... at %ld",(long)min);
				pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
				return;
			}

			connection(Region, &Region);
			select_shape(Region, &Region, HTuple("area"), HTuple("and"), HTuple(10),  HTuple("max"));

			closing_circle(Region, &Region, 1.5);
			erosion_circle(Region, &Region, 1.5);

			count_obj(Region, &Num);
			num = (Hlong)Num[0];
			if(0 < num)
			{
				union1(Region, &Region);
				fill_up(Region, &Region);

				set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("blue"));
				set_draw(pdlg->m_dlgImgRect[0]->GetView(), HTuple("margin"));
				disp_obj(Region, pdlg->m_dlgImgRect[0]->GetView());

				copy_obj(Region, &pdlg->m_dlgSetup->m_HalInsRgn[i][10], 1, -1);//PF hall region
			}
			else
			{
				gen_empty_obj(&pdlg->m_dlgSetup->m_HalInsRgn[i][10]);
			}

			//아래//////////////////////////////////////////////////////////////////
			rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][7];
			gen_rectangle1(&Region, rect.top, rect.left, rect.bottom, rect.right);

			reduce_domain(*pdlg->m_VisMod->GetHalSRMasterImage(0), Region, &ImageReduced);

			min = m_lSrTeachThr;

			if( (150 >= min) || (255 < min) ) // threshold error 관련
			{
				min = 150;
			}

			set_check("~give_error");
			herr = 	threshold(ImageReduced, &Region, min, 255); 
			set_check("give_error");
			if(H_MSG_TRUE == herr)
			{
			}
			else
			{
				status.Format("Halcon Error Occured at threshold_01!... at %ld",(long)min);
				pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
				return;
			}

			connection(Region, &Region);
			select_shape(Region, &Region, HTuple("area"), HTuple("and"), HTuple(10),  HTuple("max"));

			closing_circle(Region, &Region, 1.5);
			erosion_circle(Region, &Region, 1.5);

			count_obj(Region, &Num);
			num = (Hlong)Num[0];
			if(0 < num)
			{
				union1(Region, &Region);
				fill_up(Region, &Region);

				set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("blue"));
				set_draw(pdlg->m_dlgImgRect[0]->GetView(), HTuple("margin"));
				disp_obj(Region, pdlg->m_dlgImgRect[0]->GetView());

				copy_obj(Region, &pdlg->m_dlgSetup->m_HalInsRgn[i][11], 1, -1);//PF hall region
			}
			else
			{
				gen_empty_obj(&pdlg->m_dlgSetup->m_HalInsRgn[i][11]);
			}
		}
	}	

	m_lSrTeachThr = m_lSrTeachThr + 5;
	if(m_lSrTeachThr > 255) m_lSrTeachThr = 255;
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionAmStart()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_lSrTeachThr = 150;
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionMMinus()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	Hobject ImageReduced, Region, RegionDilation;
	HTuple Num, Min, Max;
	CRect rect;
	Hlong num, min;
	int i;
	Hobject DontCare;

	UpdateData(TRUE);
	Update(FALSE);

	//Inspect region
	//0 : SR inspect region, 1 : PF hall region, 
	//2 : up lead region, 3 : dn lead region
	//4 : user area region 5 : SR out region
	//6 : vaccum region, 7 : chip region
	//8 : A/M 0번(위쪽)  9 : A/M 0번(아래쪽)
	//10 : A/M 0번(위쪽)  11 : A/M 0번(아래쪽): 

	disp_obj(*pdlg->m_VisMod->GetHalLeadMasterImage(0), pdlg->m_dlgImgRect[0]->GetView());
	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		//#0번
		if(pdlg->m_dlgSetup->m_SysPara.m_bPara[15])
		{
			//위/////////////////////////////////////////////////////////////////

			rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][4];
			gen_rectangle1(&Region, rect.top, rect.left, rect.bottom, rect.right);

			reduce_domain(*pdlg->m_VisMod->GetHalSRMasterImage(0), Region, &ImageReduced);

			min = m_lSrTeachThr;

			if( (150 >= min) || (255 < min) ) // threshold error 관련
			{
				min = 150;
			}

			Herror herr;
			CString status;

			set_check("~give_error");
			herr = 	threshold(ImageReduced, &Region, min, 255); 
			set_check("give_error");
			if(H_MSG_TRUE == herr)
			{
			}
			else
			{
				status.Format("Halcon Error Occured at threshold_01!... at %ld",(long)min);
				pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
				return;
			}

			connection(Region, &Region);
			select_shape(Region, &Region, HTuple("area"), HTuple("and"), HTuple(10),  HTuple("max"));

			closing_circle(Region, &Region, 1.5);

			count_obj(Region, &Num);
			num = (Hlong)Num[0];
			if(0 < num)
			{
				union1(Region, &Region);
				fill_up(Region, &Region);

				set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("blue"));
				set_draw(pdlg->m_dlgImgRect[0]->GetView(), HTuple("margin"));
				disp_obj(Region, pdlg->m_dlgImgRect[0]->GetView());

				copy_obj(Region, &pdlg->m_dlgSetup->m_HalInsRgn[i][8], 1, -1);//PF hall region
			}
			else
			{
				gen_empty_obj(&pdlg->m_dlgSetup->m_HalInsRgn[i][8]);
			}

			//아래//////////////////////////////////////////////////////////////////
			rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][5];
			gen_rectangle1(&Region, rect.top, rect.left, rect.bottom, rect.right);

			reduce_domain(*pdlg->m_VisMod->GetHalSRMasterImage(0), Region, &ImageReduced);

			min = m_lSrTeachThr;

			if( (150 >= min) || (255 < min) ) // threshold error 관련
			{
				min = 150;
			}

			set_check("~give_error");
			herr = 	threshold(ImageReduced, &Region, min, 255); 
			set_check("give_error");
			if(H_MSG_TRUE == herr)
			{
			}
			else
			{
				status.Format("Halcon Error Occured at threshold_01!... at %ld",(long)min);
				pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
				return;
			}

			connection(Region, &Region);
			select_shape(Region, &Region, HTuple("area"), HTuple("and"), HTuple(10),  HTuple("max"));

			closing_circle(Region, &Region, 1.5);

			count_obj(Region, &Num);
			num = (Hlong)Num[0];
			if(0 < num)
			{
				union1(Region, &Region);
				fill_up(Region, &Region);

				set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("blue"));
				set_draw(pdlg->m_dlgImgRect[0]->GetView(), HTuple("margin"));
				disp_obj(Region, pdlg->m_dlgImgRect[0]->GetView());

				copy_obj(Region, &pdlg->m_dlgSetup->m_HalInsRgn[i][9], 1, -1);//PF hall region
			}
			else
			{
				gen_empty_obj(&pdlg->m_dlgSetup->m_HalInsRgn[i][9]);
			}
		}
		//#1번
		if(pdlg->m_dlgSetup->m_SysPara.m_bPara[16])
		{
			//위/////////////////////////////////////////////////////////////////

			rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][6];
			gen_rectangle1(&Region, rect.top, rect.left, rect.bottom, rect.right);

			reduce_domain(*pdlg->m_VisMod->GetHalSRMasterImage(0), Region, &ImageReduced);

			min = m_lSrTeachThr;

			if( (150 >= min) || (255 < min) ) // threshold error 관련
			{
				min = 150;
			}

			Herror herr;
			CString status;

			set_check("~give_error");
			herr = 	threshold(ImageReduced, &Region, min, 255); 
			set_check("give_error");
			if(H_MSG_TRUE == herr)
			{
			}
			else
			{
				status.Format("Halcon Error Occured at threshold_01!... at %ld",(long)min);
				pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
				return;
			}

			connection(Region, &Region);
			select_shape(Region, &Region, HTuple("area"), HTuple("and"), HTuple(10),  HTuple("max"));

			closing_circle(Region, &Region, 1.5);

			count_obj(Region, &Num);
			num = (Hlong)Num[0];
			if(0 < num)
			{
				union1(Region, &Region);
				fill_up(Region, &Region);

				set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("blue"));
				set_draw(pdlg->m_dlgImgRect[0]->GetView(), HTuple("margin"));
				disp_obj(Region, pdlg->m_dlgImgRect[0]->GetView());

				copy_obj(Region, &pdlg->m_dlgSetup->m_HalInsRgn[i][10], 1, -1);//PF hall region
			}
			else
			{
				gen_empty_obj(&pdlg->m_dlgSetup->m_HalInsRgn[i][10]);
			}

			//아래//////////////////////////////////////////////////////////////////
			rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][7];
			gen_rectangle1(&Region, rect.top, rect.left, rect.bottom, rect.right);

			reduce_domain(*pdlg->m_VisMod->GetHalSRMasterImage(0), Region, &ImageReduced);

			min = m_lSrTeachThr;

			if( (150 >= min) || (255 < min) ) // threshold error 관련
			{
				min = 150;
			}

			set_check("~give_error");
			herr = 	threshold(ImageReduced, &Region, min, 255); 
			set_check("give_error");
			if(H_MSG_TRUE == herr)
			{
			}
			else
			{
				status.Format("Halcon Error Occured at threshold_01!... at %ld",(long)min);
				pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
				return;
			}

			connection(Region, &Region);
			select_shape(Region, &Region, HTuple("area"), HTuple("and"), HTuple(10),  HTuple("max"));

			closing_circle(Region, &Region, 1.5);

			count_obj(Region, &Num);
			num = (Hlong)Num[0];
			if(0 < num)
			{
				union1(Region, &Region);
				fill_up(Region, &Region);

				set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("blue"));
				set_draw(pdlg->m_dlgImgRect[0]->GetView(), HTuple("margin"));
				disp_obj(Region, pdlg->m_dlgImgRect[0]->GetView());

				copy_obj(Region, &pdlg->m_dlgSetup->m_HalInsRgn[i][11], 1, -1);//PF hall region
			}
			else
			{
				gen_empty_obj(&pdlg->m_dlgSetup->m_HalInsRgn[i][11]);
			}
		}
	}	

	m_lSrTeachThr = m_lSrTeachThr - 5;
	if(m_lSrTeachThr > 255) m_lSrTeachThr = 255;
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionMMinusV()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	int i;

	set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("blue"));
	set_draw(pdlg->m_dlgImgRect[0]->GetView(), HTuple("margin"));

	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		disp_obj(pdlg->m_dlgSetup->m_HalInsRgn[i][8], pdlg->m_dlgImgRect[0]->GetView());
		disp_obj(pdlg->m_dlgSetup->m_HalInsRgn[i][9], pdlg->m_dlgImgRect[0]->GetView());
		disp_obj(pdlg->m_dlgSetup->m_HalInsRgn[i][10], pdlg->m_dlgImgRect[0]->GetView());
		disp_obj(pdlg->m_dlgSetup->m_HalInsRgn[i][11], pdlg->m_dlgImgRect[0]->GetView());
	}
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionSrMinusV()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	int i;

	set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("blue"));
	set_draw(pdlg->m_dlgImgRect[0]->GetView(), HTuple("margin"));

	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{

		disp_obj(pdlg->m_dlgSetup->m_HalInsRgn[i][0], pdlg->m_dlgImgRect[0]->GetView());
	}
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionSroutMinusV()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	int i;

	set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("blue"));
	set_draw(pdlg->m_dlgImgRect[0]->GetView(), HTuple("margin"));

	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{

		disp_obj(pdlg->m_dlgSetup->m_HalInsRgn[i][5], pdlg->m_dlgImgRect[0]->GetView());
	}
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionPfMinusV()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	int i;

	set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("blue"));
	set_draw(pdlg->m_dlgImgRect[0]->GetView(), HTuple("margin"));

	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{

		disp_obj(pdlg->m_dlgSetup->m_HalInsRgn[i][1], pdlg->m_dlgImgRect[0]->GetView());
	}
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionLeadV()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	int i;

	set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("blue"));
	set_draw(pdlg->m_dlgImgRect[0]->GetView(), HTuple("margin"));

	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{

		disp_obj(pdlg->m_dlgSetup->m_HalInsRgn[i][2], pdlg->m_dlgImgRect[0]->GetView());
	}
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignInsRegionLeadInV()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	int i;

	set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("blue"));
	set_draw(pdlg->m_dlgImgRect[0]->GetView(), HTuple("margin"));

	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{

		disp_obj(pdlg->m_dlgSetup->m_HalInsRgn[i][3], pdlg->m_dlgImgRect[0]->GetView());
	}
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignAutoregion()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

}

void CSetUpAlignDlg::ClickBtnenhctrlAlignMakeSr()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	CDialogSRSubRegion dlg;

	Update(FALSE);
	pdlg->m_dlgImgRect[m_iRectIdx]->DeleteAll();

	dlg.DoModal();

	Update(TRUE);
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignInsSrDarkP()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	MakeSRRegion();

	m_dSrTeachThrDark = m_dSrTeachThrDark + 0.01;
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignInsSrDarkM()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	MakeSRRegion();

	m_dSrTeachThrDark = m_dSrTeachThrDark - 0.01;
}

void CSetUpAlignDlg::ClickBtnenhctrlAlignInsSrWhiteP()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	MakeSRRegion();

	m_dSrTeachThrWhite = m_dSrTeachThrWhite + 0.01;
}



void CSetUpAlignDlg::ClickBtnenhctrlAlignInsSrWhiteM()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	MakeSRRegion();

	m_dSrTeachThrWhite = m_dSrTeachThrWhite - 0.01;
}

void CSetUpAlignDlg::MakeSRRegion(void)
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	Hobject ImageReduced, Region, RegionClosing, LeadTopRgn, LeadBotRgn, LeadTopRgn2, LeadTopRgn3;
	Hobject FillUp;
	HTuple Num, Wd, Ht, Max;
	CRect rect;
	Hlong num, max;
	int i;
	Hobject DontCare, OCVRgn, ChipRgn;

	//검은영역을 제거하기위한 영역
	Hobject DarkRegion, WhiteRegion;
	Hlong dark_max, white_max;


	UpdateData(TRUE);
	Update(FALSE);

	disp_obj(*pdlg->m_VisMod->GetHalSRMasterImage(0), m_lMWindowID_SR);

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*pdlg->m_VisMod->GetHalSRMasterImage(0), NULL, NULL, &Wd, &Ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_07!");
		pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
		return;
	}	

	disp_obj(*pdlg->m_VisMod->GetHalSRMasterImage(0), pdlg->m_dlgImgRect[0]->GetView());

	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		//Film
		rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionFilm[i][0];
		gen_rectangle1(&Region, rect.top, rect.left, rect.bottom, rect.right);

		//Lead top
		rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][0];
		gen_rectangle1(&LeadTopRgn, rect.top, rect.left, rect.bottom, rect.right);		// 110720 ytlee

		//Lead bottom
		rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][1];
		gen_rectangle1(&LeadBotRgn, rect.top, rect.left, rect.bottom, rect.right);		// 110720 ytlee

		//20100615 lss
		//Lead top2
		rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][2];
		gen_rectangle1(&LeadTopRgn2, rect.top, 0, rect.bottom, (Hlong)Wd[0]);
		//Lead top3
		rect = pdlg->m_dlgSetup->m_InsPara.m_InsRegionLead[i][3];
		gen_rectangle1(&LeadTopRgn3, rect.top, 0, rect.bottom, (Hlong)Wd[0]);

		//Don't care region
		gen_rectangle1(&DontCare,  pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0].top, 
								   pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0].left, 
								   pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0].bottom, 
								   pdlg->m_dlgSetup->m_InsPara.m_InsRegionSR[i][0].right);

		if(pdlg->m_dlgSetup->m_SysPara.m_bPara[25])		// 110826 ytlee
		{
			//OCV serach region
			gen_rectangle1(&OCVRgn,  pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[i][0].top, 
									   pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[i][0].left, 
									   pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[i][0].bottom, 
									   pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[i][0].right);
		}

		//Chip Search Region //20110510 lee
		gen_rectangle1(&ChipRgn,  pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0].top, 
								   pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0].left, 
								   pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0].bottom, 
								   pdlg->m_dlgSetup->m_InsPara.m_InsRegionChip[i][0].right);

		difference(Region, LeadTopRgn, &Region);
		difference(Region, LeadBotRgn, &Region);
		difference(Region, DontCare, &Region);
		difference(Region, ChipRgn, &Region);

		//union2(Region, OCVRgn, &Region);	//OCV 영역을 SR 영역에 포함시키지 않는다.

		reduce_domain(*pdlg->m_VisMod->GetHalSRMasterImage(0), Region, &ImageReduced);
		
		/*
		//Find suitable threshold
		min_max_gray(Region, *pdlg->m_VisMod->GetHalSRMasterImage(0), HTuple(0), _, &Max, _);
		max = (long)Max[0];
		max = max - 35;
		*/

		max = m_lSrTeachThr;
		dark_max = m_lSrTeachThr * m_dSrTeachThrDark;
		white_max = m_lSrTeachThr * m_dSrTeachThrWhite;

		if( (0 >= max) || (255 < max) ) // threshold error 관련
		{
			max = 50;
		}
		if( (0 >= dark_max) || (255 < dark_max) ) // threshold error 관련
		{
			dark_max = 10;
		}
		if( (0 >= white_max) || (255 < white_max) ) // threshold error 관련
		{
			white_max = 254;
		}

		set_check("~give_error");
		herr = threshold(ImageReduced, &Region, 0, max);
		set_check("give_error");
		if(H_MSG_TRUE == herr)
		{
		}
		else
		{
			status.Format("Halcon Error Occured at threshold_00!... at %ld",(long)max);
			pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
			return;
		}

		set_check("~give_error");
		herr = threshold(ImageReduced, &DarkRegion, 0, dark_max);
		set_check("give_error");

		set_check("~give_error");
		herr = threshold(ImageReduced, &WhiteRegion, white_max, 255);
		set_check("give_error");

		if(H_MSG_TRUE == herr)
		{
		}
		else
		{
			status.Format("Halcon Error Occured at threshold_00!... at %ld",(long)max);
			pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
			return;
		}


		connection(Region, &Region);
		select_shape(Region, &Region, HTuple("area"), HTuple("and"), HTuple(100000),  HTuple("max"));

		connection(DarkRegion, &DarkRegion);
		select_shape(DarkRegion, &DarkRegion, HTuple("area"), HTuple("and"), HTuple(10),  HTuple("max"));
		
		connection(WhiteRegion, &WhiteRegion);
		select_shape(WhiteRegion, &WhiteRegion, HTuple("area"), HTuple("and"), HTuple(10),  HTuple("max"));


		closing_circle(Region, &Region, 20);
		erosion_circle(Region, &Region, 15);
		erosion_rectangle1(Region, &Region, 20, 10);

		erosion_rectangle1(FillUp, &FillUp, 10.5, 5.5);
		
		//union2(Region, OCVRgn, &Region);
		dilation_circle(DarkRegion, &DarkRegion, 7);
		difference(Region, DarkRegion, &Region);

		dilation_circle(WhiteRegion, &WhiteRegion, 7);
		difference(Region, WhiteRegion, &Region);

		count_obj(Region, &Num);
		num = (Hlong)Num[0];
		if(0 < num)
		{
			union1(Region, &Region);

			//Don't care region


			//fill_up(Region, &Region);

			//disp_obj(*pdlg->m_VisMod->GetHalSRMasterImage(0), pdlg->m_dlgImgRect[0]->GetView());
			set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("blue"));
			set_draw(pdlg->m_dlgImgRect[0]->GetView(), HTuple("margin"));
			disp_obj(Region, pdlg->m_dlgImgRect[0]->GetView());

			copy_obj(Region, &pdlg->m_dlgSetup->m_HalInsRgn[i][0], 1, -1);//SR region
		}
		else
		{
			gen_empty_obj(&pdlg->m_dlgSetup->m_HalInsRgn[i][0]);
		}
	}
}

