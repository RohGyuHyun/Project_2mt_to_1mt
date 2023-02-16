// DialogProcessing.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "FilmIns.h"
#include "DialogProcessing.h"


// CDialogProcessing 대화 상자입니다.

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


// CDialogProcessing 메시지 처리기입니다.

BOOL CDialogProcessing::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_progress.SetRange(0, 100);
	m_iStep = 0;
	m_progress.SetPos(m_iStep);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDialogProcessing::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
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
