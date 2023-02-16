// DialogCharRgnSet.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "FilmIns.h"

#include "UserSockClient.h"
#include "include/_FuncModuleHal.h"
#include "include/_FuncModuleBasic.h"
#include "include/_VisionModuleFilmIns.h"
#include "include/_CtrlImageRect.h"
#include "include/_CtrlModelManage.h"

#include "DialogCharRgnSet.h"

#include "SetUpBsaeDlg.h"
#include "DialogProcessing.h"
#include "FilmInsDlg.h"


// CDialogCharRgnSet 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogCharRgnSet, CDialog)

CDialogCharRgnSet::CDialogCharRgnSet(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogCharRgnSet::IDD, pParent)
{

}

CDialogCharRgnSet::~CDialogCharRgnSet()
{
}

void CDialogCharRgnSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogCharRgnSet, CDialog)
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDialogCharRgnSet 메시지 처리기입니다.
BEGIN_EVENTSINK_MAP(CDialogCharRgnSet, CDialog)
	ON_EVENT(CDialogCharRgnSet, IDC_BTNENHCTRL_CHAR_REGION_SET, DISPID_CLICK, CDialogCharRgnSet::ClickBtnenhctrlCharRegionSet, VTS_NONE)
	ON_EVENT(CDialogCharRgnSet, IDC_BTNENHCTRL_CHAR_REGION_MERGE, DISPID_CLICK, CDialogCharRgnSet::ClickBtnenhctrlCharRegionMerge, VTS_NONE)
	ON_EVENT(CDialogCharRgnSet, IDC_BTNENHCTRL_CHAR_REGION_SEPERATE, DISPID_CLICK, CDialogCharRgnSet::ClickBtnenhctrlCharRegionSeperate, VTS_NONE)
END_EVENTSINK_MAP()

void CDialogCharRgnSet::ClickBtnenhctrlCharRegionSet()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	pdlg->m_dlgSetup->m_SetUpOCVDlg.RegionSet();
}

void CDialogCharRgnSet::ClickBtnenhctrlCharRegionMerge()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	pdlg->m_dlgSetup->m_SetUpOCVDlg.RegionMerge();
}

void CDialogCharRgnSet::ClickBtnenhctrlCharRegionSeperate()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	pdlg->m_dlgSetup->m_SetUpOCVDlg.RegionSeperate();
}

void CDialogCharRgnSet::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	pdlg->m_dlgImgRect[pdlg->m_dlgSetup->m_iRectIdx]->DeleteAll();
	pdlg->m_dlgImgRect[pdlg->m_dlgSetup->m_iRectIdx]->SetActiveTracker(TRUE);

	CDialog::OnClose();
}

HBRUSH CDialogCharRgnSet::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

    //pDC->SetTextColor(RGB(255, 255, 255));    
	//pDC->SetBkColor(RGB(0, 0, 0));

	//return (HBRUSH)pdlg->m_BrushBlack;	

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}
