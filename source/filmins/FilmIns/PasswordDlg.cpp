// PasswordDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "FilmIns.h"
#include "PasswordDlg.h"


// CPasswordDlg ��ȭ �����Դϴ�.

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


// CPasswordDlg �޽��� ó�����Դϴ�.
BEGIN_EVENTSINK_MAP(CPasswordDlg, CDialog)
	//ON_EVENT(CPasswordDlg, IDC_BTNENHCTRL_PASSWORD_OK, DISPID_CLICK, CPasswordDlg::ClickBtnenhctrlPasswordOk, VTS_NONE)
	ON_EVENT(CPasswordDlg, IDC_BTNENHCTRL_PASSWORD_CANCEL, DISPID_CLICK, CPasswordDlg::ClickBtnenhctrlPasswordCancel, VTS_NONE)
END_EVENTSINK_MAP()

/*
void CPasswordDlg::ClickBtnenhctrlPasswordOk()
{
	 TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);
	if(m_ctrlIdNumber == "")
	{
		AfxMessageBox("����� �Է��� �ֽʽÿ�.");
		return;
	}
	if(m_ctrlIdNumber.GetLength() < 8)
	{
		AfxMessageBox("�������� ����� �Է��� �ֽʽÿ�.����� 8�ڸ����� �մϴ�.");
		return;
	}
	if(m_ctrlPassword == "")
	{
		AfxMessageBox("��ȣ�� �Է��� �ֽʽÿ�.");
		return;
	}

	Log ���� 

	OnOK();
}
*/
void CPasswordDlg::ClickBtnenhctrlPasswordCancel()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	
	OnCancel();
}
