#pragma once


// C_CtrlImageRect ��ȭ �����Դϴ�.
#include "resource.h"
#include "BtnEnh.h"


#define USER_MSG_IMG_RECT			WM_USER+1 
//0 : display
//1 : live on
//2 : live off
//3 : image load
//4 : image save
//5 : gray value
class CDialogMainView;
class AFX_EXT_CLASS C_CtrlImageRect : public CDialog
{
	DECLARE_DYNAMIC(C_CtrlImageRect)

public:
	C_CtrlImageRect(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~C_CtrlImageRect();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = 10000 };

protected:
	CDialogMainView* m_DlgMainView;

	Hlong m_lWindowID;

	long m_lGrayX;
	long m_lGrayY;
	int m_iGray;
	int m_iCamIndex;

	HWND m_hnd;

	CBtnEnh m_Title;
	CBtnEnh m_Coord;
	CBtnEnh m_Gray;
	CBtnEnh m_btnLive;
	CBtnEnh m_btnGray;

public:
	//ImageRectDLL Method
	//Tracker
	void SetActiveTracker(BOOL active);
	
	//void SetActiveTracker(int idx, BOOL active);
	void AddTracker(short type, long left, long top, long right, long bottom, long color, char *cap);
	short GetNum();
	short GetType(short idx);
	BOOL SetTracker(short idx, long left, long top, long right, long bottom);
	BOOL GetTracker(short idx, long *left, long *top, long *right, long *bottom);
	BOOL DeleteTracker(short idx);
	void DeleteAll();

	//Grid
	void SetActiveGrid(BOOL active);
	BOOL GetActiveGrid();
	void SetGridPitch(double pitch);
	double GetGridPitch();

	//Gray
	void SetActiveGray(BOOL active);
	BOOL GetActiveGray();
	void GetGrayPos(long *x, long *y);
	void SetGray(int gray);

	//Zoom
	void SetZoom(long zoom, long width, long height);
	void GetZoom(long *zoom, long *image_width, long *image_height, long *start_x, long *start_y, long *end_x, long *end_y);

	//Display
	void Display();

	//Set
	void SetCamIndex(int idx, CString title, HWND wnd);

	//Live
	BOOL GetActiveLive();
	void SetActiveLive(BOOL active);

	//view handle
	Hlong GetView();	
	
	void Open();

	void Invalidate();
	void PixelGrayValue(long x, long y, short gray);

	void SetPart(long sx, long sy, long ex, long ey);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnPaint();
	afx_msg void ClickBtnenhctrlLive();
	afx_msg void ClickBtnenhctrlImgOpen();
	afx_msg void ClickBtnenhctrlImgSave();
	afx_msg void ClickBtnenhctrlZoomin();
	afx_msg void ClickBtnenhctrlZoomout();
	afx_msg void ClickBtnenhctrlZoom11();
	afx_msg void ClickBtnenhctrlZoomfit();
	afx_msg void ClickBtnenhctrlGray();
	afx_msg void ClickBtnenhctrlGrid();
	afx_msg void ClickBtnenhctrlGeneral();
	
	afx_msg void OnRegionConfirm();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	
	DECLARE_EVENTSINK_MAP()

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
