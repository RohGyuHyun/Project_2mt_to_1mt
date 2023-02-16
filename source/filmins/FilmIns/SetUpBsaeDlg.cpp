// SetUpBsaeDlg.cpp : 구현 파일입니다.
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


// CSetUpBsaeDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSetUpBsaeDlg, CDialog)

CSetUpBsaeDlg::CSetUpBsaeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetUpBsaeDlg::IDD, pParent)
	, m_cof_num(0)
{
	m_iRectIdx = 0;
	m_iSubIdx = 0;
	m_cof_pfpitch = COF_PF_PITCH;
}

CSetUpBsaeDlg::~CSetUpBsaeDlg()
{
}

void CSetUpBsaeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_Tree);
	DDX_Text(pDX, IDC_EDIT_COF_NUM, m_cof_num);
	DDX_Control(pDX, IDC_COF_WIDTH, m_ctrlCOFWidth);
	DDX_Text(pDX, IDC_EDIT_COF_PFPITCH, m_cof_pfpitch);
	DDX_Control(pDX, IDC_PF_CNT, m_ctrlPFCnt);
}


BEGIN_MESSAGE_MAP(CSetUpBsaeDlg, CDialog)
	ON_WM_CLOSE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CSetUpBsaeDlg::OnTvnSelchangedTree1)
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE1, &CSetUpBsaeDlg::OnTvnSelchangingTree1)
	ON_WM_CTLCOLOR()
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_PF_CNT, &CSetUpBsaeDlg::OnCbnSelchangePfCnt)
	ON_CBN_SELCHANGE(IDC_COF_WIDTH, &CSetUpBsaeDlg::OnCbnSelchangeCofWidth)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CSetUpBsaeDlg 메시지 처리기입니다.
BEGIN_EVENTSINK_MAP(CSetUpBsaeDlg, CDialog)
	ON_EVENT(CSetUpBsaeDlg, IDC_BTNENHCTRL_SETUP_BASE_OK, DISPID_CLICK, CSetUpBsaeDlg::ClickBtnenhctrlSetupBaseOk, VTS_NONE)
	ON_EVENT(CSetUpBsaeDlg, IDC_BTNENHCTRL_SETUP_BASE_APPLYALL, DISPID_CLICK, CSetUpBsaeDlg::ClickBtnenhctrlSetupBaseApplyall, VTS_NONE)
	ON_EVENT(CSetUpBsaeDlg, IDC_BTNENHCTRL_SETUP_BASE_APPLYALL_SAVE, DISPID_CLICK, CSetUpBsaeDlg::ClickBtnenhctrlSetupBaseApplyallSave, VTS_NONE)
END_EVENTSINK_MAP()


BOOL CSetUpBsaeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	// TODO: Add extra initialization here
	CImageList* pInfoImages = new CImageList;
	pInfoImages->Create(16, 16, ILC_COLOR8 | ILC_MASK, 2, 0);
	pInfoImages->Add(AfxGetApp()->LoadIcon(IDI_ICON1));
	pInfoImages->Add(AfxGetApp()->LoadIcon(IDI_ICON2));

	m_Tree.SetImageList(pInfoImages, TVSIL_NORMAL);

	HTREEITEM hTreeItem1;

	hTreeItem1 = m_Tree.InsertItem("VISION", 0, 1, TVI_ROOT, TVI_LAST);
	m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpSysDlg);

	hTreeItem1 = m_Tree.InsertItem("(상부)ALIGNMENT", 0, 1, TVI_ROOT, TVI_LAST);
	m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpAlignDlg);

	//20150223 by
		switch(pdlg->m_VisMod->GetVisIndex())
	{
	case VISION_CAM_TOP1:
		hTreeItem1 = m_Tree.InsertItem("(상부)CHIP", 0, 1, TVI_ROOT, TVI_LAST);
		m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpChipDlg);

		hTreeItem1 = m_Tree.InsertItem("(상부)FILM", 0, 1, TVI_ROOT, TVI_LAST);
		m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpFilmDlg);

		hTreeItem1 = m_Tree.InsertItem("(상부)LEAD", 0, 1, TVI_ROOT, TVI_LAST);
		m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpLeadDlg);

		hTreeItem1 = m_Tree.InsertItem("(상부)M/K불량", 0, 1, TVI_ROOT, TVI_LAST);
		m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpOCVDlg);
		break;
	case VISION_CAM_TOP2:
		hTreeItem1 = m_Tree.InsertItem("(상부)CHIP", 0, 1, TVI_ROOT, TVI_LAST);
		m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpChipDlg);

		hTreeItem1 = m_Tree.InsertItem("(상부)LEAD", 0, 1, TVI_ROOT, TVI_LAST);
		m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpLeadDlg);
		break;
	case VISION_CAM_BOTTOM:
		hTreeItem1 = m_Tree.InsertItem("(하부)PI", 0, 1, TVI_ROOT, TVI_LAST);
		m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpSRDlg);
		break;
	default:
		AfxMessageBox("Error : invalid index(CSetUpBsaeDlg::OnInitDialog)!");
		break;
	}
	/*
	switch(pdlg->m_VisMod->GetVisIndex())
	{
	case VISION_CAM_TOP1:
		hTreeItem1 = m_Tree.InsertItem("(상부)CHIP", 0, 1, TVI_ROOT, TVI_LAST);
		m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpChipDlg);

		hTreeItem1 = m_Tree.InsertItem("(상부)FILM", 0, 1, TVI_ROOT, TVI_LAST);
		m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpFilmDlg);

		hTreeItem1 = m_Tree.InsertItem("(상부)LEAD", 0, 1, TVI_ROOT, TVI_LAST);
		m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpLeadDlg);

		hTreeItem1 = m_Tree.InsertItem("(상부)M/K불량", 0, 1, TVI_ROOT, TVI_LAST);
		m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpOCVDlg);

		//hTreeItem1 = m_Tree.InsertItem("(하부)SR", 0, 1, TVI_ROOT, TVI_LAST);
		//m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpSRDlg);
		break;
	case VISION_CAM_BOTTOM1:
		//hTreeItem1 = m_Tree.InsertItem("(상부)CHIP", 0, 1, TVI_ROOT, TVI_LAST);
		//m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpChipDlg);

		hTreeItem1 = m_Tree.InsertItem("(하부)FILM", 0, 1, TVI_ROOT, TVI_LAST);
		m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpFilmDlg);

		hTreeItem1 = m_Tree.InsertItem("(하부)LEAD", 0, 1, TVI_ROOT, TVI_LAST);
		m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpLeadDlg);

		//hTreeItem1 = m_Tree.InsertItem("(상부)인쇄불량", 0, 1, TVI_ROOT, TVI_LAST);
		//m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpOCVDlg);

		//hTreeItem1 = m_Tree.InsertItem("(하부)SR", 0, 1, TVI_ROOT, TVI_LAST);
		//m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpSRDlg);
		break;
	case VISION_CAM_TOP2:
		hTreeItem1 = m_Tree.InsertItem("(상부)CHIP", 0, 1, TVI_ROOT, TVI_LAST);
		m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpChipDlg);

		//hTreeItem1 = m_Tree.InsertItem("(상부)FILM", 0, 1, TVI_ROOT, TVI_LAST);
		//m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpFilmDlg);

		hTreeItem1 = m_Tree.InsertItem("(상부)LEAD", 0, 1, TVI_ROOT, TVI_LAST);
		m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpLeadDlg);

		//hTreeItem1 = m_Tree.InsertItem("(상부)인쇄불량", 0, 1, TVI_ROOT, TVI_LAST);
		//m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpOCVDlg);

		//hTreeItem1 = m_Tree.InsertItem("(하부)PI", 0, 1, TVI_ROOT, TVI_LAST);
		//m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpSRDlg);
		break;
	case VISION_CAM_BOTTOM2:
		//hTreeItem1 = m_Tree.InsertItem("(상부)CHIP", 0, 1, TVI_ROOT, TVI_LAST);
		//m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpChipDlg);

		//hTreeItem1 = m_Tree.InsertItem("(상부)FILM", 0, 1, TVI_ROOT, TVI_LAST);
		//m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpFilmDlg);

		hTreeItem1 = m_Tree.InsertItem("(하부)LEAD", 0, 1, TVI_ROOT, TVI_LAST);
		m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpLeadDlg);

		//hTreeItem1 = m_Tree.InsertItem("(상부)인쇄불량", 0, 1, TVI_ROOT, TVI_LAST);
		//m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpOCVDlg);

		hTreeItem1 = m_Tree.InsertItem("(하부)PI", 0, 1, TVI_ROOT, TVI_LAST);
		m_Tree.SetItemData(hTreeItem1, (DWORD)&m_SetUpSRDlg);

		break;
	default:
		AfxMessageBox("Error : invalid index(CSetUpBsaeDlg::OnInitDialog)!");
		break;
	}
	*/

	m_SetUpSysDlg.Create(IDD_SETUP_SYS_DIALOG, this);
	m_SetUpAlignDlg.Create(IDD_SETUP_ALIGN_DIALOG, this);
	m_SetUpOCVDlg.Create(IDD_SETUP_OCV_DIALOG, this);
	m_SetUpChipDlg.Create(IDD_SETUP_CHIP_DIALOG, this);
	m_SetUpFilmDlg.Create(IDD_SETUP_FILM_DIALOG, this);
	m_SetUpLeadDlg.Create(IDD_SETUP_LEAD_DIALOG, this);
	m_SetUpSRDlg.Create(IDD_SETUP_SR_DIALOG, this);

	CRect rcDlgs;
	GetDlgItem(IDC_DLG_AREA)->GetWindowRect(rcDlgs);
	ScreenToClient(rcDlgs);

	rcDlgs.top += 12;
	rcDlgs.bottom -= 10;
	rcDlgs.left += 5;
	rcDlgs.right -= 5;

	m_SetUpSysDlg.MoveWindow(rcDlgs);  
	m_SetUpAlignDlg.MoveWindow(rcDlgs);  
	m_SetUpOCVDlg.MoveWindow(rcDlgs);
	m_SetUpChipDlg.MoveWindow(rcDlgs); 
	m_SetUpFilmDlg.MoveWindow(rcDlgs); 
	m_SetUpLeadDlg.MoveWindow(rcDlgs);  
	m_SetUpSRDlg.MoveWindow(rcDlgs);

	m_SubRectPos = rcDlgs;

	m_bDlgCreated=TRUE;
	
	SetWindowPos(&wndTopMost, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE); 

	m_ctrlPFCnt.InsertString(0, "2 PF");
	m_ctrlPFCnt.InsertString(1, "3 PF");
	m_ctrlPFCnt.InsertString(2, "4 PF");
	m_ctrlPFCnt.InsertString(3, "5 PF");
	m_ctrlPFCnt.InsertString(4, "6 PF");
	m_ctrlPFCnt.InsertString(5, "7 PF");
	m_ctrlPFCnt.InsertString(6, "8 PF");
	m_ctrlPFCnt.InsertString(7, "9 PF");
	m_ctrlPFCnt.InsertString(8, "10 PF");
	m_ctrlCOFWidth.InsertString(0, "35 mm");
	m_ctrlCOFWidth.InsertString(1, "48 mm");
	m_ctrlCOFWidth.InsertString(2, "70 mm");
	
	m_InsPara = pdlg->m_VisMod->GetInspectPara(0);
	m_SysPara = pdlg->m_VisMod->GetSystemPara(pdlg->m_RectIdx);

	delete pInfoImages;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CSetUpBsaeDlg::ClickBtnenhctrlSetupBaseOk()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	ClickBtnenhctrlSetupBaseApplyall();

	ShowWindow(SW_HIDE);

	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	//pdlg->ChangeStateLightValueEditBox();

	pdlg->m_btnSetup.SetValue(FALSE);

	return;

	//OnOK();
}

void CSetUpBsaeDlg::ClickBtnenhctrlSetupBaseApplyall()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	if(m_SetUpSysDlg.m_bShow)
	{
		m_SetUpSysDlg.Update(FALSE);
	}
	if(m_SetUpAlignDlg.m_bShow)
	{
		m_SetUpAlignDlg.Update(FALSE);
	}
	if(m_SetUpChipDlg.m_bShow)
	{
		m_SetUpChipDlg.Update(FALSE);
	}
	if(m_SetUpFilmDlg.m_bShow)
	{
		m_SetUpFilmDlg.Update(FALSE);
	}
	if(m_SetUpLeadDlg.m_bShow)
	{
		m_SetUpLeadDlg.Update(FALSE);
	}
	if(m_SetUpOCVDlg.m_bShow)
	{
		m_SetUpOCVDlg.Update(FALSE);
	}
	if(m_SetUpSRDlg.m_bShow)
	{
		m_SetUpSRDlg.Update(FALSE);
	}

	Update(FALSE);
	//OK
	//Set inspection result
	for(int i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		copy_obj(pdlg->m_dlgSetup->m_HalInsRgn[i][0], pdlg->m_VisMod->GetHalInspectRgn(0, i, 0), 1, -1);//SR region
		copy_obj(pdlg->m_dlgSetup->m_HalInsRgn[i][1], pdlg->m_VisMod->GetHalInspectRgn(0, i, 1), 1, -1);//PF hall region
		copy_obj(pdlg->m_dlgSetup->m_HalInsRgn[i][2], pdlg->m_VisMod->GetHalInspectRgn(0, i, 2), 1, -1);//up lead region
		copy_obj(pdlg->m_dlgSetup->m_HalInsRgn[i][3], pdlg->m_VisMod->GetHalInspectRgn(0, i, 3), 1, -1);//dn lead region
		copy_obj(pdlg->m_dlgSetup->m_HalInsRgn[i][4], pdlg->m_VisMod->GetHalInspectRgn(0, i, 4), 1, -1);//user area region
		copy_obj(pdlg->m_dlgSetup->m_HalInsRgn[i][5], pdlg->m_VisMod->GetHalInspectRgn(0, i, 5), 1, -1);//SR out region
		copy_obj(pdlg->m_dlgSetup->m_HalInsRgn[i][6], pdlg->m_VisMod->GetHalInspectRgn(0, i, 6), 1, -1);//vaccume region
		copy_obj(pdlg->m_dlgSetup->m_HalInsRgn[i][7], pdlg->m_VisMod->GetHalInspectRgn(0, i, 7), 1, -1);//chip region
		copy_obj(pdlg->m_dlgSetup->m_HalInsRgn[i][8], pdlg->m_VisMod->GetHalInspectRgn(0, i, 8), 1, -1);//AM0
		copy_obj(pdlg->m_dlgSetup->m_HalInsRgn[i][9], pdlg->m_VisMod->GetHalInspectRgn(0, i, 9), 1, -1);//AM0
		copy_obj(pdlg->m_dlgSetup->m_HalInsRgn[i][10], pdlg->m_VisMod->GetHalInspectRgn(0, i, 10), 1, -1);//AM1
		copy_obj(pdlg->m_dlgSetup->m_HalInsRgn[i][11], pdlg->m_VisMod->GetHalInspectRgn(0, i, 11), 1, -1);//AM1
	}
	//Set data
	pdlg->m_VisMod->SetInspectPara(0, m_InsPara);
	pdlg->m_VisMod->SetSystemPara(m_iRectIdx, m_SysPara);

	//SR bubble검사용 copare ID를 갱신
	//pdlg->m_VisMod->UpdateSRBubPrepare();

	//티칭 Region 위치 체크
	int FILM_TOP, FILM_BOTTOM, FILM_LEFT, FILM_RIGHT;
	double dFilmLength_mm, dFilmLength;
	CString strTemp;
	//ins_para.m_iParaBase[2]: PF 수량, ins_para.m_dParaBase[1]: PF 간격(4.75mm)
	dFilmLength_mm =pdlg->m_dlgSetup->m_InsPara.m_iParaBase[2] * pdlg->m_dlgSetup->m_InsPara.m_dParaBase[1]; 
	if(0 < pdlg->m_dlgSetup->m_SysPara.m_dPara[0])	// m_SysPara.m_dPara[0]: Pixel Size
		dFilmLength = ((dFilmLength_mm * 1000) / pdlg->m_dlgSetup->m_SysPara.m_dPara[0]); 
	else
		dFilmLength = 0;

	FILM_TOP = (HEIGHT - (dFilmLength * pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0])) * 0.5;		//m_iParaBase[0]: COF 개수
	FILM_BOTTOM = FILM_TOP + dFilmLength;
	FILM_LEFT = (WIDTH - (pdlg->m_dlgSetup->m_InsPara.m_dParaBase[2] / PIXEL_SIZE)) * 0.5 + 4;	// 2 pixel margin
	FILM_RIGHT = FILM_LEFT + (pdlg->m_dlgSetup->m_InsPara.m_dParaBase[2] / PIXEL_SIZE) - 8;
	
}

void CSetUpBsaeDlg::ClickBtnenhctrlSetupBaseApplyallSave()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	CString strTemp;
	
	pdlg->m_dlgSetup->m_SysPara.m_bPara[34] = false;		

	ClickBtnenhctrlSetupBaseApplyall();

	pdlg->ModelSave();

	pdlg->m_dlgSetup->m_SysPara = pdlg->m_VisMod->GetSystemPara(0);
	pdlg->m_dlgSetup->m_InsPara = pdlg->m_VisMod->GetInspectPara(0);

	if(m_SetUpSysDlg.m_bShow)		
	{
		m_SetUpSysDlg.Update(TRUE);
	}
	if(m_SetUpAlignDlg.m_bShow)
	{
		m_SetUpAlignDlg.Update(TRUE);
	}
	if(m_SetUpChipDlg.m_bShow)
	{
		m_SetUpChipDlg.Update(TRUE);
	}
	if(m_SetUpFilmDlg.m_bShow)
	{
		m_SetUpFilmDlg.Update(TRUE);
	}
	if(m_SetUpLeadDlg.m_bShow)
	{
		m_SetUpLeadDlg.Update(TRUE);
	}
	if(m_SetUpOCVDlg.m_bShow)
	{
		m_SetUpOCVDlg.Update(TRUE);
	}
	if(m_SetUpSRDlg.m_bShow)
	{
		m_SetUpSRDlg.Update(TRUE);
	}

	Update(TRUE);
}

void CSetUpBsaeDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	ShowWindow(SW_HIDE);
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	pdlg->m_btnSetup.SetValue(FALSE);

	return;

	CDialog::OnClose();
}

void CSetUpBsaeDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_bDlgCreated)
	{
		HTREEITEM hCurrent = m_Tree.GetSelectedItem();
		if(hCurrent)
			((CPropertyPage*)m_Tree.GetItemData(hCurrent))->ShowWindow(SW_SHOW);
	}
	
	*pResult = 0;
}

void CSetUpBsaeDlg::OnTvnSelchangingTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	if(m_bDlgCreated)
	{
		HTREEITEM hCurrent = m_Tree.GetSelectedItem();
		if(hCurrent)
			((CPropertyPage*)m_Tree.GetItemData(hCurrent))->ShowWindow(SW_HIDE);
	}

	*pResult = 0;
}

HBRUSH CSetUpBsaeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
/*
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

    pDC->SetTextColor(RGB(255, 255, 255));    
	pDC->SetBkColor(RGB(0, 0, 0));

	return (HBRUSH)pdlg->m_BrushBlack;
*/
	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}

void CSetUpBsaeDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	// TODO: Add your message handler code here
	HTREEITEM hCurrent;

	if(bShow)
	{		
		Update(TRUE);

		if(m_bDlgCreated)
		{
			hCurrent = m_Tree.GetSelectedItem();
			if(hCurrent)
				((CPropertyPage*)m_Tree.GetItemData(hCurrent))->ShowWindow(SW_SHOW);
		}
	}
	else
	{
		if(m_bDlgCreated)
		{
			hCurrent = m_Tree.GetSelectedItem();
			if(hCurrent)
				((CPropertyPage*)m_Tree.GetItemData(hCurrent))->ShowWindow(SW_HIDE);
		}
	}
}


void CSetUpBsaeDlg::Update(BOOL flag)
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateComBox(pdlg->m_dlgSetup->m_InsPara.m_dParaBase[2], pdlg->m_dlgSetup->m_InsPara.m_iParaBase[2]);
	OnCbnSelchangePfCnt();

	if(flag)
	{
		//m_cof_num = pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0];
		//m_cof_pitch = pdlg->m_dlgSetup->m_InsPara.m_iParaBase[2];
		//m_cof_width = pdlg->m_dlgSetup->m_InsPara.m_dParaBase[2];	

		m_cof_pfpitch = pdlg->m_dlgSetup->m_InsPara.m_dParaBase[1];

		UpdateData(FALSE);
	}
	else
	{
		UpdateData(TRUE);

		pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0] = m_cof_num;


		pdlg->m_dlgSetup->m_InsPara.m_dParaBase[1] = COF_PF_PITCH;

		//pdlg->m_dlgSetup->m_InsPara.m_iParaBase[2] = m_cof_pitch;						
		//pdlg->m_dlgSetup->m_InsPara.m_dParaBase[1] = m_cof_pfpitch;
		//pdlg->m_dlgSetup->m_InsPara.m_dParaBase[2] = m_cof_width;
	}
}

BOOL CSetUpBsaeDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message ==WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return TRUE;
	}


	return CDialog::PreTranslateMessage(pMsg);
}

void CSetUpBsaeDlg::OnCbnSelchangePfCnt()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	switch(m_ctrlPFCnt.GetCurSel())
	{
	case 0:
		pdlg->m_dlgSetup->m_InsPara.m_iParaBase[2] = 2;
		pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0] = 8;		// 110826 ytlee
		break;
	case 1:
		pdlg->m_dlgSetup->m_InsPara.m_iParaBase[2] = 3;
		pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0] = 5;
		break;
	case 2:
		pdlg->m_dlgSetup->m_InsPara.m_iParaBase[2] = 4;
		pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0] = 4;
		break;
	case 3:
		pdlg->m_dlgSetup->m_InsPara.m_iParaBase[2] = 5;
		pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0] = 3;
		break;
	case 4:
		pdlg->m_dlgSetup->m_InsPara.m_iParaBase[2] = 6;
		pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0] = 2;
		break;
	case 5:
		pdlg->m_dlgSetup->m_InsPara.m_iParaBase[2] = 7;
		pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0] = 2;
		break;
	case 6:
		pdlg->m_dlgSetup->m_InsPara.m_iParaBase[2] = 8;
		pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0] = 2;
		break;
	case 7:
		pdlg->m_dlgSetup->m_InsPara.m_iParaBase[2] = 9;
		pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0] = 1;
		break;
	case 8:
		pdlg->m_dlgSetup->m_InsPara.m_iParaBase[2] = 10;
		pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0] = 1;
		break;
	}

	m_cof_num = pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]; // 필름 수

	UpdateData(FALSE);
}

void CSetUpBsaeDlg::OnCbnSelchangeCofWidth()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	switch(m_ctrlCOFWidth.GetCurSel())
	{
		case 0:
			pdlg->m_dlgSetup->m_InsPara.m_dParaBase[2] = 35;
			break;
		case 1:
			pdlg->m_dlgSetup->m_InsPara.m_dParaBase[2] = 48;
			break;
		case 2:
			pdlg->m_dlgSetup->m_InsPara.m_dParaBase[2] = 70;
			break;
	}
}


void CSetUpBsaeDlg::UpdateComBox(int COFWidth, int PFCnt)
{
	switch(COFWidth)
	{
	case 35:
		m_ctrlCOFWidth.SetCurSel(0);
		break;
	case 48:
		m_ctrlCOFWidth.SetCurSel(1);
		break;
	case 70:
		m_ctrlCOFWidth.SetCurSel(2);
		break;
	}
	switch(PFCnt)
	{
	case 2:
		m_ctrlPFCnt.SetCurSel(0);
		break;
	case 3:
		m_ctrlPFCnt.SetCurSel(1);
		break;
	case 4:
		m_ctrlPFCnt.SetCurSel(2);
		break;
	case 5:
		m_ctrlPFCnt.SetCurSel(3);
		break;
	case 6:
		m_ctrlPFCnt.SetCurSel(4);
		break;
	case 7:
		m_ctrlPFCnt.SetCurSel(5);
		break;
	case 8:
		m_ctrlPFCnt.SetCurSel(6);
		break;
	case 9:
		m_ctrlPFCnt.SetCurSel(7);
		break;
	case 10:
		m_ctrlPFCnt.SetCurSel(8);
		break;
	}
}
