// SetUpSysDlg.cpp : 구현 파일입니다.
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
#include "PasswordDlg.h"

#include "SetUpSysDlg.h"

// CSetUpSysDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSetUpSysDlg, CDialog)

CSetUpSysDlg::CSetUpSysDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetUpSysDlg::IDD, pParent)
{
	m_iDeleteTerm = 7;
	m_dPixelSize = PIXEL_SIZE;
	m_0 = FALSE;
	m_1 = FALSE;
	m_2 = FALSE;
	m_3 = FALSE;
	m_4 = FALSE;
	m_5 = FALSE;
	m_6 = FALSE;
	m_7 = FALSE;
	m_8 = FALSE;
	m_9 = FALSE;
	m_10 = FALSE;
	m_11 = FALSE;
	m_12 = FALSE;
	m_13 = FALSE;
	m_14 = FALSE;
	m_15 = FALSE;
	m_16 = FALSE;
	m_17 = FALSE;
	m_18 = FALSE;
	m_19 = FALSE;
	m_20 = FALSE;
	m_21 = FALSE;
	m_22 = FALSE;
	m_23 = FALSE;
	m_24 = FALSE;
	m_25 = FALSE;
	m_26 = FALSE;
	m_27 = FALSE;
	m_28 = FALSE;
	m_29 = FALSE;

	m_chk_goodsave = FALSE;
	m_chk_ngsave = FALSE;
	m_chk_ngoverlay = FALSE;
	m_chk_NgRstDisp = FALSE;
	m_chk_InspParamLock = FALSE;

	m_bShow = FALSE;
}

CSetUpSysDlg::~CSetUpSysDlg()
{
}

void CSetUpSysDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PIXELSIZE_SYS, m_dPixelSize);
	DDX_Text(pDX, IDC_DELETE_TERM, m_iDeleteTerm);
	DDX_Check(pDX, IDC_CHECK_INS_USE0_SYS, m_0);
	DDX_Check(pDX, IDC_CHECK_INS_USE1_SYS, m_1);
	DDX_Check(pDX, IDC_CHECK_INS_USE2_SYS, m_2);
	DDX_Check(pDX, IDC_CHECK_INS_USE3_SYS, m_3);
	DDX_Check(pDX, IDC_CHECK_INS_USE4_SYS, m_4);
	DDX_Check(pDX, IDC_CHECK_INS_USE5_SYS, m_5);
	DDX_Check(pDX, IDC_CHECK_INS_USE6_SYS, m_6);
	DDX_Check(pDX, IDC_CHECK_INS_USE7_SYS, m_7);
	DDX_Check(pDX, IDC_CHECK_INS_USE8_SYS, m_8);
	DDX_Check(pDX, IDC_CHECK_INS_USE9_SYS, m_9);
	DDX_Check(pDX, IDC_CHECK_INS_USE10_SYS, m_10);
	DDX_Check(pDX, IDC_CHECK_INS_USE11_SYS, m_11);
	DDX_Check(pDX, IDC_CHECK_INS_USE12_SYS, m_12);
	DDX_Check(pDX, IDC_CHECK_INS_USE13_SYS, m_13);
	DDX_Check(pDX, IDC_CHECK_INS_USE14_SYS, m_14);
	DDX_Check(pDX, IDC_CHECK_INS_USE15_SYS, m_15);
	DDX_Check(pDX, IDC_CHECK_INS_USE16_SYS, m_16);
	DDX_Check(pDX, IDC_CHECK_INS_USE17_SYS, m_17);
	DDX_Check(pDX, IDC_CHECK_INS_USE18_SYS, m_18);
	DDX_Check(pDX, IDC_CHECK_INS_USE19_SYS, m_19);
	DDX_Check(pDX, IDC_CHECK_INS_USE20_SYS, m_20);
	DDX_Check(pDX, IDC_CHECK_INS_USE21_SYS, m_21);
	DDX_Check(pDX, IDC_CHECK_INS_USE22_SYS, m_22);
	DDX_Check(pDX, IDC_CHECK_INS_USE23_SYS, m_23);
	DDX_Check(pDX, IDC_CHECK_INS_USE24_SYS, m_24);
	DDX_Check(pDX, IDC_CHECK_INS_USE25_SYS, m_25);
	DDX_Check(pDX, IDC_CHECK_INS_USE26_SYS, m_26);
	DDX_Check(pDX, IDC_CHECK_INS_USE27_SYS, m_27);
	DDX_Check(pDX, IDC_CHECK_INS_USE28_SYS, m_28);
	DDX_Check(pDX, IDC_CHECK_INS_USE29_SYS, m_29);
	DDX_Check(pDX, IDC_CHECK_GOOD_SAVE, m_chk_goodsave);
	DDX_Check(pDX, IDC_CHECK_NG_SAVE, m_chk_ngsave);
	DDX_Check(pDX, IDC_CHECK_NG_SAVE_OVERLAY, m_chk_ngoverlay);
	DDX_Check(pDX, IDC_CHECK_NG_RESULT_DISPLAY, m_chk_NgRstDisp);
	DDX_Check(pDX, IDC_INSP_PARAM_LOCK, m_chk_InspParamLock);
}


BEGIN_MESSAGE_MAP(CSetUpSysDlg, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CSetUpSysDlg 메시지 처리기입니다.

void CSetUpSysDlg::Update(BOOL flag)
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	
	if(flag)
	{
		m_dPixelSize = pdlg->m_dlgSetup->m_SysPara.m_dPara[0];
		m_iDeleteTerm = pdlg->m_dlgSetup->m_SysPara.m_iPara[0];

		//디버그 필요
		/*
		switch(pdlg->m_VisMod->GetVisIndex())
		{
		case VISION_CAM_TOP1:	// 상부#1 고정 검사 항목
			m_3 = TRUE;			//Potting
			m_10 = TRUE;		//Film 수지
			m_11 = TRUE;		//Film PF/찢김
			m_12 = TRUE;		//Punch burr 편집불량
			m_13 = TRUE;		//Film 금속성 이물

			m_0 = FALSE;		//Chip out
			m_2 = FALSE;		//Chip 이면수지묻음 Broken
			m_27 = FALSE;		//기포,잉크칩
			m_28 = FALSE;		//구겨짐,찍힘
			m_29 = FALSE;		// "
			break;
		case VISION_CAM_BOTTOM1:	// 상부#2 고정 검사 항목
			m_0 = TRUE;		//Chip out
			m_2 = TRUE;		//Chip 이면수지묻음 Broken

			m_3 = FALSE;			//Potting
			m_10 = FALSE;		//Film 수지
			m_11 = FALSE;		//Film PF/찢김
			m_12 = FALSE;		//Punch burr 편집불량
			m_13 = FALSE;		//Film 금속성 이물
			m_27 = FALSE;		//기포,잉크칩
			m_28 = FALSE;		//구겨짐,찍힘
			m_29 = FALSE;		// "
			break;
		case VISION_CAM_TOP2:	// 하부#1 고정 검사 항목
			m_27 = TRUE;		//기포,잉크칩
			m_28 = TRUE;		//구겨짐,찍힘
			m_29 = TRUE;		// "

			m_0 = FALSE;		//Chip out
			m_2 = FALSE;		//Chip 이면수지묻음 Broken
			m_3 = FALSE;			//Potting
			m_10 = FALSE;		//Film 수지
			m_11 = FALSE;		//Film PF/찢김
			m_12 = FALSE;		//Punch burr 편집불량
			m_13 = FALSE;		//Film 금속성 이물
			break;
		case VISION_CAM_BOTTOM2:

			break;
		default:
			AfxMessageBox("Invalid Index ! CSetUpSysDlg::Update(BOOL)");
			break;
		}
		*/

		m_0 = pdlg->m_dlgSetup->m_SysPara.m_bPara[0];		//Chip out
		m_2 = pdlg->m_dlgSetup->m_SysPara.m_bPara[2];		//Chip 이면수지묻음 Broken
		m_3 = pdlg->m_dlgSetup->m_SysPara.m_bPara[3];			//Potting
		m_10 = pdlg->m_dlgSetup->m_SysPara.m_bPara[10];		//Film 수지
		m_11 = pdlg->m_dlgSetup->m_SysPara.m_bPara[11];		//Film PF/찢김
		m_12 = pdlg->m_dlgSetup->m_SysPara.m_bPara[12];		//Punch burr 편집불량
		m_13 = pdlg->m_dlgSetup->m_SysPara.m_bPara[13];		//Film 금속성 이물
		m_27 = pdlg->m_dlgSetup->m_SysPara.m_bPara[27];		//기포,잉크칩
		m_28 = pdlg->m_dlgSetup->m_SysPara.m_bPara[28];		//구겨짐,찍힘
		m_29 = pdlg->m_dlgSetup->m_SysPara.m_bPara[29];		// "

		// 상부#1 선택 검사 항목
		m_25 = pdlg->m_dlgSetup->m_SysPara.m_bPara[25];	//M/K 불량

		// 상부#2 선택 검사 항목
		m_15 = pdlg->m_dlgSetup->m_SysPara.m_bPara[15];	//AM #1
		m_16 = pdlg->m_dlgSetup->m_SysPara.m_bPara[16];	//AM #2
		// 상부#1#2 선택 검사 항목
		m_20 = pdlg->m_dlgSetup->m_SysPara.m_bPara[20];	//Out lead #2
		m_21 = pdlg->m_dlgSetup->m_SysPara.m_bPara[21];	//Out lead #2
		m_22 = pdlg->m_dlgSetup->m_SysPara.m_bPara[22]; //In lead #1
		m_23 = pdlg->m_dlgSetup->m_SysPara.m_bPara[23];	//In lead #2
	
		/////////////////////////////////////////////
		//스페어 파라메터
		m_1 = pdlg->m_dlgSetup->m_SysPara.m_bPara[1];

		m_4 = pdlg->m_dlgSetup->m_SysPara.m_bPara[4];
		m_5 = pdlg->m_dlgSetup->m_SysPara.m_bPara[5];
		m_6 = pdlg->m_dlgSetup->m_SysPara.m_bPara[6];
		m_7 = pdlg->m_dlgSetup->m_SysPara.m_bPara[7];
		m_8 = pdlg->m_dlgSetup->m_SysPara.m_bPara[8];
		m_9 = pdlg->m_dlgSetup->m_SysPara.m_bPara[9];

		m_14 = pdlg->m_dlgSetup->m_SysPara.m_bPara[14];
		m_15 = pdlg->m_dlgSetup->m_SysPara.m_bPara[15];
		m_16 = pdlg->m_dlgSetup->m_SysPara.m_bPara[16];
		m_17 = pdlg->m_dlgSetup->m_SysPara.m_bPara[17];
		m_18 = pdlg->m_dlgSetup->m_SysPara.m_bPara[18];
		m_19 = pdlg->m_dlgSetup->m_SysPara.m_bPara[19];
		m_24 = pdlg->m_dlgSetup->m_SysPara.m_bPara[24];
		m_25 = pdlg->m_dlgSetup->m_SysPara.m_bPara[25];
		m_26 = pdlg->m_dlgSetup->m_SysPara.m_bPara[26];
		///////////////////////////////////////////////

		m_chk_goodsave = pdlg->m_dlgSetup->m_SysPara.m_bPara[30];
		m_chk_ngsave = pdlg->m_dlgSetup->m_SysPara.m_bPara[31];
		m_chk_ngoverlay = pdlg->m_dlgSetup->m_SysPara.m_bPara[32];
		m_chk_NgRstDisp = pdlg->m_dlgSetup->m_SysPara.m_bPara[33];

		GetDlgItem(IDC_DELETE_TERM)->EnableWindow(FALSE);
		if(pdlg->m_dlgSetup->m_SysPara.m_bPara[34])		
		{
			GetDlgItem(IDC_DELETE_TERM)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_DELETE_TERM)->EnableWindow(FALSE);
		}

		UpdateData(FALSE);
	}
	else
	{
		UpdateData(TRUE);

		pdlg->m_dlgSetup->m_SysPara.m_iPara[0] = m_iDeleteTerm;
		pdlg->m_dlgSetup->m_SysPara.m_dPara[0] = m_dPixelSize;
		if(m_iDeleteTerm > 28)
		{
			AfxMessageBox("저장 가능 기간은 최대 28일입니다.");
			m_iDeleteTerm = 28;
		}
		
		//디버그 필요
		/*
		switch(pdlg->m_VisMod->GetVisIndex())
		{
		case VISION_CAM_TOP1:	// 상부#1 고정 검사 항목
			pdlg->m_dlgSetup->m_SysPara.m_bPara[3] = TRUE;		//Potting
			pdlg->m_dlgSetup->m_SysPara.m_bPara[10] = TRUE;		//Film 수지
			pdlg->m_dlgSetup->m_SysPara.m_bPara[11] = TRUE;		//Film PF/찢김
			pdlg->m_dlgSetup->m_SysPara.m_bPara[12] = TRUE;		//Punch burr 편집불량
			pdlg->m_dlgSetup->m_SysPara.m_bPara[13] = TRUE;		//Film 금속성 이물

			pdlg->m_dlgSetup->m_SysPara.m_bPara[0] = FALSE;		//Chip out
			pdlg->m_dlgSetup->m_SysPara.m_bPara[2] = FALSE;		//Chip 이면수지묻음 Broken
			pdlg->m_dlgSetup->m_SysPara.m_bPara[27] = FALSE;		//기포, 잉크칩
			pdlg->m_dlgSetup->m_SysPara.m_bPara[28] = FALSE;		//구겨짐, 찍힘
			pdlg->m_dlgSetup->m_SysPara.m_bPara[29] = FALSE;		//구겨짐, 찍힘

			break;
		case VISION_CAM_BOTTOM1:	// 상부#2 고정 검사 항목
			pdlg->m_dlgSetup->m_SysPara.m_bPara[0] = TRUE;		//Chip out
			pdlg->m_dlgSetup->m_SysPara.m_bPara[2] = TRUE;		//Chip 이면수지묻음 Broken

			pdlg->m_dlgSetup->m_SysPara.m_bPara[3] = FALSE;		//Potting
			pdlg->m_dlgSetup->m_SysPara.m_bPara[10] = FALSE;		//Film 수지
			pdlg->m_dlgSetup->m_SysPara.m_bPara[11] = FALSE;		//Film PF/찢김
			pdlg->m_dlgSetup->m_SysPara.m_bPara[12] = FALSE;		//Punch burr 편집불량
			pdlg->m_dlgSetup->m_SysPara.m_bPara[13] = FALSE;		//Film 금속성 이물
			pdlg->m_dlgSetup->m_SysPara.m_bPara[27] = FALSE;		//기포, 잉크칩
			pdlg->m_dlgSetup->m_SysPara.m_bPara[28] = FALSE;		//구겨짐, 찍힘
			pdlg->m_dlgSetup->m_SysPara.m_bPara[29] = FALSE;		//구겨짐, 찍힘

			break;
		case VISION_CAM_TOP2:	// 하부#1 고정 검사 항목
			pdlg->m_dlgSetup->m_SysPara.m_bPara[27] = TRUE;		//기포, 잉크칩
			pdlg->m_dlgSetup->m_SysPara.m_bPara[28] = TRUE;		//구겨짐, 찍힘
			pdlg->m_dlgSetup->m_SysPara.m_bPara[29] = TRUE;		//구겨짐, 찍힘

			pdlg->m_dlgSetup->m_SysPara.m_bPara[0] = FALSE;		//Chip out
			pdlg->m_dlgSetup->m_SysPara.m_bPara[2] = FALSE;		//Chip 이면수지묻음 Broken
			pdlg->m_dlgSetup->m_SysPara.m_bPara[3] = FALSE;		//Potting
			pdlg->m_dlgSetup->m_SysPara.m_bPara[10] = FALSE;		//Film 수지
			pdlg->m_dlgSetup->m_SysPara.m_bPara[11] = FALSE;		//Film PF/찢김
			pdlg->m_dlgSetup->m_SysPara.m_bPara[12] = FALSE;		//Punch burr 편집불량
			pdlg->m_dlgSetup->m_SysPara.m_bPara[13] = FALSE;		//Film 금속성 이물
			break;
		case VISION_CAM_BOTTOM2:

			break;
		default:
			AfxMessageBox("Invalid Index ! CSetUpSysDlg::Update(BOOL)");
			break;
		}
		*/

		pdlg->m_dlgSetup->m_SysPara.m_bPara[0] = m_0;		//Chip out
		pdlg->m_dlgSetup->m_SysPara.m_bPara[2] = m_2;		//Chip 이면수지묻음 Broken
		pdlg->m_dlgSetup->m_SysPara.m_bPara[3] = m_3;		//Potting
		pdlg->m_dlgSetup->m_SysPara.m_bPara[10] = m_10;		//Film 수지
		pdlg->m_dlgSetup->m_SysPara.m_bPara[11] = m_11;		//Film PF/찢김
		pdlg->m_dlgSetup->m_SysPara.m_bPara[12] = m_12;		//Punch burr 편집불량
		pdlg->m_dlgSetup->m_SysPara.m_bPara[13] = m_13;		//Film 금속성 이물
		// 상부#1 선택 검사 항목
		pdlg->m_dlgSetup->m_SysPara.m_bPara[25] = m_25;	//M/K 불량

		// 상부#2 선택 검사 항목
		pdlg->m_dlgSetup->m_SysPara.m_bPara[15] = m_15;	//AM #1
		pdlg->m_dlgSetup->m_SysPara.m_bPara[16] = m_16;	//AM #2
		// 상부#1#2 선택 검사 항목
		pdlg->m_dlgSetup->m_SysPara.m_bPara[20] = m_20;	//Out lead#1
		pdlg->m_dlgSetup->m_SysPara.m_bPara[21] = m_21;	//Out lead#1
		pdlg->m_dlgSetup->m_SysPara.m_bPara[22] = m_22;	//In lead#1
		pdlg->m_dlgSetup->m_SysPara.m_bPara[23] = m_23;	//In lead#2

		pdlg->m_dlgSetup->m_SysPara.m_bPara[27] = m_27;		//기포, 잉크칩
		pdlg->m_dlgSetup->m_SysPara.m_bPara[28] = m_28;		//구겨짐, 찍힘
		pdlg->m_dlgSetup->m_SysPara.m_bPara[29] = m_29;		//구겨짐, 찍힘

		////////////////////////////////////////////////////
		//스페어 파라메터
		pdlg->m_dlgSetup->m_SysPara.m_bPara[1] = m_1;

		pdlg->m_dlgSetup->m_SysPara.m_bPara[4] = m_4;
		pdlg->m_dlgSetup->m_SysPara.m_bPara[5] = m_5; 
		pdlg->m_dlgSetup->m_SysPara.m_bPara[6] = m_6; 
		pdlg->m_dlgSetup->m_SysPara.m_bPara[7] = m_7; 
		pdlg->m_dlgSetup->m_SysPara.m_bPara[8] = m_8; 
		pdlg->m_dlgSetup->m_SysPara.m_bPara[9] = m_9; 

		pdlg->m_dlgSetup->m_SysPara.m_bPara[14] = m_14;
		pdlg->m_dlgSetup->m_SysPara.m_bPara[17] = m_17;
		pdlg->m_dlgSetup->m_SysPara.m_bPara[18] = m_18;
		pdlg->m_dlgSetup->m_SysPara.m_bPara[19] = m_19;
		pdlg->m_dlgSetup->m_SysPara.m_bPara[24] = m_24;
		pdlg->m_dlgSetup->m_SysPara.m_bPara[26] = m_26;
		/////////////////////////////////////////////////////

		
		pdlg->m_dlgSetup->m_SysPara.m_bPara[30] = m_chk_goodsave;
		pdlg->m_dlgSetup->m_SysPara.m_bPara[31] = m_chk_ngsave;
		pdlg->m_dlgSetup->m_SysPara.m_bPara[32] = m_chk_ngoverlay;
		pdlg->m_dlgSetup->m_SysPara.m_bPara[33] = m_chk_NgRstDisp;
		pdlg->m_dlgSetup->m_SysPara.m_bPara[34] = m_chk_InspParamLock;
	}
}
void CSetUpSysDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	if(bShow)
	{
		m_bShow = TRUE;
		Update(TRUE);
	}
	else
	{
		m_bShow = FALSE;
		Update(FALSE);
	}

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

HBRUSH CSetUpSysDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
