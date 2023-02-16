// LightCtrlDlg.cpp : 구현 파일입니다.
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

#include "LightCtrlDlg.h"


// CLightCtrlDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLightCtrlDlg, CDialog)

CLightCtrlDlg::CLightCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLightCtrlDlg::IDD, pParent)
{

}

CLightCtrlDlg::~CLightCtrlDlg()
{
}

void CLightCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_cCh1SliderCtrl);
	DDX_Control(pDX, IDC_SLIDER2, m_cCh2SliderCtrl);
	DDX_Control(pDX, IDC_SLIDER3, m_cCh3SliderCtrl);
	DDX_Control(pDX, IDC_SLIDER4, m_cCh4SliderCtrl);
	DDX_Text(pDX, IDC_CH0_EDIT, m_img0_ch0);
	DDX_Text(pDX, IDC_CH1_EDIT, m_img0_ch1);
	DDX_Text(pDX, IDC_CH2_EDIT, m_img0_ch2);
	DDX_Text(pDX, IDC_CH3_EDIT, m_img0_ch3);
}


BEGIN_MESSAGE_MAP(CLightCtrlDlg, CDialog)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CLightCtrlDlg::OnNMCustomdrawSlider1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER2, &CLightCtrlDlg::OnNMCustomdrawSlider2)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER3, &CLightCtrlDlg::OnNMCustomdrawSlider3)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER4, &CLightCtrlDlg::OnNMCustomdrawSlider4)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CLightCtrlDlg 메시지 처리기입니다.

void CLightCtrlDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	
	char tmp[DAT_STR_LENG];

	sprintf(tmp, "%d", m_cCh1SliderCtrl.GetPos());
	GetDlgItem(IDC_CH0_EDIT)->SetWindowText(tmp);

	pdlg->SetLight(pdlg->m_iLightChIdx[0], m_cCh1SliderCtrl.GetPos());

	*pResult = 0;
}

void CLightCtrlDlg::OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	char tmp[DAT_STR_LENG];

	sprintf(tmp, "%d", m_cCh2SliderCtrl.GetPos());
	GetDlgItem(IDC_CH1_EDIT)->SetWindowText(tmp);

	pdlg->SetLight(pdlg->m_iLightChIdx[1], m_cCh2SliderCtrl.GetPos());

	*pResult = 0;
}

void CLightCtrlDlg::OnNMCustomdrawSlider3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	char tmp[DAT_STR_LENG];

	sprintf(tmp, "%d", m_cCh3SliderCtrl.GetPos());
	GetDlgItem(IDC_CH2_EDIT)->SetWindowText(tmp);

	pdlg->SetLight(pdlg->m_iLightChIdx[2], m_cCh3SliderCtrl.GetPos());
	*pResult = 0;
}

void CLightCtrlDlg::OnNMCustomdrawSlider4(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	char tmp[DAT_STR_LENG];

	sprintf(tmp, "%d", m_cCh4SliderCtrl.GetPos());
	GetDlgItem(IDC_CH3_EDIT)->SetWindowText(tmp);
	
	pdlg->SetLight(pdlg->m_iLightChIdx[3], m_cCh4SliderCtrl.GetPos());

	*pResult = 0;
}

BOOL CLightCtrlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	
	m_cCh1SliderCtrl.SetRange(0,255);
	m_cCh2SliderCtrl.SetRange(0,255);
	m_cCh3SliderCtrl.SetRange(0,255);
	m_cCh4SliderCtrl.SetRange(0,255);
	m_cCh1SliderCtrl.SetPos(m_img0_ch0);
	m_cCh2SliderCtrl.SetPos(m_img0_ch1);
	m_cCh3SliderCtrl.SetPos(m_img0_ch2);
	m_cCh4SliderCtrl.SetPos(m_img0_ch3);

	//20150223 by
	switch(pdlg->m_VisMod->GetVisIndex())
	{
		case VISION_CAM_TOP1:
			GetDlgItem(IDC_STATIC_LABEL_CH_1)->SetWindowText("사이드");
			GetDlgItem(IDC_STATIC_LABEL_CH_2)->SetWindowText("수직사각");
			GetDlgItem(IDC_STATIC_LABEL_CH_3)->SetWindowText("동축");
			GetDlgItem(IDC_STATIC_LABEL_CH_4)->SetWindowText("백라이트");
			GetDlgItem(IDC_STATIC_LABEL_CH_4)->EnableWindow(TRUE);
			m_cCh4SliderCtrl.EnableWindow(TRUE);
			GetDlgItem(IDC_CH3_EDIT)->EnableWindow(TRUE);
			break;
		case VISION_CAM_TOP2:
			GetDlgItem(IDC_STATIC_LABEL_CH_1)->SetWindowText("사이드");
			GetDlgItem(IDC_STATIC_LABEL_CH_2)->SetWindowText("수직사각");
			GetDlgItem(IDC_STATIC_LABEL_CH_3)->SetWindowText("동축");
			GetDlgItem(IDC_STATIC_LABEL_CH_4)->SetWindowText("-");
			GetDlgItem(IDC_STATIC_LABEL_CH_4)->EnableWindow(FALSE);
			m_cCh4SliderCtrl.EnableWindow(FALSE);
			GetDlgItem(IDC_CH3_EDIT)->EnableWindow(FALSE);
			break;
		case VISION_CAM_BOTTOM:
			GetDlgItem(IDC_STATIC_LABEL_CH_1)->SetWindowText("백라이트");
			GetDlgItem(IDC_STATIC_LABEL_CH_2)->SetWindowText("동축#1");
			GetDlgItem(IDC_STATIC_LABEL_CH_3)->SetWindowText("동축#2");
			GetDlgItem(IDC_STATIC_LABEL_CH_4)->SetWindowText("-");
			GetDlgItem(IDC_STATIC_LABEL_CH_4)->EnableWindow(FALSE);
			m_cCh4SliderCtrl.EnableWindow(FALSE);
			GetDlgItem(IDC_CH3_EDIT)->EnableWindow(FALSE);
			break;
		default:

			break;
	}
	/*
	switch(pdlg->m_VisMod->GetVisIndex())
	{
		case VISION_CAM_TOP1:
			GetDlgItem(IDC_STATIC_LABEL_CH_1)->SetWindowText("사이드바");
			GetDlgItem(IDC_STATIC_LABEL_CH_2)->SetWindowText("백라이트");
			GetDlgItem(IDC_STATIC_LABEL_CH_3)->SetWindowText("수직사각");
			GetDlgItem(IDC_STATIC_LABEL_CH_4)->SetWindowText("동축");			
			break;
		case VISION_CAM_BOTTOM1:
			GetDlgItem(IDC_STATIC_LABEL_CH_1)->SetWindowText("사이드바");
			GetDlgItem(IDC_STATIC_LABEL_CH_2)->SetWindowText("백라이트");
			GetDlgItem(IDC_STATIC_LABEL_CH_3)->SetWindowText("수직사각");
			GetDlgItem(IDC_STATIC_LABEL_CH_4)->SetWindowText("동축");	
			break;
		case VISION_CAM_TOP2:
			GetDlgItem(IDC_STATIC_LABEL_CH_1)->SetWindowText("사이드바");
			GetDlgItem(IDC_STATIC_LABEL_CH_2)->SetWindowText("수직사각");
			GetDlgItem(IDC_STATIC_LABEL_CH_3)->SetWindowText("동축");
			GetDlgItem(IDC_STATIC_LABEL_CH_4)->SetWindowText("-");
			GetDlgItem(IDC_STATIC_LABEL_CH_4)->EnableWindow(FALSE);
			m_cCh4SliderCtrl.EnableWindow(FALSE);
			GetDlgItem(IDC_CH3_EDIT)->EnableWindow(FALSE);
			break;
		case VISION_CAM_BOTTOM2:
			GetDlgItem(IDC_STATIC_LABEL_CH_1)->SetWindowText("동축#1");
			GetDlgItem(IDC_STATIC_LABEL_CH_2)->SetWindowText("동축#2");
			GetDlgItem(IDC_STATIC_LABEL_CH_3)->SetWindowText("사이드바");
			GetDlgItem(IDC_STATIC_LABEL_CH_4)->SetWindowText("-");
			GetDlgItem(IDC_STATIC_LABEL_CH_4)->EnableWindow(FALSE);
			m_cCh4SliderCtrl.EnableWindow(FALSE);
			GetDlgItem(IDC_CH3_EDIT)->EnableWindow(FALSE);
			break;
		default:

			break;
	}
	*/

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CLightCtrlDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	EndDialog(0);
	CDialog::OnClose();
}
BEGIN_EVENTSINK_MAP(CLightCtrlDlg, CDialog)
	ON_EVENT(CLightCtrlDlg, IDC_BTNENHCTRL_LIGHT_OK, DISPID_CLICK, CLightCtrlDlg::ClickBtnenhctrlLightOk, VTS_NONE)
	ON_EVENT(CLightCtrlDlg, IDC_BTNENHCTRL_LIGHT_CANCEL, DISPID_CLICK, CLightCtrlDlg::ClickBtnenhctrlLightCancel, VTS_NONE)
END_EVENTSINK_MAP()

void CLightCtrlDlg::ClickBtnenhctrlLightOk()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_img0_ch0 = m_cCh1SliderCtrl.GetPos();
	m_img0_ch1 = m_cCh2SliderCtrl.GetPos();
	m_img0_ch2 = m_cCh3SliderCtrl.GetPos();
	m_img0_ch3 = m_cCh4SliderCtrl.GetPos();
	
	OnOK();
}

void CLightCtrlDlg::ClickBtnenhctrlLightCancel()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	pdlg->SetLight(pdlg->m_iLightChIdx[0], m_img0_ch0);
	pdlg->SetLight(pdlg->m_iLightChIdx[1], m_img0_ch1);
	pdlg->SetLight(pdlg->m_iLightChIdx[2], m_img0_ch2);
	pdlg->SetLight(pdlg->m_iLightChIdx[3], m_img0_ch3);

	OnCancel();
}
