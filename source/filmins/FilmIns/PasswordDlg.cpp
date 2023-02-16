// PasswordDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "FilmIns.h"
#include "PasswordDlg.h"


// CPasswordDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPasswordDlg, CDialog)

CPasswordDlg::CPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordDlg::IDD, pParent)
	, m_ctrlPassword(_T(""))
	, m_ctrlIdNumber(_T(""))
{

}

CPasswordDlg::~CPasswordDlg()
{
}

void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PASSWORD_INPUT, m_ctrlPassword);
	DDX_Text(pDX, IDC_EDIT_IDNUMBER_INPUT, m_ctrlIdNumber);
}


BEGIN_MESSAGE_MAP(CPasswordDlg, CDialog)
END_MESSAGE_MAP()


// CPasswordDlg 메시지 처리기입니다.
BEGIN_EVENTSINK_MAP(CPasswordDlg, CDialog)
	//ON_EVENT(CPasswordDlg, IDC_BTNENHCTRL_PASSWORD_OK, DISPID_CLICK, CPasswordDlg::ClickBtnenhctrlPasswordOk, VTS_NONE)
	ON_EVENT(CPasswordDlg, IDC_BTNENHCTRL_PASSWORD_CANCEL, DISPID_CLICK, CPasswordDlg::ClickBtnenhctrlPasswordCancel, VTS_NONE)
END_EVENTSINK_MAP()

/*
void CPasswordDlg::ClickBtnenhctrlPasswordOk()
{
	 TODO: 여기에 메시지 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	if(m_ctrlIdNumber == "")
	{
		AfxMessageBox("사번을 입력해 주십시요.");
		return;
	}
	if(m_ctrlIdNumber.GetLength() < 8)
	{
		AfxMessageBox("정상적인 사번을 입력해 주십시요.사번은 8자리여야 합니다.");
		return;
	}
	if(m_ctrlPassword == "")
	{
		AfxMessageBox("암호를 입력해 주십시요.");
		return;
	}

	Log 저장 

	OnOK();
}
*/
void CPasswordDlg::ClickBtnenhctrlPasswordCancel()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	
	OnCancel();
}
