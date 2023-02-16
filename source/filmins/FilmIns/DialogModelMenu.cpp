// DialogModelMenu.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "FilmIns.h"
#include "DialogModelMenu.h"


// CDialogModelMenu 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDialogModelMenu, CDialog)

CDialogModelMenu::CDialogModelMenu(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogModelMenu::IDD, pParent)
{

}

CDialogModelMenu::~CDialogModelMenu()
{
}

void CDialogModelMenu::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogModelMenu, CDialog)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CDialogModelMenu 메시지 처리기입니다.

void CDialogModelMenu::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(tme);
    tme.dwFlags = TME_LEAVE;
    tme.hwndTrack = m_hWnd;
    _TrackMouseEvent(&tme);

	CDialog::OnMouseMove(nFlags, point);
}

void CDialogModelMenu::OnMouseLeave()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CPoint point;
	CRect rect;

	GetWindowRect(&rect);
	GetCursorPos(&point);

	if(!rect.PtInRect(point))
	{
		EndDialog(-1);
	}

	CDialog::OnMouseLeave();
}

void CDialogModelMenu::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CRect rect;

	GetClientRect(&rect);

	MoveWindow(m_Rect.left - m_Rect.Width() - 2, m_Rect.top, rect.Width(), rect.Height());
}

void CDialogModelMenu::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.
	CRect rect;
	CPen pen,*oldPen;
	CBrush brush,*oldBrush;
	
	GetClientRect(&rect);
	
	//빈공간 채우기
	pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	oldPen=dc.SelectObject(&pen);
	brush.CreateSolidBrush(RGB(0, 0, 0));
	oldBrush=dc.SelectObject(&brush);
	
	dc.Rectangle(&rect);
	
	dc.SelectObject(oldPen);
	pen.DeleteObject();
	dc.SelectObject(oldBrush);
	brush.DeleteObject();
}

BOOL CDialogModelMenu::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE) 
		{
			EndDialog(-1);
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
BEGIN_EVENTSINK_MAP(CDialogModelMenu, CDialog)
	ON_EVENT(CDialogModelMenu, IDC_BTNENHCTRL_MODEL_NEW, DISPID_CLICK, CDialogModelMenu::ClickBtnenhctrlModelNew, VTS_NONE)
	ON_EVENT(CDialogModelMenu, IDC_BTNENHCTRL_MODEL_DELETE, DISPID_CLICK, CDialogModelMenu::ClickBtnenhctrlModelDelete, VTS_NONE)
	ON_EVENT(CDialogModelMenu, IDC_BTNENHCTRL_MODEL_SELECT, DISPID_CLICK, CDialogModelMenu::ClickBtnenhctrlModelSelect, VTS_NONE)
END_EVENTSINK_MAP()

void CDialogModelMenu::ClickBtnenhctrlModelNew()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_Ret = 0;
	EndDialog(IDOK);
}

void CDialogModelMenu::ClickBtnenhctrlModelDelete()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_Ret = 1;
	EndDialog(IDOK);
}

void CDialogModelMenu::ClickBtnenhctrlModelSelect()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_Ret = 2;
	EndDialog(IDOK);
}
