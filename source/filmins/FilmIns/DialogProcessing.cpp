// DialogProcessing.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "FilmIns.h"
#include "DialogProcessing.h"


// CDialogProcessing ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDialogProcessing, CDialog)

CDialogProcessing::CDialogProcessing(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogProcessing::IDD, pParent)
{

}

CDialogProcessing::~CDialogProcessing()
{
}

void CDialogProcessing::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_PROCESS, m_progress);
}


BEGIN_MESSAGE_MAP(CDialogProcessing, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDialogProcessing �޽��� ó�����Դϴ�.

BOOL CDialogProcessing::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_progress.SetRange(0, 100);
	m_iStep = 0;
	m_progress.SetPos(m_iStep);

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDialogProcessing::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if(bShow)
	{
		m_iStep = 70;
		m_progress.SetPos(m_iStep);

		SetTimer(0, 100, NULL);
	}
	else
	{
		KillTimer(0);
	}
}

void CDialogProcessing::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if(90 > m_iStep)
	{
		m_iStep += 10;
	}
	else if(100 > m_iStep)
	{
		m_iStep += 1;	
	}
	m_progress.SetPos(m_iStep);
	

	CDialog::OnTimer(nIDEvent);
}
