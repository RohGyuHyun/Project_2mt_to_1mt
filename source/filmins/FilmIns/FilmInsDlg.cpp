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


//메모리 릭 체크 start
#ifdef _DEBUG
#define new DEBUG_NEW

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#endif
//메모리 릭 체크 end

//High resolution timer for total read and link count 
UINT _TimerID;
TIMECAPS _Caps;
BOOL _TimerPost;
DWORD _TimerCounter;
void CALLBACK TimerReadFunc(UINT wID,UINT wUser,DWORD dwUser,DWORD dw1,DWORD dw2);

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// CFilmInsDlg 대화 상자
CFilmInsDlg::CFilmInsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFilmInsDlg::IDD, pParent)
	, m_edit_test(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	int i, j;

	for(i = 0; i < IMG_RECT_MAX_NUM; i++)
	{
		m_bInspectStart[i] = FALSE;

		m_dlgImgRect[i] = NULL;
		m_iMapIdx[i] = 0;
		m_iNGViewIdx[i] = 0;
		m_iNGSubViewIdx[i] = 0;
		m_iRslt[i] = 0;
	
		m_bFlagGrabStart[i] = FALSE;
		m_bCheckGrab = FALSE;

		m_pThread[i] = NULL;
		m_pInspectThread[i] = NULL;
		
		m_iBufIdx[i] = 0;//Buffer index
		m_iGrbIdx[i] = 0;//Grab index
		m_iPrcIdx[i] = 0;//Process index

		m_iNGDeviceIdx[i] = 0;
		m_bInspectStartNGView[i] = 0;

		m_iPrcDevIdx[i] = 0;
		m_iGrbDevIdx[i] = 0;

		//NG Scroll
		m_iNGScrollIdx[i] = 0;
		m_iNGScrollLastNum[i] = 0;

		for(j = 0; j < SUB_RGN_MAX_NUM; j++)
		{
			m_bChkNGKind[i][j] = 0;
		}
	}

	for(i = 0; i < 4; i++)
	{
		m_iLightChIdx[i] = 0;
		m_iLightOnOffIdx[i] = 0;
	}

	memset(m_sNGLotID, NULL, sizeof(char) * MAX_PATH * IMG_RECT_MAX_NUM);
	memset(m_sNGDate, NULL, sizeof(char) * MAX_PATH * IMG_RECT_MAX_NUM);

	memset(m_VisFlagData, NULL, sizeof(typeVisionFlagData) * IMG_RECT_MAX_NUM);

	//Live
	m_bLiveOverlayDisp = TRUE;
	
	m_BrushBlack.CreateSolidBrush(RGB(0, 0, 0));

	m_bCheckLoadMaster = FALSE;

	m_RectIdx = 0;
	m_lColorMode = 1;
	m_lProfileMode = 1;

	//값이 0보다크면 그랩하지않고 기존 영상으로 검사
	m_lTest = 0;
	
	m_VisMod = new C_VisionModule();
	m_dlgSetup = NULL; 
	
	//Comizoa DIO Handle
	m_hComiDevice32 = NULL;
	m_hComiDevice64 = NULL;

	m_img0_ch0 = 0;	m_img0_ch1 = 0;	m_img0_ch2 = 0;	m_img0_ch3 = 0;
	m_img1_ch0 = 0;	m_img1_ch1 = 0;	m_img1_ch2 = 0;	m_img1_ch3 = 0;
	m_img2_ch0 = 0;	m_img2_ch1 = 0;	m_img2_ch2 = 0;	m_img2_ch3 = 0;
	m_img3_ch0 = 0;	m_img3_ch1 = 0;	m_img3_ch2 = 0;	m_img3_ch3 = 0;
	m_img4_ch0 = 0;	m_img4_ch1 = 0;	m_img4_ch2 = 0;	m_img4_ch3 = 0;
	m_img5_ch0 = 0;	m_img5_ch1 = 0;	m_img5_ch2 = 0;	m_img5_ch3 = 0;
	m_img6_ch0 = 0;	m_img6_ch1 = 0;	m_img6_ch2 = 0;	m_img6_ch3 = 0;
	m_img7_ch0 = 0;	m_img7_ch1 = 0;	m_img7_ch2 = 0;	m_img7_ch3 = 0;
	m_img8_ch0 = 0;	m_img8_ch1 = 0;	m_img8_ch2 = 0;	m_img8_ch3 = 0;
	m_img9_ch0 = 0;	m_img9_ch1 = 0;	m_img9_ch2 = 0;	m_img9_ch3 = 0;
	m_img10_ch0 = 0; m_img10_ch1 = 0; m_img10_ch2 = 0; m_img10_ch3 = 0;
	m_img11_ch0 = 0; m_img11_ch1 = 0; m_img11_ch2 = 0; m_img11_ch3 = 0;

	m_bIsColor = FALSE;
	m_strStartTime.Empty();
}

CFilmInsDlg::~CFilmInsDlg()
{

}

void CFilmInsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTNENHCTRL_MAIN_TITLE, m_ctrlMainTitle);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_LINE, m_label_mode);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_ONLINE, m_label_online);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_TOTAL, m_label_tot);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_GOOD, m_label_good);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_NG, m_label_ng);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_GRABTIME, m_label_gtime);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_PROCESSTIME, m_label_ptime);
	DDX_Control(pDX, IDC_LIST_RSLT, m_list_rslt);
	DDX_Control(pDX, IDC_LIST_TCP, m_list_tcp);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_MODEL, m_label_model);
	DDX_Text(pDX, IDC_EDIT_IMG0_CH0, m_img0_ch0);
	DDX_Text(pDX, IDC_EDIT_IMG0_CH1, m_img0_ch1);
	DDX_Text(pDX, IDC_EDIT_IMG0_CH2, m_img0_ch2);
	DDX_Text(pDX, IDC_EDIT_IMG0_CH3, m_img0_ch3);
	DDX_Text(pDX, IDC_EDIT_IMG1_CH0, m_img1_ch0);
	DDX_Text(pDX, IDC_EDIT_IMG1_CH1, m_img1_ch1);
	DDX_Text(pDX, IDC_EDIT_IMG1_CH2, m_img1_ch2);
	DDX_Text(pDX, IDC_EDIT_IMG1_CH3, m_img1_ch3);
	DDX_Text(pDX, IDC_EDIT_IMG2_CH0, m_img2_ch0);
	DDX_Text(pDX, IDC_EDIT_IMG2_CH1, m_img2_ch1);
	DDX_Text(pDX, IDC_EDIT_IMG2_CH3, m_img2_ch2);
	DDX_Text(pDX, IDC_EDIT_IMG2_CH4, m_img2_ch3);
	DDX_Text(pDX, IDC_EDIT_IMG3_CH0, m_img3_ch0);
	DDX_Text(pDX, IDC_EDIT_IMG3_CH1, m_img3_ch1);
	DDX_Text(pDX, IDC_EDIT_IMG3_CH2, m_img3_ch2);
	DDX_Text(pDX, IDC_EDIT_IMG3_CH3, m_img3_ch3);
	DDX_Text(pDX, IDC_EDIT_IMG4_CH0, m_img4_ch0);
	DDX_Text(pDX, IDC_EDIT_IMG4_CH1, m_img4_ch1);
	DDX_Text(pDX, IDC_EDIT_IMG4_CH2, m_img4_ch2);
	DDX_Text(pDX, IDC_EDIT_IMG4_CH3, m_img4_ch3);
	DDX_Text(pDX, IDC_EDIT_IMG5_CH0, m_img5_ch0);
	DDX_Text(pDX, IDC_EDIT_IMG5_CH1, m_img5_ch1);
	DDX_Text(pDX, IDC_EDIT_IMG5_CH2, m_img5_ch2);
	DDX_Text(pDX, IDC_EDIT_IMG5_CH3, m_img5_ch3);
	DDX_Text(pDX, IDC_EDIT_IMG6_CH0, m_img6_ch0);
	DDX_Text(pDX, IDC_EDIT_IMG6_CH1, m_img6_ch1);
	DDX_Text(pDX, IDC_EDIT_IMG6_CH2, m_img6_ch2);
	DDX_Text(pDX, IDC_EDIT_IMG6_CH3, m_img6_ch3);
	DDX_Text(pDX, IDC_EDIT_IMG7_CH0, m_img7_ch0);
	DDX_Text(pDX, IDC_EDIT_IMG7_CH1, m_img7_ch1);
	DDX_Text(pDX, IDC_EDIT_IMG7_CH2, m_img7_ch2);
	DDX_Text(pDX, IDC_EDIT_IMG7_CH3, m_img7_ch3);
	DDX_Text(pDX, IDC_EDIT_IMG8_CH0, m_img8_ch0);
	DDX_Text(pDX, IDC_EDIT_IMG8_CH1, m_img8_ch1);
	DDX_Text(pDX, IDC_EDIT_IMG8_CH2, m_img8_ch2);
	DDX_Text(pDX, IDC_EDIT_IMG8_CH3, m_img8_ch3);
	DDX_Text(pDX, IDC_EDIT_IMG9_CH0, m_img9_ch0);
	DDX_Text(pDX, IDC_EDIT_IMG9_CH1, m_img9_ch1);
	DDX_Text(pDX, IDC_EDIT_IMG9_CH2, m_img9_ch2);
	DDX_Text(pDX, IDC_EDIT_IMG9_CH3, m_img9_ch3);
	DDX_Text(pDX, IDC_EDIT_IMG10_CH0, m_img10_ch0);
	DDX_Text(pDX, IDC_EDIT_IMG10_CH1, m_img10_ch1);
	DDX_Text(pDX, IDC_EDIT_IMG10_CH2, m_img10_ch2);
	DDX_Text(pDX, IDC_EDIT_IMG10_CH3, m_img10_ch3);
	DDX_Text(pDX, IDC_EDIT_IMG11_CH0, m_img11_ch0);
	DDX_Text(pDX, IDC_EDIT_IMG11_CH1, m_img11_ch1);
	DDX_Text(pDX, IDC_EDIT_IMG11_CH2, m_img11_ch2);
	DDX_Text(pDX, IDC_EDIT_IMG11_CH3, m_img11_ch3);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_MAP0, m_map0);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_MAP1, m_map1);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_MAP2, m_map2);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_MAP3, m_map3);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_MAP4, m_map4);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_MAP5, m_map5);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_MAP6, m_map6);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_MAP7, m_map7);
	DDX_Control(pDX, IDC_SCROLLBAR_NG, m_scroll_ng);
	DDX_Control(pDX, IDC_BTNENHCTRL_SETUP, m_btnSetup);
	DDX_Control(pDX, IDC_BTNENHCTRL_RUNSTOP, m_btnStartStop);
	DDX_Control(pDX, IDC_BTNENHCTRL_LABEL_NGVIEW, m_label_ngview);
	DDX_Text(pDX, IDC_EDIT_TEST, m_edit_test);
}

BEGIN_MESSAGE_MAP(CFilmInsDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_STN_CLICKED(IDC_RECT_IMG_MAP0, &CFilmInsDlg::OnRectImgMap0)
	ON_STN_CLICKED(IDC_RECT_IMG_MAP1, &CFilmInsDlg::OnRectImgMap1)
	ON_STN_CLICKED(IDC_RECT_IMG_MAP2, &CFilmInsDlg::OnRectImgMap2)
	ON_STN_CLICKED(IDC_RECT_IMG_MAP3, &CFilmInsDlg::OnRectImgMap3)
	ON_STN_CLICKED(IDC_RECT_IMG_MAP4, &CFilmInsDlg::OnRectImgMap4)
	ON_STN_CLICKED(IDC_RECT_IMG_MAP5, &CFilmInsDlg::OnRectImgMap5)
	ON_STN_CLICKED(IDC_RECT_IMG_MAP6, &CFilmInsDlg::OnRectImgMap6)
	ON_STN_CLICKED(IDC_RECT_IMG_MAP7, &CFilmInsDlg::OnRectImgMap7)
	ON_STN_CLICKED(IDC_RECT_IMG_MAP8, &CFilmInsDlg::OnRectImgMap8)
	ON_STN_CLICKED(IDC_RECT_IMG_MAP9, &CFilmInsDlg::OnRectImgMap9)
	ON_STN_CLICKED(IDC_RECT_IMG_MAP10, &CFilmInsDlg::OnRectImgMap10)
	ON_STN_CLICKED(IDC_RECT_IMG_MAP11, &CFilmInsDlg::OnRectImgMap11)
	ON_MESSAGE(USER_MSG_IMG_RECT, OnImageRectMessage)
	ON_MESSAGE(USER_MSG_VIS, OnVisMessage)
	ON_MESSAGE(USER_MSG_SOCKET, OnSocketMessage)
	ON_MESSAGE(WM_ONCONNECT,OnConnect)
	ON_MESSAGE(WM_ONCLOSE, OnClose)
	ON_MESSAGE(WM_RECEIVE_DATA,OnReceive)
	ON_WM_TIMER()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_BUTTON1, &CFilmInsDlg::OnBnClickedButton1)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON2, &CFilmInsDlg::OnBnClickedButton2)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_TCP, &CFilmInsDlg::OnNMRClickListTcp)
END_MESSAGE_MAP()


// CFilmInsDlg 메시지 처리기
static void CALLBACK TimerReadFunc(UINT wID,UINT wUser,DWORD dwUser,DWORD dw1,DWORD dw2)
{
	if(_TimerPost==FALSE)
	{
		_TimerCounter++;
	}
	else
	{
		AfxMessageBox("Error : high resolution timer event loss!");
	}	
}

BOOL CFilmInsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.
	// TODO: 여기에 추가 초기화 작업을 추가합니다.	
	//high resolution timer for IO reading & PLC init
	//(timer)
	_TimerPost=FALSE;
	timeGetDevCaps(&_Caps,sizeof(_Caps));
	_Caps.wPeriodMin = 1;
	timeBeginPeriod(_Caps.wPeriodMin);
	_TimerID = timeSetEvent(_Caps.wPeriodMin, _Caps.wPeriodMin, (LPTIMECALLBACK)&TimerReadFunc, 0, (UINT)TIME_PERIODIC);
	if(!_TimerID)
	{
		AfxMessageBox("Error : high resolution timer set fail!");
		WriteStatusList("Error : high resolution timer set fail!");
	}	

	//1PC 3Client Selector
	CWinApp *App = AfxGetApp();
	int m_cmdLine, frIdx, chIdx, iPortNum;
	CString strTemp;
 
	m_cmdLine = atoi(App->m_lpCmdLine);
	m_VisMod->SetVisIndex(m_cmdLine);													//카메라 타입에 따른 분기를 위한 변수

	SetLightIndex(m_VisMod->GetVisIndex());	 											// 조명 인덱스

	//20150223 by
	m_strDBPath.Format("D:\\DATABASE_2mt_%d\\MODEL", m_VisMod->GetVisIndex());
	m_strRegiPath.Format("SOFTWARE\\FILMINS_2mt_%d\\MODEL", m_VisMod->GetVisIndex());
	m_strLogPath.Format("E:\\Insp_Result_2mt_%d", m_VisMod->GetVisIndex());
	/*
	m_strDBPath.Format("D:\\DATABASE_%d\\MODEL", m_VisMod->GetVisIndex());				// 모델 저장 경로 지정
	m_strRegiPath.Format("SOFTWARE\\FILMINS_%d\\MODEL", m_VisMod->GetVisIndex());		// 모델 레지스트리 경로 지정
	m_strLogPath.Format("E:\\Insp_Result_%d", m_VisMod->GetVisIndex());					//로그, 이미지 저장경로
	*/

	strTemp.Format("FilmIns Inspection %d", m_VisMod->GetVisIndex() + 1);
	SetWindowText(strTemp);

	//20150223 by
	switch(m_VisMod->GetVisIndex())
	{
	case VISION_CAM_TOP1:
		strTemp.Format("::CAM #%d(상부)", m_VisMod->GetVisIndex() + 1);					// 디스플레이 제목표시줄 이름
		frIdx = M_DEV1;																	// 프레임그래버 보드 인덱스
		chIdx = 0;																		// 프레임그래버 채널 인덱스
		iPortNum = SOCKET_PORT_NUM1;													// 소켓 포트
		m_bIsColor = FALSE;																// 카메라 컬러 지원여부
		GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
		break;
	case VISION_CAM_TOP2:
		strTemp.Format("::CAM #%d(상부)", m_VisMod->GetVisIndex() + 1);
		frIdx = M_DEV0;
		chIdx = 0;
		iPortNum = SOCKET_PORT_NUM2;
		m_bIsColor = FALSE;
		break;
	case VISION_CAM_BOTTOM:
		strTemp.Format("::CAM #%d(하부)", m_VisMod->GetVisIndex() + 1);
		frIdx = M_DEV0;
		chIdx = 1;
		iPortNum = SOCKET_PORT_NUM3;
		m_bIsColor = TRUE;
		break;
	default:
		return 0;
		break;
	}
	/*
	switch(m_VisMod->GetVisIndex())
	{
	case VISION_CAM_TOP1:
		strTemp.Format("::CAM #%d(상부1)", m_VisMod->GetVisIndex() + 1);				// 디스플레이 제목표시줄 이름
		frIdx = M_DEV0;																	// 프레임그래버 보드 인덱스
		chIdx = 0;																		// 프레임그래버 채널 인덱스
		iPortNum = SOCKET_PORT_NUM1;													// 소켓 포트
		m_bIsColor = FALSE;																// 카메라 컬러 지원여부
		break;
	case VISION_CAM_BOTTOM1:
		strTemp.Format("::CAM #%d(하부1)", m_VisMod->GetVisIndex() + 1);
		frIdx = M_DEV0;
		chIdx = 0;
		iPortNum = SOCKET_PORT_NUM2;
		m_bIsColor = FALSE;
		break;
	case VISION_CAM_TOP2:
		strTemp.Format("::CAM #%d(상부2)", m_VisMod->GetVisIndex() + 1);
		frIdx = M_DEV1;
		chIdx = 0;
		iPortNum = SOCKET_PORT_NUM3;
		m_bIsColor = TRUE;
		break;
	case VISION_CAM_BOTTOM2:
		strTemp.Format("::CAM #%d(하부2)", m_VisMod->GetVisIndex() + 1);
		frIdx = M_DEV1;
		chIdx = 0;
		iPortNum = SOCKET_PORT_NUM4;
		m_bIsColor = TRUE;
		break;
	default:
		return 0;
		break;
	}
	*/

	if(m_bIsColor)
		GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_COLORMODE)->SetWindowText("COLOR MODE");
	else
		GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_COLORMODE)->SetWindowText("MONO MODE");

	Sleep(50);

	Herror herr;		
	CString status;
	Hlong width, height;
	HTuple wd, ht;
	int i, j;
	char temp[MAX_MOD_NAME];


	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);
	m_strStartTime.Format("%04d%02d%02d", lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);

	CRect sys_rect;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &sys_rect, NULL);

	//Load system file
	//Load/save system parameter
	m_VisMod->LoadSystemPara(m_strDBPath);
	
	MoveWindow(sys_rect.Width() - MAINVIEW_MOVE_OFFSET, 0, MAINVIEW_MOVE_OFFSET, sys_rect.Height(), TRUE);

	for(int i = 0; i < IMG_RECT_MAX_NUM; i++)
	{
		m_dlgImgRect[i] = new C_CtrlImageRect;
		m_dlgImgRect[i]->Create(IMAGE_RECT_DIALOG, this);
	}

	m_ctrlMainTitle.SetForeColor( RGB(255,255,255) );
	m_ctrlMainTitle.SetBackColor( RGB(64, 64, 64) );
	m_ctrlMainTitle.SetWindowText("::Film Inspection (system ver.150604.1445.2mt)");
	m_dlgImgRect[0]->MoveWindow(0, 0, sys_rect.Width() - MAINVIEW_MOVE_OFFSET, sys_rect.Height(), FALSE);
	m_dlgImgRect[0]->SetCamIndex(0, strTemp, this->m_hWnd);
	m_dlgImgRect[0]->ShowWindow(SW_SHOW);

	//영상 Size 초기화 //이시점에서 강제로 해줘야 함.
	m_lWidth[0] = WIDTH;
	m_lHeight[0] = HEIGHT;

	//Model load Begin
 	C_CtrlModelManageDlg *dlg = new C_CtrlModelManageDlg(NULL,m_strDBPath, m_strRegiPath);
	dlg->Open();

	sprintf(temp, "%s", dlg->GetName());
	m_label_model.SetCaption(temp);
	CString str0, str1;
	str0 = dlg->GetName();
	str1 = dlg->GetPath();

	if( (str0.IsEmpty()) || (str1.IsEmpty()) )
	{
		WriteStatusList("Error : invalid model!");
	}
	else
	{
		//Load setup INI file
		m_VisMod->LoadInspectSelectPara(dlg->GetPath());
		if( m_VisMod->LoadInspectPara(dlg->GetPath()) )
		{
			UpdateLight(FALSE);
		}
		else
		{
			WriteStatusList("Error : can't load setup parameter file!");
		}
	}

	delete dlg;
	//Model Load End

	// 맵 이미지 촬상 수 고정
	typeInspectPara inspara;
	inspara = m_VisMod->GetInspectPara(0);

	//20150223 by
	switch(m_VisMod->GetVisIndex())
	{
	case VISION_CAM_TOP1:
		inspara.m_iParaBase[1] = USE_MAP_CNT_VIS1;
		break;
	case VISION_CAM_TOP2:
		inspara.m_iParaBase[1] = USE_MAP_CNT_VIS2;
		break;
	case VISION_CAM_BOTTOM:
		inspara.m_iParaBase[1] = USE_MAP_CNT_VIS3;
		break;
	}
	/*
	switch(m_VisMod->GetVisIndex())
	{
	case VISION_CAM_TOP1:
		inspara.m_iParaBase[1] = USE_MAP_CNT_VIS1;
		break;
	case VISION_CAM_BOTTOM1:
		inspara.m_iParaBase[1] = USE_MAP_CNT_VIS2;
		break;
	case VISION_CAM_TOP2:
		inspara.m_iParaBase[1] = USE_MAP_CNT_VIS3;
		break;
	case VISION_CAM_BOTTOM2:
		inspara.m_iParaBase[1] = USE_MAP_CNT_VIS4;
		break;
	}
	*/
	m_VisMod->SetInspectPara(0, inspara);

	typeMainSystemRslt sys_rslt;

	//Load/save system result
	m_VisMod->LoadSystemRslt(m_strDBPath);
	sys_rslt = m_VisMod->GetMainSysRslt();
	
	sprintf(temp, "%ld", sys_rslt.m_lCnt[1]);
	m_label_tot.SetCaption(temp);
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[2]);
	m_label_good.SetCaption(temp);
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[3]);
	m_label_ng.SetCaption(temp);
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[4]);
	m_label_gtime.SetCaption(temp);
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[5]);
	m_label_ptime.SetCaption(temp);


#ifdef _CAM
	//Comizoa DIO Device handle
	if(COMI_LoadDll())
	{
		//20150223 by
		m_hComiDevice64 = COMI_LoadDevice(0xB402, 0);	// DO 64
		m_hComiDevice32 = COMI_LoadDevice(0xB404, 0);	// DIO 32
		WriteStatusList("Success : I/O Initialize OK !");

	}
	else
	{
		WriteStatusList("Error : I/O Initialize failed !");
	}

	if(m_cCam.Open(frIdx, chIdx, m_bIsColor))
	{
		WriteStatusList("Success : CAMERA Initialize OK !");
	}
	else
	{
		WriteStatusList("Error : CAMERA Initialize Failed !");
	}

	Sleep(500);

	
	for(i = 0; i < 2; i++)
	{
		for(j = 0; j < IMAGE_MAP_MAX_NUM; j++)
		{
			if(m_bIsColor)//컬러 
				m_cCam.Grab(0, i, j, m_VisMod->GetHalImage(0, i, j), m_bIsColor, 
							m_VisMod->GetHalRedImage(0, i, j), m_VisMod->GetHalGreenImage(0, i, j), m_VisMod->GetHalBlueImage(0, i, j));
			else//모노
				m_cCam.Grab(0, i, j, m_VisMod->GetHalImage(0, i, j));
		}
	}
	
	Sleep(500);

	//20150223 by
	COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[0], FALSE);
	COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[1], FALSE);
	COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[2], FALSE);
	if(m_iLightOnOffIdx[3] > VISION_CAM_NONE)
		COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[3], FALSE);
	/*
	COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[0], FALSE);
	COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[1], FALSE);
	COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[2], FALSE);
	if(m_iLightOnOffIdx[3] > VISION_CAM_NONE)
		COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[3], FALSE);
`	*/
#else
	//image load
	CString tmpPath_Name;
	CFileDialog dlgFile(TRUE,"Image","*.*",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"All Image(*.*)|*.*|");
	if(dlgFile.DoModal() == IDOK)
	{
		for(i = 0; i < IMAGE_MAP_MAX_NUM; i++)
		{

			tmpPath_Name = dlgFile.GetPathName();
			read_image(m_VisMod->GetHalImage(0, 0, i), HTuple((LPCTSTR)tmpPath_Name));
		}
	}
	else
	{
		for(i = 0; i < IMAGE_MAP_MAX_NUM; i++)
		{	
			gen_image_const(m_VisMod->GetHalImage(0, m_iBufIdx[0], i),HTuple("byte"),WIDTH,HEIGHT);
		}
	}

	delete dlgFile;	
#endif

	//Halcon image initialize
	for(i = 0; i < IMG_RECT_MAX_NUM; i++)
	{
		set_check("~give_error");
			herr = get_image_pointer1(*m_VisMod->GetHalImage(i, m_iBufIdx[0], m_iMapIdx[0]), NULL, NULL, &wd, &ht);
		set_check("give_error");

		if(H_MSG_TRUE == herr)
		{
		}
		else
		{
			status.Format("Halcon Error Occured at get_image_pointer1_00!");
			m_VisMod->WriteErrorList((LPCTSTR)status);

			return FALSE;
		}			
		
		width = (Hlong)wd[0];
		height = (Hlong)ht[0];
		m_dlgImgRect[i]->SetZoom(i, (long)width, (long)height);
		m_lWidth[i] = width;
		m_lHeight[i] = height;

		set_font(m_dlgImgRect[i]->GetView(), "-Arial-12-*-*-*-*-*-1-");
		set_draw(m_dlgImgRect[i]->GetView(), HTuple("margin"));
	}

	m_dlgProcessing.Create(IDD_DIALOG_PROCESSING, this);
	m_dlgProcessing.ShowWindow(SW_HIDE);
	m_dlgSetup = new CSetUpBsaeDlg;
    m_dlgSetup->Create(IDD_SETUP_BASE_DIALOG, this);
	m_dlgSetup->ShowWindow(SW_HIDE);

	m_label_mode.SetWindowText("STOP MODE");
	m_label_mode.SetForeColor(RGB(255, 0, 0));
	m_label_mode.SetOffice2007ColorInnerBorder(RGB(255, 0, 0));
	m_label_mode.SetOffice2007ColorOuterBorder(RGB(255, 0, 0));


	//Init result list
	//Status
	LV_COLUMN m_lvColumn;

	m_lvColumn.mask= LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	m_lvColumn.fmt=LVCFMT_LEFT;
	m_lvColumn.pszText=" ";
	m_lvColumn.iSubItem=0;
	m_lvColumn.cx=700;
	m_list_rslt.InsertColumn(0, &m_lvColumn);

	//TCP
	m_lvColumn.mask= LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	m_lvColumn.fmt=LVCFMT_LEFT;
	m_lvColumn.pszText="TCP/IP";
	m_lvColumn.iSubItem=0;
	m_lvColumn.cx=700;
	m_list_tcp.InsertColumn(0, &m_lvColumn);

	set_system(HTuple("pregenerate_shape_models"),HTuple("false"));

	//MAP, NG display Begin
	Hlong idc[IMAGE_MAP_MAX_NUM];

	idc[0] = IDC_RECT_IMG_MAP0;
	idc[1] = IDC_RECT_IMG_MAP1;
	idc[2] = IDC_RECT_IMG_MAP2;
	idc[3] = IDC_RECT_IMG_MAP3;
	idc[4] = IDC_RECT_IMG_MAP4; 
	idc[5] = IDC_RECT_IMG_MAP5;
	idc[6] = IDC_RECT_IMG_MAP6;
	idc[7] = IDC_RECT_IMG_MAP7;
	idc[8] = IDC_RECT_IMG_MAP8;
	idc[9] = IDC_RECT_IMG_MAP9;
	idc[10] = IDC_RECT_IMG_MAP10;
	idc[11] = IDC_RECT_IMG_MAP11;
	
	Hobject img;
	for(i = 0; i < IMAGE_MAP_MAX_NUM; i++)
	{
		set_check("~father");
 		open_window(0, 0, 80, 80, (Hlong)(GetDlgItem(idc[i]))->m_hWnd, "transparent", "", &m_lWindowID[i]);
		set_check("father");

		set_part(m_lWindowID[i], 0, 0, m_lHeight[0], m_lWidth[0]);
	}

	Hlong ngidc[IMAGE_NG_NUM];

	ngidc[0] = IDC_STATIC_NGRECT0;                                                                                
	ngidc[1] = IDC_STATIC_NGRECT1;
	ngidc[2] = IDC_STATIC_NGRECT2;

	for(i = 0; i < IMAGE_NG_NUM; i++)
	{
		set_check("~father");
		open_window(0, 0, 300, 290, (Hlong)(GetDlgItem(ngidc[i]))->m_hWnd, "transparent", "", &m_lNGWindowID[i]);
		set_check("father");

		set_draw(m_lNGWindowID[i], HTuple("margin"));
	}
#ifdef _CAM
	OnRectImgMap2();
	//MAP, NG display End
#endif
	
	//소켓초기화/////////////////////////////////
	Sleep(500);
	m_Client = new CClientSock;
	m_Client->Create();
	m_Client->SetWnd(this->m_hWnd); 
	Sleep(100);

	m_Client->Connect((LPCTSTR)CONTROL_PC_IP_ADDRESS, iPortNum);

	//NG scroll
	m_scroll_ng.SetScrollRange(0, NG_MAX_NUM);
    m_scroll_ng.SetScrollPos(0);
    m_scroll_ng.EnableScrollBar(ESB_ENABLE_BOTH);
 
    SCROLLINFO info = {sizeof(SCROLLINFO), SIF_PAGE, 0};
    info.nPage = 0;
    m_scroll_ng.SetScrollInfo(&info);

	SetStaticMapType(m_VisMod->GetVisIndex());

	WriteStatusList(">>>>>Application started!");

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}


// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CFilmInsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CRect rect;
		CPen pen,*oldPen;
		CBrush brush,*oldBrush;

		CPaintDC dc(this);

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

		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CFilmInsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CFilmInsDlg::HandleAllMessage()
{
	int returnValue;
	MSG Mess;

	do {
		returnValue=::PeekMessage(&Mess, NULL, 0, 0, PM_REMOVE);
		if (returnValue)
		{
			::TranslateMessage(&Mess);
			::DispatchMessage (&Mess);
		}
	} while(returnValue);
	
	return returnValue;
}

void CFilmInsDlg::DoEvents()
{
	MSG message;
	if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&message);
		::DispatchMessage(&message);
	}
}
void CFilmInsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// USER MESSAGE BEGIN
//
LRESULT CFilmInsDlg::OnVisMessage(WPARAM para0, LPARAM para1)
{
	int i;

	CRect sys_rect;

	CString status;

	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);

	typeSystemPara sys_para;
	typeInspectRslt ins_rslt;	

	typeInspectPara ins_para;
	ins_para = m_VisMod->GetInspectPara(0);	

	sys_para = m_VisMod->GetSystemPara(0);
	ins_rslt = m_VisMod->GetInspectRslt(0);

	switch((int)para0)
	{
	case 0:
		//Grab process///////////////////////////////////////////////////////
		//Display result
		//(0 : grab/1 : process, int buf_idx, BOOL chk_ng_type)
		InspectResult((int)para0, (int)para1);  //20100609  lee

		break;
	case 1:
		//Inspect process/////////////////////////////////////////////////////

		//Display result
		//(0 : grab/1 : process, int buf_idx, BOOL chk_ng_type)
		InspectResult((int)para0, (int)para1);

		//대표 NG type을 알아낸다.
		CheckNGKind(0, (int)para1);
		//Process end

		for(i = 0; (i < m_VisMod->GetCOFNum(0)) && (i < SUB_RGN_MAX_NUM); i++)
		{
			m_VisFlagData[0].m_iInsErrNum[i] = ins_rslt.m_iRsltErrNum[i];//에러개수
			m_VisFlagData[0].m_iInsFindChip[i] = ins_rslt.m_iRsltChipFind[i];

			if(1 == ins_rslt.m_iRslt[i])
			{
				if(2 == ins_rslt.m_iRsltChipFind[i])
				{
					 m_VisFlagData[0].m_iInsRsltType[i] = PNCH;
					 sprintf(m_VisFlagData[0].m_sInsRsltType[i], "펀치*****");
				}
				else if(3 == ins_rslt.m_iRsltChipFind[i])
				{
					m_VisFlagData[0].m_iInsRsltType[i] = PCND;
					sprintf(m_VisFlagData[0].m_sInsRsltType[i], "불량-펀치");
				}
				else
				{
					m_VisFlagData[0].m_iInsRsltType[i] = 0;
					sprintf(m_VisFlagData[0].m_sInsRsltType[i], "양품*****");
				}
			}
			else if(2 == ins_rslt.m_iRsltChipFind[i]) 
			{

				m_VisFlagData[0].m_iInsRsltType[i] = PNCH;

				sprintf(m_VisFlagData[0].m_sInsRsltType[i], "펀치*****");
			}
			else if(1 != ins_rslt.m_iRsltAlign[i]) // 우선순위 변경 Align실패 우선
			{
				m_VisFlagData[0].m_iInsRsltType[i] = AMIF;

				sprintf(m_VisFlagData[0].m_sInsRsltType[i], "불량-정렬");

			}
			else if(-20 == ins_rslt.m_iRsltChipFind[i])
			{
				m_VisFlagData[0].m_iInsRsltType[i] = NOCP;

				sprintf(m_VisFlagData[0].m_sInsRsltType[i], "불량-노칩");
				
			}
			else if( (-50 >= ins_rslt.m_iRsltChipFind[i]) && (-60 < ins_rslt.m_iRsltChipFind[i]) )
			{
				m_VisFlagData[0].m_iInsRsltType[i] = MKND;

				sprintf(m_VisFlagData[0].m_sInsRsltType[i], "불량-마킹");
				
			}
			else
			{
				m_VisFlagData[0].m_iInsRsltType[i] = m_bChkNGKind[0][i];

				sprintf(m_VisFlagData[0].m_sInsRsltType[i], "불량-기타");

				char temp[100];
				sprintf(temp, "[NGType:%d/%s]\n", m_VisFlagData[0].m_iInsRsltType[i], m_VisFlagData[0].m_sInsRsltType[i]);
				TRACE0(temp);	
			}
		}
		break;
	//20100629 lss
	case 2:

		break;
	//Lot start
	case 3:
		
		break;
	//Lot end
	case 4:

		break;
	//Vision start
	case 5:

		break;
	//NG display
	case 6:
	
		break;
	case 7:

		break;
	default:
		AfxMessageBox("Error : invalid index(CFilmInsDlg::OnVisMessage)!");
		break;
	}


	return 0;
}

LRESULT CFilmInsDlg::OnSocketMessage(WPARAM para0, LPARAM para1)
{
	int idx = (int)para1;
	int nOffset, nCofNum;
	int i;
	CString strPacket, strTemp1, strTemp2;

	switch(idx)
	{
	case 0:	//Find Start 결과 전송
		nOffset = (int)m_VisFlagData[0].m_dData[0];
		strPacket.Format("%cVIS%d,0006,%04d%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), nOffset, PACKET_CHAR_ETX);
		break;
	case 1:	//Align Start 결과 전송
		nOffset = (int)(m_VisFlagData[0].m_dData[1] + ALIGN_OFFSET_DATA);
		strPacket.Format("%cVIS%d,0008,%04d%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), nOffset, PACKET_CHAR_ETX);
		break;
	case 2: //Inspectiong start 결과 전송
		nCofNum = m_VisMod->GetCOFNum(0);
		nOffset = (int)(m_VisFlagData[0].m_dData[2] + ALIGN_OFFSET_DATA);
		strTemp1.Format("%cVIS%d,0004,%d,%04d,%04d,%d", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), m_iPrcIdx[0], m_iPrcDevIdx[0], nOffset, nCofNum);

		for(i = 0; (i < nCofNum) && (i < SUB_RGN_MAX_NUM); i++)
		{
			strTemp2.Format(",%03d", m_VisFlagData[0].m_iInsRsltType[i]);
			strTemp1 += strTemp2;
		}
		strTemp2.Format("%c", PACKET_CHAR_ETX);
		strPacket = strTemp1 + strTemp2;
		break;
	case 3: //Grab Start 결과전송
		strPacket.Format("%cVIS%d,0002,%d,%04d%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), m_iGrbIdx[0], m_iGrbDevIdx[0], PACKET_CHAR_ETX);
		break;
	case 4:	//OCV Teach
		strPacket.Format("%cVIS%d,0026,%d,%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), (int)para0, PACKET_CHAR_ETX);
		break;
	case 5: //Model change

		break;
	case 6:	//Auto run
		strPacket.Format("%cVIS%d,0014%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), PACKET_CHAR_ETX);
		break;
	case 7:	//Grab&Display
		strPacket.Format("%cVIS%d,0016%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), PACKET_CHAR_ETX);
		break;
	case 8:	//Lot Start

		break;
	case 9:	//Lot End

		break;
	case 10: //NG Display
		strPacket.Format("%cVIS%d,0022%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), PACKET_CHAR_ETX);
		break;
	}

	OnSend(strPacket);

	return 0;
}


LRESULT CFilmInsDlg::OnImageRectMessage(WPARAM para0, LPARAM para1)
{
	
	// TODO: Add your control notification handler code here
	char temp[100];
	long x, y;
	HTuple gray, gray2;
	int loop;
	HTuple wd, ht;
	int rslt = 1;
	//Halcon image initialize
	HTuple StartSecond, EndSecond;

	typeInspectRslt ins_rslt;
	typeShiftRslt shift_rslt;
	
	typeMainSystemRslt sys_rslt;
	sys_rslt = m_VisMod->GetMainSysRslt();

	Herror herr;
	CString status;

	switch((int)para1)
	{
	//Display
	case 0:
		if(m_dlgSetup->IsWindowVisible())
		{
			//20150223 by
			switch(m_VisMod->GetVisIndex())
			{
			case VISION_CAM_TOP1:
				DisplayView((int)para0, m_iBufIdx[0], 2);
				break;
			case VISION_CAM_TOP2:
				DisplayView((int)para0, m_iBufIdx[0], 0);
				break;
			case VISION_CAM_BOTTOM:
				DisplayView((int)para0, m_iBufIdx[0], 2);
				break;
			}
			/*
			switch(m_VisMod->GetVisIndex())
			{
			case VISION_CAM_TOP1:
				DisplayView((int)para0, m_iBufIdx[0], 2);
				break;
			case VISION_CAM_BOTTOM1:
				DisplayView((int)para0, m_iBufIdx[0], 2);
				break;
			case VISION_CAM_TOP2:
				DisplayView((int)para0, m_iBufIdx[0], 0);
				break;
			case VISION_CAM_BOTTOM2:
				DisplayView((int)para0, m_iBufIdx[0], 0);
				break;
			}
			*/
		}
		else
			DisplayView((int)para0, m_iBufIdx[0], m_iMapIdx[0]);
	
		m_dlgImgRect[para0]->Display();

		break;
	
	//Live on
	case 1:
		if(!m_bInspectStart[0]) 
		{
			m_bLiveOverlayDisp = FALSE;

			//20100629 lss
			switch(para0)
			{
			case 0:
				SetTimer(0, 50, NULL);
				//Light on
				COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[0], TRUE);
				COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[1], TRUE);
				COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[2], TRUE);
				if(m_iLightOnOffIdx[3] > VISION_CAM_NONE)
					COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[3], TRUE);
				/*
				COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[0], TRUE);
				COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[1], TRUE);
				COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[2], TRUE);
				if(m_iLightOnOffIdx[3] > VISION_CAM_NONE)
					COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[3], TRUE);
				*/

				Sleep(50);

				break;
			}
		}
		HandleAllMessage();
		break;
	//Live off
	case 2:
	
		if(!m_bInspectStart[0])
		{
			//20100629 lss
			switch(para0)
			{
			case 0:
				KillTimer(0);

				//Light off
				//20150223 by
				COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[0], FALSE);
				COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[1], FALSE);
				COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[2], FALSE);
				if(m_iLightOnOffIdx[3] > VISION_CAM_NONE)
					COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[3], FALSE);
				/*
				COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[0], FALSE);
				COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[1], FALSE);
				COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[2], FALSE);
				if(m_iLightOnOffIdx[3] > VISION_CAM_NONE)
					COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[3], FALSE);
				*/
				break;
			}
		}
		HandleAllMessage();
		break;
	//Image load
	case 3:
		//20100629 lss
		switch(para0)
		{
		case 0:
			Hal_OpenHalImage(m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), "", FALSE);
			
			set_check("~give_error");
			herr = get_image_pointer1(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), NULL, NULL, &wd, &ht);
			set_check("give_error");
			if(H_MSG_TRUE == herr)
			{
			}
			else
			{
				status.Format("Halcon Error Occured at get_image_pointer1_02!");
				m_VisMod->WriteErrorList((LPCTSTR)status);
				return 0;
			}
		
			if(m_bIsColor)
				decompose3(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), m_VisMod->GetHalRedImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), m_VisMod->GetHalGreenImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), m_VisMod->GetHalBlueImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]));

			m_dlgImgRect[para0]->SetZoom(0, (Hlong)wd[0], (Hlong)ht[0]);
			disp_obj(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[para0]->GetView());
			disp_obj(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), m_lWindowID[m_iMapIdx[0]]);
			
			break;
		}

		break;
	//Image save
	case 4:
		//20100629 lss
		switch(para0)
		{
		case 0:
			Hal_SaveHalImage(m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), "", "bmp");
			break;
		case 1:
			Hal_SaveHalImage(m_VisMod->GetHalImage((int)para0, 2, 0), "", "bmp");
			break;
		}

		break;
	//Gray value
	case 5:
		//20100629 lss
		switch(para0)
		{
		case 0:
			m_dlgImgRect[para0]->GetGrayPos(&x, &y);
			//Get gray 
			get_grayval(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), y, x, &gray);
			m_dlgImgRect[para0]->SetGray((Hlong)gray[0]);

			set_color(m_dlgImgRect[para0]->GetView(), HTuple("blue"));
			disp_obj(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[para0]->GetView());
			 //20110418 lee
			if(m_lProfileMode == 1)
			{
				for(loop=0;loop<m_lWidth[para0]-1;loop++)
				{
					get_grayval(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), y, loop, &gray);
					get_grayval(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), y, loop+1, &gray2);
					disp_line(m_dlgImgRect[(int)para0]->GetView(), y - (Hlong)gray[0], loop, y - (Hlong)gray2[0], loop+1);				
				}
			}
 	        else if(m_lProfileMode == 2)
			{
				set_color(m_dlgImgRect[para0]->GetView(), HTuple("yellow")); 
				for(loop=0;loop<m_lHeight[para0]-1;loop++)
				{
					get_grayval(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), loop, x, &gray);
					get_grayval(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), loop+1, x, &gray2);
					disp_line(m_dlgImgRect[(int)para0]->GetView(), loop, x + (Hlong)gray[0], loop+1, x + (Hlong)gray2[0]);				
				}
			}
			else if(m_lProfileMode == 3)
			{
				for(loop=0;loop<m_lWidth[para0]-1;loop++)
				{
					get_grayval(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), y, loop, &gray);
					get_grayval(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), y, loop+1, &gray2);
					disp_line(m_dlgImgRect[(int)para0]->GetView(), y - (Hlong)gray[0], loop, y - (Hlong)gray2[0], loop+1);				
				}
				set_color(m_dlgImgRect[para0]->GetView(), HTuple("yellow")); 
				for(loop=0;loop<m_lHeight[para0]-1;loop++)
				{
					get_grayval(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), loop, x, &gray);
					get_grayval(*m_VisMod->GetHalImage((int)para0, m_iBufIdx[0], m_iMapIdx[0]), loop+1, x, &gray2);
					disp_line(m_dlgImgRect[(int)para0]->GetView(), loop,x + (Hlong)gray[0], loop+1, x + (Hlong)gray2[0]);				
				}
			}

			break;
		}

		break;
		//Region confirm
		case 6:
			
			break;
		//Test
		case 7:
			//20100629 lss
			switch((int)para0)
			{
			case 0:
				//if(!m_bInspectStart[0])
				if(1) //imsi
				{
					m_bLiveOverlayDisp = TRUE;
					
					count_seconds(&StartSecond);
					
					typeInspectPara para;
					para = m_VisMod->GetInspectPara(0);
					para.m_bIsPunchSkip[0] = 0;
					para.m_bIsPunchSkip[1] = 0;
					m_VisMod->SetInspectPara(0, para);

					m_iRslt[0] = m_VisMod->Inspection((int)para0, m_iBufIdx[0]);
					/*
					if( (VISION_CAM_BOTTOM == m_VisMod->GetVisIndex()) )
						m_iRslt[0] = m_VisMod->Inspection_Bottom((int)para0, m_iBufIdx[0]);
					else 
						m_iRslt[0] = m_VisMod->Inspection((int)para0, m_iBufIdx[0]);
						*/

					ins_rslt = m_VisMod->GetInspectRslt((int)para0);
					shift_rslt = m_VisMod->GetShiftRslt((int)para0, m_iBufIdx[0]);
					
					count_seconds(&EndSecond);
					EndSecond = EndSecond - StartSecond;
					sys_rslt.m_lCnt[5] = (long)(((double)EndSecond[0]) * 1000);
					m_VisMod->SetMainSysRslt(sys_rslt);
					
					sprintf(temp, "%ld ", sys_rslt.m_lCnt[5]);
					m_label_ptime.SetCaption(temp);


					if(m_bIsColor)
					{
						switch(m_lColorMode)
						{
						case 1:
							disp_obj(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[0]->GetView());
							break;
						case 2:
							disp_obj(*m_VisMod->GetHalRedImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[0]->GetView());
							break;
						case 3:
							disp_obj(*m_VisMod->GetHalGreenImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[0]->GetView());
							break;
						case 0:	
							disp_obj(*m_VisMod->GetHalBlueImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[0]->GetView());
							break;	
						}
					}
					else
					{
						disp_obj(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[para0]->GetView());
					}
					
					//Set result/////////////////////////////
					//Set image
					m_VisMod->SetHalNGImage(0, m_iBufIdx[0], m_iNGViewIdx[0]);
					
					//Set result region
					m_VisMod->SetInspectNGRsltRgn(0, m_iNGViewIdx[0]);
					
					//Set result data
					m_VisMod->SetInspectNGRslt(0, m_iNGViewIdx[0], shift_rslt, ins_rslt);
				
																	//(auto_mode, main_view, chk_ng_type, chk_ng_Disp, ng_view_data_save)
					ResultCheck_Display((int)para0, m_iBufIdx[0], m_iNGViewIdx[0], TRUE, TRUE, FALSE, TRUE ,FALSE);//main view NG overlay 
					ResultCheck_Display((int)para0, m_iBufIdx[0], m_iNGViewIdx[0], TRUE, FALSE, FALSE, TRUE ,FALSE);//ng view  //20110509 lee	
				}
				else
				{
					WriteStatusList("WARRNING : 검사 진행 중 입니다!");
				}
				break;
			}

		break;
	default:
		AfxMessageBox("Error : invalid index!");
		break;
	}

	return 0;
}


//
//USER MESSAGE END

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// USER FUNCTION BEGIN
//
BOOL CFilmInsDlg::StartInspectThread()
{
	m_bInspectStart[0] = TRUE;

	HandleAllMessage();
	
	m_pThread[0] = AfxBeginThread(GrabThreadCam0, this, THREAD_PRIORITY_NORMAL);
	if (m_pThread[0]) m_pThread[0]->m_bAutoDelete = TRUE;

	m_pInspectThread[0] = AfxBeginThread(InspectThread, this, THREAD_PRIORITY_NORMAL);
	if (m_pInspectThread[0]) m_pInspectThread[0]->m_bAutoDelete = TRUE;

	//IMSI 이미지 저장 스레드
	/*
	m_pSaveImageThread = AfxBeginThread(SaveImageThread, this, THREAD_PRIORITY_NORMAL);
	if(m_pSaveImageThread) m_pSaveImageThread->m_bAutoDelete = TRUE;
	*/

	return TRUE;	
}

BOOL CFilmInsDlg::StopInspectThread()
{
	m_bInspectStart[0] = FALSE;


	return TRUE;	
}


//20150604 ngh Auto Delete
UINT CFilmInsDlg::DeleteSearchThread(LPVOID pParam)
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)pParam;
	CString strFolder;
	
	while(TRUE)
	{
		if(pdlg->m_bDeleteFolderStart)
		{
			pdlg->m_bDeleteFolder = FALSE;
			pdlg->m_bDeleteFolderStart = FALSE;
			if(pdlg->StartAutoDelete_Img())
			{
				pdlg->m_bDeleteFolder = TRUE;
				break;
				//return 0;
			}
			else
			{
				pdlg->m_bDeleteFolder = FALSE;
				break;
				//return 0;
			}
		}
		Sleep(1);
	}

	return 0;
}

UINT CFilmInsDlg::GrabThreadCam0(LPVOID pParam)
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)pParam;

	typeInspectPara ins_para;
//	typeShiftRslt shift_rslt;

	while(TRUE)
	{ 
		//검사 시작 신호 대기 
		if (!pdlg->m_bInspectStart[0]) 
		{
			AfxEndThread(0);
            break;
		}

		//Grab flag
		if( (pdlg->m_VisFlagData[0].m_bFlag[7]) && (pdlg->m_bInspectStart[0]) )//7 : grab start가 되었으면 플래그 on
		{
			pdlg->m_VisFlagData[0].m_bFlag[7] = FALSE;

			pdlg->m_VisMod->SetNGViewMode(FALSE, -1);

			//Grab Start
			pdlg->m_bCheckLoadMaster = FALSE;

			ins_para = pdlg->m_VisMod->GetInspectPara(0);	
			pdlg->GrabProcess(0, pdlg->m_iGrbIdx[0], ins_para.m_iParaBase[1]);

			//result display
			::SendMessage(pdlg->m_hWnd, USER_MSG_VIS, 0, pdlg->m_iGrbIdx[0]);

			//Grab완료 flag
			::SendMessage(pdlg->m_hWnd, USER_MSG_SOCKET, 0, 3);
		}

		Sleep(1);
	}
	return 0;
}
/*
//IMSI 이미지 저장 스레드
UINT CFilmInsDlg::SaveImageThread(LPVOID pParam)
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)pParam;

	char temp[DAT_STR_LENG];
	char path[MAX_PATH];
	int i;

	Hobject SubRgn, TotRgn;
	HTuple Num;
	Herror herr;
	CString status, strTemp, Dir;
	
	typeSystemPara sys_para;
	sys_para = pdlg->m_VisMod->GetSystemPara(0);

	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);

	int cof_idx = 0;

	while(TRUE)
	{
		if(!pdlg->m_bInspectStart[0]) 
		{
			AfxEndThread(0);
            break;
		}

		if(pdlg->m_bSaveImageStart)
		{
			pdlg->m_bSaveImageStart = FALSE;

			for(i = 0; (i < pdlg->m_VisMod->GetMapNum(0)) && (i < IMAGE_MAP_MAX_NUM); i++)
			{
				if(0 < pdlg->m_lTest)
				{
					pdlg->m_strLotNo = "Test";

				}
				memset(path, NULL, sizeof(path));

				sprintf(path, "%s\\%s\\%s\\IMAGE\\NG", pdlg->m_strLogPath, pdlg->m_strStartTime, pdlg->m_strLotNo);

				Dir.Format("%s", path);
				MakeDirectories(Dir);

				if(0 < pdlg->m_lTest)
				{
					sprintf(path, "%s\\%sTest[%ld]_%d.bmp", path, pdlg->m_strLotNo, pdlg->m_lTest + 1, i);  //20100726 lee 
					pdlg->m_lTest++;
				}
				else
				{
					//sprintf(path, "%s\\%s_VIS%d[%04d]_%d.bmp", path, pdlg->m_strLotNo, pdlg->m_VisMod->GetVisIndex(), pdlg->m_iPrcDevIdx[0] + cof_idx, i);//20100726 lee
					sprintf(path, "%s\\%s_VIS%d[%04d]_%d.bmp", path, pdlg->m_strLotNo, pdlg->m_VisMod->GetVisIndex(), pdlg->m_iPrcDevIdx[0] + cof_idx++, i);//20100726 lee
				}

				set_check("~give_error");
				herr = write_image(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iPrcIdx[0], i), "bmp", 0, path);
				set_check("give_error");
				if(H_MSG_TRUE == herr)
				{
				}
				else
				{
					strTemp.Format(_T("이미지 저장에 실패하였습니다. 하드디스크 용량 또는 저장 경로를 확인해 주세요!!"));
					AfxMessageBox(strTemp);
					status.Format("Halcon Error Occured at write_image_00!");
					pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
				}

				Sleep(10);
			}

			cof_idx = 0;
			
		}

		Sleep(10);
		
	}

	return 0;
}
*/

UINT CFilmInsDlg::InspectThread(LPVOID pParam)
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)pParam;


	HTuple Width, Height, Pointer, Type;
	Hobject RegionRect;
	Hlong chip_offset;
	
	CRect rect,rect_chip;
	BOOL punch_err;
	int pf_pitch;
	Hlong area;

	CString status;

	typeInspectPara ins_para;
	typeInspectRslt ins_rslt;
	typeSystemPara sys_para;
	sys_para = pdlg->m_VisMod->GetSystemPara(0);

	while(TRUE)
	{ 
		//검사 시작 Flag가 Reset 되면 종료 후 나간다. 
		if(!pdlg->m_bInspectStart[0]) 
		{
			AfxEndThread(0);
            break;
		}

		//단순 Grab and display
		if(pdlg->m_VisFlagData[0].m_bFlag[3])
		{
			ins_para = pdlg->m_VisMod->GetInspectPara(0);

			pdlg->m_VisFlagData[0].m_bFlag[3] = FALSE;

			//첫번째 제품찾기 후 결과 촬상 및 디스플레이
			//Grab
			//20150223 by
			COMI_DO_PutOne((pdlg->m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? pdlg->m_hComiDevice32 : pdlg->m_hComiDevice64, pdlg->m_iLightOnOffIdx[0], TRUE);
			COMI_DO_PutOne((pdlg->m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? pdlg->m_hComiDevice32 : pdlg->m_hComiDevice64, pdlg->m_iLightOnOffIdx[1], TRUE);
			COMI_DO_PutOne((pdlg->m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? pdlg->m_hComiDevice32 : pdlg->m_hComiDevice64, pdlg->m_iLightOnOffIdx[2], TRUE);
			if(pdlg->m_iLightOnOffIdx[3] > VISION_CAM_NONE)
				COMI_DO_PutOne((pdlg->m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? pdlg->m_hComiDevice32 : pdlg->m_hComiDevice64, pdlg->m_iLightOnOffIdx[3], TRUE);
			/*
			COMI_DO_PutOne(pdlg->m_hComiDevice64, pdlg->m_iLightOnOffIdx[0], TRUE);
			COMI_DO_PutOne(pdlg->m_hComiDevice64, pdlg->m_iLightOnOffIdx[1], TRUE);
			COMI_DO_PutOne(pdlg->m_hComiDevice64, pdlg->m_iLightOnOffIdx[2], TRUE);
			if(pdlg->m_iLightOnOffIdx[3] > VISION_CAM_NONE)
				COMI_DO_PutOne(pdlg->m_hComiDevice64, pdlg->m_iLightOnOffIdx[3], TRUE);
			*/

			Sleep(50);

			pdlg->SetLight(pdlg->m_iLightChIdx[0], pdlg->m_img7_ch0);
			pdlg->SetLight(pdlg->m_iLightChIdx[1], pdlg->m_img7_ch1);
			pdlg->SetLight(pdlg->m_iLightChIdx[2], pdlg->m_img7_ch2);
			pdlg->SetLight(pdlg->m_iLightChIdx[3], pdlg->m_img7_ch3);

			Sleep(100);

			if(pdlg->m_bIsColor)
				pdlg->m_cCam.Grab(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX, pdlg->m_VisMod->GetHalImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX), pdlg->m_bIsColor, 
						pdlg->m_VisMod->GetHalRedImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX), pdlg->m_VisMod->GetHalGreenImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX), pdlg->m_VisMod->GetHalBlueImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX));
			else
				pdlg->m_cCam.Grab(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX, pdlg->m_VisMod->GetHalImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX));
			
			disp_obj(*pdlg->m_VisMod->GetHalImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX), pdlg->m_dlgImgRect[0]->GetView()); 

			set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("green"));
			disp_line(pdlg->m_dlgImgRect[0]->GetView(), pdlg->m_lHeight[0] * 0.5, 0, pdlg->m_lHeight[0] * 0.5, pdlg->m_lWidth[0]);
			disp_line(pdlg->m_dlgImgRect[0]->GetView(), 0,pdlg-> m_lWidth[0] * 0.5, pdlg->m_lHeight[0], pdlg->m_lWidth[0] * 0.5);

			//Disp align inspect region
			gen_rectangle1(&RegionRect, ins_para.m_InsRegionAlign[0][0].top, ins_para.m_InsRegionAlign[0][0].left, ins_para.m_InsRegionAlign[0][0].bottom, ins_para.m_InsRegionAlign[0][0].right);
			disp_obj(RegionRect, pdlg->m_dlgImgRect[0]->GetView()); 
			gen_rectangle1(&RegionRect, ins_para.m_InsRegionAlign[0][2].top, ins_para.m_InsRegionAlign[0][2].left, ins_para.m_InsRegionAlign[0][2].bottom, ins_para.m_InsRegionAlign[0][2].right);
			disp_obj(RegionRect, pdlg->m_dlgImgRect[0]->GetView()); 

			//수평 기준 라인 Display (48mm기준)
			set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("red"));

			disp_line(pdlg->m_dlgImgRect[0]->GetView(), 0, (pdlg->m_lWidth[0] * 0.5)+(pdlg->m_dlgSetup->m_InsPara.m_dParaBase[2] * 1000/pdlg->m_dlgSetup->m_SysPara.m_dPara[0]/2) ,
							pdlg->m_lHeight[0], (pdlg->m_lWidth[0] * 0.5)+(pdlg->m_dlgSetup->m_InsPara.m_dParaBase[2] * 1000/pdlg->m_dlgSetup->m_SysPara.m_dPara[0]/2));
			disp_line(pdlg->m_dlgImgRect[0]->GetView(), 0, (pdlg->m_lWidth[0] * 0.5)-(pdlg->m_dlgSetup->m_InsPara.m_dParaBase[2] * 1000/pdlg->m_dlgSetup->m_SysPara.m_dPara[0]/2) ,
							pdlg->m_lHeight[0], (pdlg->m_lWidth[0] * 0.5)-(pdlg->m_dlgSetup->m_InsPara.m_dParaBase[2] * 1000/pdlg->m_dlgSetup->m_SysPara.m_dPara[0]/2));

			::SendMessage(pdlg->m_hWnd, USER_MSG_SOCKET, 0, 7);
		}
		//Find Start first chip
		else if(pdlg->m_VisFlagData[0].m_bFlag[0])
		{
			pdlg->m_VisFlagData[0].m_bFlag[0] = FALSE;

			//첫번째 제품찾기 
			//Grab
			//20150223 by
			COMI_DO_PutOne((pdlg->m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? pdlg->m_hComiDevice32 : pdlg->m_hComiDevice64, pdlg->m_iLightOnOffIdx[0], TRUE);
			COMI_DO_PutOne((pdlg->m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? pdlg->m_hComiDevice32 : pdlg->m_hComiDevice64, pdlg->m_iLightOnOffIdx[1], TRUE);
			COMI_DO_PutOne((pdlg->m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? pdlg->m_hComiDevice32 : pdlg->m_hComiDevice64, pdlg->m_iLightOnOffIdx[2], TRUE);
			if(pdlg->m_iLightOnOffIdx[3] > VISION_CAM_NONE)
				COMI_DO_PutOne((pdlg->m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? pdlg->m_hComiDevice32 : pdlg->m_hComiDevice64, pdlg->m_iLightOnOffIdx[3], TRUE);
			/*
			COMI_DO_PutOne(pdlg->m_hComiDevice64, pdlg->m_iLightOnOffIdx[0], TRUE);
			COMI_DO_PutOne(pdlg->m_hComiDevice64, pdlg->m_iLightOnOffIdx[1], TRUE);
			COMI_DO_PutOne(pdlg->m_hComiDevice64, pdlg->m_iLightOnOffIdx[2], TRUE);
			if(pdlg->m_iLightOnOffIdx[3] > VISION_CAM_NONE)
				COMI_DO_PutOne(pdlg->m_hComiDevice64, pdlg->m_iLightOnOffIdx[3], TRUE);
			*/

			pdlg->SetLight(pdlg->m_iLightChIdx[0], pdlg->m_img7_ch0);
			pdlg->SetLight(pdlg->m_iLightChIdx[1], pdlg->m_img7_ch1);
			pdlg->SetLight(pdlg->m_iLightChIdx[2], pdlg->m_img7_ch2);
			pdlg->SetLight(pdlg->m_iLightChIdx[3], pdlg->m_img7_ch3);
			
			Sleep(100);

			if(pdlg->m_bIsColor)
				pdlg->m_cCam.Grab(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX, pdlg->m_VisMod->GetHalImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX), pdlg->m_bIsColor, 
						pdlg->m_VisMod->GetHalRedImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX), pdlg->m_VisMod->GetHalGreenImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX), pdlg->m_VisMod->GetHalBlueImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX));
			else
				pdlg->m_cCam.Grab(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX, pdlg->m_VisMod->GetHalImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX));
		
			disp_obj(*pdlg->m_VisMod->GetHalImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX), pdlg->m_dlgImgRect[0]->GetView()); 

			//Find chip(위에서 아래로 검사 밝은 부분이 있는지 검사, 기준은 중간값 126이상)
			if(pdlg->m_VisMod->FindFirstChip(pdlg->m_dlgImgRect[0]->GetView(), pdlg->m_VisMod->GetHalImage(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX), 50, &chip_offset))
			{
				pdlg->m_VisFlagData[0].m_dData[0] = WIDTH - chip_offset;
			}
			else
			{
				pdlg->m_VisFlagData[0].m_dData[0] = ALIGN_FAILED_DATA;
			}

			//SendPacket 
			::SendMessage(pdlg->m_hWnd, USER_MSG_SOCKET, 0, 0);
		}
		//Align
		else if(pdlg->m_VisFlagData[0].m_bFlag[4])
		{
			ins_para = pdlg->m_VisMod->GetInspectPara(0);	

			pdlg->m_VisFlagData[0].m_bFlag[4] = FALSE;

			//Grab
			//20150223 by
			COMI_DO_PutOne((pdlg->m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? pdlg->m_hComiDevice32 : pdlg->m_hComiDevice64, pdlg->m_iLightOnOffIdx[0], TRUE);
			COMI_DO_PutOne((pdlg->m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? pdlg->m_hComiDevice32 : pdlg->m_hComiDevice64, pdlg->m_iLightOnOffIdx[1], TRUE);
			COMI_DO_PutOne((pdlg->m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? pdlg->m_hComiDevice32 : pdlg->m_hComiDevice64, pdlg->m_iLightOnOffIdx[2], TRUE);
			if(pdlg->m_iLightOnOffIdx[3] > VISION_CAM_NONE)
				COMI_DO_PutOne((pdlg->m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? pdlg->m_hComiDevice32 : pdlg->m_hComiDevice64, pdlg->m_iLightOnOffIdx[3], TRUE);
			/*
			COMI_DO_PutOne(pdlg->m_hComiDevice64, pdlg->m_iLightOnOffIdx[0], TRUE);
			COMI_DO_PutOne(pdlg->m_hComiDevice64, pdlg->m_iLightOnOffIdx[1], TRUE);
			COMI_DO_PutOne(pdlg->m_hComiDevice64, pdlg->m_iLightOnOffIdx[2], TRUE);
			if(pdlg->m_iLightOnOffIdx[3] > VISION_CAM_NONE)
				COMI_DO_PutOne(pdlg->m_hComiDevice64, pdlg->m_iLightOnOffIdx[3], TRUE);
			*/

			pdlg->SetLight(pdlg->m_iLightChIdx[0], pdlg->m_img0_ch0);
			pdlg->SetLight(pdlg->m_iLightChIdx[1], pdlg->m_img0_ch1);
			pdlg->SetLight(pdlg->m_iLightChIdx[2], pdlg->m_img0_ch2);
			pdlg->SetLight(pdlg->m_iLightChIdx[3], pdlg->m_img0_ch3);
			
			Sleep(100);

			if(pdlg->m_bIsColor)
				pdlg->m_cCam.Grab(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX, pdlg->m_VisMod->GetHalImage(0, 0, ins_para.m_iParaMapAlign[0]), pdlg->m_bIsColor, 
						pdlg->m_VisMod->GetHalRedImage(0, 0, ins_para.m_iParaMapAlign[0]), pdlg->m_VisMod->GetHalGreenImage(0, 0, ins_para.m_iParaMapAlign[0]), pdlg->m_VisMod->GetHalBlueImage(0, 0, ins_para.m_iParaMapAlign[0]));
			else
				pdlg->m_cCam.Grab(0, 0, SEARCH_DEVICE_IMAGE_MAP_IDX, pdlg->m_VisMod->GetHalImage(0, 0, ins_para.m_iParaMapAlign[0]));

			disp_obj(*pdlg->m_VisMod->GetHalImage(0, 0, ins_para.m_iParaMapAlign[0]), pdlg->m_dlgImgRect[0]->GetView()); 

			//Init
			pdlg->m_VisMod->ResetInspectRslt(0); 

			//검사 
 			if(pdlg->m_VisMod->InspectionFirstAlign(0, 0)) 
			{
				ins_rslt = pdlg->m_VisMod->GetInspectRslt(0); 

				if(1 == ins_rslt.m_iRsltAlign[0])
				{
					/*
					//20150225 by 선두칩 찾기에 펀치 유무 추가
					rect.left = ins_para.m_InsRegionFilm[0][0].left;
					rect.top = ins_para.m_InsRegionFilm[0][0].top;
					rect.right = ins_para.m_InsRegionFilm[0][0].right;
					rect.bottom = ins_para.m_InsRegionFilm[0][0].bottom;
					
					rect_chip.left = ins_para.m_InsRegionChip[0][0].left;
					rect_chip.top = ins_para.m_InsRegionChip[0][0].top;
					rect_chip.right = ins_para.m_InsRegionChip[0][0].right;
					rect_chip.bottom = ins_para.m_InsRegionChip[0][0].bottom;

					pf_pitch = (int)((ins_para.m_dParaBase[1] * 1000.) / sys_para.m_dPara[0]);

					if( !pdlg->m_VisMod->FindPuchingChip(0, pdlg->m_VisMod->GetHalImage(0, 0, ins_para.m_iParaMapFilm[4]),  
									*pdlg->m_VisMod->GetHalInspectRgn(0, 0, 0),
									rect,
									rect_chip,
									ins_para.m_iParaFilm[19],
									ins_para.m_iParaFilm[9],
									pf_pitch,
									ins_para.m_iParaFilm[13],
									ins_para.m_iParaFilm[14],
									ins_para.m_iParaFilm[12],
									ins_para.m_iParaFilm[15],
									ins_para.m_iParaFilm[20],    
									ins_para.m_iParaBase[2], 
									ins_para.m_iParaFilm[25],
									ins_para.m_iParaFilm[24],
									&punch_err, pdlg->m_VisMod->GetHalInspectRgn(0,0,19), &area))
					{			
					*/
						set_color(pdlg->m_dlgImgRect[0]->GetView(), "green");
						set_tposition(pdlg->m_dlgImgRect[0]->GetView(), 10 + (0 * 200), 10);
						write_string(pdlg->m_dlgImgRect[0]->GetView(), HTuple(HTuple("SCORE : ") + HTuple(ins_rslt.m_dScoreAlign[0][0] * 100)));
						set_tposition(pdlg->m_dlgImgRect[0]->GetView(), 40 + (0 * 200), 10);
						write_string(pdlg->m_dlgImgRect[0]->GetView(), HTuple(HTuple("SCORE : ") + HTuple(ins_rslt.m_dScoreAlign[0][1] * 100)));

						pdlg->m_VisFlagData[0].m_dData[1] = ins_rslt.m_dOffsetX[0];

						//Disp inspect region
						gen_rectangle1(&RegionRect, ins_para.m_InsRegionAlign[0][0].top, ins_para.m_InsRegionAlign[0][0].left, ins_para.m_InsRegionAlign[0][0].bottom, ins_para.m_InsRegionAlign[0][0].right);
						disp_obj(RegionRect, pdlg->m_dlgImgRect[0]->GetView());
						gen_rectangle1(&RegionRect, ins_para.m_InsRegionAlign[0][2].top, ins_para.m_InsRegionAlign[0][2].left, ins_para.m_InsRegionAlign[0][2].bottom, ins_para.m_InsRegionAlign[0][2].right);
						disp_obj(RegionRect, pdlg->m_dlgImgRect[0]->GetView());

						::SendMessage(pdlg->m_hWnd, USER_MSG_SOCKET, 0, 1); //Align 성공
				/*	
				}
					else
					{
					
						set_color(pdlg->m_dlgImgRect[0]->GetView(), "red");
						set_tposition(pdlg->m_dlgImgRect[0]->GetView(), 10 + (0 * 200), 10);
						write_string(pdlg->m_dlgImgRect[0]->GetView(), HTuple(HTuple("FAIL : can't find model!")));

						pdlg->m_VisFlagData[0].m_dData[1] = ALIGN_FAILED_DATA;

						//Disp inspect region
						gen_rectangle1(&RegionRect, ins_para.m_InsRegionAlign[0][0].top, ins_para.m_InsRegionAlign[0][0].left, ins_para.m_InsRegionAlign[0][0].bottom, ins_para.m_InsRegionAlign[0][0].right);
						disp_obj(RegionRect, pdlg->m_dlgImgRect[0]->GetView()); 
						gen_rectangle1(&RegionRect, ins_para.m_InsRegionAlign[0][2].top, ins_para.m_InsRegionAlign[0][2].left, ins_para.m_InsRegionAlign[0][2].bottom, ins_para.m_InsRegionAlign[0][2].right);
						disp_obj(RegionRect, pdlg->m_dlgImgRect[0]->GetView()); 

						::SendMessage(pdlg->m_hWnd, USER_MSG_SOCKET, 0, 1);	//Align 실패
					}
					*/
				}
				else
				{
					set_color(pdlg->m_dlgImgRect[0]->GetView(), "red");
					set_tposition(pdlg->m_dlgImgRect[0]->GetView(), 10 + (0 * 200), 10);
					write_string(pdlg->m_dlgImgRect[0]->GetView(), HTuple(HTuple("FAIL : can't find model!")));

					pdlg->m_VisFlagData[0].m_dData[1] = ALIGN_FAILED_DATA;

					//Disp inspect region
					gen_rectangle1(&RegionRect, ins_para.m_InsRegionAlign[0][0].top, ins_para.m_InsRegionAlign[0][0].left, ins_para.m_InsRegionAlign[0][0].bottom, ins_para.m_InsRegionAlign[0][0].right);
					disp_obj(RegionRect, pdlg->m_dlgImgRect[0]->GetView()); 
					gen_rectangle1(&RegionRect, ins_para.m_InsRegionAlign[0][2].top, ins_para.m_InsRegionAlign[0][2].left, ins_para.m_InsRegionAlign[0][2].bottom, ins_para.m_InsRegionAlign[0][2].right);
					disp_obj(RegionRect, pdlg->m_dlgImgRect[0]->GetView()); 

					::SendMessage(pdlg->m_hWnd, USER_MSG_SOCKET, 0, 1);	//Align 실패
				}
			}
			else
			{
				//Fail
				set_color(pdlg->m_dlgImgRect[0]->GetView(), "red");
				set_tposition(pdlg->m_dlgImgRect[0]->GetView(), 10 + (0 * 200), 10);
				write_string(pdlg->m_dlgImgRect[0]->GetView(), HTuple(HTuple("FAIL : can't find model!")));

				pdlg->m_VisFlagData[0].m_dData[1] = ALIGN_FAILED_DATA;

				//Disp inspect region
				gen_rectangle1(&RegionRect, ins_para.m_InsRegionAlign[0][0].top, ins_para.m_InsRegionAlign[0][0].left, ins_para.m_InsRegionAlign[0][0].bottom, ins_para.m_InsRegionAlign[0][0].right);
				disp_obj(RegionRect, pdlg->m_dlgImgRect[0]->GetView()); 
				gen_rectangle1(&RegionRect, ins_para.m_InsRegionAlign[0][2].top, ins_para.m_InsRegionAlign[0][2].left, ins_para.m_InsRegionAlign[0][2].bottom, ins_para.m_InsRegionAlign[0][2].right);
				disp_obj(RegionRect, pdlg->m_dlgImgRect[0]->GetView()); 

				::SendMessage(pdlg->m_hWnd, USER_MSG_SOCKET, 0, 1);	//Align 실패
			}
		}
		//Main process
		else if(pdlg->m_VisFlagData[0].m_bFlag[10])
		{
			pdlg->m_VisFlagData[0].m_bFlag[10] = FALSE;

			//Process
			pdlg->InspectProcess(0, pdlg->m_iPrcIdx[0], pdlg->m_iMapIdx[0]);

			::SendMessage(pdlg->m_hWnd, USER_MSG_VIS, 1, pdlg->m_iPrcIdx[0]); 

			//Align offset
			ins_rslt = pdlg->m_VisMod->GetInspectRslt(0); 
			pdlg->m_VisFlagData[0].m_dData[2] = ins_rslt.m_dOffsetX[0];//실패하면 0으로 넘어온다.

			::SendMessage(pdlg->m_hWnd, USER_MSG_SOCKET, 0, 2);
		}

		//NG view
		if(pdlg->m_bInspectStartNGView[0])
		{
			pdlg->m_bInspectStartNGView[0] = FALSE;
			pdlg->NGViewDisp();
			
			::SendMessage(pdlg->m_hWnd, USER_MSG_SOCKET, 0, 10);
		}

		Sleep(1);
	}

	return 0;
}

void CFilmInsDlg::GrabProcess(int cam_idx, int grab_idx, int img_cnt)
{
	int i = 0;
	
	//20110510 lee
	//int OldColorMode;
	HTuple StartSecond, EndSecond;

	count_seconds(&StartSecond);	

#ifdef _CAM
	//Light on
	//20150223 by
	COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[0], TRUE);
	COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[1], TRUE);
	COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[2], TRUE);
	if(m_iLightOnOffIdx[3] > VISION_CAM_NONE)
		COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[3], TRUE);
	/*
	COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[0], TRUE);
	COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[1], TRUE);
	COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[2], TRUE);
	if(m_iLightOnOffIdx[3] > VISION_CAM_NONE)
		COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[3], TRUE);
	*/
	DWORD test;
	for(i = 0; (i < img_cnt) && (i < IMAGE_MAP_MAX_NUM ); i++)
	{
		//DoEvents();
		switch(i)
		{
		case 0:
			SetLight(m_iLightChIdx[0], m_img0_ch0);
			SetLight(m_iLightChIdx[1], m_img0_ch1);
			SetLight(m_iLightChIdx[2], m_img0_ch2);
			SetLight(m_iLightChIdx[3], m_img0_ch3);
			break;
		case 1:
			SetLight(m_iLightChIdx[0], m_img1_ch0);
			SetLight(m_iLightChIdx[1], m_img1_ch1);
			SetLight(m_iLightChIdx[2], m_img1_ch2);
			SetLight(m_iLightChIdx[3], m_img1_ch3);
			break;
		case 2:
			SetLight(m_iLightChIdx[0], m_img2_ch0);
			SetLight(m_iLightChIdx[1], m_img2_ch1);
			SetLight(m_iLightChIdx[2], m_img2_ch2);
			SetLight(m_iLightChIdx[3], m_img2_ch3);
			break;
		case 3:
			SetLight(m_iLightChIdx[0], m_img3_ch0);
			SetLight(m_iLightChIdx[1], m_img3_ch1);
			SetLight(m_iLightChIdx[2], m_img3_ch2);
			SetLight(m_iLightChIdx[3], m_img3_ch3);
			break;
		case 4:
			SetLight(m_iLightChIdx[0], m_img4_ch0);
			SetLight(m_iLightChIdx[1], m_img4_ch1);
			SetLight(m_iLightChIdx[2], m_img4_ch2);
			SetLight(m_iLightChIdx[3], m_img4_ch3);
			break;
		case 5:
			SetLight(m_iLightChIdx[0], m_img5_ch0);
			SetLight(m_iLightChIdx[1], m_img5_ch1);
			SetLight(m_iLightChIdx[2], m_img5_ch2);
			SetLight(m_iLightChIdx[3], m_img5_ch3);
			break;
		case 6:
			SetLight(m_iLightChIdx[0], m_img6_ch0);
			SetLight(m_iLightChIdx[1], m_img6_ch1);
			SetLight(m_iLightChIdx[2], m_img6_ch2);
			SetLight(m_iLightChIdx[3], m_img6_ch3);
			break;
		case 7:
			SetLight(m_iLightChIdx[0], m_img7_ch0);
			SetLight(m_iLightChIdx[1], m_img7_ch1);
			SetLight(m_iLightChIdx[2], m_img7_ch2);
			SetLight(m_iLightChIdx[3], m_img7_ch3);
			break;
		case 8:
			SetLight(m_iLightChIdx[0], m_img8_ch0);
			SetLight(m_iLightChIdx[1], m_img8_ch1);
			SetLight(m_iLightChIdx[2], m_img8_ch2);
			SetLight(m_iLightChIdx[3], m_img8_ch3);
			break;
		case 9:
			SetLight(m_iLightChIdx[0], m_img9_ch0);
			SetLight(m_iLightChIdx[1], m_img9_ch1);
			SetLight(m_iLightChIdx[2], m_img9_ch2);
			SetLight(m_iLightChIdx[3], m_img9_ch3);
			break;
		case 10:
			SetLight(m_iLightChIdx[0], m_img10_ch0);
			SetLight(m_iLightChIdx[1], m_img10_ch1);
			SetLight(m_iLightChIdx[2], m_img10_ch2);
			SetLight(m_iLightChIdx[3], m_img10_ch3);
			break;
		case 11:
			SetLight(m_iLightChIdx[0], m_img11_ch0);
			SetLight(m_iLightChIdx[1], m_img11_ch1);
			SetLight(m_iLightChIdx[2], m_img11_ch2);
			SetLight(m_iLightChIdx[3], m_img11_ch3);
			break;
		}
		
		test = _TimerCounter;


		while(TRUE)
		{
			if((_TimerCounter - test) > 15)
			{
				break;
			}
		}


		if(m_bIsColor)
			m_cCam.Grab(cam_idx, grab_idx, i, m_VisMod->GetHalImage(cam_idx, grab_idx, i), m_bIsColor, 
						m_VisMod->GetHalRedImage(cam_idx, grab_idx, i), m_VisMod->GetHalGreenImage(cam_idx, grab_idx, i), m_VisMod->GetHalBlueImage(cam_idx, grab_idx, i));
		else
			m_cCam.Grab(cam_idx, grab_idx, i, m_VisMod->GetHalImage(cam_idx, grab_idx, i));

	}

	count_seconds(&EndSecond);

	EndSecond = EndSecond - StartSecond;
	m_lGrabTime = (long)(((double)EndSecond[0]) * 1000);

	//20150223 by
	COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[0], FALSE);
	COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[1], FALSE);
	COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[2], FALSE);
	if(m_iLightOnOffIdx[3] > VISION_CAM_NONE)
		COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[3], FALSE);
	/*
	COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[0], FALSE);
	COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[1], FALSE);
	COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[2], FALSE);
	if(m_iLightOnOffIdx[3] > VISION_CAM_NONE)
		COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[3], FALSE);
	*/
#endif
}


void CFilmInsDlg::InspectProcess(int cam_idx, int prc_idx, int map_Idx)
{
	HTuple StartSecond, EndSecond;

	typeInspectPara ins_para;
	ins_para = m_VisMod->GetInspectPara(0);	

	count_seconds(&StartSecond);

	//NG review data
	memset(m_sNGLotID[0], NULL, sizeof(m_sNGLotID));
	memset(m_sNGDate[0], NULL, sizeof(m_sNGDate));
	m_iNGDeviceIdx[0] = 0;

	m_iRslt[0] = 1;

	m_iRslt[0] = m_VisMod->Inspection(cam_idx, prc_idx);

	//20150129 by 하부를 별도로 나눌 필요가 없어짐.
	/*
	switch(m_VisMod->GetVisIndex())
	{
	case VISION_CAM_TOP1:
	case VISION_CAM_BOTTOM1:
	case VISION_CAM_TOP2:
		m_iRslt[0] = m_VisMod->Inspection(cam_idx, prc_idx);
		break;
	case VISION_CAM_BOTTOM2:
		m_iRslt[0] = m_VisMod->Inspection_Bottom(cam_idx, prc_idx);
		break;
	default:
		AfxMessageBox("Error : invalid index(CFilmInsDlg::NGViewDisp0)!");
		break;
	}
	*/
	
	count_seconds(&EndSecond);

	EndSecond = EndSecond - StartSecond;
	m_lProcessTime = (long)(((double)EndSecond[0]) * 1000);

	disp_obj(*m_VisMod->GetHalImage(0, prc_idx, map_Idx), m_dlgImgRect[0]->GetView());
}

void CFilmInsDlg::InspectResult(int grab_proc_type, int buf_idx)
{
	char temp[100];
	int i;
	
	typeSystemPara sys_para;
	sys_para = m_VisMod->GetSystemPara(0);

	typeMainSystemRslt sys_rslt;
	sys_rslt = m_VisMod->GetMainSysRslt();
	
	typeInspectRslt ins_rslt;
	ins_rslt = m_VisMod->GetInspectRslt(0);
	
	//typeShiftRslt 사용하지 않음
	typeShiftRslt shift_rslt;
	shift_rslt = m_VisMod->GetShiftRslt(0, buf_idx);
	
	typeLotInfo lot_info;
	
	switch(grab_proc_type)
	{
	case 0:
		sys_rslt.m_lCnt[4] = m_lGrabTime;
		break;
		//Process
	case 1:
		//Lot info.
		lot_info = m_VisMod->GetLotInfo();

		for(i = 0; (i < m_VisMod->GetCOFNum(0)) && (i < SUB_RGN_MAX_NUM); i++)
		{
			//total
			sys_rslt.m_lCnt[1]++;
			lot_info.m_lTot++;
			
			if(1 == ins_rslt.m_iRslt[i])
			{
				if(2 == ins_rslt.m_iRsltChipFind[i])
				{
					//punch
					lot_info.m_lPunch++;
				}
				else
				{
					//good
					lot_info.m_lGood++;
				}
				
				//good
				sys_rslt.m_lCnt[2]++;
			}
			else
			{
				//ng
				sys_rslt.m_lCnt[3]++;
				lot_info.m_lNG++;
			}
		}
		sys_rslt.m_lCnt[5] = m_lProcessTime;
		
		lot_info.m_fRatio = (int)( ((float)lot_info.m_lGood) / ((float)(lot_info.m_lGood + lot_info.m_lNG)) * 100000 );
		lot_info.m_fRatio = lot_info.m_fRatio * 0.001;
		m_VisMod->SetLotInfo(lot_info);
		LotDisp(lot_info);

		//Set result/////////////////////////////
		m_iNGViewIdx[0]++;

		if(NG_VIEW_MAX_NUM <= m_iNGViewIdx[0])
		{
			m_iNGViewIdx[0] = 0;
		}
		//Set image
		m_VisMod->SetHalNGImage(0, buf_idx, m_iNGViewIdx[0]);
		//Set result region
		//(int cam_idx, int ng_view_idx)
		m_VisMod->SetInspectNGRsltRgn(0, m_iNGViewIdx[0]);
		//Set result data
		m_VisMod->SetInspectNGRslt(0, m_iNGViewIdx[0], shift_rslt, ins_rslt); 

		//Display
		//(int cam_idx, int buf_idx, int ng_view_idx, BOOL auto_mode, BOOL main_view, long ng_view_start)

		//결과 디스플레이 옵션 설정때문 디스플레이 한다. //20110509 lee
		if(sys_para.m_bPara[33])
		{
			ResultCheck_Display(0, buf_idx, m_iNGViewIdx[0], TRUE, TRUE, FALSE, TRUE ,FALSE);			
			//NG view
			ResultCheck_Display(0, buf_idx, m_iNGViewIdx[0], TRUE, FALSE, FALSE, TRUE ,FALSE);
		}
		else
		{
			ResultCheck_Display(0, buf_idx, m_iNGViewIdx[0], TRUE, TRUE, FALSE, FALSE ,FALSE);			
			//NG view
			ResultCheck_Display(0, buf_idx, m_iNGViewIdx[0], TRUE, FALSE, FALSE, FALSE ,FALSE);
		}
	
		break;
	}

	sprintf(temp, "%ld", sys_rslt.m_lCnt[1]);
	m_label_tot.SetCaption(temp);

	sprintf(temp, "%ld ", sys_rslt.m_lCnt[2]);
	m_label_good.SetCaption(temp);
	
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[3]);
	m_label_ng.SetCaption(temp);
	
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[4]);
	m_label_gtime.SetCaption(temp);
	
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[5]);
	m_label_ptime.SetCaption(temp);
	
	m_VisMod->SetMainSysRslt(sys_rslt);
	
	//Monitoring
	sprintf(temp, "%d", m_iGrbIdx[0]);
	GetDlgItem(IDC_STATIC_GRABIDX)->SetWindowText(temp);
	
	sprintf(temp, "%d", m_iBufIdx[0]);
	GetDlgItem(IDC_STATIC_BUFFERIDX)->SetWindowText(temp);
	
	sprintf(temp, "%d", m_iPrcIdx[0]);
	GetDlgItem(IDC_STATIC_PRCESSIDX)->SetWindowText(temp);

	sprintf(temp, "%d", m_iNGViewIdx[0]);
	GetDlgItem(IDC_STATIC_NGVIEWIDX)->SetWindowText(temp);
}

BOOL CFilmInsDlg::WriteLogStatus(CString path, CString file, CString bak_file, CString log, long limit_line)
{
	// check message level
	FILE*	stream;
	char	strFile[MAX_PATH], strToFile[MAX_PATH];
    long	result, line;
	char	szTime[DAT_STR_LENG]; 

	char buffer[DAT_STR_LENG];

	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);

	sprintf(strFile, "%s\\%s", path, file); 

	if((stream = fopen( strFile, "a+" )) == NULL)
	{
		HANDLE fd = CreateFile( strFile,
							GENERIC_READ|GENERIC_WRITE,
							//GENERIC_WRITE,
							FILE_SHARE_READ|FILE_SHARE_WRITE,
							NULL,
							OPEN_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
		stream = fopen( strFile, "a+" );
		if(stream == NULL)
		{
			//20150206 by IMSI
			//AfxMessageBox("Error : log file open fail!(WriteLogStatus)" );
			CloseHandle(fd);
			return FALSE;
		}

		CloseHandle(fd);
	}

	//Check first time
    result = fseek(stream, 0L, SEEK_SET);
    if(result)
	{
		fclose(stream);
		return FALSE;
	}
	line = 0;
	while(fgets(buffer, DAT_STR_LENG, stream)) 
	{
		line++;
		if(1 < line)
		{
			break;
		}
	}

    result = fseek(stream, 0L, SEEK_END);
    if(result)
	{
		fclose(stream);
		return FALSE;
	}

	memset(szTime, NULL, sizeof(szTime));
	sprintf(szTime, "%s", log);
    fputs(szTime, stream);
	fputs("\n", stream);

    result = fseek(stream, 0L, SEEK_SET);
    if(result)
	{
		fclose(stream);
		return FALSE;
	}

	//Check limit line
	line = 0;
	if(limit_line)
	{
		while(fgets(buffer, DAT_STR_LENG, stream)) line++;
		if(line > limit_line)
		{
			sprintf(strToFile, "%s\\%s", path, bak_file); 
			CopyFile(strFile, strToFile, FALSE);
			fclose(stream);
			DeleteFile(strFile);
		}
		else
		{
			fclose(stream);
		}
	}
	else
	{
		fclose(stream);
	}

	return TRUE;
}


BOOL CFilmInsDlg::WriteLogAVI(CString path, CString file, CString bak_file, CString log, long limit_line)
{
	// check message level
	FILE*	stream;
	char	strFile[MAX_PATH], strToFile[MAX_PATH];
    long	result, line;
	char	szTime[DAT_STR_LENG]; 

	char buffer[DAT_STR_LENG];

	sprintf(strFile, "%s\\%s", path, file); 

	if((stream = fopen( strFile, "a+" )) == NULL)
	{
		HANDLE fd = CreateFile( strFile,
							GENERIC_READ|GENERIC_WRITE,
							//GENERIC_WRITE,
							FILE_SHARE_READ|FILE_SHARE_WRITE,
							NULL,
							OPEN_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
		stream = fopen( strFile, "a+" );
		if(stream == NULL)
		{
			//20150206 by IMSI
			//AfxMessageBox("Error : log file open fail!(WriteLogAVI)" );
			CloseHandle(fd);
			return FALSE;
		}

		CloseHandle(fd);
	}

	//Check first time
    result = fseek(stream, 0L, SEEK_SET);
    if(result)
	{
		fclose(stream);
		return FALSE;
	}
	line = 0;
	while(fgets(buffer, DAT_STR_LENG, stream)) 
	{
		line++;
		if(1 < line)
		{
			break;
		}
	}
	if(0 == line)
	{
	    result = fseek(stream, 0L, SEEK_END);
		if(result)
		{
			fclose(stream);
			return FALSE;
		}
		memset(szTime, NULL, sizeof(szTime));
		sprintf(szTime, "PC,CAM,LOTID,DATE,TIME,DEVICE_INDEX,DEFECT_INDEX,DEFECT_TYPE,AREA,SIZEX,SIZEY,POSX,POSY");
		fputs(szTime, stream);
		fputs("\n", stream);	
	}

    result = fseek(stream, 0L, SEEK_END);
    if(result)
	{
		fclose(stream);
		return FALSE;
	}

	memset(szTime, NULL, sizeof(szTime));
	sprintf(szTime, "%s", log);
    fputs(szTime, stream);
	fputs("\n", stream);

    result = fseek(stream, 0L, SEEK_SET);
    if(result)
	{
		fclose(stream);
		return FALSE;
	}

	//Check limit line
	line = 0;
	if(limit_line)
	{
		while(fgets(buffer, DAT_STR_LENG, stream)) line++;
		if(line > limit_line)
		{
			sprintf(strToFile, "%s\\%s", path, bak_file); 
			CopyFile(strFile, strToFile, FALSE);
			fclose(stream);
			DeleteFile(strFile);
		}
		else
		{
			fclose(stream);
		}
	}
	else
	{
		fclose(stream);
	}

	return TRUE;
}

void CFilmInsDlg::WriteStatusList(LPCTSTR lpszItem)
{
	//Log
	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);
	char path[MAX_PATH];
	char temp[DAT_STR_LENG];
	char name[DAT_STR_LENG];
	CString Dir;

	memset(temp, NULL, DAT_STR_LENG);
	sprintf(temp, "[%02d:%02d:%02d]%s", lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond, lpszItem);
  
	memset(name, NULL, sizeof(temp));
	sprintf(name, "System_Log_%04d%02d%02d.txt", lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);

	m_list_rslt.InsertItem(0, temp);

	if(LIST_MONI_NUM <= m_list_rslt.GetItemCount())
	{
		m_list_rslt.DeleteItem(LIST_MONI_NUM - 1);
	}

	memset(path, NULL, MAX_PATH);
	sprintf(path, "%s\\%04d%02d%02d\\SYSTEM_LOG\\", m_strLogPath, lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);

	Dir.Format("%s", path);
	MakeDirectories(Dir);

	WriteLogStatus(path, name, "System_Log.bak", temp, 0);
}



double CFilmInsDlg::CalculateContrastValues()
{
	// Setting parameter
	int AREA_SIZE = 40;				// 체크 하는 영역의 크기 width & height = AREA_SIZE x 2
	int CHECK_CNT = 5;				// 가로 세로 CHECK_CNT x CHECK_CNT 개의 영역 계산
	HTuple ldGray = HTuple(6);
	HTuple direction = HTuple(0);

	int colSize = m_lWidth[0] / (CHECK_CNT+1);
	int rowSize = m_lHeight[0] / (CHECK_CNT+1);
	HTuple total;
	HRegion rect;
	double contrast = .0;
	double tmp = .0;

	set_color(m_dlgImgRect[0]->GetView(), HTuple("red"));

	total = HTuple(.0);
	for(int col = 1 ; col < CHECK_CNT + 1; col++)
	{
		int cCol = colSize * col;
		HTuple contrastSum = HTuple(.0);
		for(int row = 1; row < CHECK_CNT + 1; row++)
		{
			int cRow = rowSize * row;
			rect = HRegion::GenRectangle1(cRow - AREA_SIZE, cCol - AREA_SIZE, cRow + AREA_SIZE, cCol + AREA_SIZE);
			rect.DispRegion(m_dlgImgRect[0]->GetView());

			rect.CoocFeatureImage(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]),
				ldGray,
				direction,
				&tmp, // ignore Correlation
				&tmp, // ignore Homogeneity
				&contrast);

			//mean = rect.Intensity(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]), &deviation);
			set_tposition(m_dlgImgRect[0]->GetView(), cRow + AREA_SIZE, cCol);
			write_string(m_dlgImgRect[0]->GetView(), HTuple(contrast));
			contrastSum = contrastSum + contrast;
		}
		set_tposition(m_dlgImgRect[0]->GetView(), m_lHeight[0] - 150, cCol);
		write_string(m_dlgImgRect[0]->GetView(), HTuple(contrastSum / HTuple(CHECK_CNT)));
		total = total + contrastSum;
	}

	total = total/HTuple(CHECK_CNT*CHECK_CNT);

	set_tposition(m_dlgImgRect[0]->GetView(), m_lHeight[0] - 150, 50);
	write_string(m_dlgImgRect[0]->GetView(), HTuple("Column Avg:"));
	set_tposition(m_dlgImgRect[0]->GetView(), m_lHeight[0] - 100, 50);
	write_string(m_dlgImgRect[0]->GetView(), HTuple("Total Avg: ").Concat(total));

	double result;
	tuple_real(total, &result);
	return result;
}


int CFilmInsDlg::CalculateGrayValues()
{
	// Setting parameter
	int AREA_SIZE = 40;				// 체크 하는 영역의 크기 width & height = AREA_SIZE x 2
	int CHECK_CNT = 5;				// 가로 세로 CHECK_CNT x CHECK_CNT 개의 영역 계산

	int colSize = m_lWidth[0] / (CHECK_CNT+1);
	int rowSize = m_lHeight[0] / (CHECK_CNT+1);
	HTuple mean, total;
	HRegion rect;
	double deviation = .0;

	set_color(m_dlgImgRect[0]->GetView(), HTuple("orange red"));

	total = HTuple(.0);
	for(int col = 1 ; col < CHECK_CNT + 1; col++)
	{
		int cCol = colSize * col;
		HTuple meanSum = HTuple(.0);
		for(int row = 1; row < CHECK_CNT + 1; row++)
		{
			int cRow = rowSize * row;
			rect = HRegion::GenRectangle1(cRow - AREA_SIZE, cCol - AREA_SIZE, cRow + AREA_SIZE, cCol + AREA_SIZE);
			rect.DispRegion(m_dlgImgRect[0]->GetView());
			mean = rect.Intensity(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]), &deviation);
			set_tposition(m_dlgImgRect[0]->GetView(), cRow + AREA_SIZE, cCol);
			write_string(m_dlgImgRect[0]->GetView(), HTuple(mean));
			meanSum = meanSum + mean;
		}
		set_tposition(m_dlgImgRect[0]->GetView(), m_lHeight[0] - 150, cCol);
		write_string(m_dlgImgRect[0]->GetView(), HTuple(meanSum / HTuple(CHECK_CNT)));
		total = total + meanSum;
	}
	set_tposition(m_dlgImgRect[0]->GetView(), m_lHeight[0] - 150, 50);
	write_string(m_dlgImgRect[0]->GetView(), HTuple("Column Avg:"));
	set_tposition(m_dlgImgRect[0]->GetView(), m_lHeight[0] - 100, 50);
	write_string(m_dlgImgRect[0]->GetView(), HTuple("Total Avg: ").Concat(total/HTuple(CHECK_CNT*CHECK_CNT)));

	Hlong gray_total;
	tuple_round(total, &gray_total);
	return (int) gray_total / (CHECK_CNT*CHECK_CNT);
}

void CFilmInsDlg::UpdateLight(BOOL update)
{
	typeInspectPara ins_para;
	ins_para = m_VisMod->GetInspectPara(0);	

	if(update)
	{
		//Map#0
		ins_para.m_iLightVar[0][0] = m_img0_ch0;//use 1 instead of 0
		ins_para.m_iLightVar[0][1] = m_img0_ch1;
		ins_para.m_iLightVar[0][2] = m_img0_ch2;
		ins_para.m_iLightVar[0][3] = m_img0_ch3;

		//Map#1
		ins_para.m_iLightVar[1][0] = m_img1_ch0;
		ins_para.m_iLightVar[1][1] = m_img1_ch1;
		ins_para.m_iLightVar[1][2] = m_img1_ch2;
		ins_para.m_iLightVar[1][3] = m_img1_ch3;

		//Map#2
		ins_para.m_iLightVar[2][0] = m_img2_ch0;
		ins_para.m_iLightVar[2][1] = m_img2_ch1;
		ins_para.m_iLightVar[2][2] = m_img2_ch2;
		ins_para.m_iLightVar[2][3] = m_img2_ch3;

		//Map#3
		ins_para.m_iLightVar[3][0] = m_img3_ch0;
		ins_para.m_iLightVar[3][1] = m_img3_ch1;
		ins_para.m_iLightVar[3][2] = m_img3_ch2;
		ins_para.m_iLightVar[3][3] = m_img3_ch3;

		//Map#4
		ins_para.m_iLightVar[4][0] = m_img4_ch0;
		ins_para.m_iLightVar[4][1] = m_img4_ch1;
		ins_para.m_iLightVar[4][2] = m_img4_ch2;
		ins_para.m_iLightVar[4][3] = m_img4_ch3;

		//Map#5
		ins_para.m_iLightVar[5][0] = m_img5_ch0;
		ins_para.m_iLightVar[5][1] = m_img5_ch1;
		ins_para.m_iLightVar[5][2] = m_img5_ch2;
		ins_para.m_iLightVar[5][3] = m_img5_ch3;

		//Map#6
		ins_para.m_iLightVar[6][0] = m_img6_ch0;
		ins_para.m_iLightVar[6][1] = m_img6_ch1;
		ins_para.m_iLightVar[6][2] = m_img6_ch2;
		ins_para.m_iLightVar[6][3] = m_img6_ch3;

		//Map#7
		ins_para.m_iLightVar[7][0] = m_img7_ch0;
		ins_para.m_iLightVar[7][1] = m_img7_ch1;
		ins_para.m_iLightVar[7][2] = m_img7_ch2;
		ins_para.m_iLightVar[7][3] = m_img7_ch3;

		//Map#8
		ins_para.m_iLightVar[8][0] = m_img8_ch0;
		ins_para.m_iLightVar[8][1] = m_img8_ch1;
		ins_para.m_iLightVar[8][2] = m_img8_ch2;
		ins_para.m_iLightVar[8][3] = m_img8_ch3;

		//Map#9
		ins_para.m_iLightVar[9][0] = m_img9_ch0;
		ins_para.m_iLightVar[9][1] = m_img9_ch1;
		ins_para.m_iLightVar[9][2] = m_img9_ch2;
		ins_para.m_iLightVar[9][3] = m_img9_ch3;

		//Map#10
		ins_para.m_iLightVar[10][0] = m_img10_ch0;
		ins_para.m_iLightVar[10][1] = m_img10_ch1;
		ins_para.m_iLightVar[10][2] = m_img10_ch2;
		ins_para.m_iLightVar[10][3] = m_img10_ch3;

		//Map#11
		ins_para.m_iLightVar[11][0] = m_img11_ch0;
		ins_para.m_iLightVar[11][1] = m_img11_ch1;
		ins_para.m_iLightVar[11][2] = m_img11_ch2;
		ins_para.m_iLightVar[11][3] = m_img11_ch3;

		m_VisMod->SetInspectPara(0, ins_para);
	}
	else
	{
		//20150223 by
		if(VISION_CAM_TOP1 != m_VisMod->GetVisIndex())
		{
			for(int i = 0; i < IMAGE_MAP_MAX_NUM; i++)
				ins_para.m_iLightVar[i][3] = 0;
		}

		/*
		if( (VISION_CAM_TOP2 == m_VisMod->GetVisIndex()) ||
			(VISION_CAM_BOTTOM2 == m_VisMod->GetVisIndex()) )
		{
			for(int i = 0; i < IMAGE_MAP_MAX_NUM; i++)
				ins_para.m_iLightVar[i][3] = 0;
		}
		*/

		//Map#0
		m_img0_ch0 = ins_para.m_iLightVar[0][0];//use 1 instead of 0
		m_img0_ch1 = ins_para.m_iLightVar[0][1];
		m_img0_ch2 = ins_para.m_iLightVar[0][2];
		m_img0_ch3 = ins_para.m_iLightVar[0][3];

		//Map#1
		m_img1_ch0 = ins_para.m_iLightVar[1][0];
		m_img1_ch1 = ins_para.m_iLightVar[1][1];
		m_img1_ch2 = ins_para.m_iLightVar[1][2];
		m_img1_ch3 = ins_para.m_iLightVar[1][3];

		//Map#2
		m_img2_ch0 = ins_para.m_iLightVar[2][0];
		m_img2_ch1 = ins_para.m_iLightVar[2][1];
		m_img2_ch2 = ins_para.m_iLightVar[2][2];
		m_img2_ch3 = ins_para.m_iLightVar[2][3];

		//Map#3
		m_img3_ch0 = ins_para.m_iLightVar[3][0];
		m_img3_ch1 = ins_para.m_iLightVar[3][1];
		m_img3_ch2 = ins_para.m_iLightVar[3][2];
		m_img3_ch3 = ins_para.m_iLightVar[3][3];

		//Map#4
		m_img4_ch0 = ins_para.m_iLightVar[4][0];
		m_img4_ch1 = ins_para.m_iLightVar[4][1];
		m_img4_ch2 = ins_para.m_iLightVar[4][2];
		m_img4_ch3 = ins_para.m_iLightVar[4][3];

		//Map#5
		m_img5_ch0 = ins_para.m_iLightVar[5][0];
		m_img5_ch1 = ins_para.m_iLightVar[5][1];
		m_img5_ch2 = ins_para.m_iLightVar[5][2];
		m_img5_ch3 = ins_para.m_iLightVar[5][3];

		//Map#6
		m_img6_ch0 = ins_para.m_iLightVar[6][0];
		m_img6_ch1 = ins_para.m_iLightVar[6][1];
		m_img6_ch2 = ins_para.m_iLightVar[6][2];
		m_img6_ch3 = ins_para.m_iLightVar[6][3];

		//Map#7
		m_img7_ch0 = ins_para.m_iLightVar[7][0];
		m_img7_ch1 = ins_para.m_iLightVar[7][1];
		m_img7_ch2 = ins_para.m_iLightVar[7][2];
		m_img7_ch3 = ins_para.m_iLightVar[7][3];

		//Map#8
		m_img8_ch0 = ins_para.m_iLightVar[8][0];
		m_img8_ch1 = ins_para.m_iLightVar[8][1];
		m_img8_ch2 = ins_para.m_iLightVar[8][2];
		m_img8_ch3 = ins_para.m_iLightVar[8][3];

		//Map#9
		m_img9_ch0 = ins_para.m_iLightVar[9][0];
		m_img9_ch1 = ins_para.m_iLightVar[9][1];
		m_img9_ch2 = ins_para.m_iLightVar[9][2];
		m_img9_ch3 = ins_para.m_iLightVar[9][3];

		//Map#10
		m_img10_ch0 = ins_para.m_iLightVar[10][0];
		m_img10_ch1 = ins_para.m_iLightVar[10][1];
		m_img10_ch2 = ins_para.m_iLightVar[10][2];
		m_img10_ch3 = ins_para.m_iLightVar[10][3];

		//Map#11
		m_img11_ch0 = ins_para.m_iLightVar[11][0];
		m_img11_ch1 = ins_para.m_iLightVar[11][1];
		m_img11_ch2 = ins_para.m_iLightVar[11][2];
		m_img11_ch3 = ins_para.m_iLightVar[11][3];
	}

	UpdateData(update);
}

void CFilmInsDlg::SetLight(int ch, int var)
{
	int rslt = 0;
	int bit = 128;
	int stNum = 0;
	HANDLE hDevice = NULL;

	if(ch < 0)
		return;

	switch(ch)
	{
	case 0:	// 0 ~ 6 ch = m_hComiDevice64
		stNum = 8;
		hDevice = m_hComiDevice64;
		break;
	case 1: 
		stNum = 17;
		hDevice = m_hComiDevice64;
		break;
	case 2:
		stNum = 26;
		hDevice = m_hComiDevice64;
		break;
	case 3:
		stNum = 35;
		hDevice = m_hComiDevice64;
		break;
	case 4:
		stNum = 44;
		hDevice = m_hComiDevice64;
		break;
	case 5:
		stNum = 53;
		hDevice = m_hComiDevice64;
		break;
	case 6:
		stNum = 62;
		hDevice = m_hComiDevice64;
		break;
	case 7:	// 7 ~ 9 ch = m_hComiDevice32
		stNum = 8;
		hDevice = m_hComiDevice32;
		break;
	case 8:
		stNum = 17;
		hDevice = m_hComiDevice32;
		break;
	case 9:
		stNum = 26;
		hDevice = m_hComiDevice32;
		break;
	default:
		AfxMessageBox("Error : Invaild Light Index (CFilmInsDlg::SetLight())");
		return;

	}

	for(int i = 0; i < 8; i++)
	{
		rslt = var / bit;
		COMI_DO_PutOne(hDevice, stNum - i, !rslt);
		var = var - (rslt * bit);

		bit /= 2;
	}
}
/*
void CFilmInsDlg::SetLight(HANDLE hdev, int ch, int var)
{
	int rslt = 0;
	int bit = 128;
	int stNum = 0;
	HANDLE hDevice = NULL;
	hDevice = hdev;

	if(ch < 0)
		return;

	for(int i = 0; i < LIGHT_NUM_PER_1CH; i++)
	{
		rslt = var / bit;
		COMI_DO_PutOne(hDevice, ch - i, !rslt);
		var = var - (rslt * bit);

		bit /= 2;
	}
}
*/
void CFilmInsDlg::UpdateMapButton(int idx)
{
	m_map0.SetForeColor(RGB(255, 255, 255));
	m_map1.SetForeColor(RGB(255, 255, 255));
	m_map2.SetForeColor(RGB(255, 255, 255));
	m_map3.SetForeColor(RGB(255, 255, 255));
	m_map4.SetForeColor(RGB(255, 255, 255));
	m_map5.SetForeColor(RGB(255, 255, 255));
	m_map6.SetForeColor(RGB(255, 255, 255));
	m_map7.SetForeColor(RGB(255, 255, 255));

	switch(idx)
	{
	case 0:
		m_map0.SetForeColor(RGB(255, 255, 0));  
		break;
	case 1:
		m_map1.SetForeColor(RGB(255, 255, 0));  
		break;
	case 2:
		m_map2.SetForeColor(RGB(255, 255, 0));  
		break;
	case 3:
		m_map3.SetForeColor(RGB(255, 255, 0));  
		break;
	case 4:
		m_map4.SetForeColor(RGB(255, 255, 0));  
		break;
	case 5:
		m_map5.SetForeColor(RGB(255, 255, 0));  
		break;
	case 6:
		m_map6.SetForeColor(RGB(255, 255, 0));  
		break;
	case 7:
		m_map7.SetForeColor(RGB(255, 255, 0));  
		break;
	}
}

void CFilmInsDlg::ModelSave()
{
	char temp[MAX_MOD_NAME];
	CString str0, str1;
	
	//Model
	C_CtrlModelManageDlg *moddlg = new C_CtrlModelManageDlg(NULL,m_strDBPath, m_strRegiPath);
	//Model save
	moddlg->Open();

	sprintf(temp, "%s", moddlg->GetName());
	
	str0 = moddlg->GetName();
	str1 = moddlg->GetPath();
	if( (str0.IsEmpty()) || (str1.IsEmpty()) )
	{
		WriteStatusList("Error : invalid model!");
	}
	else
	{
		m_dlgProcessing.ShowWindow(SW_SHOW);
		HandleAllMessage();
		UpdateLight(TRUE);
		//Save setup INI file
		m_VisMod->SaveInspectSelectPara(moddlg->GetPath());
		if( m_VisMod->SaveInspectPara(moddlg->GetPath()) )
		{
			
		}
		else
		{
			WriteStatusList("Error : can't save setup parameter file!");
		}

		m_dlgProcessing.ShowWindow(SW_HIDE);
	}

	delete moddlg;

}

void CFilmInsDlg::CheckNGKind(int cam_idx, int buf_idx)
{
	//NG type만 얻어 낸다(no display).
	ResultCheck_Display(cam_idx, buf_idx, m_iNGViewIdx[0], FALSE, FALSE, TRUE, FALSE ,FALSE); //20110509 lee
}

void CFilmInsDlg::LotDisp(typeLotInfo lot)
{
	char tmp[200];

	typeLotInfo lot_info;

	lot_info = m_VisMod->GetLotInfo();

	memset(tmp, NULL, sizeof(tmp));

	sprintf(tmp, "%s", lot_info.m_sNo);
	GetDlgItem(IDC_STATIC_LOTNO)->SetWindowText(tmp);
	
	sprintf(tmp, "%s", lot_info.m_sStart);
	GetDlgItem(IDC_STATIC_LOT_STIME)->SetWindowText(tmp);

	sprintf(tmp, "%s", lot_info.m_sEnd);
	GetDlgItem(IDC_STATIC_LOT_ETIME)->SetWindowText(tmp);

	sprintf(tmp, "%ld", lot_info.m_lDeviceIdx);
	GetDlgItem(IDC_STATIC_LOT_DEVICE)->SetWindowText(tmp);

	sprintf(tmp, "%ld", lot_info.m_lTot);
	GetDlgItem(IDC_STATIC_LOT_TOT)->SetWindowText(tmp);

	sprintf(tmp, "%ld", lot_info.m_lGood);
	GetDlgItem(IDC_STATIC_LOT_GOOD)->SetWindowText(tmp);

	sprintf(tmp, "%ld", lot_info.m_lNG);
	GetDlgItem(IDC_STATIC_LOT_NG)->SetWindowText(tmp);

	sprintf(tmp, "%f", lot_info.m_fRatio);
	GetDlgItem(IDC_STATIC_LOT_RATIO)->SetWindowText(tmp);

	sprintf(tmp, "%ld", lot_info.m_lPunch);
	GetDlgItem(IDC_STATIC_LOT_PUNCH)->SetWindowText(tmp);
}


void CFilmInsDlg::WriteLot(typeLotInfo lot_info)
{
	//Log
	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);
	char path[MAX_PATH];
	char temp[DAT_STR_LENG];
	char name[DAT_STR_LENG];
	CString Dir;

	memset(temp, NULL, DAT_STR_LENG);

	memset(path, NULL, MAX_PATH);
	sprintf(path, "%s\\%04d%02d%02d\\LOT_LOG\\", m_strLogPath, lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);

	Dir.Format("%s", path);
	MakeDirectories(Dir);

	memset(temp, NULL, sizeof(temp));

	memset(name, NULL, sizeof(temp));
	sprintf(name, "Lot_Log%04d%02d%02d.txt", lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);

	sprintf(temp, "No. : %s", lot_info.m_sNo);
	WriteLogStatus(path, name, "Lot_Log.bak", temp, 0);	

	sprintf(temp, "START TIME : %s", lot_info.m_sStart);
	WriteLogStatus(path, name, "Lot_Log.bak", temp, 0);

	sprintf(temp, "END TIME : %s", lot_info.m_sEnd);
	WriteLogStatus(path, name, "Lot_Log.bak", temp, 0);

	sprintf(temp, "DEVICE ID : %ld", lot_info.m_lDeviceIdx);
	WriteLogStatus(path, name, "Lot_Log.bak", temp, 0);

	sprintf(temp, "TOTAL DEVICE : %ld", lot_info.m_lTot);
	WriteLogStatus(path, name, "Lot_Log.bak", temp, 0);

	sprintf(temp, "GOOD DEVICE : %ld", lot_info.m_lGood);
	WriteLogStatus(path, name, "Lot_Log.bak", temp, 0);

	sprintf(temp, "NG DEVICE : %ld", lot_info.m_lNG);
	WriteLogStatus(path, name, "Lot_Log.bak", temp, 0);

	sprintf(temp, "RATIO(GOOD/NG) : %f", lot_info.m_fRatio);
	WriteLogStatus(path, name, "Lot_Log.bak", temp, 0);

	sprintf(temp, "PUNCH DEVICED : %ld", lot_info.m_lPunch);
	WriteLogStatus(path, name, "Lot_Log.bak", temp, 0);
}

BOOL CFilmInsDlg::CheckDeleteDate()
{
	CString str;
	CTime now_time = CTime::GetCurrentTime();
	int temp_year, temp_month, temp_day;
	CTime delete_time;
	int TempDeleteDay=0;
	int i=0;
	temp_year=0;
	temp_month=0;
	temp_day=0;

	temp_day = now_time.GetDay();
	temp_month = now_time.GetMonth();
	temp_year = now_time.GetYear();

	m_dlgSetup->m_SysPara = m_VisMod->GetSystemPara(0);

	int save_day = m_dlgSetup->m_SysPara.m_iPara[0];
	if( (temp_day - save_day) < 0)
	{
		if(now_time.GetMonth() == 2 || now_time.GetMonth() == 4 || now_time.GetMonth() == 6 ||
			now_time.GetMonth() == 8 || now_time.GetMonth() == 9 || now_time.GetMonth() == 11)
		{
			m_iDel_Year = now_time.GetYear();
			m_iDel_Month = now_time.GetMonth() - 1;
			m_iDel_RecentDay = 31 - (save_day - temp_day);
		}
		else if(now_time.GetMonth() == 5 || now_time.GetMonth() == 7 || now_time.GetMonth() == 10 || now_time.GetMonth() == 12)
		{
			m_iDel_Year = now_time.GetYear();
			m_iDel_Month = now_time.GetMonth() - 1;
			m_iDel_RecentDay = 30 - (save_day - temp_day);
		}
		else if(now_time.GetMonth() == 3)
		{
			m_iDel_Year = now_time.GetYear();
			m_iDel_Month = now_time.GetMonth() - 1;
			m_iDel_RecentDay = 28 - (save_day - temp_day);
		}
		else if(now_time.GetMonth() == 1)
		{
			m_iDel_Year = now_time.GetYear() - 1;
			m_iDel_Month = 12;
			m_iDel_RecentDay = 31 - (save_day - temp_day);
		}
	}
	else if( (temp_day - save_day) == 0)
	{
		if(now_time.GetMonth() == 1)
		{
			m_iDel_Year = now_time.GetYear() - 1;
			m_iDel_Month = 12;
			m_iDel_RecentDay = 0;
		}
		else
		{
			m_iDel_Year = now_time.GetYear();
			m_iDel_Month = now_time.GetMonth() - 1;
			m_iDel_RecentDay = 0;
		}
	}
	else
	{
		m_iDel_Year = now_time.GetYear();
		m_iDel_Month = now_time.GetMonth();
		m_iDel_RecentDay = temp_day - save_day;
	}
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  저장 기간을 개월로 했을 경우
	int save_month = m_dlgSetup->m_SysPara.m_iPara[0];
	if( (temp_month - save_month) < 0)
	{
		m_iDel_Year = now_time.GetYear() - 1;

		for(i = 0; i <= (save_month - temp_month); i++)
		{
			if(temp_month == i+1)
			{
				m_iDel_Month = 12 - (save_month - temp_month);
			}
		}
	}
	else if( (temp_month - save_month) == 0)
	{
		m_iDel_Year = now_time.GetYear() - 1;

		m_iDel_Month = 12;
	}
	else
	{
		m_iDel_Year = now_time.GetYear();

		m_iDel_Month = temp_month - save_month;
	}
*/// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	/*  //저장 기간 3개월 일 경우
	if( (temp_month - 2) <= 0)
	{
		m_iDel_Year = now_time.GetYear() - 1;

		if(temp_month == 1)
			m_iDel_Month = 10;
		else if(temp_month == 2)
			m_iDel_Month = 11;
	}
	else
	{
		m_iDel_Year = now_time.GetYear();

		if(temp_month == 3)
		{
			m_iDel_Year = now_time.GetYear() - 1;
			m_iDel_Month = 12;
		}
		else
			m_iDel_Month = temp_month - 3;
	}
	*/
	return TRUE;
}


BOOL CFilmInsDlg::DeleteFolder(CString strFolder)
{
	CFileFind ff;
	CString strTarget = strFolder;
	CStringArray strFileArray;

	if(strTarget.Right(1) != '\\')
	{
		strTarget += '\\';
	}
	
	strTarget += "*.*";
	if(ff.FindFile(strTarget)==TRUE)
	{
		CString Filename;
		BOOL bFlag = true;
		while(bFlag == TRUE)
		{
			bFlag = ff.FindNextFile();
			Filename = ff.GetFilePath();
			if(ff.IsDots())
			{
				continue;
			}
			if(ff.IsDirectory())
			{
				DeleteFolder(Filename);
				RemoveDirectory(Filename);
			}
			else
			{
				DeleteFile(Filename);
			}
		}
	}
	ff.Close();

	return TRUE;
}


BOOL CFilmInsDlg::AutoTeachOCV()
{
	BOOL tot_rslt = TRUE;
	int rslt = 1;
	double rx, ry, rscore;
	Hlong rn;
	double offset_x, offset_y;
	Hobject ModImage, Shape, Region;
	CRect rect;
	double cx, cy;

	//Set inspect data
	m_dlgSetup->m_InsPara = m_VisMod->GetInspectPara(0);
	m_dlgSetup->m_SysPara = m_VisMod->GetSystemPara(0);

	disp_obj(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_dlgSetup->m_InsPara.m_iParaMapOCV[0]), m_dlgImgRect[0]->GetView());
	
	//Check punching & align
	rslt = m_VisMod->InspectionAlignForAutoOCVTeach(0, m_iBufIdx[0], &ModImage, &Shape, 
												   &rx, &ry, &rscore, &rn,
												   &offset_x, &offset_y);

	//결과 저장 //20100730 lee
	m_offset_x = offset_x;
	m_offset_y = offset_y;

	if(1 == rslt)
	{
		//Total char center
		cx = m_dlgSetup->m_InsPara.m_dCharCenterX[m_dlgSetup->m_InsPara.m_iParaOCV[2]];
		cy = m_dlgSetup->m_InsPara.m_dCharCenterY[m_dlgSetup->m_InsPara.m_iParaOCV[2]];

		rect = m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1];

  		m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1].top = rect.top + offset_y;
		m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1].left = rect.left + offset_x;
		m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1].bottom = rect.bottom + offset_y;
		m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1].right = rect.right + offset_x;
		
		//Teach
		if(m_dlgSetup->m_SetUpOCVDlg.ExtRegionSet())
		{
			m_dlgSetup->m_SetUpOCVDlg.ExRegionMerge();//20141216 lss
			m_dlgSetup->m_SetUpOCVDlg.ExRegionMerge();//20141216 lss

			if(m_dlgSetup->m_SetUpOCVDlg.ExtOCVTeach())
			{


			}
			else
			{
				tot_rslt = FALSE;
			}
		}
		else
		{
			tot_rslt = FALSE;
		}

		/*
		//OCV teach region 조정 
		dx = m_dlgSetup->m_InsPara.m_dCharCenterX[m_dlgSetup->m_InsPara.m_iParaOCV[2]] - cx;
		dy = m_dlgSetup->m_InsPara.m_dCharCenterY[m_dlgSetup->m_InsPara.m_iParaOCV[2]] - cy;
		m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].left = m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].left + dx;
		m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].top = m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].top + dy;
		m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].right = m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].right + dx;
		m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].bottom = m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].bottom + dy;
  		gen_rectangle1(&Region, m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].top + offset_y, 
							m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].left + offset_x, 
							m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].bottom + offset_y, 
							m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].right + offset_x); 
		*/	  

  		gen_rectangle1(&Region, m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].top + offset_y, 
							m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].left + offset_x, 
							m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].bottom + offset_y, 
							m_dlgSetup->m_InsPara.m_InsRegionOCV[0][0].right + offset_x); 		
		set_color(m_dlgImgRect[0]->GetView(), "yellow");
		disp_obj(Region, m_dlgImgRect[0]->GetView());

		//OCV teach region
  		gen_rectangle1(&Region, m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1].top, 
							m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1].left, 
							m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1].bottom, 
							m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1].right); 
		set_color(m_dlgImgRect[0]->GetView(), "green");
		disp_obj(Region, m_dlgImgRect[0]->GetView());

		//Align model
		set_color(m_dlgImgRect[0]->GetView(), "blue");
		disp_obj(Shape, m_dlgImgRect[0]->GetView());
		set_color(m_dlgImgRect[0]->GetView(), "green");
		disp_obj(ModImage, m_dlgImgRect[0]->GetView());

		//Teach영역은 윈래데로 돌려 놓는다.
		m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1] = rect;



		//Set data
		m_VisMod->SetInspectPara(0, m_dlgSetup->m_InsPara);
		m_VisMod->SetSystemPara(0, m_dlgSetup->m_SysPara);

		//Save
		ModelSave();
	}
	else if(2 == rslt)
	{
		//Punching
		//Retry after one step moving
		set_color(m_dlgImgRect[0]->GetView(), "red");
		set_tposition(m_dlgImgRect[0]->GetView(), 10, 10);
		write_string(m_dlgImgRect[0]->GetView(), HTuple(HTuple("FAIL : OCV teaching fail-punching chip!")));

		tot_rslt = FALSE;
	}
	else
	{
		//Error
		//Retry after one step moving
		set_color(m_dlgImgRect[0]->GetView(), "red");
		set_tposition(m_dlgImgRect[0]->GetView(), 10, 10);
		write_string(m_dlgImgRect[0]->GetView(), HTuple(HTuple("FAIL : OCV teaching fail-can't find model/no pattern!")));

		tot_rslt = FALSE;
	}

	//Align search
  	gen_rectangle1(&Region, m_dlgSetup->m_InsPara.m_InsRegionAlign[0][0].top, 
							m_dlgSetup->m_InsPara.m_InsRegionAlign[0][0].left, 
							m_dlgSetup->m_InsPara.m_InsRegionAlign[0][0].bottom, 
							m_dlgSetup->m_InsPara.m_InsRegionAlign[0][0].right); 
	set_color(m_dlgImgRect[0]->GetView(), "yellow");
	disp_obj(Region, m_dlgImgRect[0]->GetView());

	// 20110530 Dongwhe* BEGIN 2nd Align Search Region Display
	gen_rectangle1(&Region, m_dlgSetup->m_InsPara.m_InsRegionAlign[0][2].top, 
		m_dlgSetup->m_InsPara.m_InsRegionAlign[0][2].left, 
		m_dlgSetup->m_InsPara.m_InsRegionAlign[0][2].bottom, 
		m_dlgSetup->m_InsPara.m_InsRegionAlign[0][2].right); 
	set_color(m_dlgImgRect[0]->GetView(), "yellow");
	disp_obj(Region, m_dlgImgRect[0]->GetView());
	// 20110530 Dongwhe* END

	return tot_rslt;
}

void CFilmInsDlg::NGViewDisp()
{
	HTuple StartSecond, EndSecond;
	char temp[1000];
	char path[MAX_PATH];
	char full_path[MAX_PATH];
	int i;
	CString str, str0, str1;
	CRect sys_rect;
	CString Dir;
	Herror Herr;
	CString status;
	

	typeInspectRslt ins_rslt;
	typeInspectPara ins_para; //20110531 lee

	typeShiftRslt shift_rslt;

	typeMainSystemRslt sys_rslt;
	sys_rslt = m_VisMod->GetMainSysRslt();

	int para0 = 0;

	memset(temp, NULL, 1000);
	GetDlgItem(IDC_STATIC_NG_DATE)->SetWindowText(m_sNGDate[0]);
	GetDlgItem(IDC_STATIC_NG_LOTNO)->SetWindowText(m_sNGLotID[0]);
	sprintf(temp, "%d", m_iNGDeviceIdx[0] + 1);
	GetDlgItem(IDC_STATIC_NG_DEVICENO)->SetWindowText(temp);
	
	//NG index
 	m_iNGViewIdx[0]++;

	if(NG_VIEW_MAX_NUM <= m_iNGViewIdx[0]) 
	{
		m_iNGViewIdx[0] = 0;
	} 

	m_VisMod->SetNGViewMode(TRUE, m_iNGViewIdx[0]);

	//Read NG View image
	memset(path, NULL, sizeof(path));
	sprintf(path, "%s\\%s\\%s\\IMAGE\\NG", m_strLogPath, m_sNGDate[0], m_sNGLotID[0]);
	
	Dir.Format("%s", path);
	MakeDirectories(Dir);

	ins_para = m_VisMod->GetInspectPara(0); 

	for(i = 0; i < ins_para.m_iParaBase[1]; i++)
	{
		sprintf(full_path, "%s\\%s_VIS%d[%04d]_%d.bmp", path, m_sNGLotID[0], m_VisMod->GetVisIndex(), m_iNGDeviceIdx[0], i);
		
		set_check("~give_error");

		Herr = read_image(m_VisMod->GetHalImage(0, m_iNGViewIdx[0], i), HTuple((LPCTSTR)full_path));
		
		set_check("give_error");
		if(H_MSG_TRUE == Herr)
		{
			//AfxMessageBox("load ok"); 
		}
		else
		{
			gen_image_const(m_VisMod->GetHalNGImage(0, m_iNGViewIdx[0], i),HTuple("byte"),m_lWidth[0],m_lHeight[0]);
			//AfxMessageBox("load fail"); 
			return;
		}

		//20150107 by.
		switch(m_VisMod->GetVisIndex())
		{
			case VISION_CAM_TOP1:
			case VISION_CAM_TOP2:
				break;
			case VISION_CAM_BOTTOM:
				if(m_bIsColor)
					decompose3(*m_VisMod->GetHalImage(0, m_iNGViewIdx[0], i), 
													m_VisMod->GetHalRedImage(0, m_iNGViewIdx[0], i),
													m_VisMod->GetHalGreenImage(0, m_iNGViewIdx[0], i), 
													m_VisMod->GetHalBlueImage(0, m_iNGViewIdx[0], i));
				break;
			default:
				AfxMessageBox("Error : invalid index(CFilmInsDlg::NGViewDisp0)!");
				break;
		}
	}

	//::PostMessage(m_hWnd, USER_MSG_IMG_RECT, 0, 7);
	//Inspection/////////////////////////////////
	m_bLiveOverlayDisp = TRUE;

	count_seconds(&StartSecond);

	m_iRslt[0] = m_VisMod->Inspection(para0, m_iNGViewIdx[0]);
	//20150129 by 하부를 별도로 나눌 필요가 없어짐.
	/*
	switch(m_VisMod->GetVisIndex())
	{
	case VISION_CAM_TOP1:
	case VISION_CAM_BOTTOM1:
	case VISION_CAM_TOP2:
		m_iRslt[0] = m_VisMod->Inspection(para0, m_iNGViewIdx[0]);
		break;
	case VISION_CAM_BOTTOM2:
		m_iRslt[0] = m_VisMod->Inspection_Bottom(para0, m_iNGViewIdx[0]);
		break;
	default:
		AfxMessageBox("Error : invalid index(CFilmInsDlg::NGViewDisp0)!");
		break;
	}
	*/

	//검사 결과를 가져 온다. 
	ins_rslt = m_VisMod->GetInspectRslt(para0);
	//Device 정보를 가져 온다. 
	shift_rslt = m_VisMod->GetShiftRslt(para0, m_iNGViewIdx[0]);//20110531 lee
	
	count_seconds(&EndSecond);
	EndSecond = EndSecond - StartSecond;
	sys_rslt.m_lCnt[5] = (long)(((double)EndSecond[0]) * 1000);
	m_VisMod->SetMainSysRslt(sys_rslt);
	
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[5]);
	m_label_ptime.SetCaption(temp);

	//불량 영상 Display
	disp_obj(*m_VisMod->GetHalImage(para0, m_iNGViewIdx[0], m_iMapIdx[0]), m_dlgImgRect[para0]->GetView());//20110531 lee

	//Set result/////////////////////////////
	//Set image
	//벌써 위에서 read했기 때문에 필요없음	
	//Set result region
	m_VisMod->SetInspectNGRsltRgn(0, m_iNGViewIdx[0]);//20110531 lee
	
	//Set result data
	m_VisMod->SetInspectNGRslt(0, m_iNGViewIdx[0], shift_rslt, ins_rslt);//20110531 lee


	//두번째 인자는 사용되지 않는다. 현재 NULL로 넣었다.
	ResultCheck_Display(para0, NULL, m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE, TRUE);//main view //20110509 lee
	ResultCheck_Display(para0, NULL, m_iNGViewIdx[0], FALSE, FALSE, FALSE, TRUE, TRUE);//ng view  //20110509 lee	

	m_VisMod->SetNGViewMode(TRUE,-1);//LSS 20141122
}

BOOL CFilmInsDlg::Test()
{
	BOOL rslt = TRUE;

	HTuple gray;
	HTuple wd, ht;

	//Halcon image initialize
	HTuple StartSecond, EndSecond;

	typeInspectRslt ins_rslt;
	typeShiftRslt shift_rslt;
	
	typeMainSystemRslt sys_rslt;
	sys_rslt = m_VisMod->GetMainSysRslt();

	count_seconds(&StartSecond);
	rslt = m_VisMod->Inspection(0, m_iBufIdx[0]);
	
	ins_rslt = m_VisMod->GetInspectRslt(0);
	shift_rslt = m_VisMod->GetShiftRslt(0, m_iBufIdx[0]);
	
	count_seconds(&EndSecond);
	EndSecond = EndSecond - StartSecond;
	sys_rslt.m_lCnt[5] = (long)(((double)EndSecond[0]) * 1000);
	m_VisMod->SetMainSysRslt(sys_rslt);
	
	disp_obj(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[0]->GetView());
	
	//Set result/////////////////////////////
	//Set image
	m_VisMod->SetHalNGImage(0, m_iBufIdx[0], m_iNGViewIdx[0]);
	
	//Set result region
	m_VisMod->SetInspectNGRsltRgn(0, m_iNGViewIdx[0]);
	
	//Set result data
	m_VisMod->SetInspectNGRslt(0, m_iNGViewIdx[0], shift_rslt, ins_rslt);
	
	ResultCheck_Display(0, m_iBufIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//main view //20110509 lee
	ResultCheck_Display(0, m_iBufIdx[0], m_iNGViewIdx[0], FALSE, FALSE, FALSE, TRUE ,FALSE);//ng vie  //20110509 lee

	return rslt;
}


void CFilmInsDlg::WriteTCPList(LPCTSTR lpszItem, BOOL IsRCVorSND)
{
	//Log
	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);
	char path[MAX_PATH];
	char temp[DAT_STR_LENG];
	char name[DAT_STR_LENG];
	CString Dir;

	memset(temp, NULL, DAT_STR_LENG);
	//sprintf(temp, "[%02d:%02d:%02d]%s", lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond, lpszItem);
	if(IsRCVorSND)
		sprintf(temp, "[%02d:%02d:%02d %s]%s", lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond,"CTRL->VIS", lpszItem);
	else
		sprintf(temp, "[%02d:%02d:%02d %s]%s", lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond,"VIS->CTRL", lpszItem);	
  
	memset(name, NULL, sizeof(temp));
	sprintf(name, "TCP_Log_%04d%02d%02d.txt", lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);

	m_list_tcp.InsertItem(0, temp);

	if(LIST_MONI_NUM <= m_list_tcp.GetItemCount())
	{
		m_list_tcp.DeleteItem(LIST_MONI_NUM - 1);
	}

	memset(path, NULL, MAX_PATH);
	sprintf(path, "%s\\%04d%02d%02d\\SYSTEM_LOG\\", m_strLogPath, lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay);

	Dir.Format("%s", path);
	MakeDirectories(Dir);

	WriteLogStatus(path, name, "TCP_Log.bak", temp, 0);
}

BOOL CFilmInsDlg::ScrollCheck(int cur_scroll_idx)
{
	BOOL rslt = FALSE;
	
	if( (cur_scroll_idx >= m_iNGScrollIdx[0]) && ( (0 <= m_iNGSubViewIdx[0]) && (IMAGE_NG_NUM > m_iNGSubViewIdx[0]) ) )
	{
		rslt = TRUE;
	}
	
	return rslt;
}

void CFilmInsDlg::ScrollCheck()
{
	m_iNGSubViewIdx[0]++;
}

/*
//IMSI 이미지 저장 스레드
void CFilmInsDlg::ResultLog(int cam_idx, int buf_idx, int ng_view_idx, int cof_idx, int defect_idx, int defect_type,
							long area, double cx, double cy, double sizex, double sizey, BOOL save)
{
	char temp[DAT_STR_LENG];
	char path[MAX_PATH];
	int i;

	Hobject SubRgn, TotRgn;
	HTuple Num;
	Herror herr;
	CString status, strTemp, Dir;
	
	typeSystemPara sys_para;
	sys_para = m_VisMod->GetSystemPara(0);

	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);

	if( (cam_idx == 1) && (defect_type) )
	{
		Sleep(1);
	}

	if(defect_type)
	{
		//m_strLogPath
		memset(path, NULL, sizeof(path));
		sprintf(path, "%s\\%s\\%s\\", m_strLogPath, m_strStartTime, m_strLotNo);
		Dir.Format("%s", path);
		MakeDirectories(Dir);

		//Log
		//PC,LOTID,DEVICE_IDX,DEFECT_IDX,POS_X,POS_Y,
		sprintf(temp, "%d,0,%s,%04d-%02d-%02d,%02d:%02d:%02d,%d,%d,%d,%f,%f,%f,%f,%f", m_VisMod->GetVisIndex(), m_strLotNo, 
											lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay,
											lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond,
											m_iPrcDevIdx[0] + cof_idx, defect_idx, defect_type,
											(float)area, sizex, sizey, cx, cy);

		WriteLogAVI(path, "Defect_File.csv", "Defect_File.bak", temp, 0);

		//Save NG image
		if(save)
		{
			//NG image
			if(sys_para.m_bPara[31])
			{
				m_bSaveImageStart = TRUE;
			}
		}
	}
	else
	{
		if(save)
		{
			//Save good image
			for(i = 0; (i < m_VisMod->GetMapNum(0)) && (i < IMAGE_MAP_MAX_NUM); i++)
			{
				if(i != 3) continue; //20110608 lee
				//Good
				if(sys_para.m_bPara[30])
				{
					memset(path, NULL, sizeof(path));
					sprintf(path, "%s\\%s\\%s\\IMAGE\\GOOD", m_strLogPath, m_strStartTime, m_strLotNo);

					Dir.Format("%s", path);
					MakeDirectories(Dir);
					sprintf(path, "%s\\%sPC%d[%d]_%d.bmp", path, m_strLotNo, cam_idx, m_iPrcDevIdx[0] + cof_idx, i);
				
					set_check("~give_error");
					herr = write_image(*m_VisMod->GetHalImage(0, m_iPrcIdx[0], i), "bmp", 0, path);
					set_check("give_error");
					if(H_MSG_TRUE == herr)
					{
					}
					else
					{
						strTemp.Format(_T("이미지 저장에 실패하였습니다. 하드디스크 용량 또는 저장 경로를 확인해 주세요!!"));
						AfxMessageBox(strTemp);
						status.Format("Halcon Error Occured at write_image_01!");
						m_VisMod->WriteErrorList((LPCTSTR)status);
						return;
					}
				}
			}
		}
	}
}
*/

void CFilmInsDlg::ResultLog(int cam_idx, int buf_idx, int ng_view_idx, int cof_idx, int defect_idx, int defect_type,
							long area, double cx, double cy, double sizex, double sizey, BOOL save)
{
#ifdef _CAM
	char temp[DAT_STR_LENG];
	char path[MAX_PATH];
	int i;

	Hobject SubRgn, TotRgn;
	HTuple Num;
	Herror herr;
	CString status, strTemp, Dir;
	
	typeSystemPara sys_para;
	sys_para = m_VisMod->GetSystemPara(0);

	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);

	if( (cam_idx == 1) && (defect_type) )
	{
		Sleep(1);
	}

	if(defect_type)
	{
		//m_strLogPath
		memset(path, NULL, sizeof(path));
		sprintf(path, "%s\\%s\\%s\\", m_strLogPath, m_strStartTime, m_strLotNo);
		Dir.Format("%s", path);
		MakeDirectories(Dir);

		//Log
		//PC,LOTID,DEVICE_IDX,DEFECT_IDX,POS_X,POS_Y,
		sprintf(temp, "%d,0,%s,%04d-%02d-%02d,%02d:%02d:%02d,%d,%d,%d,%f,%f,%f,%f,%f", m_VisMod->GetVisIndex(), m_strLotNo, 
											lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay,
											lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond,
											m_iPrcDevIdx[0] + cof_idx, defect_idx, defect_type,
											(float)area, sizex, sizey, cx, cy);

		WriteLogAVI(path, "Defect_File.csv", "Defect_File.bak", temp, 0);

		//Save NG image
		if(save)
		{
			for(i = 0; (i < m_VisMod->GetMapNum(0)) && (i < IMAGE_MAP_MAX_NUM); i++)
			{
				if( (1 == cam_idx) && (0 < i) )
				{
					continue;
				}

				//NG image
				if(sys_para.m_bPara[31])
				{

					if(0 < m_lTest)
					{
						m_strLotNo = "Test";

					}
					memset(path, NULL, sizeof(path));

					sprintf(path, "%s\\%s\\%s\\IMAGE\\NG", m_strLogPath, m_strStartTime, m_strLotNo);

					Dir.Format("%s", path);
					MakeDirectories(Dir);

					if(0 < m_lTest)
					{
						sprintf(path, "%s\\%sTest[%ld]_%d.bmp", path, m_strLotNo, m_lTest + 1, i);  //20100726 lee 
						m_lTest++;
					}
					else
					{
						sprintf(path, "%s\\%s_VIS%d[%04d]_%d.bmp", path, m_strLotNo, m_VisMod->GetVisIndex(), m_iPrcDevIdx[0] + cof_idx, i);//20100726 lee
					}

					set_check("~give_error");
					herr = write_image(*m_VisMod->GetHalImage(cam_idx, m_iPrcIdx[cam_idx], i), "bmp", 0, path);
					set_check("give_error");
					if(H_MSG_TRUE == herr)
					{
					}
					else
					{
						strTemp.Format(_T("이미지 저장에 실패하였습니다. 하드디스크 용량 또는 저장 경로를 확인해 주세요!!"));
						AfxMessageBox(strTemp);
						status.Format("Halcon Error Occured at write_image_00!");
						m_VisMod->WriteErrorList((LPCTSTR)status);
						return;
					}

					Sleep(10);
				}
			}
		}
	}
	else
	{
		if(save)
		{
			//Save good image
			for(i = 0; (i < m_VisMod->GetMapNum(0)) && (i < IMAGE_MAP_MAX_NUM); i++)
			{
				if(i != 3) continue; //20110608 lee
				//Good
				if(sys_para.m_bPara[30])
				{
					memset(path, NULL, sizeof(path));
					sprintf(path, "%s\\%s\\%s\\IMAGE\\GOOD", m_strLogPath, m_strStartTime, m_strLotNo);

					Dir.Format("%s", path);
					MakeDirectories(Dir);
					sprintf(path, "%s\\%sPC%d[%d]_%d.bmp", path, m_strLotNo, cam_idx, m_iPrcDevIdx[0] + cof_idx, i);
				
					set_check("~give_error");
					herr = write_image(*m_VisMod->GetHalImage(0, m_iPrcIdx[0], i), "bmp", 0, path);
					set_check("give_error");
					if(H_MSG_TRUE == herr)
					{
					}
					else
					{
						strTemp.Format(_T("이미지 저장에 실패하였습니다. 하드디스크 용량 또는 저장 경로를 확인해 주세요!!"));
						AfxMessageBox(strTemp);
						status.Format("Halcon Error Occured at write_image_01!");
						m_VisMod->WriteErrorList((LPCTSTR)status);
						return;
					}
				}
			}
		}
	}
#else

	return;

#endif

}

void CFilmInsDlg::ResultPotLog(int cam_idx, int buf_idx, int ng_view_idx, int device_idx, Hlong area)
{
#ifdef _CAM
	char temp[DAT_STR_LENG];
	char path[MAX_PATH];
	
	Hobject SubRgn, TotRgn;
	HTuple Num;
	
	CString status, strTemp;
	
	CString Dir;

	typeSystemPara sys_para;
	sys_para = m_VisMod->GetSystemPara(cam_idx);
	
	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);
	
	//m_strLogPath
	memset(path, NULL, sizeof(path));
	sprintf(path, "%s\\%s\\%s\\", m_strLogPath, m_strStartTime, m_strLotNo);
	
	Dir.Format("%s", path);
	MakeDirectories(Dir);

	CString smodel;
	smodel = m_label_model.GetCaption();
	m_dlgSetup->m_InsPara = m_VisMod->GetInspectPara(m_RectIdx);
	
	//Log
	sprintf(temp, "%04d%02d%02d,%02d:%02d:%02d,%s,%s,%d,%f,%d",  
		lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay,
		lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond,
		smodel, m_strLotNo,
		m_iPrcDevIdx[0] + device_idx, (float)area, m_dlgSetup->m_InsPara.m_iParaChip[12]);
	WriteLogPot(path, "PotLog_File.csv", "PotLog_File.bak", temp, 0);
#else

	return;

#endif
}

BOOL CFilmInsDlg::WriteLogPot(CString path, CString file, CString bak_file, CString log, long limit_line)
{
#ifdef _CAM
	// check message level
	FILE*	stream;
	char	strFile[MAX_PATH], strToFile[MAX_PATH];
    long	result, line;
	char	szTime[DAT_STR_LENG]; 

	char buffer[DAT_STR_LENG];

	sprintf(strFile, "%s\\%s", path, file); 

	if((stream = fopen( strFile, "a+" )) == NULL)
	{
		HANDLE fd = CreateFile( strFile,
							GENERIC_READ|GENERIC_WRITE,
							//GENERIC_WRITE,
							FILE_SHARE_READ|FILE_SHARE_WRITE,
							NULL,
							OPEN_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL
							);
		stream = fopen( strFile, "a+" );
		if(stream == NULL)
		{
			//20150206 by IMSI
			//AfxMessageBox("Error : log file open fail!(WriteLogPot)" );
			CloseHandle(fd);
			return FALSE;
		}

		CloseHandle(fd);
	}

	//Check first time
    result = fseek(stream, 0L, SEEK_SET);
    if(result)
	{
		fclose(stream);
		return FALSE;
	}
	line = 0;
	while(fgets(buffer, DAT_STR_LENG, stream)) 
	{
		line++;
		if(1 < line)
		{
			break;
		}
	}
	if(0 == line)
	{
	    result = fseek(stream, 0L, SEEK_END);
		if(result)
		{
			fclose(stream);
			return FALSE;
		}
		memset(szTime, NULL, sizeof(szTime));
		sprintf(szTime, "DATE,TIME,DEVICEID,LOTID,DEVICE_INDEX,INSPECT_AREA,TEACHING_AREA");		// 120104 ytlee
		fputs(szTime, stream);
		fputs("\n", stream);	
	}

    result = fseek(stream, 0L, SEEK_END);
    if(result)
	{
		fclose(stream);
		return FALSE;
	}

	memset(szTime, NULL, sizeof(szTime));
	sprintf(szTime, "%s", log);
    fputs(szTime, stream);
	fputs("\n", stream);

    result = fseek(stream, 0L, SEEK_SET);
    if(result)
	{
		fclose(stream);
		return FALSE;
	}

	//Check limit line
	line = 0;
	if(limit_line)
	{
		while(fgets(buffer, DAT_STR_LENG, stream)) line++;
		if(line > limit_line)
		{
			sprintf(strToFile, "%s\\%s", path, bak_file); 
			CopyFile(strFile, strToFile, FALSE);
			fclose(stream);
			DeleteFile(strFile);
		}
		else
		{
			fclose(stream);
		}
	}
	else
	{
		fclose(stream);
	}

	return TRUE;
#else

	return TRUE;

#endif

	
}

BOOL CFilmInsDlg::RemoteControlModel(short menu_select, CString model_name)
{
	BOOL rslt = TRUE;
	CString str, mod_name, status, mod_desc, Dir, str0, str1;
	C_CtrlModelManageDlg *dlg;

	switch(menu_select)
	{
	case 0:	//NEW

		break;
	case 1:	//DELETE

		break;
	case 2:	//SELECT
		m_dlgProcessing.ShowWindow(SW_SHOW);
		HandleAllMessage();

		str.Empty();
		str = m_label_model.GetCaption();
		if(0 == str.Compare(model_name))
		{
			//이미 있으면
			status.Format("Warnning : this model already loaded!");
			WriteStatusList((LPCTSTR)status);
			m_dlgProcessing.ShowWindow(SW_HIDE);
			return FALSE;
		}

		dlg = new C_CtrlModelManageDlg(NULL, m_strDBPath, m_strRegiPath);

		//Model save
		dlg->Open();

		str0 = dlg->GetName();
		str1 = dlg->GetPath();

		if( (str0.IsEmpty()) || (str1.IsEmpty()) )
		{
			WriteStatusList("Error : invalid model!");
			rslt =FALSE;
		}
		else
		{
			
		}

		mod_desc.Format("%s", "REMOTE CONTROLLED");
		if(dlg->RemoteModel(model_name, mod_desc))
		{
			status.Format("%s", dlg->GetName());
			m_label_model.SetCaption((LPCTSTR)status);
			//성공 
			dlg->Open();
			//Make directory
			Dir.Format("%s", dlg->GetPath());
			MakeDirectories(Dir);
			//Load setup INI file
			
			m_VisMod->LoadInspectSelectPara(dlg->GetPath());		// 110723 ytlee
			if( m_VisMod->LoadInspectPara(dlg->GetPath()))
			{
				status.Format("Model change OK!(%s)", dlg->GetPath());
				WriteStatusList((LPCTSTR)status);

				status.Format("%s", dlg->GetName());
				m_label_model.SetCaption((LPCTSTR)status);

				UpdateLight(FALSE);

				ResetCount();

				rslt = TRUE;
			}
			else
			{
				status.Format("Error : can't load setup parameter file!(%s)", dlg->GetPath());
				WriteStatusList((LPCTSTR)status);

				rslt = FALSE;
			}

			m_dlgProcessing.ShowWindow(SW_HIDE);

			delete dlg;
		}
		break;
	default:

		break;
	}

	return rslt;
}

void CFilmInsDlg::SetLightIndex(int camidx)
{
	/*
		0 : 바 조명
		1 : 백라이트
		2 : 동축
		3 : 수직사각
	*/
	switch(camidx)
	{
	case VISION_CAM_TOP1:
		m_iLightChIdx[0] = 0;			//조명 채널 인덱스. 조명 배선이 잘못되어 백라이트와 수직사각 자리가 바뀜.
		m_iLightChIdx[1] = 3;
		m_iLightChIdx[2] = 2;
		m_iLightChIdx[3] = 1;

		m_iLightOnOffIdx[0] = 0;		//조명 온오프 인덱스
		m_iLightOnOffIdx[1] = 27;
		m_iLightOnOffIdx[2] = 18;
		m_iLightOnOffIdx[3] = 9;
		break;
	case VISION_CAM_TOP2:
		m_iLightChIdx[0] = 4;
		m_iLightChIdx[1] = 5;
		m_iLightChIdx[2] = 6;
		m_iLightChIdx[3] = -1;

		m_iLightOnOffIdx[0] = 36;
		m_iLightOnOffIdx[1] = 45;
		m_iLightOnOffIdx[2] = 54;
		m_iLightOnOffIdx[3] = -1;
		break;
	case VISION_CAM_BOTTOM:
		m_iLightChIdx[0] = 7;
		m_iLightChIdx[1] = 8;
		m_iLightChIdx[2] = 9;
		m_iLightChIdx[3] = -1;

		m_iLightOnOffIdx[0] = 0;
		m_iLightOnOffIdx[1] = 9;
		m_iLightOnOffIdx[2] = 18;
		m_iLightOnOffIdx[3] = -1;
		break;	
	default:
		m_iLightChIdx[0] = 0;
		m_iLightChIdx[1] = 0;
		m_iLightChIdx[2] = 0;
		m_iLightChIdx[3] = 0;

		m_iLightOnOffIdx[0] = 0;
		m_iLightOnOffIdx[1] = 0;
		m_iLightOnOffIdx[2] = 0;
		m_iLightOnOffIdx[3] = 0;

		break;
	}
}

/*
void CFilmInsDlg::SetLightIndex(int camidx)
{
	switch(camidx)
	{
	case VISION_CAM_TOP1:
	case VISION_CAM_BOTTOM1:
		m_iLightChIdx[0] = 8;		//SetLight() 참고 인덱스
		m_iLightChIdx[1] = 17;
		m_iLightChIdx[2] = 26;
		m_iLightChIdx[3] = 35;

		m_iLightOnOffIdx[0] = 0;		//조명 온오프 인덱스
		m_iLightOnOffIdx[1] = 9;
		m_iLightOnOffIdx[2] = 18;
		m_iLightOnOffIdx[3] = 27;
		break;
	case VISION_CAM_TOP2:
	case VISION_CAM_BOTTOM2:
		m_iLightChIdx[0] = 44;
		m_iLightChIdx[1] = 53;
		m_iLightChIdx[2] = 62;
		m_iLightChIdx[3] = -1;

		m_iLightOnOffIdx[0] = 36;
		m_iLightOnOffIdx[1] = 45;
		m_iLightOnOffIdx[2] = 54;
		m_iLightOnOffIdx[3] = -1;
		break;
	default:
		m_iLightChIdx[0] = -1;
		m_iLightChIdx[1] = -1;
		m_iLightChIdx[2] = -1;
		m_iLightChIdx[3] = -1;

		m_iLightOnOffIdx[0] = -1;
		m_iLightOnOffIdx[1] = -1;
		m_iLightOnOffIdx[2] = -1;
		m_iLightOnOffIdx[3] = -1;

		break;
	}
}
*/

void CFilmInsDlg::OnSend(CString data)
{
	int rslt = 1;
	int size = 0;
	char temp[PACKET_MAX_NUM] = {0, };
	
	sprintf(temp, "%s", data);
	size = strlen(temp);

	WriteTCPList(temp, FALSE);

	m_Client->Send((LPSTR)temp, size);

}

void CFilmInsDlg::ResultCheck_Display(int cam_idx, int buf_idx, int ng_view_idx, BOOL auto_mode, BOOL main_view, BOOL chk_ng_type, BOOL chk_ng_Disp, BOOL ng_view_data_save) //20220509 lee
{
	char temp[DAT_STR_LENG];
	int i, j, step;
	int ng_idx = 0;
	long rslt_num = 0;
	long rslt_num2 = 0;
	long rslt_num3 = 0;
	long sx, sy, ex, ey;
	int cur_scroll_idx = 0;
	long ng_idx_ims	= 0;
	int nRow, nCol, nStep = 80;
	
	BOOL save[SUB_RGN_MAX_NUM];

	for(i = 0; i < SUB_RGN_MAX_NUM; i++)
	{
		save[i] = TRUE;	
	}
	
	Hobject *RsltRegion, SelectRgn, *RsltRegion2, SelectRgn2, *RsltRegion3, SelectRgn3;
	HTuple Num, CRow, CCol, Row0, Col0, Row1, Col1, Area;
	HTuple CRow2, CCol2, Row2, Col2, Row3, Col3, Area2;
	HTuple CRow3, CCol3, Row4, Col4, Row5, Col5, Area3;
	CRect rect;
	
	typeInspectNGRslt ins_ng_rslt;
	ins_ng_rslt = m_VisMod->GetInspectNGRslt(cam_idx, ng_view_idx);
	
	typeSystemPara sys_para;
	sys_para = m_VisMod->GetSystemPara(0);
	
	typeInspectPara ins_para;
	ins_para = m_VisMod->GetInspectPara(0);
	
	SYSTEMTIME	lpSystemTime;
	GetLocalTime(&lpSystemTime);
	
	set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
	
	
	if(chk_ng_type)
	{
		//No display
		for(i = 0; i < SUB_RGN_MAX_NUM; i++)
		{
			m_bChkNGKind[cam_idx][i] = 0;
		}
	}
	else
	{
		if(1 == m_iRslt[cam_idx])
		{
			if(main_view)
			{
				sprintf(temp, "GOOD");
				m_label_ngview.SetCaption(temp);
				m_label_ngview.SetForeColor(RGB(0, 255, 0));
			}
		}
		else
		{
			if(main_view)
			{
				sprintf(temp, "NG");
				m_label_ngview.SetCaption(temp);
				m_label_ngview.SetForeColor(RGB(255, 0, 0));
			}
			
			for(i = 0; i < IMAGE_NG_NUM; i++)
			{
				clear_window(m_lNGWindowID[i]);
			}
			
		}
		
		m_iNGSubViewIdx[cam_idx] = 0;
		
		if(main_view)
		{
			//NG scroll
			m_iNGScrollIdx[cam_idx] = 0;

			m_iNGScrollLastNum[cam_idx] = 0;//표시할 전체불량개수
			
			if(auto_mode)
			{
				memset(temp, NULL, sizeof(temp));

				GetDlgItem(IDC_STATIC_NG_DATE)->SetWindowText(m_sNGDate[cam_idx]);
				GetDlgItem(IDC_STATIC_NG_LOTNO)->SetWindowText(m_sNGLotID[cam_idx]);
				sprintf(temp, "%d", m_iNGDeviceIdx[cam_idx]);
				GetDlgItem(IDC_STATIC_NG_DEVICENO)->SetWindowText(temp);
			}
		}
		else
		{
			//NG scroll
			m_scroll_ng.SetScrollRange(0, m_iNGScrollLastNum[cam_idx]);
		}		
	}

	//Display
	for(i = 0; (i < m_VisMod->GetCOFNum(0)) && (i < SUB_RGN_MAX_NUM); i++)
	{
		nRow = ins_para.m_InsRegionChip[i][0].left;//2600 - (3600 /  m_VisMod->GetCOFNum(0)) * i;
		nCol = 200;

		if(chk_ng_type)
		{
			//No display
			
		}
		else
		{
			
			if(ins_ng_rslt.m_InsRslt.m_bRegionSkip[i])
			{
				continue;
			}
			
			if(main_view)
			{
				set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("blue"));
				set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);

				write_string(m_dlgImgRect[cam_idx]->GetView(), HTuple(HTuple("CHIP#") + HTuple(i + 1)) );
			}
			
			if(1 == ins_ng_rslt.m_InsRslt.m_iRslt[i])
			{
				if(main_view)
				{ 
					//Good
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow + 350);

					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					write_string(m_dlgImgRect[cam_idx]->GetView(), "GOOD");
					
					if(auto_mode)
					{
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, 0, GOOD, 0, 0., 0., 0., 0., TRUE);
					}
				}
			} 
			else
			{
				if(main_view)
				{
					//NG
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow + 200);

					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG");
				}
			}
			nCol = nCol + nStep; 
		}
		
		//Align//////////////////////////////////
		switch(ins_ng_rslt.m_InsRslt.m_iRsltAlign[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
				//20110509 lee
				if(main_view && chk_ng_Disp)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 0);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					//Pattern
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("blue"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 2);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					//Rect + cross
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 1);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					//Score
					for(j = 0; j < PATTERN_INS_NUM; j++)
					{
						if(0 < ins_ng_rslt.m_InsRslt.m_dScoreAlign[i])
						{
							set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
							set_tposition(m_dlgImgRect[cam_idx]->GetView(), ins_ng_rslt.m_InsRslt.m_dCYAlign[i][j] - 150, ins_ng_rslt.m_InsRslt.m_dCXAlign[i][j]);
							write_string(m_dlgImgRect[cam_idx]->GetView(), (Hlong)(ins_ng_rslt.m_InsRslt.m_dScoreAlign[i][j] * 100.));
						}
					}
				}
			}
			break;
		case -10:
			//Align fail
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = AMIF;
				}
			}
			else
			{

				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Align fail
					ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, AMIF, 0, 0., 0., 0., 0., save[i]);
					save[i] = FALSE;
					ng_idx++;
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 0);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : ALIGN 인식실패");
					
					if(auto_mode)
					{
						//Align fail
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, AMIF, 0, 0., 0., 0., 0., save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					m_iNGScrollLastNum[cam_idx]++;
				}
				else
				{
					cur_scroll_idx++;
					if(ScrollCheck(cur_scroll_idx))
					{
						//Image
						//Search
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 0);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						
						sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER;
						sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER;
						ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER;
						ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER;
						
						set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
						
						disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapAlign[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						//Search
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 0);
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
						write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : ALIGN 인식실패");
						
						ScrollCheck();
					}
				}
			}
			break;
		case -11:
			//Align fail-no pattern
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = STDF;
				}				
			}
			else
			{
				
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Align fail
					ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, STDF, 0, 0., 0., 0., 0., save[i]);
					save[i] = FALSE;
					ng_idx++;
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 0);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : ALIGN 인식실패-NO DATA");
					
					if(auto_mode)
					{										
						//Align fail
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, STDF, 0, 0., 0., 0., 0., save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					
					m_iNGScrollLastNum[cam_idx]++;
				}
				else
				{
					cur_scroll_idx++;
					if(ScrollCheck(cur_scroll_idx))
					{
						//Image
						//Search
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 0);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						
						sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER;
						sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER;
						ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER;
						ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER;
						
						set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
						
						disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapAlign[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						//Search
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 0);
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
						write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : ALIGN 인식실패-NO DATA");
						
						ScrollCheck();
					}
				}
				
			}
			
			break;
		case -12:
			//Align limit error
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = AMPL;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Align fail
					ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, AMPL, 0, 0., 0., 0., 0., save[i]);
					save[i] = FALSE;
					ng_idx++;
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 0);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : ALIGN 인식실패-POS.LMT.");
					
					if(auto_mode)
					{										
						//Align fail
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, AMPL, 0, 0., 0., 0., 0., save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					
					m_iNGScrollLastNum[cam_idx]++;
				}
				else
				{
					cur_scroll_idx++;
					if(ScrollCheck(cur_scroll_idx))
					{
						//Image
						//Search
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 0);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						
						sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER;
						sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER;
						ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER;
						ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER;
						
						set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
						
						disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapAlign[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						//Search
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 0);
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
						write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : ALIGN 인식실패-POS.LMT.");
						
						ScrollCheck();
					}
				}
			}
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		//Chip///////////////////////////////
		switch(ins_ng_rslt.m_InsRslt.m_iRsltChipFind[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					//Chip
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 11);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
				}
			}
			break;
		case 2:
			//Punching chip
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{	
				//20110509 lee
				if(main_view && chk_ng_Disp)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("blue"));
					 
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					write_string(m_dlgImgRect[cam_idx]->GetView(), "SKIP : PUNCH CHIP");
					
					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : PUNCH CHIP", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
					}
				}
			}
			break;
		case 3:
			//No chip
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = PCND;//TPED -> PCND //CHOKI20101004 편집불량 -> Punching불량
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//No chip
					ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, PCND, 0, 0., 0., 0., 0., save[i]);//TPED -> PCND //CHOKI20101004 편집불량 -> Punching불량
					save[i] = FALSE;
					ng_idx++;
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 19);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : PF막힘/PUNCH BURR/편집불량");



					//20150210 lss
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-4,test2-4);
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-2,test2-2);
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);



					
					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : NO CHIP", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//No chip
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, PCND, 0, 0., 0., 0., 0., save[i]);//TPED -> PCND //CHOKI20101004 편집불량 -> Punching불량
						save[i] = FALSE;
						ng_idx++;
					}
					//CHOKI20111027
					if(ng_view_data_save)
					{
						//ResultIMSLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx_ims, PCND, 0, 0., 0., 0., 0., save[i]);
						ng_idx_ims++;
					}
					//CHOKI20111027
					
					m_iNGScrollLastNum[cam_idx]++;
				}
				else
				{
					cur_scroll_idx++;
					if(ScrollCheck(cur_scroll_idx))
					{
						//Image
						//Search
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 19);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						
						sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER;
						sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER;
						ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER;
						ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER;
						
						set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
						
						disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapFilm[1]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						//Search
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
						write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : PF막힘/PUNCH BURR/편집불량");
						
						ScrollCheck();
					}
				}
			}	
			break;
		case -20:
			//No chip
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = NOCP;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//No chip
					ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, NOCP, 0, 0., 0., 0., 0., save[i]);
					save[i] = FALSE;
					ng_idx++;
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : NO/WRONG CHIP");
					
					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : NO CHIP", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//No chip
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, NOCP, 0, 0., 0., 0., 0., save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					//CHOKI20111027
					if(ng_view_data_save)
					{
						//ResultIMSLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx_ims, NOCP, 0, 0., 0., 0., 0., save[i]);
						ng_idx_ims++;
					}
					//CHOKI20111027
					
					m_iNGScrollLastNum[cam_idx]++;
				}
				else
				{
					cur_scroll_idx++;
					if(ScrollCheck(cur_scroll_idx))
					{
						//Image
						//Search
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						
						sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER;
						sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER;
						ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER;
						ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER;
						
						set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
						
						disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						//Search
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
						write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : NO/WRONG CHIP");
						
						ScrollCheck();
					}
				}
			}
			break;
			
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		switch(ins_ng_rslt.m_InsRslt.m_iRsltChipOut[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
			}
			break;
		case -21:
			//Chipout
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = OSCO;
				}				
			}
			else
			{

				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//개수별
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipOutNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, OSCO, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					//Chip
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 11);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 12);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					//Chip out
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : CHIP OUT");
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					//CHOKI20100331*/
					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-4,test2-4);
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-2,test2-2);
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					//CHOKI20100331*/
					
					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : CHIP OUT", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//Log
						//개수별
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipOutNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, OSCO, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}
					//CHOKI20111027
					if(ng_view_data_save)
					{
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipOutNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							//ResultIMSLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx_ims, OSCO, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx_ims++;
						}
					}
					//CHOKI20111027
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					//Image
					//Chipout
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 12);
					
					//개수별
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipOutNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[1]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							//chipout
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : CHIP OUT");

							//CHOKI20100705 불량크기 표기 //CHOKI20111116 //CHOKI20111117 초기화 접근성 문제로 m_VisMod->m_dPixelSizeData 로 모두 변경함.
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 15, sx + 10);
							
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
								HTuple(((Hlong)Col1[j]-(Hlong)Col0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
								HTuple(((Hlong)Row1[j]-(Hlong)Row0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));
							
							
							ScrollCheck();
						}
					}
				}
			}
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		
		switch(ins_ng_rslt.m_InsRslt.m_iRsltChipBroken[i])
		{
		case 0:
			//No inspection
          			break;
		case 1:
			//Good
			break;
		case -22:
			//Broken
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = BRKN;
				}						  
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//개수별
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipBrokenNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, BRKN, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					//Chip
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 11);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					//Regin
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 13);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : CHIP BROKEN");
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					//CHOKI20100331*/
					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-4,test2-4);
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-2,test2-2);
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					//CHOKI20100331*/
					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : Chip 이면결함", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//Log
						//개수별
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipBrokenNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, BRKN, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}
					//CHOKI20111027
					if(ng_view_data_save)
					{
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipBrokenNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							//ResultIMSLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx_ims, OSCO, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx_ims++;
						}
					}
					//CHOKI20111027
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					//Image
					//Regin
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 13);
					
					
					//개수별
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipBrokenNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[2]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							//Regin
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : CHIP BROKEN");
							
							ScrollCheck();
						}
					}
				}
			}
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
				
		//Chip 이면 결함(Broken, 수지묻음)
		switch(ins_ng_rslt.m_InsRslt.m_iRsltChipMold[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
				
			}
			break;
		case -23:
			//Mold
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = OSCD;
				}						  
			}
			else
			{

				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//개수별
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipMoldNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, OSCD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					//Chip
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 11);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					//Regin
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 14);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : Chip 이면수지묻음");
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					//CHOKI20100331*/
					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-4,test2-4);
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-2,test2-2);
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					//CHOKI20100331*/
					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : Chip 이면결함", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//Log
						//개수별
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipMoldNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, OSCD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}
					//CHOKI20111027
					if(ng_view_data_save)
					{
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipMoldNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							//ResultIMSLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx_ims, OSCD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx_ims++;
						}
					}
					//CHOKI20111027
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					//Image
					//Regin
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 14);
					
					
					//개수별
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipMoldNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[3]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							//Regin
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : Chip 이면수지묻음");
							
							ScrollCheck();
						}
					}
				}
			}
			break;
				  default:
					  AfxMessageBox("Error : invaild index!");
					  break;
		}
		
		//Pot
		switch(ins_ng_rslt.m_InsRslt.m_iRsltChipNPot[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					break;
				}

				//Pot
				if(main_view)
				{
					if( (1 == ins_ng_rslt.m_InsRslt.m_iRsltChipDPot[i]) && (1 == ins_ng_rslt.m_InsRslt.m_iRsltChipNPot[i]) )
					{
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					}
					else
					{
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					}
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 15);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					Herror herr;		
					CString status;
					
					set_check("~give_error");
					herr = area_center(*RsltRegion, &Area, &CRow, &CCol);		// 120127 ytlee		wrongchip 검사 할콘에러 관련
					set_check("give_error");
					if(H_MSG_TRUE == herr)
					{
					}
					else
					{
						status.Format("Halcon Error Occured at tuple_sort_00!");
						m_VisMod->WriteErrorList((LPCTSTR)status);
					}	

					//CHOKI20111229
					if(auto_mode && ins_ng_rslt.m_InsRslt.m_iRsltChipFind[i] == 1)	
					{
						ResultPotLog(cam_idx, buf_idx, ng_view_idx, i, (Hlong)Area[0]);
					}
					//CHOKI20111229
					
					//NPot
					write_string(m_dlgImgRect[cam_idx]->GetView(), HTuple( HTuple("POT AREA : ") + HTuple(Area) ) );
				}
			}
			break;
		case -24:
			//No pot
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = NPTD;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//개수별
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, NPTD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					//Chip
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 11);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					//Pot
					if( (1 == ins_ng_rslt.m_InsRslt.m_iRsltChipDPot[i]) && (1 == ins_ng_rslt.m_InsRslt.m_iRsltChipNPot[i]) )
					{
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					}
					else
					{
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					}
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 15);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					
					Herror herr;		
					CString status;
					
					set_check("~give_error");
					herr = area_center(*RsltRegion, &Area, &CRow, &CCol);		// 120127 ytlee		wrongchip 검사 할콘에러 관련
					set_check("give_error");
					if(H_MSG_TRUE == herr)
					{
					}
					else
					{
						status.Format("Halcon Error Occured at area_center_01!");
						m_VisMod->WriteErrorList((LPCTSTR)status);
					}	

					//CHOKI20111229
					if(auto_mode && ins_ng_rslt.m_InsRslt.m_iRsltChipFind[i] == 1)	
					{
						ResultPotLog(cam_idx, buf_idx, ng_view_idx, i, (Hlong)Area[0]);
					}
					//CHOKI20111229
					
					//NPot
					write_string(m_dlgImgRect[cam_idx]->GetView(), HTuple( HTuple("NG : NO/UNDER POTTING : ") + HTuple(Area) ) );
					
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					//CHOKI20100331*/
					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					//CHOKI20100331*/
					
					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : NO POTTING", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//Log
						//개수별
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, NPTD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}
					//CHOKI20111027
					if(ng_view_data_save)
					{
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							save[i] = FALSE;
							ng_idx_ims++;
						}
					}
					//CHOKI20111027
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					cur_scroll_idx++;
					if(ScrollCheck(cur_scroll_idx))
					{
						//Image
						//Search
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 15);

						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						
						area_center(*RsltRegion, &Area, _, _);
						
						sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER;
						sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER;
						ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER;
						ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER;
						
						set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
						
						disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[4]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						//Search
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
						write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("NG : NO/UNDER POTTING : ") + HTuple(Area) ));
						
						ScrollCheck();
					}
				}
			}
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		switch(ins_ng_rslt.m_InsRslt.m_iRsltChipDPot[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
				
			}
			break;
		case -25:
			//Double pot
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = DPTD;
				}				
			}
			else
			{

				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//개수별
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, DPTD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					//Chip
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 11);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					//Pot
					if( (1 == ins_ng_rslt.m_InsRslt.m_iRsltChipDPot[i]) && (1 == ins_ng_rslt.m_InsRslt.m_iRsltChipNPot[i]) )
					{
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					}
					else
					{
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					}
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 15);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					
					//DPot
					write_string(m_dlgImgRect[cam_idx]->GetView(), HTuple("NG : DOUBLE POTTING : ")  + HTuple(Area) );
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					//CHOKI20100331*/
					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					//CHOKI20100331*/
					if(auto_mode)
					{
						//Log
						//개수별
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, DPTD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}
					//CHOKI20111027
					if(ng_view_data_save)
					{
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							save[i] = FALSE;
							ng_idx_ims++;
						}
					}
					//CHOKI20111027
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					cur_scroll_idx++;
					if(ScrollCheck(cur_scroll_idx))
					{
						//Image
						//Search
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 15);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						
						area_center(*RsltRegion, &Area, _, _);
						
						sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER;
						sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER;
						ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER;
						ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER;
						
						set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
						
						disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[4]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						//Search
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
						write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("NG : DOUBLE POTTING : ") + HTuple(Area) ));
						
						ScrollCheck();
					}
				}
			}
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		//Pot particle///////////////////////////
		switch(ins_ng_rslt.m_InsRslt.m_iRsltChipPotP[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					break;
				}

				//인식기 영역
				if(main_view)
				{
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));

					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 18);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
				}					
			}
			break;
		case -26:
			//Pot particle
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = PPCD;
				}				
			}
			else
			{

				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//개수별
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotPNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, PPCD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					//인식기 영역
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));

					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 18);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());

					//Regin
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 16);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : POT Particle");
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					//CHOKI20100331*/
					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					//CHOKI20100331*/
					
					if(auto_mode)
					{
						//Log
						//개수별
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotPNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, PPCD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}
					//CHOKI20111027
					if(ng_view_data_save)
					{
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotPNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							save[i] = FALSE;
							ng_idx_ims++;
						}
					}
					//CHOKI20111027
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					//Image
					//Regin
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 16);
					
					
					//개수별
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotPNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[1]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							//Regin
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : POT Particle");
																																																																																													//CHOKI20100705 불량크기 표기
							//CHOKI20100705 불량크기 표기//CHOKI20111117 초기화 접근성 문제로 m_VisMod->m_dPixelSizeData 로 모두 변경함.
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 15, sx + 10);
							
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
								HTuple(((Hlong)Col1[j]-(Hlong)Col0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
								HTuple(((Hlong)Row1[j]-(Hlong)Row0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));
							
							
							ScrollCheck();
						}
					}
				}
			}
			break;
		default:																																																																																																																																																																																																																						
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		//도포범위불량///////////////////////////
		switch(ins_ng_rslt.m_InsRslt.m_iRsltChipPotR[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
			
			}
			break;
		case -27:
			//도포범위불량
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = ESAD;
				}				
			}
			else
			{

				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//개수별
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotRNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, ESAD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					//Regin
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 17);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : 도포범위불량");
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					//CHOKI20100331*/
					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					//CHOKI20100331*/
					
					if(auto_mode)
					{
						//Log
						//개수별
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotRNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, ESAD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}
					//CHOKI20111027
					if(ng_view_data_save)
					{
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotRNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							save[i] = FALSE;
							ng_idx_ims++;
						}
					}
					//CHOKI20111027
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					//Image
					//Regin
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 17);
					
					
					//개수별
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipPotRNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapChip[5]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							//Regin
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : 도포범위불량");
							
							ScrollCheck();
						}
					}
				}
			}
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		//Film///////////////////////////////
		//Mold
		switch(ins_ng_rslt.m_InsRslt.m_iRsltFilmMold[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					break;
				}

				if(main_view)
				{
					//SR out pattern region
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("white"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 29);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
				}
			}
			break;
		case -30:
			//No film pattern
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = STDF;
				}				
			}
			else
			{

				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Film data fail
					ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, STDF, 0, 0., 0., 0., 0., save[i]);
					save[i] = FALSE;
					ng_idx++;
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 24);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : FILM NO DATA");
					
					if(auto_mode)
					{					
						//Film data fail
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, STDF, 0, 0., 0., 0., 0., save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					//CHOKI20111027
					if(ng_view_data_save)
					{
						ng_idx_ims++;
					}
					//CHOKI20111027
					
					m_iNGScrollLastNum[cam_idx]++;
				}
				else
				{
					cur_scroll_idx++;
					if(ScrollCheck(cur_scroll_idx))
					{
						//Image
						//Search
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 24);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						
						sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER;
						sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER;
						ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER;
						ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER;
						
						set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
						
						disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapFilm[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						//Search
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
						write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("NG : FILM NO DATA"));
						
						ScrollCheck();
					}
				}
			}
			break;
		case -31:
			//수지 묻음
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = RSTD;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//개수별
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iFilmMoldNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, RSTD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 23);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : FILM 수지/잉크 묻음/긁힘");
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					//CHOKI20100331*/
					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					//CHOKI20100331*/
					
					if(auto_mode)
					{
						//Log
						//개수별
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iFilmMoldNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, RSTD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}
					//CHOKI20111027
					if(ng_view_data_save)
					{
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iFilmMoldNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							save[i] = FALSE;
							ng_idx_ims++;
						}
					}
					//CHOKI20111027
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					//Image
					//수지묻음
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 23);
					
					
					//개수별
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iFilmMoldNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapFilm[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : FILM 수지/잉크 묻음/긁힘");

							//CHOKI20100705 불량크기 표기//CHOKI20111117 초기화 접근성 문제로 m_VisMod->m_dPixelSizeData 로 모두 변경함.
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 15, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
								HTuple(((Hlong)Col1[j]-(Hlong)Col0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
								HTuple(((Hlong)Row1[j]-(Hlong)Row0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));
							
							ScrollCheck();
						}
					}
				}
			}
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		switch(ins_ng_rslt.m_InsRslt.m_iRsltFilmPF[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 26);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
				}
			}
			break;
		case -32:
			//PF 찢김
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = PFHD;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//개수별
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iFilmPFNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, PFHD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
						
						char temp[100];
						sprintf(temp, "[NG save:PFHD]\n");
						TRACE(temp);

						save[i] = FALSE;
						
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 26);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 25);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : FILM PF 찢김/막힘");
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					//CHOKI20100331*/
					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					//CHOKI20100331*/
					
					if(auto_mode)
					{
									
						//Log
						//개수별
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iFilmPFNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, PFHD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}
					//CHOKI20111027
					if(ng_view_data_save)
					{
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iFilmPFNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							//ResultIMSLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx_ims, PFHD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx_ims++;
						}
					}
					//CHOKI20111027
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					//Image
					//수지묻음
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 25);
					
					
					//개수별
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iFilmPFNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapFilm[1]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : FILM PF 찢김/막힘");

							//CHOKI20100705 불량크기 표기//CHOKI20111117 초기화 접근성 문제로 m_VisMod->m_dPixelSizeData 로 모두 변경함.
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 15, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
								HTuple(((Hlong)Col1[j]-(Hlong)Col0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
								HTuple(((Hlong)Row1[j]-(Hlong)Row0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));							
							ScrollCheck();
						}
					}
				}
			}
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		//Metal particle/////////////////////////
		switch(ins_ng_rslt.m_InsRslt.m_iRsltFilmMetal[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 26);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
				}
			}
			break;
		case -33:
			//Metal particle
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = SRMP;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//개수별
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iFilmMetalNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, SRMP, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 26);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 50);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : SR면 METAL PART.");
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					//CHOKI20100331*/
					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-4,test2-4);
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-2,test2-2);
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					//CHOKI20100331*/

					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : FILM PF 찢김", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//Log
						//개수별
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iFilmMetalNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, SRMP, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}
					//CHOKI20111027
					if(ng_view_data_save)
					{
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iFilmMetalNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							//ResultIMSLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx_ims, SRMP, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx_ims++;
						}
					}
					//CHOKI20111027

					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					//Image
					//수지묻음
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 50);
					
					
					//개수별
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iFilmMetalNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapFilm[6]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : SR면 METAL PART.");

							//CHOKI20100705 불량크기 표기//CHOKI20111117 초기화 접근성 문제로 m_VisMod->m_dPixelSizeData 로 모두 변경함.
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 15, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
								HTuple(((Hlong)Col1[j]-(Hlong)Col0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
								HTuple(((Hlong)Row1[j]-(Hlong)Row0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));
							
							
							ScrollCheck();
						}
					}
				}
			}
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}


		//Lead///////////////////////////////
		if(chk_ng_type)
		{
			//No display
				
		}
		else
		{
			//20110509 lee
			if(main_view && chk_ng_Disp)
			{
				//Top search
				set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
				RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 30);
				disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
			}
		}
		//Top
		switch(ins_ng_rslt.m_InsRslt.m_iRsltLeadTop[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
				
			}
			
			break;
		case -40:
			//Top lead ng
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = LFPT;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//개수별
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadTopNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, LFPT, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
						save[i] = FALSE;
						ng_idx++;
					}

					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadTopNum2[i]) && (j < NG_MAX_NUM) && (j < rslt_num2) ; j++)
					{
						rect = CRect((Hlong)Row2[j], (Hlong)Col2[j], (Hlong)Row3[j], (Hlong)Col3[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, LFPT, (Hlong)Area2[j], (double)CCol2[j], (double)CRow2[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
						save[i] = FALSE;
						ng_idx++;
					}

					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadTopNum3[i]) && (j < NG_MAX_NUM) && (j < rslt_num3) ; j++)
					{
						rect = CRect((Hlong)Row4[j], (Hlong)Col4[j], (Hlong)Row5[j], (Hlong)Col5[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, LFPT, (Hlong)Area3[j], (double)CCol3[j], (double)CRow3[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					//Lead 상부결함
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : LEAD 변색/결함(OUT)");

					//Image//////////////////////
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 32);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();

					//Image2/////////////////////
					RsltRegion2 = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 34);
					disp_obj(*RsltRegion2, m_dlgImgRect[cam_idx]->GetView());
					
					area_center(*RsltRegion2, &Area2, &CRow2, &CCol2);
					smallest_rectangle1(*RsltRegion2, &Row2, &Col2, &Row3, &Col3);
					rslt_num2 = (Hlong)Row2.Num();

					//Image3/////////////////////(긁힘)
					RsltRegion3 = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 36);
					disp_obj(*RsltRegion3, m_dlgImgRect[cam_idx]->GetView());
					
					area_center(*RsltRegion3, &Area3, &CRow3, &CCol3);
					smallest_rectangle1(*RsltRegion3, &Row4, &Col4, &Row5, &Col5);
					rslt_num3 = (Hlong)Row4.Num();


					//CHOKI20100331*/
					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-4,test2-4);
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-2,test2-2);
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}

					for(long cho=0;rslt_num2>cho;cho++)
					{
						test2=abs((long)((Hlong)Row3[cho]-(Hlong)Row2[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col3[cho]-(Hlong)Col2[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-4,test2-4);
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-2,test2-2);
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow2[cho],(Hlong)CCol2[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow2[cho], (Hlong)CCol2[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}


					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					//CHOKI20100331*/
					
					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : LEAD 변색/결함(상부)", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//Log
						//개수별
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadTopNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, LFPT, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx++;
						}

						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadTopNum2[i]) && (j < NG_MAX_NUM) && (j < rslt_num2) ; j++)
						{
							rect = CRect((Hlong)Row2[j], (Hlong)Col2[j], (Hlong)Row3[j], (Hlong)Col3[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, LFPT, (Hlong)Area2[j], (double)CCol2[j], (double)CRow2[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx++;
						}

						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadTopNum3[i]) && (j < NG_MAX_NUM) && (j < rslt_num3) ; j++)
						{
							rect = CRect((Hlong)Row4[j], (Hlong)Col4[j], (Hlong)Row5[j], (Hlong)Col5[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, LFPT, (Hlong)Area3[j], (double)CCol3[j], (double)CRow3[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}
					//CHOKI20111027
					if(ng_view_data_save)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : LEAD 변색/결함(상부)", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//Log
						//개수별
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadTopNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							//ResultIMSLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx_ims, LFPT, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx_ims++;
						}

						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadTopNum2[i]) && (j < NG_MAX_NUM) && (j < rslt_num2) ; j++)
						{
							rect = CRect((Hlong)Row2[j], (Hlong)Col2[j], (Hlong)Row3[j], (Hlong)Col3[j]);
							//cx, cy, sizex, sizey
							//ResultIMSLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx_ims, LFPT, (Hlong)Area2[j], (double)CCol2[j], (double)CRow2[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx_ims++;
						}

						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadTopNum3[i]) && (j < NG_MAX_NUM) && (j < rslt_num3) ; j++)
						{
							rect = CRect((Hlong)Row4[j], (Hlong)Col4[j], (Hlong)Row5[j], (Hlong)Col5[j]);
							//cx, cy, sizex, sizey
							//ResultIMSLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx_ims, LFPT, (Hlong)Area3[j], (double)CCol3[j], (double)CRow3[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx_ims++;
						}
					}
					//CHOKI20111027
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
					m_iNGScrollLastNum[cam_idx]+=rslt_num2;
					m_iNGScrollLastNum[cam_idx]+=rslt_num3;
				}
				else
				{
					//Image
					//수지묻음
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 32);
					
					//개수별
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadTopNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapLead[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : LEAD 변색/결함(OUT)");

							//CHOKI20100705 불량크기 표기//CHOKI20111117 초기화 접근성 문제로 m_VisMod->m_dPixelSizeData 로 모두 변경함.
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 15, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
								HTuple(((Hlong)Col1[j]-(Hlong)Col0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
								HTuple(((Hlong)Row1[j]-(Hlong)Row0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));
							
							
							ScrollCheck();
						}
					}

					//Image2
					//수지묻음
					RsltRegion2 = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 34);
					
					//개수별
					area_center(*RsltRegion2, &Area2, &CRow2, &CCol2);
					smallest_rectangle1(*RsltRegion2, &Row2, &Col2, &Row3, &Col3);
					rslt_num2 = (Hlong)Row2.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadTopNum2[i]) && (j < NG_MAX_NUM) && (j < rslt_num2) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row2[j], (Hlong)Col2[j], (Hlong)Row3[j], (Hlong)Col3[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col2[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row2[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col3[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row3[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapLead[1]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion2, &SelectRgn2, j + 1);
							disp_obj(SelectRgn2, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : LEAD 변색/결함(OUT)");

							//CHOKI20100705 불량크기 표기//CHOKI20111117 초기화 접근성 문제로 m_VisMod->m_dPixelSizeData 로 모두 변경함.
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 15, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
								HTuple(((Hlong)Col3[j]-(Hlong)Col2[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
								HTuple(((Hlong)Row3[j]-(Hlong)Row2[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));

							
							
							ScrollCheck();
						}
					}

					//Image3
					//수지묻음
					RsltRegion3 = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 36);
					
					//개수별
					area_center(*RsltRegion3, &Area3, &CRow3, &CCol3);
					smallest_rectangle1(*RsltRegion3, &Row4, &Col4, &Row5, &Col5);
					rslt_num3 = (Hlong)Row4.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadTopNum3[i]) && (j < NG_MAX_NUM) && (j < rslt_num3) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row4[j], (Hlong)Col4[j], (Hlong)Row5[j], (Hlong)Col5[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col4[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row4[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col5[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row5[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapLead[4]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion3, &SelectRgn3, j + 1);
							disp_obj(SelectRgn3, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : LEAD 변색/결함(OUT)");

							//CHOKI20100705 불량크기 표기//CHOKI20111117 초기화 접근성 문제로 m_VisMod->m_dPixelSizeData 로 모두 변경함.
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 15, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
								HTuple(((Hlong)Col5[j]-(Hlong)Col4[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
								HTuple(((Hlong)Row5[j]-(Hlong)Row4[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));

							ScrollCheck();
						}
					}
				}
			}
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		//Bottom
		if(chk_ng_type)
		{
			//No display
			
		}
		else
		{
			//20110509 lee
			if(main_view && chk_ng_Disp)

			{
				//Bottom search
				set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
				RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 31);
				disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
			}
		}

		switch(ins_ng_rslt.m_InsRslt.m_iRsltLeadBot[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
				
			}
			break;
		case -41:
			//Bottom lead ng
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = LFPB;
				}					
			}
			else
			{

				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//개수별
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadBotNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, LFPB, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
						save[i] = FALSE;
						ng_idx++;
					}

					//Image2/////////////////
					//Log
					//개수별
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadBotNum2[i]) && (j < NG_MAX_NUM) && (j < rslt_num2) ; j++)
					{
						rect = CRect((Hlong)Row2[j], (Hlong)Col2[j], (Hlong)Row3[j], (Hlong)Col3[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, LFPB, (Hlong)Area2[j], (double)CCol2[j], (double)CRow2[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
						save[i] = FALSE;
						ng_idx++;
					}

					//Image3/////////////////
					//Log
					//개수별
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadBotNum3[i]) && (j < NG_MAX_NUM) && (j < rslt_num3) ; j++)
					{
						rect = CRect((Hlong)Row4[j], (Hlong)Col4[j], (Hlong)Row5[j], (Hlong)Col5[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, LFPB, (Hlong)Area3[j], (double)CCol3[j], (double)CRow3[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
						save[i] = FALSE;
						ng_idx++;
					}	
					break;
				}

				if(main_view)
				{
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					//Lead 하부결함
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : LEAD 변색/결함(IN)");

					//Image//////////////////////
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 33);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
										
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();

					//Image2/////////////////////
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion2 = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 35);
					disp_obj(*RsltRegion2, m_dlgImgRect[cam_idx]->GetView());
										
					area_center(*RsltRegion2, &Area2, &CRow2, &CCol2);
					smallest_rectangle1(*RsltRegion2, &Row2, &Col2, &Row3, &Col3);
					rslt_num2 = (Hlong)Row2.Num();

					//Image3/////////////////////(긁힘)
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion3 = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 37);
					disp_obj(*RsltRegion3, m_dlgImgRect[cam_idx]->GetView());
										
					area_center(*RsltRegion3, &Area3, &CRow3, &CCol3);
					smallest_rectangle1(*RsltRegion3, &Row4, &Col4, &Row5, &Col5);
					rslt_num3 = (Hlong)Row4.Num();

					//CHOKI20100331*/
					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-4,test2-4);
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-2,test2-2);
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					//CHOKI20100331*/

					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : NG : LEAD 변색/결함(하부)", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//Image//////////////////
						//Log
						//개수별
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadBotNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, LFPB, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx++;
						}

						//Image2/////////////////
						//Log
						//개수별
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadBotNum2[i]) && (j < NG_MAX_NUM) && (j < rslt_num2) ; j++)
						{
							rect = CRect((Hlong)Row2[j], (Hlong)Col2[j], (Hlong)Row3[j], (Hlong)Col3[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, LFPB, (Hlong)Area2[j], (double)CCol2[j], (double)CRow2[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx++;
						}

						//Image3/////////////////
						//Log
						//개수별
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadBotNum3[i]) && (j < NG_MAX_NUM) && (j < rslt_num3) ; j++)
						{
							rect = CRect((Hlong)Row4[j], (Hlong)Col4[j], (Hlong)Row5[j], (Hlong)Col5[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, LFPB, (Hlong)Area3[j], (double)CCol3[j], (double)CRow3[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}
					//CHOKI20111027
					if(ng_view_data_save)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : LEAD 변색/결함(상부)", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//Log
						//개수별
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadBotNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							//ResultIMSLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx_ims, LFPB, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx_ims++;
						}

						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadBotNum2[i]) && (j < NG_MAX_NUM) && (j < rslt_num2) ; j++)
						{
							rect = CRect((Hlong)Row2[j], (Hlong)Col2[j], (Hlong)Row3[j], (Hlong)Col3[j]);
							//cx, cy, sizex, sizey
							//ResultIMSLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx_ims, LFPB, (Hlong)Area2[j], (double)CCol2[j], (double)CRow2[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx_ims++;
						}

						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadBotNum3[i]) && (j < NG_MAX_NUM) && (j < rslt_num3) ; j++)
						{
							rect = CRect((Hlong)Row4[j], (Hlong)Col4[j], (Hlong)Row5[j], (Hlong)Col5[j]);
							//cx, cy, sizex, sizey
							//ResultIMSLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx_ims, LFPB, (Hlong)Area3[j], (double)CCol3[j], (double)CRow3[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx_ims++;
						}
					}
					//CHOKI20111027
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
					m_iNGScrollLastNum[cam_idx]+=rslt_num2;
					m_iNGScrollLastNum[cam_idx]+=rslt_num3;
				}
				else
				{
					//Image//////////////////////
					//수지묻음
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 33);
										
					//개수별
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadBotNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapLead[2]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : LEAD 변색/결함(IN)");

							//CHOKI20100705 불량크기 표기 //CHOKI20111116//CHOKI20111117 초기화 접근성 문제로 m_VisMod->m_dPixelSizeData 로 모두 변경함.
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 15, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
								HTuple(((Hlong)Col1[j]-(Hlong)Col0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
								HTuple(((Hlong)Row1[j]-(Hlong)Row0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));	
							
							ScrollCheck();
						}
					}

					//Image2/////////////////////
					//수지묻음
					RsltRegion2 = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 35);
										
					//개수별
					area_center(*RsltRegion2, &Area2, &CRow2, &CCol2);
					smallest_rectangle1(*RsltRegion2, &Row2, &Col2, &Row3, &Col3);
					rslt_num2 = (Hlong)Row2.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadBotNum2[i]) && (j < NG_MAX_NUM) && (j < rslt_num2) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row2[j], (Hlong)Col2[j], (Hlong)Row3[j], (Hlong)Col3[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col2[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row2[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col3[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row3[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapLead[3]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion2, &SelectRgn2, j + 1);
							disp_obj(SelectRgn2, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : LEAD 변색/결함(IN)");

							//CHOKI20100705 불량크기 표기//CHOKI20111117 초기화 접근성 문제로 m_VisMod->m_dPixelSizeData 로 모두 변경함.
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 15, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
								HTuple(((Hlong)Col3[j]-(Hlong)Col2[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
								HTuple(((Hlong)Row3[j]-(Hlong)Row2[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));					
							
							ScrollCheck();
						}
					}

					//Image3/////////////////////
					//수지묻음
					RsltRegion3 = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 37);
										
					//개수별
					area_center(*RsltRegion3, &Area3, &CRow3, &CCol3);
					smallest_rectangle1(*RsltRegion3, &Row4, &Col4, &Row5, &Col5);
					rslt_num3 = (Hlong)Row4.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadBotNum3[i]) && (j < NG_MAX_NUM) && (j < rslt_num3) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row4[j], (Hlong)Col4[j], (Hlong)Row5[j], (Hlong)Col5[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col4[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row4[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col5[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row5[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapLead[4]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion3, &SelectRgn3, j + 1);
							disp_obj(SelectRgn3, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : LEAD 변색/결함(IN)");

								//CHOKI20100705 불량크기 표기//CHOKI20111117 초기화 접근성 문제로 m_VisMod->m_dPixelSizeData 로 모두 변경함.
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 15, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
								HTuple(((Hlong)Col5[j]-(Hlong)Col4[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
								HTuple(((Hlong)Row5[j]-(Hlong)Row4[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));

							
							
							ScrollCheck();
						}
					}
				}
			}
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}

		//AM scratch//////////////////////////////////////////////////////
		switch(ins_ng_rslt.m_InsRslt.m_iRsltAM[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
				set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("blue"));
				RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 61);
				disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());

				RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 62);
				disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());

				RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 63);
				disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());

				RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 64);
				disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
			}
			break;
		case -42:
			//AM
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = AMSC;
				}					
			}
			else
			{

				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//개수별
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadAMNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, AMSC, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					//AM스크레치 불량
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : A/M SCRATCH");

					//Image//////////////////////
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("blue"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 61);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());

					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 62);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());

					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 63);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());

					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 64);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());

					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 60);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
										
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();


					//CHOKI20100331*/
					long test1, test2; set_line_width(m_dlgImgRect[cam_idx]->GetView(),2);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-4,test2-4);
						//disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1-2,test2-2);
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					//CHOKI20100331*/

					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : NG : LEAD 변색/결함(하부)", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//Image//////////////////
						//Log
						//개수별
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadAMNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, AMSC, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx++;
						}

					}
					//CHOKI20111027
					if(ng_view_data_save)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : LEAD 변색/결함(상부)", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//Log
						//개수별
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadAMNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							//ResultIMSLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx_ims, LFPB, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx_ims++;
						}

					}
					//CHOKI20111027
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					//Image//////////////////////
					//AM
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 60);
										
					//개수별
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iLeadAMNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapLead[4]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : A/M SCRATCH");

							//CHOKI20100705 불량크기 표기 //CHOKI20111116//CHOKI20111117 초기화 접근성 문제로 m_VisMod->m_dPixelSizeData 로 모두 변경함.
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 15, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
								HTuple(((Hlong)Col1[j]-(Hlong)Col0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
								HTuple(((Hlong)Row1[j]-(Hlong)Row0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));	
							
							ScrollCheck();
						}
					}

				}
			}
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		//OCV////////////////////////////////
		switch(ins_ng_rslt.m_InsRslt.m_iRsltOCV[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			if(chk_ng_type)
			{
				//No display

			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					break;
				}

				//Good
				if(main_view)
				{
					//OCV total search(pattern)
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 40);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					//OCV total modimage(pattern)
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 41);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					//Character search(pattern modimage)
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("green"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 42);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					//Character pattern(pattern shape)
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("blue"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 43);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
				}
			}
			break;
		case -50:
			//인쇄품질-CAN'T FIND PATTERN-전체 pattern	
			if(chk_ng_type)
			{ 
				//TRACE("Marking NOT FIND Error Log Save\n");

				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = MKND;
				}					
			}
			else
			{			
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					for(j = 0; (j < NG_MAX_NUM) && (j < rslt_num) ; j++) //20101007 lee
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, MKND, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
						//TRACE("Marking Error Log Save\n");
						save[i] = FALSE;
						ng_idx++;
					}

					break;
				}
				if(main_view)
				{
					//OCV total search(pattern)
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 40);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : 인쇄품질-NO CHARS.");
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					
					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : 인쇄품질-NO CHARS.", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//Log
						//개수별
//						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iOCVPatNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						for(j = 0; (j < NG_MAX_NUM) && (j < rslt_num) ; j++) //20101007 lee
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, MKND, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							//TRACE("Marking Error Log Save\n");
							save[i] = FALSE;
							ng_idx++;
						}
					}
					//CHOKI20111027
					if(ng_view_data_save)
					{
						for(j = 0; (j < NG_MAX_NUM) && (j < rslt_num) ; j++)//for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipOutNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							//ResultIMSLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx_ims, MKND, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx_ims++;
						}
					}
					//CHOKI20111027
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					cur_scroll_idx++;
					if(ScrollCheck(cur_scroll_idx))
					{
						//Image
						//Search
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 40);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						
						sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER;
						sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER;
						ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER;
						ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER;
						
						set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
						
						disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapOCV[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						//Search
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
						write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("NG : 인쇄품질-NO CHARS."));
						
						ScrollCheck();
					}
				}
			}
			break;
		case -51:
			//인쇄품질-NO PATTERN-개별 pattern
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = STDF;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//OCV pattern data fail
					ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, STDF, 0, 0., 0., 0., 0., save[i]);
					save[i] = FALSE;
					ng_idx++;
					break;
				}

				if(main_view)
				{
					//OCV total search(pattern)
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 40);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : 인쇄품질-NO DATA");
					
					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : 인쇄품질-NO DATA", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//OCV pattern data fail
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, STDF, 0, 0., 0., 0., 0., save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					//CHOKI20111027
					if(ng_view_data_save)
					{
						//ResultIMSLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx_ims, STDF, 0, 0., 0., 0., 0., save[i]);
						ng_idx_ims++;
					}
					//CHOKI20111027
					
					m_iNGScrollLastNum[cam_idx]++;
				}
				else
				{
					cur_scroll_idx++;
					if(ScrollCheck(cur_scroll_idx))
					{
						//Image
						//Search
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 40);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						
						sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER;
						sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER;
						ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER;
						ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER;
						
						set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
						
						disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapOCV[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						//Search
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
						write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("NG : 인쇄품질-NO DATA"));
						
						ScrollCheck();
					}
				}
			}
			break;
		case -52:
			//인쇄품질-CAN'T FIND PATTERN-개별 character
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = MKND;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//개수별
					for(j = 0; (j < NG_MAX_NUM) && (j < rslt_num) ; j++) //201007 lee
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, MKND, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					//Sub search region(if can't find, use)
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 44);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : 인쇄품질-NO CHAR.");
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					
					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : 인쇄품질-NO CHAR.", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//Log
						//개수별
						//for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iOCVPatNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						for(j = 0; (j < NG_MAX_NUM) && (j < rslt_num) ; j++) //201007 lee
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, MKND, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}
					//CHOKI20111027
					if(ng_view_data_save)
					{
						for(j = 0; (j < NG_MAX_NUM) && (j < rslt_num) ; j++)//for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iChipOutNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							//ResultIMSLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx_ims, MKND, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx_ims++;
						}
					}
					//CHOKI20111027
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					//Image
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 44);
					
					
					//개수별
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iOCVPatNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapOCV[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : 인쇄품질-NO CHAR.");
							
							ScrollCheck();
						}
					}
				}
			}
			break;
		case -53:
			//인쇄품질-NO PATTERN-개별 character
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = STDF;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//OCV pattern data fail
					ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, STDF, 0, 0., 0., 0., 0., save[i]);
					save[i] = FALSE;
					ng_idx++;
					break;
				}

				if(main_view)
				{
					//OCV total search(pattern)
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 40);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;
					
					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : 인쇄품질-NO DATA");
					
					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : 인쇄품질-NO DATA", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//OCV pattern data fail
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, STDF, 0, 0., 0., 0., 0., save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					
					m_iNGScrollLastNum[cam_idx]++;
				}
				else
				{
					cur_scroll_idx++;
					if(ScrollCheck(cur_scroll_idx))
					{
						//Image
						//Search
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 40);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						
						sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER;
						sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER;
						ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER;
						ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER;
						
						set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
						
						disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapOCV[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						//Search
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
						write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("NG : 인쇄품질-NO DATA"));
						
						ScrollCheck();
					}
				}
			}
			break;
		case -54:
		/*
		//Character quality
		
		  if(main_view)
		  {
		  //Char defect NG
		  set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
		  RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 45);
		  disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
		  
			line = line + step;
			set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
			set_tposition(m_dlgImgRect[cam_idx]->GetView(), line, 1600);
			
			  write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : 인쇄품질-CHARS. DEFECT");
			  
				if(auto_mode)
				{
				sprintf(temp, "CAM #%d[%02d:%02d:%02d] : 인쇄품질-CHARS. DEFECT", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
				WriteStatusList(temp);
				
				  //Log
				  //개수별
				  area_center(*RsltRegion, &Area, &CRow, &CCol);
				  smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
				  rslt_num = (Hlong)Row0.Num();
				  for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iOCVVarNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
				  {
				  rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
				  //cx, cy, sizex, sizey
				  ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, MKND, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0]);
				  ng_idx++;
				  }
				  }
				  }
			*/
			break;
		case -55:
			//No character quality data
			/*
			if(main_view)
			{
			//OCV total search(pattern)
			set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
			RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 40);
			disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
			
			  line = line + step;
			  set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
			  set_tposition(m_dlgImgRect[cam_idx]->GetView(), line, 1600);
			  
				write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : 인쇄품질-NO DATA");
				
				  if(auto_mode)
				  {
				  sprintf(temp, "CAM #%d[%02d:%02d:%02d] : 인쇄품질-NO DATA", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
				  WriteStatusList(temp);
				  
					//OCV variation data fail
					ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, STDF, 0, 0., 0., 0., 0.);
					ng_idx++;
					}
					}
			*/
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		//SR/////////////////////////////////////
		//SR bubble
		switch(ins_ng_rslt.m_InsRslt.m_iRsltPIFold[i])
		{
		case 0:
			//No inspection
			break;
		case 1:
			//Good
			if(chk_ng_type)
			{
				//No display
				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 50);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
				}
			}
			break;
		case -60:
			//No SR pattern
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = STDF;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Film data fail
					ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, STDF, 0, 0., 0., 0., 0., save[i]);
					save[i] = FALSE;
					ng_idx++;
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 50);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : SR NO DATA");
					
					if(auto_mode)
					{					
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : SR NO DATA", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//Film data fail
						//ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, STDF, 0, 0., 0., 0., 0., save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					//CHOKI20111027
					if(ng_view_data_save)
					{
						//ResultIMSLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx_ims, STDF, 0, 0., 0., 0., 0., save[i]);
						ng_idx_ims++;
					}
					//CHOKI20111027
					
					m_iNGScrollLastNum[cam_idx]++;
				}
				else
				{
					cur_scroll_idx++;
					if(ScrollCheck(cur_scroll_idx))
					{
						//Image
						//Search
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 50);
						smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
						
						sx = (Hlong)Col0[0] - SUB_NG_VIEW_INNER;
						sy = (Hlong)Row0[0] - SUB_NG_VIEW_INNER;
						ex = (Hlong)Col1[0] + SUB_NG_VIEW_INNER;
						ey = (Hlong)Row1[0] + SUB_NG_VIEW_INNER;
						
						set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
						
						disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapSR[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						//Search
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 10);
						disp_obj(*RsltRegion, m_lNGWindowID[m_iNGSubViewIdx[0]]);
						
						set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
						set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
						write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("NG : SR NO DATA"));
						
						ScrollCheck();
					}
				}
			}
			break;
		case -61:
			//기포
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = BUBD;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//개수별
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iPIFoldNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, BUBD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 50);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 51);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : SR 기포");
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					//CHOKI20100331*/
					long test1, test2; 
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),3);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					//CHOKI20100331*/
					
					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : SR 기포", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//Log
						//개수별
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iPIFoldNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, BUBD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}
					//CHOKI20111027
					if(ng_view_data_save)
					{
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iPIFoldNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							//ResultIMSLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx_ims, BUBD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx_ims++;
						}
					}
					//CHOKI20111027
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					//Image
					//기포
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 51);
					
					
					//개수별
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iPIFoldNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapSR[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : SR 기포");
							
							ScrollCheck();
						}
					}
				}
			}
			break;
		case -71:
			//찍힘
			if(chk_ng_type)
			{
				//No display
				if(0 == m_bChkNGKind[cam_idx][i])
				{
					m_bChkNGKind[cam_idx][i] = FPCD;
				}				
			}
			else
			{
				//20110509 lee
				if(!chk_ng_Disp && auto_mode)
				{
					//Log
					//개수별
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iPIHallNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
						//cx, cy, sizex, sizey
						ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, FPCD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
						save[i] = FALSE;
						ng_idx++;
					}
					break;
				}

				if(main_view)
				{
					//Search
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("yellow"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 52);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 53);
					disp_obj(*RsltRegion, m_dlgImgRect[cam_idx]->GetView());
					
					
					set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
					
					set_tposition(m_dlgImgRect[cam_idx]->GetView(), nCol, nRow);
					nCol  += nStep;

					write_string(m_dlgImgRect[cam_idx]->GetView(), "NG : PI 찍힘/구겨짐");
					
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					//CHOKI20100331*/
					long test1, test2;
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),3);
					for(long cho=0;rslt_num>cho;cho++)
					{
						test2=abs((long)((Hlong)Row1[cho]-(Hlong)Row0[cho]));
						if(test2<30) test2=test2+5;
						test1=abs((long)((Hlong)Col1[cho]-(Hlong)Col0[cho]));
						if(test1<30) test1=test1+5;
						set_color(m_dlgImgRect[cam_idx]->GetView(), HTuple("red"));
						disp_ellipse(m_dlgImgRect[cam_idx]->GetView(),(Hlong)CRow[cho],(Hlong)CCol[cho],0,test1,test2);
						set_tposition(m_dlgImgRect[cam_idx]->GetView(), (Hlong)CRow[cho], (Hlong)CCol[cho]+test1+5);  write_string(m_dlgImgRect[cam_idx]->GetView(),  HTuple(i));
					}
					set_line_width(m_dlgImgRect[cam_idx]->GetView(),1);
					//CHOKI20100331*/
					
					if(auto_mode)
					{
						//sprintf(temp, "CAM #%d[%02d:%02d:%02d] : SR 기포", cam_idx + 1, lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
						//WriteStatusList(temp);
						
						//Log
						//개수별
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iPIHallNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							ResultLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx, FPCD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx++;
						}
					}
					//CHOKI20111027
					if(ng_view_data_save)
					{
						for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iPIHallNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							//ResultIMSLog(cam_idx, buf_idx, ng_view_idx, i, ng_idx_ims, FPCD, (Hlong)Area[j], (double)CCol[j], (double)CRow[j], rect.Width() * sys_para.m_dPara[0], rect.Height() * sys_para.m_dPara[0], save[i]);
							save[i] = FALSE;
							ng_idx_ims++;
						}
					}
					//CHOKI20111027
					
					m_iNGScrollLastNum[cam_idx]+=rslt_num;
				}
				else
				{
					//Image
					//기포
					RsltRegion = m_VisMod->GetHalNGRsltRgn(cam_idx, ng_view_idx, i, 53);
					
					
					//개수별
					area_center(*RsltRegion, &Area, &CRow, &CCol);
					smallest_rectangle1(*RsltRegion, &Row0, &Col0, &Row1, &Col1);
					rslt_num = (Hlong)Row0.Num();
					for(j = 0; (j < ins_ng_rslt.m_InsRslt.m_iPIHallNum[i]) && (j < NG_MAX_NUM) && (j < rslt_num) ; j++)
					{
						cur_scroll_idx++;
						if(ScrollCheck(cur_scroll_idx))
						{
							rect = CRect((Hlong)Row0[j], (Hlong)Col0[j], (Hlong)Row1[j], (Hlong)Col1[j]);
							//cx, cy, sizex, sizey
							
							sx = (Hlong)Col0[j] - SUB_NG_VIEW_INNER;
							sy = (Hlong)Row0[j] - SUB_NG_VIEW_INNER;
							ex = (Hlong)Col1[j] + SUB_NG_VIEW_INNER;
							ey = (Hlong)Row1[j] + SUB_NG_VIEW_INNER;
							
							set_part(m_lNGWindowID[m_iNGSubViewIdx[0]], sy, sx, ey, ex);
							
							disp_obj(*m_VisMod->GetHalNGImage(0, ng_view_idx, ins_para.m_iParaMapSR[0]), m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							
							select_obj(*RsltRegion, &SelectRgn, j + 1);
							disp_obj(SelectRgn, m_lNGWindowID[m_iNGSubViewIdx[0]]);
							
							set_color(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple("red"));
							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 10, sx + 10);
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], "NG : PI 찍힘/구겨짐");

							set_tposition(m_lNGWindowID[m_iNGSubViewIdx[0]], sy + 15, sx + 10);
							
							write_string(m_lNGWindowID[m_iNGSubViewIdx[0]], HTuple( HTuple("Dx : ") + 
								HTuple(((Hlong)Col1[j]-(Hlong)Col0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") + HTuple("Dy : ") + 
								HTuple(((Hlong)Row1[j]-(Hlong)Row0[j])*m_VisMod->m_dPixelSizeData) + HTuple("um, ") ));
							
							ScrollCheck();
						}
					}
				}
			}
			break;
		default:
			AfxMessageBox("Error : invaild index!");
			break;
		}
		
		
		
	}
}




//
//USER FUNCTION END
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//Message Map Fucntions Begin
//
void CFilmInsDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	typeInspectPara ins_para;
	typeSystemPara sys_para;
	int i;
	double dist, sy, pos;
	CString strFrameRate;
	ULONG StartTime, Endtime;

	Hobject RegionRect;

	switch(nIDEvent)
	{
	case 0:

#ifdef _CAM	
		StartTime = GetTickCount();

		if(m_bIsColor)
			m_cCam.Grab(0, m_iBufIdx[0], m_iMapIdx[0], m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_bIsColor, 
				m_VisMod->GetHalRedImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_VisMod->GetHalGreenImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_VisMod->GetHalBlueImage(0, m_iBufIdx[0], m_iMapIdx[0]));
		else
			m_cCam.Grab(0, m_iBufIdx[0], m_iMapIdx[0], m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]));

		Endtime = GetTickCount();

		strFrameRate.Format("%3.3f",1000 / (double)(Endtime - StartTime));
		GetDlgItem(IDC_STATIC_FRAME_RATE)->SetWindowText(strFrameRate);
#endif
		if(m_bIsColor)
		{
			switch(m_lColorMode)
			{
			case 1:
				disp_obj(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[0]->GetView());
				disp_obj(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_lWindowID[m_iMapIdx[0]]);
				break;
			case 2:
				disp_obj(*m_VisMod->GetHalRedImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[0]->GetView());
				disp_obj(*m_VisMod->GetHalRedImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_lWindowID[m_iMapIdx[0]]);
				break;
			case 3:
				disp_obj(*m_VisMod->GetHalGreenImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[0]->GetView());
				disp_obj(*m_VisMod->GetHalGreenImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_lWindowID[m_iMapIdx[0]]);
				break;
			case 0:	
				disp_obj(*m_VisMod->GetHalBlueImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[0]->GetView());
				disp_obj(*m_VisMod->GetHalBlueImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_lWindowID[m_iMapIdx[0]]);
				break;	
			}
		}
		else
			disp_obj(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[0]->GetView());

		set_color(m_dlgImgRect[0]->GetView(), HTuple("green"));
		disp_line(m_dlgImgRect[0]->GetView(), m_lHeight[0] * 0.5, 0, m_lHeight[0] * 0.5, m_lWidth[0]);
		disp_line(m_dlgImgRect[0]->GetView(), 0, m_lWidth[0] * 0.5, m_lHeight[0], m_lWidth[0] * 0.5);
		//수평 기준 라인 Display (48mm기준)
		set_color(m_dlgImgRect[0]->GetView(), HTuple("red"));

		//if(m_VisMod->GetPCNum(0) == 0)
		//{
		disp_line(m_dlgImgRect[0]->GetView(), 0, (m_lWidth[0] * 0.5)+(m_dlgSetup->m_InsPara.m_dParaBase[2] * 1000/m_dlgSetup->m_SysPara.m_dPara[0]/2) ,
						m_lHeight[0], (m_lWidth[0] * 0.5)+(m_dlgSetup->m_InsPara.m_dParaBase[2] * 1000/m_dlgSetup->m_SysPara.m_dPara[0]/2));
		disp_line(m_dlgImgRect[0]->GetView(), 0, (m_lWidth[0] * 0.5)-(m_dlgSetup->m_InsPara.m_dParaBase[2] * 1000/m_dlgSetup->m_SysPara.m_dPara[0]/2) ,
						m_lHeight[0], (m_lWidth[0] * 0.5)-(m_dlgSetup->m_InsPara.m_dParaBase[2] * 1000/m_dlgSetup->m_SysPara.m_dPara[0]/2));
		//}
	

		/////////////////////////////////////////
		//Display film region for start position
		//
		ins_para = m_VisMod->GetInspectPara(0);
		sys_para = m_VisMod->GetSystemPara(0);
	
		double dCofWidth;
		//COF pitch
		dCofWidth = ins_para.m_iParaBase[2] * ins_para.m_dParaBase[1]; //20110513 lee
		//Pixel size
		sys_para.m_dPara[0];

		if(0 < sys_para.m_dPara[0])
		{
			dist = ( (dCofWidth * 1000) / sys_para.m_dPara[0]) * 0.5; //20110513 lee
		}
		else
		{
			dist = 0;
		}

		sy = (m_lHeight[0] * 0.5) - (dist * m_VisMod->GetCOFNum(0));

		if( (0 > sy) || (m_lHeight[0] < sy) )
		{
			sy = 0;
		}

		pos = sy;

		set_color(m_dlgImgRect[0]->GetView(), HTuple("yellow"));
		for(i = 0; (i < m_VisMod->GetCOFNum(0)) && (i < SUB_RGN_MAX_NUM); i++)
		{
			//Up
			pos = pos + (dist * i * 2);
			if( (0 > pos) || (m_lHeight[0] < pos) )
			{
				pos = 0;
			}
			disp_line(m_dlgImgRect[0]->GetView(), pos, 0, pos, m_lWidth[0]);

			//Dn
			pos = pos + (dist * 2);
			if( (0 > pos) || (m_lHeight[0] < pos) )
			{
				pos = m_lHeight[0];
			}
			
			disp_line(m_dlgImgRect[0]->GetView(), pos, 0, pos, m_lWidth[0]);
			//다시 초기 위치로 변경 한다.
			pos = sy;

		}

		if(m_lProfileMode == 4)
		{
			CalculateGrayValues();
		}

		if(m_lProfileMode == 5)
		{
			CalculateContrastValues();
		}

		//
		//
		/////////////////////////////////////////

		//Align inspect region
		set_color(m_dlgImgRect[0]->GetView(), HTuple("green"));
		gen_rectangle1(&RegionRect, ins_para.m_InsRegionAlign[0][0].top, ins_para.m_InsRegionAlign[0][0].left, ins_para.m_InsRegionAlign[0][0].bottom, ins_para.m_InsRegionAlign[0][0].right);
		disp_obj(RegionRect, m_dlgImgRect[0]->GetView()); 
		gen_rectangle1(&RegionRect, ins_para.m_InsRegionAlign[0][2].top, ins_para.m_InsRegionAlign[0][2].left, ins_para.m_InsRegionAlign[0][2].bottom, ins_para.m_InsRegionAlign[0][2].right);
		disp_obj(RegionRect, m_dlgImgRect[0]->GetView()); 
		break;
	case 1:	// SOCKET Reconnect try
		/*
		{
			int portnum = 0;

			switch(m_VisMod->GetVisIndex())
			{
			case VISION_CAM_TOP1:
				portnum = SOCKET_PORT_NUM1;
				break;
			case VISION_CAM_BOTTOM1:
				portnum = SOCKET_PORT_NUM2;
				break;
			case VISION_CAM_TOP2:
				portnum = SOCKET_PORT_NUM3;
				break;
			case VISION_CAM_BOTTOM2:
				portnum = SOCKET_PORT_NUM4;
				break;
			}

			m_Client->Connect((LPCTSTR)CONTROL_PC_IP_ADDRESS, portnum);

			break;
		}
		*/
		break;
	//Monitoring
	case 10:
		break;
	}
	//2910000330 3012302030

	CDialog::OnTimer(nIDEvent);
}

void CFilmInsDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	for(int i = 0; i < IMG_RECT_MAX_NUM; i++)
	{
		delete m_dlgImgRect[i];	
		m_dlgImgRect[i] = NULL;
	}

	m_BrushBlack.DeleteObject();
	
	delete m_dlgSetup;
	m_dlgSetup = NULL;

	delete m_Client;
	m_Client = NULL;

	delete m_VisMod;
	m_VisMod = NULL;
}


void CFilmInsDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
		if(!pScrollBar || !pScrollBar->m_hWnd)
        return;

    if(pScrollBar->GetDlgCtrlID() == IDC_SCROLLBAR_NG)
    {
        UINT nCurPos = pScrollBar->GetScrollPos();
        switch(nSBCode)
        {
        case SB_BOTTOM:         // 7-Scroll to bottom.
            pScrollBar->SetScrollPos(0);
            break;

        case SB_ENDSCROLL:      // 8-End scroll.
            break;

        case SB_LINEDOWN:       // 1-Scroll one line down.
            nCurPos += 1;

			if(nCurPos >= m_iNGScrollLastNum[0])
                nCurPos = m_iNGScrollLastNum[0] - 1;
			
            pScrollBar->SetScrollPos(nCurPos);
            break;

        case SB_LINEUP:         // 0-Scroll one line up.
            nCurPos -= 1;
            if(nCurPos < 0)
                nCurPos = 0;
            pScrollBar->SetScrollPos(nCurPos);
            break;

        case SB_PAGEDOWN:       // 3-Scroll one page down.
            nCurPos += 5;
			
            if(nCurPos >= m_iNGScrollLastNum[0])
                nCurPos = m_iNGScrollLastNum[0] - 1;
			
            pScrollBar->SetScrollPos(nCurPos);
            break;

        case SB_PAGEUP:         // 2-Scroll one page up.
            nCurPos -= 5;
            if(nCurPos < 0)
                nCurPos = 0;
            pScrollBar->SetScrollPos(nCurPos);
            break;

        case SB_THUMBPOSITION:  // 4-Scroll to the absolute position. The current position is provided in nPos.
            pScrollBar->SetScrollPos(nPos);
            break;

        case SB_THUMBTRACK:     // 5-Drag scroll box to specified position. The current position is provided in nPos.
            pScrollBar->SetScrollPos(nPos);
            break;

        case SB_TOP:            // 6-Scroll to top.
            pScrollBar->SetScrollPos(0);
            break;
        }

		if(0 > nCurPos)
			nCurPos = 0;
		m_iNGScrollIdx[0] = nCurPos;

		//NG view
		ResultCheck_Display(0, 0, m_iNGViewIdx[0], FALSE, FALSE, FALSE ,TRUE ,FALSE); //20110509 lee
    }


	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CFilmInsDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message ==WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}


HBRUSH CFilmInsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	pDC->SetTextColor(RGB(255, 255, 255));    
	pDC->SetBkColor(RGB(0, 0, 0));

	return (HBRUSH)m_BrushBlack;

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.

	//return hbr;
}

LRESULT CFilmInsDlg::OnConnect(WPARAM wParam, LPARAM lParam)
{
//m_Client->GetLastError
	KillTimer(1);

	m_label_online.SetCaption("ON-LINE");
	m_label_online.SetForeColor(RGB(0, 255, 0));
	m_label_online.SetOffice2007ColorInnerBorder(RGB(0, 255, 0));
	m_label_online.SetOffice2007ColorOuterBorder(RGB(0, 255, 0));

	WriteStatusList("Success : SOCKET connected !");

	return 0;
}

LRESULT CFilmInsDlg::OnClose(WPARAM wParam, LPARAM lParam)
{
	delete m_Client;
	m_Client = NULL;

	m_label_online.SetCaption("OFF-LINE");
	m_label_online.SetForeColor(RGB(255, 0, 0));
	m_label_online.SetOffice2007ColorInnerBorder(RGB(255, 0, 0));
	m_label_online.SetOffice2007ColorOuterBorder(RGB(255, 0, 0));

	m_Client = new CClientSock();
	m_Client->Create();
	m_Client->SetWnd(this->m_hWnd); 

	//SetTimer(1, 1000, NULL);

	WriteStatusList("Error : SOCKET disconnected !");

	return 0;
}


LRESULT CFilmInsDlg::OnReceive(WPARAM wParam, LPARAM lParam)
{
	char temp[PACKET_MAX_NUM];
	CString rcv_dat, rcv_temp, snd_dat;
	CString strCamIdx, strCmd, strBufIdx, strDevIdx, strModel, strLotNo, strLotStarTime, strChipCnt, strTemp, status;
	int nCount = 0;
	typeLotInfo lot_info;
	
	typeInspectPara ins_para;
	ins_para = m_VisMod->GetInspectPara(0);	

	m_Client->Receive((LPSTR)temp, PACKET_MAX_NUM);
	rcv_temp = temp;

	nCount = count((LPCTSTR)rcv_temp, (LPCTSTR)rcv_temp+rcv_temp.GetLength(), PACKET_CHAR_ETX); // 두개의 패킷이 붙어서 올 경우 ETX 를 기준으로 잘라서 각각 처리한다.
	
	for(int i = 0; i < nCount; i++)
	{
		AfxExtractSubString(rcv_dat, rcv_temp, i, PACKET_CHAR_ETX);
		rcv_dat.Delete(0, 1);

		WriteTCPList((LPCTSTR)rcv_dat, TRUE);
		
		AfxExtractSubString(strCmd, rcv_dat, 1, ',');	//Parsing Comman

		if(!strCmd.Compare("0001"))						//Grab Start
		{
			AfxExtractSubString(strBufIdx, rcv_dat, 2, ',');
			AfxExtractSubString(strDevIdx, rcv_dat, 3, ',');

			m_iGrbIdx[0] = atoi((LPCTSTR)strBufIdx);
			m_iGrbDevIdx[0] = atoi((LPCTSTR)strDevIdx);

			m_VisFlagData[0].m_bFlag[7] = TRUE;
		}
		else if(!strCmd.Compare("0003"))				//Inspect Start
		{
			AfxExtractSubString(strBufIdx, rcv_dat, 2, ',');
			AfxExtractSubString(strDevIdx, rcv_dat, 3, ',');
			AfxExtractSubString(strLotNo, rcv_dat, 4, ',');
			AfxExtractSubString(strLotStarTime, rcv_dat, 5, ',');
			AfxExtractSubString(strChipCnt, rcv_dat, 6, ',');
			
			if(!strChipCnt.IsEmpty())
			{
				//칩 수 만큼 펀칭 결과를 담는다.
				for(int i = 0; i < atoi((LPCTSTR)strChipCnt) && i < COF_MAX_NUM; i++)
				{
					AfxExtractSubString(strTemp, rcv_dat, 7+i, ',');
					ins_para.m_bIsPunchSkip[i] = atoi((LPCTSTR)strTemp);
				}
				
				m_VisMod->SetInspectPara(0, ins_para);
			}

			m_iPrcIdx[0] = atoi((LPCTSTR)strBufIdx);
			m_iBufIdx[0] = m_iPrcIdx[0];
			m_iPrcDevIdx[0] = atoi((LPCTSTR)strDevIdx);
			
			m_strLotNo = strLotNo;
			m_strStartTime = strLotStarTime;

			m_VisFlagData[0].m_bFlag[10] = TRUE;
		}
		else if(!strCmd.Compare("0005"))				//Find Start
		{
			m_VisFlagData[0].m_bFlag[0] = TRUE;
		}
		else if(!strCmd.Compare("0007"))				//Align Start
		{
			m_VisFlagData[0].m_bFlag[4] = TRUE;
		}
		else if(!strCmd.Compare("0009"))				//OCV Teach Start
		{

		}
		else if(!strCmd.Compare("0011"))				//Model Change
		{
			AfxExtractSubString(strModel, rcv_dat, 2, ',');

			if(RemoteControlModel(2, strModel))
			{
				//성공 응답패킷 전송
				snd_dat.Format("%cVIS%d,0012,%s%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), strModel, PACKET_CHAR_ETX);
				DoEvents();
				OnSend(snd_dat);
				DoEvents();
				

			}
			else
			{
				//실패 응답패킷 전송
				snd_dat.Format("%cVIS%d,0012%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), PACKET_CHAR_ETX);
				DoEvents();
				OnSend(snd_dat);
				DoEvents();

			}
		}
		else if(!strCmd.Compare("0013"))				//Auto Run
		{
			if(!m_btnStartStop.GetValue())
			{
				m_btnStartStop.SetValue(TRUE);
				ClickBtnenhctrlRunstop();
			}

			SendMessage(USER_MSG_SOCKET, 0, 6);
		}
		else if(!strCmd.Compare("0015"))				//Grab&Display
		{
			m_VisFlagData[0].m_bFlag[3] = TRUE;

		}
		else if(!strCmd.Compare("0017"))				//Lot Start
		{
			/*AfxExtractSubString(strLotNo, rcv_dat, 2, ',');
			AfxExtractSubString(strLotStarTime, rcv_dat, 3, ',');

			m_strStartTime = strLotStarTime;
			m_strLotNo = strLotNo;

			m_iGrbIdx[0] = 0;
			m_iBufIdx[0] = 0;
			m_iPrcIdx[0] = 0;
			m_iRslt[0] = 1;

			//Flag
			m_bFlagGrabStart[0] = FALSE;

			//NG view
			m_iNGViewIdx[0] = 0;
			m_iNGSubViewIdx[0] = 0;

			m_bInspectStartNGView[0] = FALSE;

			//Lot info.
			m_VisMod->ResetLotInfo();
			lot_info = m_VisMod->GetLotInfo();

			sprintf(lot_info.m_sNo, "%s", strLotNo);
			sprintf(lot_info.m_sStart, "%s", strLotStarTime);

			m_VisMod->SetLotInfo(lot_info);
			LotDisp(lot_info);

			//NG view clear
			for(i = 0; i < IMAGE_NG_NUM; i++)
			{
				clear_window(m_lNGWindowID[i]);
			}

			status.Format("******************************** %s Lot Start!! (%s)",		
			lot_info.m_sNo, lot_info.m_sStart);
			m_VisMod->WriteErrorList((LPCTSTR)status);
			
			snd_dat.Format("%cVIS%d,0018%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), PACKET_CHAR_ETX);
			OnSend(snd_dat);

			ClickBtnenhctrlLabelNgview();*/

			//20150604 ngh Auto Delete
			m_bDeleteFolderStart = TRUE;
			m_pDeleSearchThread = AfxBeginThread(DeleteSearchThread, this, THREAD_PRIORITY_NORMAL);
			if (m_pDeleSearchThread) m_pDeleSearchThread->m_bAutoDelete = TRUE;
			while(TRUE)
			{
				if(m_bDeleteFolder)
				{
					m_bDeleteFolder = FALSE;
					m_bDeleteFolderStart = FALSE;
					AfxExtractSubString(strLotNo, rcv_dat, 2, ',');
					AfxExtractSubString(strLotStarTime, rcv_dat, 3, ',');

					m_strStartTime = strLotStarTime;
					m_strLotNo = strLotNo;

					m_iGrbIdx[0] = 0;
					m_iBufIdx[0] = 0;
					m_iPrcIdx[0] = 0;
					m_iRslt[0] = 1;

					//Flag
					m_bFlagGrabStart[0] = FALSE;

					//NG view
					m_iNGViewIdx[0] = 0;
					m_iNGSubViewIdx[0] = 0;

					m_bInspectStartNGView[0] = FALSE;

					//Lot info.
					m_VisMod->ResetLotInfo();
					lot_info = m_VisMod->GetLotInfo();

					sprintf(lot_info.m_sNo, "%s", strLotNo);
					sprintf(lot_info.m_sStart, "%s", strLotStarTime);

					m_VisMod->SetLotInfo(lot_info);
					LotDisp(lot_info);

					//NG view clear
					for(i = 0; i < IMAGE_NG_NUM; i++)
					{
						clear_window(m_lNGWindowID[i]);
					}

					status.Format("******************************** %s Lot Start!! (%s)",		
					lot_info.m_sNo, lot_info.m_sStart);
					m_VisMod->WriteErrorList((LPCTSTR)status);
					
					snd_dat.Format("%cVIS%d,0018%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), PACKET_CHAR_ETX);
					OnSend(snd_dat);

					ClickBtnenhctrlLabelNgview();

					break;
				}

				Sleep(1);
			}
		}
		else if(!strCmd.Compare("0019"))				//Lot End
		{
			AfxExtractSubString(strLotNo, rcv_dat, 2, ',');

			//Lot info.
			lot_info = m_VisMod->GetLotInfo();

			SYSTEMTIME	lpSystemTime;
			GetLocalTime(&lpSystemTime);

			sprintf(lot_info.m_sEnd, "%04d-%02d-%02d,%02d:%02d:%02d",  
					lpSystemTime.wYear, lpSystemTime.wMonth, lpSystemTime.wDay,
					lpSystemTime.wHour, lpSystemTime.wMinute, lpSystemTime.wSecond);
			
			m_VisMod->SetLotInfo(lot_info);
			LotDisp(lot_info);
			WriteLot(lot_info);

			snd_dat.Format("%cVIS%d,0020%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), PACKET_CHAR_ETX);
			OnSend(snd_dat);
		}
		else if(!strCmd.Compare("0021"))				//NG Display
		{
			ClickBtnenhctrlLabelNgview();

			SetWindowPos(&wndTopMost, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);
			SetWindowPos(&wndNoTopMost, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);

			AfxExtractSubString(strBufIdx, rcv_dat, 2, ',');
			AfxExtractSubString(strDevIdx, rcv_dat, 3, ',');
			AfxExtractSubString(strLotNo, rcv_dat, 4, ',');
			AfxExtractSubString(strLotStarTime, rcv_dat, 5, ',');
			AfxExtractSubString(strChipCnt, rcv_dat, 6, ',');
			
			if(!strChipCnt.IsEmpty())
			{
				//칩 수 만큼 펀칭 결과를 담는다.
				for(int i = 0; i < atoi((LPCTSTR)strChipCnt) && i < COF_MAX_NUM; i++)
				{
					AfxExtractSubString(strTemp, rcv_dat, 7+i, ',');
					ins_para.m_bIsPunchSkip[i] = atoi((LPCTSTR)strTemp);
				}
				
				m_VisMod->SetInspectPara(0, ins_para);
			}

			m_strLotNo = strLotNo;
			m_strStartTime = strLotStarTime;

			lot_info = m_VisMod->GetLotInfo();
			sprintf(m_sNGDate[0], "%s", (LPCTSTR)m_strStartTime);
			sprintf(m_sNGLotID[0], "%s", (LPCTSTR)m_strLotNo);

			m_iNGDeviceIdx[0] = atoi((LPCTSTR)strDevIdx);

			m_bInspectStartNGView[0] = TRUE;
		}
		else if(!strCmd.Compare("0023"))		// SetFocus Window
		{
			SetWindowPos(&wndTopMost, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);
			SetWindowPos(&wndNoTopMost, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE);

			//성공 응답패킷 전송
			snd_dat.Format("%cVIS%d,0024%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), PACKET_CHAR_ETX);
			OnSend(snd_dat);
		}
		else if(!strCmd.Compare("0025"))		//Auto Teach OCV
		{
			//OCV는 상부1(모노)에서만 하는 것으로 가정.
			ins_para = m_VisMod->GetInspectPara(0);	

			//Grab
			//20150223 by
			COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[0], TRUE);
			COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[1], TRUE);
			COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[2], TRUE);
			if(m_iLightOnOffIdx[3] > VISION_CAM_NONE)
				COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[3], TRUE);
			/*
			COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[0], TRUE);
			COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[1], TRUE);
			COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[2], TRUE);
			COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[3], TRUE);
			*/

			SetLight(m_iLightChIdx[0], m_img2_ch0);
			SetLight(m_iLightChIdx[1], m_img2_ch1);
			SetLight(m_iLightChIdx[2], m_img2_ch2);
			SetLight(m_iLightChIdx[3], m_img2_ch3);
			
			Sleep(100);

			m_cCam.Grab(0, 0, 0, m_VisMod->GetHalImage(0, m_iBufIdx[0], ins_para.m_iParaMapOCV[0]));

			Sleep(100);

			//20150223 by
			COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[0], FALSE);
			COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[1], FALSE);
			COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[2], FALSE);
			if(m_iLightOnOffIdx[3] > VISION_CAM_NONE)
				COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[3], FALSE);
			/*
			COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[0], FALSE);
			COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[1], FALSE);
			COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[2], FALSE);
			COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[3], FALSE);
			*/

			if(AutoTeachOCV())
			{
				::SendMessage(m_hWnd, USER_MSG_SOCKET, 1, 4);	// Success
			}
			else
			{
				::SendMessage(m_hWnd, USER_MSG_SOCKET, 0, 4);	// Fail
			}

		}
		else	//잘못된 패킷
		{
			
		}
	}

	return 0;
}

void CFilmInsDlg::DisplayView(int cam_idx, int buf_idx, int img_idx)
{
	if(m_bIsColor)
	{
		switch(m_lColorMode)
		{
		case 1:
			disp_obj(*m_VisMod->GetHalImage(cam_idx, buf_idx, img_idx), m_dlgImgRect[0]->GetView());
			disp_obj(*m_VisMod->GetHalImage(cam_idx, buf_idx, img_idx), m_lWindowID[img_idx]);	
			break;
		case 2:
			disp_obj(*m_VisMod->GetHalRedImage(cam_idx, buf_idx, img_idx), m_dlgImgRect[0]->GetView());
			disp_obj(*m_VisMod->GetHalRedImage(cam_idx, buf_idx, img_idx), m_lWindowID[img_idx]);	
			break;
		case 3:
			disp_obj(*m_VisMod->GetHalGreenImage(cam_idx, buf_idx, img_idx), m_dlgImgRect[0]->GetView());
			disp_obj(*m_VisMod->GetHalGreenImage(cam_idx, buf_idx, img_idx), m_lWindowID[img_idx]);	
			break;
		case 0:	
			disp_obj(*m_VisMod->GetHalBlueImage(cam_idx, buf_idx, img_idx), m_dlgImgRect[0]->GetView());
			disp_obj(*m_VisMod->GetHalBlueImage(cam_idx, buf_idx, img_idx), m_lWindowID[img_idx]);	
			break;	
		}
	}
	else
	{
		disp_obj(*m_VisMod->GetHalImage(cam_idx, buf_idx, img_idx), m_dlgImgRect[0]->GetView());
		disp_obj(*m_VisMod->GetHalImage(cam_idx, buf_idx, img_idx), m_lWindowID[img_idx]);	
	}

}

void CFilmInsDlg::OnRectImgMap0()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_iMapIdx[0] = 0;

	DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);

	UpdateMapButton(m_iMapIdx[0]);     
#ifdef _CAM
	if(!m_bInspectStart[0])
	{
		SetLight(m_iLightChIdx[0], m_img0_ch0);
		SetLight(m_iLightChIdx[1], m_img0_ch1);
		SetLight(m_iLightChIdx[2], m_img0_ch2);
		SetLight(m_iLightChIdx[3], m_img0_ch3);
	}
#endif
	if(m_bLiveOverlayDisp)
	{
		//(int cam_idx, int buf_idx, int ng_view_idx, BOOL auto_mode, BOOL main_view, BOOL chk_ng_type, BOOL chk_ng_Disp, BOOL ng_view_data_save) //20220509 lee
		ResultCheck_Display(0, m_iPrcIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//20110509 lee
	}
}

void CFilmInsDlg::OnRectImgMap1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_iMapIdx[0] = 1;

	DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);

	UpdateMapButton(m_iMapIdx[0]);     
#ifdef _CAM
	if(!m_bInspectStart[0])
	{
		SetLight(m_iLightChIdx[0], m_img1_ch0);
		SetLight(m_iLightChIdx[1], m_img1_ch1);
		SetLight(m_iLightChIdx[2], m_img1_ch2);
		SetLight(m_iLightChIdx[3], m_img1_ch3);
	}
#endif
	if(m_bLiveOverlayDisp)
	{
		ResultCheck_Display(0, m_iPrcIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//20110509 lee
	}
}

void CFilmInsDlg::OnRectImgMap2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_iMapIdx[0] = 2;

	DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);

	UpdateMapButton(m_iMapIdx[0]);     
#ifdef _CAM	
	if(!m_bInspectStart[0])
	{
		SetLight(m_iLightChIdx[0], m_img2_ch0);
		SetLight(m_iLightChIdx[1], m_img2_ch1);
		SetLight(m_iLightChIdx[2], m_img2_ch2);
		SetLight(m_iLightChIdx[3], m_img2_ch3);
	}
#endif
	if(m_bLiveOverlayDisp)
	{
		ResultCheck_Display(0, m_iPrcIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//20110509 lee
	}
}

void CFilmInsDlg::OnRectImgMap3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_iMapIdx[0] = 3;

	DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);

	UpdateMapButton(m_iMapIdx[0]);     
#ifdef _CAM	
	if(!m_bInspectStart[0])
	{
		SetLight(m_iLightChIdx[0], m_img3_ch0);
		SetLight(m_iLightChIdx[1], m_img3_ch1);
		SetLight(m_iLightChIdx[2], m_img3_ch2);
		SetLight(m_iLightChIdx[3], m_img3_ch3);
	}
#endif
	if(m_bLiveOverlayDisp)
	{
		ResultCheck_Display(0, m_iPrcIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//20110509 lee
	}
}

void CFilmInsDlg::OnRectImgMap4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_iMapIdx[0] = 4;

	DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);

	UpdateMapButton(m_iMapIdx[0]);     
#ifdef _CAM	
	if(!m_bInspectStart[0])
	{
		SetLight(m_iLightChIdx[0], m_img4_ch0);
		SetLight(m_iLightChIdx[1], m_img4_ch1);
		SetLight(m_iLightChIdx[2], m_img4_ch2);
		SetLight(m_iLightChIdx[3], m_img4_ch3);
	}
#endif
	if(m_bLiveOverlayDisp)
	{
		ResultCheck_Display(0, m_iPrcIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//20110509 lee
	}
}

void CFilmInsDlg::OnRectImgMap5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_iMapIdx[0] = 5;

	DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);

	UpdateMapButton(m_iMapIdx[0]);     
	
	if(!m_bInspectStart[0])
	{
		SetLight(m_iLightChIdx[0], m_img5_ch0);
		SetLight(m_iLightChIdx[1], m_img5_ch1);
		SetLight(m_iLightChIdx[2], m_img5_ch2);
		SetLight(m_iLightChIdx[3], m_img5_ch3);
	}

	if(m_bLiveOverlayDisp)
	{
		ResultCheck_Display(0, m_iPrcIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//20110509 lee
	}
}

void CFilmInsDlg::OnRectImgMap6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_iMapIdx[0] = 6;

	DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);

	UpdateMapButton(m_iMapIdx[0]);     
	
	if(!m_bInspectStart[0])
	{
		SetLight(m_iLightChIdx[0], m_img6_ch0);
		SetLight(m_iLightChIdx[1], m_img6_ch1);
		SetLight(m_iLightChIdx[2], m_img6_ch2);
		SetLight(m_iLightChIdx[3], m_img6_ch3);
	}

	if(m_bLiveOverlayDisp)
	{
		ResultCheck_Display(0, m_iPrcIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//20110509 lee
	}
}

void CFilmInsDlg::OnRectImgMap7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_iMapIdx[0] = 7;

	DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);

	UpdateMapButton(m_iMapIdx[0]);     
	
	if(!m_bInspectStart[0])
	{
		SetLight(m_iLightChIdx[0], m_img7_ch0);
		SetLight(m_iLightChIdx[1], m_img7_ch1);
		SetLight(m_iLightChIdx[2], m_img7_ch2);
		SetLight(m_iLightChIdx[3], m_img7_ch3);
	}

	if(m_bLiveOverlayDisp)
	{
		ResultCheck_Display(0, m_iPrcIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//20110509 lee
	}
}

void CFilmInsDlg::OnRectImgMap8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_iMapIdx[0] = 8;

	DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);

	UpdateMapButton(m_iMapIdx[0]);     
	
	if(!m_bInspectStart[0])
	{
		SetLight(m_iLightChIdx[0], m_img8_ch0);
		SetLight(m_iLightChIdx[1], m_img8_ch1);
		SetLight(m_iLightChIdx[2], m_img8_ch2);
		SetLight(m_iLightChIdx[3], m_img8_ch3);
	}

	if(m_bLiveOverlayDisp)
	{
		ResultCheck_Display(0, m_iPrcIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//20110509 lee
	}
}

void CFilmInsDlg::OnRectImgMap9()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_iMapIdx[0] = 9;
	
	DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);

	UpdateMapButton(m_iMapIdx[0]);     
	
	if(!m_bInspectStart[0])
	{
		SetLight(m_iLightChIdx[0], m_img9_ch0);
		SetLight(m_iLightChIdx[1], m_img9_ch1);
		SetLight(m_iLightChIdx[2], m_img9_ch2);
		SetLight(m_iLightChIdx[3], m_img9_ch3);
	}

	if(m_bLiveOverlayDisp)
	{
		ResultCheck_Display(0, m_iPrcIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//20110509 lee
	}
}

void CFilmInsDlg::OnRectImgMap10()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_iMapIdx[0] = 10;

	DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);

	UpdateMapButton(m_iMapIdx[0]);     
	
	if(!m_bInspectStart[0])
	{
		SetLight(m_iLightChIdx[0], m_img10_ch0);
		SetLight(m_iLightChIdx[1], m_img10_ch1);
		SetLight(m_iLightChIdx[2], m_img10_ch2);
		SetLight(m_iLightChIdx[3], m_img10_ch3);
	}

	if(m_bLiveOverlayDisp)
	{
		ResultCheck_Display(0, m_iPrcIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//20110509 lee
	}
}

void CFilmInsDlg::OnRectImgMap11()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_iMapIdx[0] = 11;

	DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);
	UpdateMapButton(m_iMapIdx[0]);     
	
	if(!m_bInspectStart[0])
	{
		SetLight(m_iLightChIdx[0], m_img11_ch0);
		SetLight(m_iLightChIdx[1], m_img11_ch1);
		SetLight(m_iLightChIdx[2], m_img11_ch2);
		SetLight(m_iLightChIdx[3], m_img11_ch3);
	}

	if(m_bLiveOverlayDisp)
	{
		ResultCheck_Display(0, m_iPrcIdx[0], m_iNGViewIdx[0], FALSE, TRUE, FALSE, TRUE ,FALSE);//20110509 lee
	}

}



BEGIN_EVENTSINK_MAP(CFilmInsDlg, CDialog)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_RUNSTOP, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlRunstop, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_MODEL, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlModel, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_SETUP, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlSetup, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_EXIT, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlExit, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_SEL_PROFILEMODE, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelSelProfilemode, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_SEL_COLORMODE, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelSelColormode, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_RESULT4, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelResult4, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP_TEST, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMapTest, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP_SAVE, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMapSave, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP_LOAD, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMapLoad, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP_LOAD3, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMapLoad3, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP0, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMap0, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP1, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMap1, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP2, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMap2, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP3, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMap3, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP4, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMap4, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP5, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMap5, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP6, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMap6, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP7, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMap7, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP8, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMap8, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP9, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMap9, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP10, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMap10, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAP11, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMap11, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL2, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrl2, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_NGVIEW, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelNgview, VTS_NONE)
	ON_EVENT(CFilmInsDlg, IDC_BTNENHCTRL_LABEL_MAINVIEW, DISPID_CLICK, CFilmInsDlg::ClickBtnenhctrlLabelMainview, VTS_NONE)
END_EVENTSINK_MAP()

void CFilmInsDlg::ClickBtnenhctrlRunstop()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//if(m_btnStartStop.GetBCheck())

	if(m_btnStartStop.GetValue())
	{
		for(int i = 0; i < IMG_RECT_MAX_NUM; i++)
		{
			//Stop live
			KillTimer(i);

			m_bLiveOverlayDisp = TRUE;
		}

		m_btnStartStop.SetCaption("STOP");

		m_label_mode.SetCaption("RUN MODE");
		m_label_mode.SetForeColor(RGB(0, 255, 0));
		m_label_mode.SetOffice2007ColorInnerBorder(RGB(0, 255, 0));
		m_label_mode.SetOffice2007ColorOuterBorder(RGB(0, 255, 0));

		m_VisMod->SetNGViewMode(FALSE, -1);

    	StartInspectThread();

		WriteStatusList(">>>>RUN MODE(Inspection started!)");
	}
	else
	{
		m_btnStartStop.SetCaption("RUN");

		m_label_mode.SetCaption("STOP MODE");
		m_label_mode.SetForeColor(RGB(255, 0, 0));
		m_label_mode.SetOffice2007ColorInnerBorder(RGB(255, 0, 0));
		m_label_mode.SetOffice2007ColorOuterBorder(RGB(255, 0, 0));

		StopInspectThread();

		WriteStatusList(">>>>STOP MODE(Inspection stopped!)");
	}
	
}

void CFilmInsDlg::ClickBtnenhctrlModel()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	int idx = 0;
 	CRect rect;
 	char temp[MAX_MOD_NAME];
 	CString str0, str1;	
 	CString Dir;
 
 	if(m_btnStartStop.GetValue())
 	{
 		MessageBox("Error : 현재 검사 진행 중입니다! 검사 정지 후 재 시도 바랍니다.", NULL, MB_ICONEXCLAMATION);
 		return;
 	}
 
 	//Close setup dialog
 	m_dlgSetup->ShowWindow(SW_HIDE);
 
 	for(int i = 0; i < IMG_RECT_MAX_NUM; i++)
 	{
 		//Stop live
 		KillTimer(i);
 	}
 
 	CDialogModelMenu dlg;
 
 	GetDlgItem(IDC_BTNENHCTRL_MODEL)->GetWindowRect(rect);
 	dlg.m_Rect = rect;
 
 	//20100712 lee
 	idx = (int)dlg.DoModal();
 	if(idx == 2) return; 
 
 	idx = dlg.m_Ret;
 
 	//Model
 	C_CtrlModelManageDlg *moddlg = new C_CtrlModelManageDlg(NULL,m_strDBPath, m_strRegiPath);
	
 	switch(idx)
 	{
 	//New
 	case 0:
 		moddlg->DoModal(idx);
 		WriteStatusList("MODEL CHANGE-NEW");
 		//Make directory
 		Dir.Format("%s", moddlg->GetPath());
 		MakeDirectories(Dir);
 		//MakeDefault Param
 		m_VisMod->MakeDefaultInspPara();
 		break;
 	//Delete
 	case 1:
 		if(moddlg->DoModal(idx))
 		{
 			WriteStatusList("MODEL CHANGE-DELETE");
 		}
 		break;
 	//Select
 	case 2:
 		//Model save
 		moddlg->Open();
 		sprintf(temp, "%s", moddlg->GetName());
 
 		str0 = moddlg->GetName();
 		str1 = moddlg->GetPath();
 		if( (str0.IsEmpty()) || (str1.IsEmpty()) )
 		{
 			WriteStatusList("Error : invalid model!");
 		}
 		else
 		{
 			m_dlgProcessing.ShowWindow(SW_SHOW);
 			HandleAllMessage();
 
 			//no need
 			UpdateLight(TRUE);

 			m_dlgProcessing.ShowWindow(SW_HIDE);
 		}
 
 
 		if(moddlg->DoModal(idx))
 		{
 			WriteStatusList("MODEL CHANGE-SELECT");
 		}
 
 		m_dlgProcessing.ShowWindow(SW_SHOW);
 		HandleAllMessage();
 
 		//Load setup parameter
 		moddlg->Open();
 		sprintf(temp, "%s", moddlg->GetName());
 		m_label_model.SetCaption(temp);
 		
 		str0 = moddlg->GetName();
 		str1 = moddlg->GetPath();
 		if( (str0.IsEmpty()) || (str1.IsEmpty()) )
 		{
 			//MessageBox("Error : invalid model!", NULL, MB_ICONEXCLAMATION);
 			WriteStatusList("Error : invalid model!");
 		
 		}
 		else
 		{
 			//모델 로딩 이므로 기존 등록 모델 삭제 한다.?
 			//clear_all_variation_models(); //20110508 lee
 			clear_all_shape_models(); //20110508 lee
			//m_VisMod->LoadSystemPara(m_strDBPath);
 			//Load setup INI file
			m_VisMod->LoadInspectSelectPara(moddlg->GetPath());		// 110723 ytlee
 			if( m_VisMod->LoadInspectPara(moddlg->GetPath()) )
 			{
 				UpdateLight(FALSE);

				ResetCount();
 			}
 			else
 			{
 				WriteStatusList("Error : can't load setup parameter file!");
 
 				//Make directory
 				Dir.Format("%s", moddlg->GetPath());
 				MakeDirectories(moddlg->GetPath());
 			}
 		}

		m_dlgProcessing.ShowWindow(SW_HIDE);
 
 		break;
 	//Convertion
 	case 3:
 		//moddlg->DoModal(idx);
 	
 		break;
 	default:
 		
 		break;
 	}
 
 	delete moddlg;
}

void CFilmInsDlg::ResetCount()
{
	typeMainSystemRslt sys_rslt;
	sys_rslt = m_VisMod->GetMainSysRslt();

	sys_rslt.m_lCnt[1] = 0;
	sys_rslt.m_lCnt[2] = 0;
	sys_rslt.m_lCnt[3] = 0;

	char temp[10];
	sprintf(temp, "%ld", sys_rslt.m_lCnt[1]);
	m_label_tot.SetCaption(temp);
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[2]);
	m_label_good.SetCaption(temp);
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[3]);
	m_label_ng.SetCaption(temp);

	m_VisMod->SetMainSysRslt(sys_rslt);
}

void CFilmInsDlg::ClickBtnenhctrlSetup()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//Set inspect data
	m_dlgSetup->m_InsPara = m_VisMod->GetInspectPara(0);
	m_dlgSetup->m_SysPara = m_VisMod->GetSystemPara(m_RectIdx);

	//Set inspection result
	for(int i = 0; (i < m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{
		copy_obj(*m_VisMod->GetHalInspectRgn(0, i, 0), &m_dlgSetup->m_HalInsRgn[i][0], 1, -1);//SR region
		copy_obj(*m_VisMod->GetHalInspectRgn(0, i, 1), &m_dlgSetup->m_HalInsRgn[i][1], 1, -1);//PF hall region
		copy_obj(*m_VisMod->GetHalInspectRgn(0, i, 2), &m_dlgSetup->m_HalInsRgn[i][2], 1, -1);//up lead region
		copy_obj(*m_VisMod->GetHalInspectRgn(0, i, 3), &m_dlgSetup->m_HalInsRgn[i][3], 1, -1);//dn lead region
		copy_obj(*m_VisMod->GetHalInspectRgn(0, i, 4), &m_dlgSetup->m_HalInsRgn[i][4], 1, -1);//user area region
		copy_obj(*m_VisMod->GetHalInspectRgn(0, i, 5), &m_dlgSetup->m_HalInsRgn[i][5], 1, -1);//SR out region
		copy_obj(*m_VisMod->GetHalInspectRgn(0, i, 6), &m_dlgSetup->m_HalInsRgn[i][6], 1, -1);//vaccum region
		copy_obj(*m_VisMod->GetHalInspectRgn(0, i, 7), &m_dlgSetup->m_HalInsRgn[i][7], 1, -1);//chip region
		copy_obj(*m_VisMod->GetHalInspectRgn(0, i, 8), &m_dlgSetup->m_HalInsRgn[i][8], 1, -1);//AM0
		copy_obj(*m_VisMod->GetHalInspectRgn(0, i, 9), &m_dlgSetup->m_HalInsRgn[i][9], 1, -1);//AM0
		copy_obj(*m_VisMod->GetHalInspectRgn(0, i, 10), &m_dlgSetup->m_HalInsRgn[i][10], 1, -1);//AM1
		copy_obj(*m_VisMod->GetHalInspectRgn(0, i, 11), &m_dlgSetup->m_HalInsRgn[i][11], 1, -1);//AM1
	}

	m_dlgSetup->ShowWindow(SW_SHOW);

	WriteStatusList("Setup started!");
}

void CFilmInsDlg::ClickBtnenhctrlExit()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_btnStartStop.GetValue())
	{
		AfxMessageBox("Run 모드 입니다. Stop 모드로 변경해 주세요!");
		return;
	}
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	//Model
	C_CtrlModelManageDlg *moddlg = new C_CtrlModelManageDlg(NULL,m_strDBPath, m_strRegiPath);
	moddlg->Open();

	UpdateData(TRUE);

	StopInspectThread();	

	for(int i = 0; i < IMG_RECT_MAX_NUM; i++)
	{
		KillTimer(i);
	}

	KillTimer(1);

#ifdef _CAM
	
	//Light init
	//20150223 by
	COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[0], FALSE);
	COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[1], FALSE);
	COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[2], FALSE);
	if(m_iLightOnOffIdx[3] > VISION_CAM_NONE)
		COMI_DO_PutOne((m_VisMod->GetVisIndex() > VISION_CAM_TOP2) ? m_hComiDevice32 : m_hComiDevice64, m_iLightOnOffIdx[3], FALSE);
	/*
	COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[0], FALSE);
	COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[1], FALSE);
	COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[2], FALSE);
	if(m_iLightOnOffIdx[3] > VISION_CAM_NONE)
		COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[3], FALSE);
	*/
	
	Sleep(50);

	COMI_UnloadDll();

	m_cCam.Close();
#endif	
	//Model save
	char temp[MAX_MOD_NAME];

	sprintf(temp, "%s", moddlg->GetName());
	m_label_model.SetCaption(temp);
	CString str0, str1;
	str0 = moddlg->GetName();
	str1 = moddlg->GetPath();
	if( (str0.IsEmpty()) || (str1.IsEmpty()) )
	{
		MessageBox("Error : invalid model!", NULL, MB_ICONEXCLAMATION);
	}
	else
	{
		//Load system file
		//Load/save system parameter
		m_VisMod->SaveSystemPara(m_strDBPath);
		//Load/save system result
		m_VisMod->SaveSystemRslt(m_strDBPath);

		ModelSave();

		WriteStatusList(">>>>>Application exit!");

		delete moddlg;
	}

	EndDialog(0);
}



void CFilmInsDlg::ClickBtnenhctrlLabelSelProfilemode()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(m_lProfileMode == 0)
	{
		m_lProfileMode = 1;	
		GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_PROFILEMODE)->SetWindowText("X Profile Mode");
	}
	else if(m_lProfileMode == 1)
	{
		m_lProfileMode = 2;	
		GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_PROFILEMODE)->SetWindowText("Y Profile Mode");
	}
	else if(m_lProfileMode == 2)
	{
		m_lProfileMode = 3;	
		GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_PROFILEMODE)->SetWindowText("X/Y Profile Mode");
	}
	else if(m_lProfileMode == 3)
	{
		// 20110524 Dongwhee BEGIN
		m_lProfileMode = 4;
		GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_PROFILEMODE)->SetWindowText("Gray Value View");
	}
	else if(m_lProfileMode == 4)
	{
		// 20110603 Dongwhee BEGIN : Add Contrast View
		m_lProfileMode = 5;	
		GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_PROFILEMODE)->SetWindowText("Contrast View");
	}
	else if(m_lProfileMode == 5)
	{
		m_lProfileMode = 0;	
		GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_PROFILEMODE)->SetWindowText("Only Gray Mode");
	}
}

void CFilmInsDlg::ClickBtnenhctrlLabelSelColormode()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(m_bIsColor)
	{
		Hobject MultiChannelImage, GrayImage;
		//20110510 lee
		if(m_lColorMode == 0)
		{
			m_lColorMode = 1;
			GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_COLORMODE)->SetWindowText("COLOR MODE");
			if(m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]) != NULL)
			{
				DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);
			}
		}
		else if(m_lColorMode == 1)
		{
			m_lColorMode = 2;	
			GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_COLORMODE)->SetWindowText("RED MODE");
			if(m_VisMod->GetHalRedImage(0, m_iBufIdx[0], m_iMapIdx[0]) != NULL)
			{
				DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);
			}
		}
		else if(m_lColorMode == 2)
		{
			m_lColorMode = 3;	
			GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_COLORMODE)->SetWindowText("GREEN MODE");
			if(m_VisMod->GetHalGreenImage(0, m_iBufIdx[0], m_iMapIdx[0]) != NULL)
			{
				DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);
			}
		}
		else if(m_lColorMode == 3)
		{
			m_lColorMode = 0;
			GetDlgItem(IDC_BTNENHCTRL_LABEL_SEL_COLORMODE)->SetWindowText("BLUE MODE");
			if(m_VisMod->GetHalBlueImage(0, m_iBufIdx[0], m_iMapIdx[0]) != NULL)
			{
				DisplayView(0, m_iBufIdx[0], m_iMapIdx[0]);
			}
		}
	}
}

void CFilmInsDlg::ClickBtnenhctrlLabelMapTest()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_bCheckLoadMaster = FALSE;
	
	if(!m_bInspectStart[0])
	{
		CString tmpFull_Name, tmpPath_Name, tmpFile_Name;
		BOOL bWorking;
		CFileFind finder;
		CString str;
		int leng;
		int i;
		Hobject image;
		Herror Herr;

		CString add_str;
		long pos;
		char temp[MAX_PATH];

		int chk_end = 0;

		//Read
		CFileDialog dlgFile(TRUE,NULL,NULL,
								OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
								"Image(*.*)|*.*|");

		if(dlgFile.DoModal() == IDOK)
		{
			tmpFull_Name = dlgFile.GetPathName(); 
			tmpPath_Name = dlgFile.GetPathName();
			tmpFile_Name = dlgFile.GetFileName();

			leng = tmpPath_Name.GetLength();
			for(i = (leng - 1); i > 0; i--)
			{
				if('\\' == tmpPath_Name[i])
				{
					tmpPath_Name.Delete(i, 1);
					break;
				}
				else
				{
					tmpPath_Name.Delete(i, 1);
				}
			}
			
			// build a string with wildcards
			CString strWildcard(tmpPath_Name), OldFName, path;
			strWildcard += _T("\\*]_0.*");
			
			// start working for files
			bWorking = finder.FindFile(strWildcard);
			
			while (bWorking)
			{
				bWorking = finder.FindNextFile();
				
				// skip . and .. files; otherwise, we'd
				// recur infinitely!
				if (finder.IsDots())
					continue;
				
				// if it's a directory, recursively search it
				if (finder.IsDirectory() != 0)
				{
					continue;
				}
				else
				{
					for(i = 0; i < IMAGE_NG_NUM; i++)
					{
						clear_window(m_lNGWindowID[i]);
					}
					str = finder.GetFilePath();

					pos = str.Find("]_", 0);
					memset(temp, NULL, sizeof(temp));
					for(i = 0; i < pos + 2; i++)
					{
						temp[i] = str.GetAt(i);
					}

					for(i = 0; (i < m_VisMod->GetMapNum(0)) && (i < IMAGE_MAP_MAX_NUM); i++)
					{	
						add_str.Format("%s%d.bmp", temp, i);
											
						set_check("~give_error");
						Herr = read_image(m_VisMod->GetHalImage(0, m_iBufIdx[0], i), HTuple((LPCTSTR)add_str));
						
						
						Herror herr;
						CString status;
						
						set_check("~give_error");
						herr = copy_image(*m_VisMod->GetHalImage(0, m_iBufIdx[0], i), m_VisMod->GetHalNGImage(0, m_iBufIdx[0], i));
						set_check("give_error");
						if(H_MSG_TRUE == herr)
						{
						}
						else
						{
							status.Format("Halcon Error Occured at rgb1_to_gray_03!");
							m_VisMod->WriteErrorList((LPCTSTR)status);
							return;
						}
						
						set_check("give_error");
						if(H_MSG_TRUE == Herr)
						{
							disp_obj(*m_VisMod->GetHalImage(0, m_iBufIdx[0], i), m_lWindowID[i]);
							
						}
						else
						{
							gen_image_const(m_VisMod->GetHalImage(0, m_iBufIdx[0], i),HTuple("byte"),m_lWidth[0],m_lHeight[0]);	
						}
					}

		
					if(Test())
					{

					}
					else
					{
						HandleAllMessage();
						add_str.Format("불량 발생-테스트를 계속 진행하시겠습니까?(%s%d.bmp)", temp, 0);
						if(IDYES != MessageBox(add_str, "확인", MB_YESNO) )
						{
							break;
						}					
					}

					chk_end++;

					if(10 < chk_end)
					{
						HandleAllMessage();
						if(IDYES != MessageBox("테스트를 계속 진행하시겠습니까?", "확인", MB_YESNO) )
						{
							break;
						}
						else
						{
							chk_end = 0;
						}
					}

					continue;
				}

			}
		}
		
		delete dlgFile;	
	}
	else
	{
		WriteStatusList("WARRNING : 검사 진행 중 입니다!");
	}
}

void CFilmInsDlg::ClickBtnenhctrlLabelMapSave()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	int i;
	Herror Herr;

	typeInspectPara ins_para;
	ins_para = m_VisMod->GetInspectPara(0);	

	//image load
	CString tmpPath_Name, add_str;
	long pos;
	char temp[MAX_PATH];
				
	CFileDialog dlgFile(FALSE,"Image","*.*",
							OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
							"All Image(*.*)|*.*|");
	if(dlgFile.DoModal() == IDOK)
	{
		tmpPath_Name = dlgFile.GetPathName();
		
		//read_image(m_VisMod->GetHalImage(0, m_iBufIdx[0], 0), HTuple((LPCTSTR)tmpPath_Name));
		
		pos = tmpPath_Name.Find(".", 0);
		
		memset(temp, NULL, sizeof(temp));
		for(i = 0; i < pos; i++)
		{
			temp[i] = tmpPath_Name.GetAt(i);
		}

		for(i = 0; /*(i < ins_para.m_iParaBase[1]) &&*/ (i < IMAGE_MAP_MAX_NUM); i++)
		{
			add_str.Format("%s[Gray]_%d.bmp", temp, i);
			
			set_check("~give_error");
			Herr = write_image(*m_VisMod->GetHalImage(0, m_iBufIdx[0], i), "bmp", 0, HTuple((LPCTSTR)add_str));
			set_check("give_error");
			if(H_MSG_TRUE == Herr)
			{
				
			}
			else
			{
				gen_image_const(m_VisMod->GetHalNGImage(0, m_iNGViewIdx[0], i),HTuple("byte"),m_lWidth[0],m_lHeight[0]);
			}
		}	
	}
	else
	{
		//gen_image_const(m_VisMod->GetHalImage(m_iBufIdx[0], i, m_iMapIdx[0]),HTuple("byte"),2048,2048);
	}
	delete dlgFile;		
}

void CFilmInsDlg::ClickBtnenhctrlLabelMapLoad()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	int i;
	Herror Herr;
	HTuple wd, ht;

	typeInspectPara ins_para;
	ins_para = m_VisMod->GetInspectPara(0);	

	//image load
	CString tmpPath_Name, add_str;
	long pos;
	char temp[MAX_PATH];

	m_bCheckLoadMaster = FALSE;

	CFileDialog dlgFile(TRUE,"Image","*.*",
							OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
							"All Image(*.*)|*.*|");
	if(dlgFile.DoModal() == IDOK)
	{
		tmpPath_Name = dlgFile.GetPathName();
		
		pos = tmpPath_Name.Find("]_", 0);
		
		memset(temp, NULL, sizeof(temp));
		for(i = 0; i < pos + 2; i++)
		{
			temp[i] = tmpPath_Name.GetAt(i);
		}
	
		get_image_pointer1(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]), NULL, NULL, &wd, &ht);
		m_dlgImgRect[0]->SetZoom(0, (Hlong)wd[0], (Hlong)ht[0]);
		
		for(i = 0; i < IMAGE_MAP_MAX_NUM; i++)
		{
			add_str.Format("%s%d.bmp", temp, i);

			set_check("~give_error");
			//Herr = read_image(m_VisMod->GetHalImage(0, m_iBufIdx[0], i), HTuple((LPCTSTR)add_str));

			Herr = read_image(m_VisMod->GetHalImage(0, m_iBufIdx[0], i), (LPCTSTR)add_str);
			if(m_bIsColor)
				decompose3(*m_VisMod->GetHalImage(0, m_iBufIdx[0], i), 
							m_VisMod->GetHalRedImage(0, m_iBufIdx[0], i), 
							m_VisMod->GetHalGreenImage(0, m_iBufIdx[0], i), 
							m_VisMod->GetHalBlueImage(0, m_iBufIdx[0], i));

			set_check("give_error");
			if(H_MSG_TRUE == Herr)
			{
				
			}
			else
			{
				gen_image_const(m_VisMod->GetHalImage(0, m_iBufIdx[0], i),HTuple("byte"),m_lWidth[0],m_lHeight[0]);
				CString status;
				status.Format("Halcon Error Occured at rgb1_to_gray_02!");
				m_VisMod->WriteErrorList((LPCTSTR)status);

								
			}

			disp_obj(*m_VisMod->GetHalImage(0, m_iBufIdx[0], i), m_lWindowID[i]);	
		}		

		disp_obj(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[0]->GetView());
	}
	else
	{
		
	}
	delete dlgFile;	
}

void CFilmInsDlg::ClickBtnenhctrlLabelMapLoad3()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(m_btnStartStop.GetValue())
	{
		AfxMessageBox("검사 중에는 사용할 수 없습니다.");
		return;
	}

	m_bCheckLoadMaster = FALSE;
	
	ULONG StartTime, Endtime;
	CString strTime, status, strPacket;

	strPacket.Format("%cVIS%d,0030%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), PACKET_CHAR_ETX);
	OnSend(strPacket);

	status.Format("Align Rail Positive Move");
	WriteStatusList((LPCTSTR)status);

	Sleep(1000);

	m_VisMod->SetNGViewMode(FALSE, -1);

	for(int k=0;k<1;k++)
	{
		StartTime = GetTickCount(); 
		GrabProcess(0, m_iBufIdx[0], IMAGE_MAP_MAX_NUM);
		Endtime = GetTickCount();

		strTime.Format("%d",Endtime - StartTime);
		m_label_gtime.SetWindowText(strTime);
		for(int i = 0; (i < IMAGE_MAP_MAX_NUM); i++)
		{
			disp_obj(*m_VisMod->GetHalImage(0, m_iBufIdx[0], i), m_lWindowID[i]);
		}

		disp_obj(*m_VisMod->GetHalImage(0, m_iBufIdx[0], m_iMapIdx[0]), m_dlgImgRect[0]->GetView());
	}

	m_iPrcIdx[0]= m_iGrbIdx[0];
	m_iBufIdx[0] = m_iPrcIdx[0];

	m_bCheckGrab = TRUE; 

	status.Format("Grab complete");
	WriteStatusList((LPCTSTR)status);

	strPacket.Format("%cVIS%d,0031%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), PACKET_CHAR_ETX);
	OnSend(strPacket);
}

void CFilmInsDlg::ClickBtnenhctrlLabelResult4()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(GetDlgItem(IDC_RECT_IMG_MAP0)->IsWindowVisible())
	{
		GetDlgItem(IDC_BTNENHCTRL_LABEL_RESULT4)->SetWindowText("INS.ITEM");
		GetDlgItem(IDC_RECT_IMG_MAP0)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RECT_IMG_MAP1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RECT_IMG_MAP2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RECT_IMG_MAP3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RECT_IMG_MAP4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RECT_IMG_MAP5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RECT_IMG_MAP6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RECT_IMG_MAP7)->ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_BTNENHCTRL_LABEL_RESULT4)->SetWindowText("MAP DATA");
		GetDlgItem(IDC_RECT_IMG_MAP0)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RECT_IMG_MAP1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RECT_IMG_MAP2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RECT_IMG_MAP3)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RECT_IMG_MAP4)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RECT_IMG_MAP5)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RECT_IMG_MAP6)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RECT_IMG_MAP7)->ShowWindow(SW_SHOW);
	}
}




void CFilmInsDlg::ClickBtnenhctrlLabelMap0()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	
	CLightCtrlDlg dlg;

	OnRectImgMap0();

	dlg.m_img0_ch0 = m_img0_ch0;
	dlg.m_img0_ch1 = m_img0_ch1;
	dlg.m_img0_ch2 = m_img0_ch2;
	dlg.m_img0_ch3 = m_img0_ch3;
	if(dlg.DoModal()==IDOK)
	{
		m_img0_ch0 = dlg.m_img0_ch0 ;
		m_img0_ch1 = dlg.m_img0_ch1 ;
		m_img0_ch2 = dlg.m_img0_ch2 ;
		m_img0_ch3 = dlg.m_img0_ch3 ;
	}
	UpdateData(false);
}

void CFilmInsDlg::ClickBtnenhctrlLabelMap1()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	
	CLightCtrlDlg dlg;

	OnRectImgMap1();

	dlg.m_img0_ch0 = m_img1_ch0;
	dlg.m_img0_ch1 = m_img1_ch1;
	dlg.m_img0_ch2 = m_img1_ch2;
	dlg.m_img0_ch3 = m_img1_ch3;
	if(dlg.DoModal()==IDOK)
	{
		m_img1_ch0 = dlg.m_img0_ch0 ;
		m_img1_ch1 = dlg.m_img0_ch1 ;
		m_img1_ch2 = dlg.m_img0_ch2 ;
		m_img1_ch3 = dlg.m_img0_ch3 ;
	}
	UpdateData(false);
}

void CFilmInsDlg::ClickBtnenhctrlLabelMap2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	
	CLightCtrlDlg dlg;

	OnRectImgMap2();

	dlg.m_img0_ch0 = m_img2_ch0;
	dlg.m_img0_ch1 = m_img2_ch1;
	dlg.m_img0_ch2 = m_img2_ch2;
	dlg.m_img0_ch3 = m_img2_ch3;
	if(dlg.DoModal()==IDOK)
	{
		m_img2_ch0 = dlg.m_img0_ch0 ;
		m_img2_ch1 = dlg.m_img0_ch1 ;
		m_img2_ch2 = dlg.m_img0_ch2 ;
		m_img2_ch3 = dlg.m_img0_ch3 ;
	}
	UpdateData(false);
}

void CFilmInsDlg::ClickBtnenhctrlLabelMap3()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	
	CLightCtrlDlg dlg;

	OnRectImgMap3();

	dlg.m_img0_ch0 = m_img3_ch0;
	dlg.m_img0_ch1 = m_img3_ch1;
	dlg.m_img0_ch2 = m_img3_ch2;
	dlg.m_img0_ch3 = m_img3_ch3;
	if(dlg.DoModal()==IDOK)
	{
		m_img3_ch0 = dlg.m_img0_ch0 ;
		m_img3_ch1 = dlg.m_img0_ch1 ;
		m_img3_ch2 = dlg.m_img0_ch2 ;
		m_img3_ch3 = dlg.m_img0_ch3 ;
	}
	UpdateData(false);
}

void CFilmInsDlg::ClickBtnenhctrlLabelMap4()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	
	CLightCtrlDlg dlg;

	OnRectImgMap4();

	dlg.m_img0_ch0 = m_img4_ch0;
	dlg.m_img0_ch1 = m_img4_ch1;
	dlg.m_img0_ch2 = m_img4_ch2;
	dlg.m_img0_ch3 = m_img4_ch3;
	if(dlg.DoModal()==IDOK)
	{
		m_img4_ch0 = dlg.m_img0_ch0 ;
		m_img4_ch1 = dlg.m_img0_ch1 ;
		m_img4_ch2 = dlg.m_img0_ch2 ;
		m_img4_ch3 = dlg.m_img0_ch3 ;
	}
	UpdateData(false);
}

void CFilmInsDlg::ClickBtnenhctrlLabelMap5()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	
	CLightCtrlDlg dlg;

	OnRectImgMap5();

	dlg.m_img0_ch0 = m_img5_ch0;
	dlg.m_img0_ch1 = m_img5_ch1;
	dlg.m_img0_ch2 = m_img5_ch2;
	dlg.m_img0_ch3 = m_img5_ch3;
	if(dlg.DoModal()==IDOK)
	{
		m_img5_ch0 = dlg.m_img0_ch0 ;
		m_img5_ch1 = dlg.m_img0_ch1 ;
		m_img5_ch2 = dlg.m_img0_ch2 ;
		m_img5_ch3 = dlg.m_img0_ch3 ;
	}
	UpdateData(false);
}

void CFilmInsDlg::ClickBtnenhctrlLabelMap6()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	
	CLightCtrlDlg dlg;

	OnRectImgMap6();

	dlg.m_img0_ch0 = m_img6_ch0;
	dlg.m_img0_ch1 = m_img6_ch1;
	dlg.m_img0_ch2 = m_img6_ch2;
	dlg.m_img0_ch3 = m_img6_ch3;
	if(dlg.DoModal()==IDOK)
	{
		m_img6_ch0 = dlg.m_img0_ch0 ;
		m_img6_ch1 = dlg.m_img0_ch1 ;
		m_img6_ch2 = dlg.m_img0_ch2 ;
		m_img6_ch3 = dlg.m_img0_ch3 ;
	}
	UpdateData(false);
}

void CFilmInsDlg::ClickBtnenhctrlLabelMap7()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다..
	UpdateData(TRUE);
	
	CLightCtrlDlg dlg;

	OnRectImgMap7();

	dlg.m_img0_ch0 = m_img7_ch0;
	dlg.m_img0_ch1 = m_img7_ch1;
	dlg.m_img0_ch2 = m_img7_ch2;
	dlg.m_img0_ch3 = m_img7_ch3;
	if(dlg.DoModal()==IDOK)
	{
		m_img7_ch0 = dlg.m_img0_ch0 ;
		m_img7_ch1 = dlg.m_img0_ch1 ;
		m_img7_ch2 = dlg.m_img0_ch2 ;
		m_img7_ch3 = dlg.m_img0_ch3 ;
	}
	UpdateData(false);
}

void CFilmInsDlg::ClickBtnenhctrlLabelMap8()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	
	CLightCtrlDlg dlg;
	
	OnRectImgMap8();
	
	dlg.m_img0_ch0 = m_img8_ch0;
	dlg.m_img0_ch1 = m_img8_ch1;
	dlg.m_img0_ch2 = m_img8_ch2;
	dlg.m_img0_ch3 = m_img8_ch3;
	if(dlg.DoModal()==IDOK)
	{
		m_img8_ch0 = dlg.m_img0_ch0 ;
		m_img8_ch1 = dlg.m_img0_ch1 ;
		m_img8_ch2 = dlg.m_img0_ch2 ;
		m_img8_ch3 = dlg.m_img0_ch3 ;
	}
	UpdateData(false);
}

void CFilmInsDlg::ClickBtnenhctrlLabelMap9()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	
	CLightCtrlDlg dlg;
	
	OnRectImgMap9();
	
	dlg.m_img0_ch0 = m_img9_ch0;
	dlg.m_img0_ch1 = m_img9_ch1;
	dlg.m_img0_ch2 = m_img9_ch2;
	dlg.m_img0_ch3 = m_img9_ch3;
	if(dlg.DoModal()==IDOK)
	{
		m_img9_ch0 = dlg.m_img0_ch0 ;
		m_img9_ch1 = dlg.m_img0_ch1 ;
		m_img9_ch2 = dlg.m_img0_ch2 ;
		m_img9_ch3 = dlg.m_img0_ch3 ;
	}
	UpdateData(false);
}

void CFilmInsDlg::ClickBtnenhctrlLabelMap10()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	
	CLightCtrlDlg dlg;
	
	OnRectImgMap10();
	
	dlg.m_img0_ch0 = m_img10_ch0;
	dlg.m_img0_ch1 = m_img10_ch1;
	dlg.m_img0_ch2 = m_img10_ch2;
	dlg.m_img0_ch3 = m_img10_ch3;
	if(dlg.DoModal()==IDOK)
	{
		m_img10_ch0 = dlg.m_img0_ch0 ;
		m_img10_ch1 = dlg.m_img0_ch1 ;
		m_img10_ch2 = dlg.m_img0_ch2 ;
		m_img10_ch3 = dlg.m_img0_ch3 ;
	}
	UpdateData(false);
	
}

void CFilmInsDlg::ClickBtnenhctrlLabelMap11()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	
	CLightCtrlDlg dlg;
	
	OnRectImgMap11();
	
	dlg.m_img0_ch0 = m_img11_ch0;
	dlg.m_img0_ch1 = m_img11_ch1;
	dlg.m_img0_ch2 = m_img11_ch2;
	dlg.m_img0_ch3 = m_img11_ch3;
	if(dlg.DoModal()==IDOK)
	{
		m_img11_ch0 = dlg.m_img0_ch0 ;
		m_img11_ch1 = dlg.m_img0_ch1 ;
		m_img11_ch2 = dlg.m_img0_ch2 ;
		m_img11_ch3 = dlg.m_img0_ch3 ;
	}
	UpdateData(false);
}

void CFilmInsDlg::SetStaticMapType(int vis_idx)
{
	switch(vis_idx)
	{
	case VISION_CAM_TOP1:
		GetDlgItem(IDC_STATIC_MAP0_TYPE0)->SetWindowText("Align");
		GetDlgItem(IDC_STATIC_MAP0_TYPE1)->SetWindowText("PF/Punch불량");
		GetDlgItem(IDC_STATIC_MAP0_TYPE2)->SetWindowText("찢김/편집불량");

		GetDlgItem(IDC_STATIC_MAP1_TYPE0)->SetWindowText("Chip Search");
		GetDlgItem(IDC_STATIC_MAP1_TYPE1)->SetWindowText("Potting");
		GetDlgItem(IDC_STATIC_MAP1_TYPE2)->SetWindowText("No Bond");
		GetDlgItem(IDC_STATIC_MAP1_TYPE3)->SetWindowText("Pot part(흑)");

		GetDlgItem(IDC_STATIC_MAP2_TYPE0)->SetWindowText("SR 수지(흑/백)");
		GetDlgItem(IDC_STATIC_MAP2_TYPE1)->SetWindowText("SR 긁힘");
		GetDlgItem(IDC_STATIC_MAP2_TYPE2)->SetWindowText("SR 금속성");
		GetDlgItem(IDC_STATIC_MAP2_TYPE3)->SetWindowText("M/K불량");

		GetDlgItem(IDC_STATIC_MAP3_TYPE0)->SetWindowText("Lead 변색(우선)");
		GetDlgItem(IDC_STATIC_MAP3_TYPE1)->SetWindowText("Lead 긁힘/수지");
		GetDlgItem(IDC_STATIC_MAP3_TYPE2)->SetWindowText("Pot part(백)");

		GetDlgItem(IDC_STATIC_MAP7_TYPE0)->SetWindowText("제품감지");

		GetDlgItem(IDC_STATIC_M8)->SetWindowText("CHIP");
		GetDlgItem(IDC_STATIC_M9)->SetWindowText("SR");
		GetDlgItem(IDC_STATIC_M10)->SetWindowText("PF/USER");
		GetDlgItem(IDC_STATIC_M11)->SetWindowText("SR-OUT");
		break;
	case VISION_CAM_TOP2:
		GetDlgItem(IDC_STATIC_MAP0_TYPE0)->SetWindowText("Align");
		GetDlgItem(IDC_STATIC_MAP0_TYPE1)->SetWindowText("Lead 긁힘/수지");
		GetDlgItem(IDC_STATIC_MAP0_TYPE2)->SetWindowText("ChipOut");

		GetDlgItem(IDC_STATIC_MAP1_TYPE0)->SetWindowText("Chip Broken");
		GetDlgItem(IDC_STATIC_MAP1_TYPE1)->SetWindowText("Chip이면수지묻음");

		GetDlgItem(IDC_STATIC_M8)->SetWindowText("CHIP");
		break;
	case VISION_CAM_BOTTOM:
		GetDlgItem(IDC_STATIC_MAP0_TYPE0)->SetWindowText("Align");
		GetDlgItem(IDC_STATIC_MAP1_TYPE0)->SetWindowText("구겨짐");
		GetDlgItem(IDC_STATIC_MAP2_TYPE0)->SetWindowText("찍힘");
		

		GetDlgItem(IDC_STATIC_M9)->SetWindowText("SR");
		

		break;
	/*
	case VISION_CAM_BOTTOM2:
		GetDlgItem(IDC_STATIC_MAP0_TYPE0)->SetWindowText("Align");
		GetDlgItem(IDC_STATIC_MAP0_TYPE1)->SetWindowText("A/M Scratch");

		GetDlgItem(IDC_STATIC_MAP1_TYPE0)->SetWindowText("Ink Chip");
		GetDlgItem(IDC_STATIC_MAP1_TYPE1)->SetWindowText("기포");

		GetDlgItem(IDC_STATIC_MAP2_TYPE0)->SetWindowText("찍힘/구겨짐(흑)");
		GetDlgItem(IDC_STATIC_MAP2_TYPE1)->SetWindowText("PI Scratch");

		GetDlgItem(IDC_STATIC_M9)->SetWindowText("SR,A/M");
		break;
	*/
	}	
}

void CFilmInsDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//for(int i = 0; i < 10; i++)
	//SendMessage(USER_MSG_IMG_RECT, 0, 7);
	//OnReceive(0, 1);

	CString strTemp1;
	//strTemp1.Format("%cVIS1,0004,0,0000,0000,2,000,042%c", PACKET_CHAR_STX, PACKET_CHAR_ETX);
	//OnSend(strTemp1);
	//TRACE("test");
/*
	Hobject aaa;
	gen_rectangle1(&aaa, 100, 100, 200, 200);
	write_region(aaa, "D:\\TEST");
	*/
	//strTemp1.Format("%cVIS%d,0004,%d,%04d,%04d,%d,%03d,%03d,%03d,%03d,%03d%c", PACKET_CHAR_STX, m_VisMod->GetVisIndex(), 0, 2000, 4999, 5, 0, 0, 0, 0, 0, PACKET_CHAR_ETX);

	//AutoTeachOCV();
/*
[19:30:23 VIS->CTRL]VIS0,0004,1,1995,4998,5,000,000,000,000,000
[19:30:24 CTRL->VIS]VIS0,0001,1,2005,4273280E06,20141215
[19:30:24 CTRL->VIS]VIS0,0003,0,2000,4273280E06,20141215,5,0,0,0,0,0,


	typeMainSystemRslt sys_rslt;
	sys_rslt = m_VisMod->GetMainSysRslt();

	sys_rslt.m_lCnt[1] = 0;
	sys_rslt.m_lCnt[2] = 0;
	sys_rslt.m_lCnt[3] = 0;

	char temp[10];
	sprintf(temp, "%ld", sys_rslt.m_lCnt[1]);
	m_label_tot.SetCaption(temp);
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[2]);
	m_label_good.SetCaption(temp);
	sprintf(temp, "%ld ", sys_rslt.m_lCnt[3]);
	m_label_ng.SetCaption(temp);


	m_VisMod->SetMainSysRslt(sys_rslt);
	*/
}


void CFilmInsDlg::OnBnClickedButton2()
{
	typeInspectPara para;
	para = m_VisMod->GetInspectPara(0);

	int a = para.m_iParaBase[1];
		//원형 Punch가 있
	//erosion_rectangle1 (Region, RegionErosion, 100, 3)

	/*
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	typeInspectPara ins_para;	
	ins_para = m_VisMod->GetInspectPara(0);	

	//Grab
	COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[0], TRUE);
	COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[1], TRUE);
	COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[2], TRUE);
	COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[3], TRUE);

	SetLight(m_iLightChIdx[0], m_img2_ch0);
	SetLight(m_iLightChIdx[1], m_img2_ch1);
	SetLight(m_iLightChIdx[2], m_img2_ch2);
	SetLight(m_iLightChIdx[3], m_img2_ch3);
	
	Sleep(100);

	m_cCam.Grab(0, 0, 0, m_VisMod->GetHalImage(0, m_iBufIdx[0], ins_para.m_iParaMapOCV[0]));

	Sleep(100);

	COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[0], FALSE);
	COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[1], FALSE);
	COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[2], FALSE);
	COMI_DO_PutOne(m_hComiDevice64, m_iLightOnOffIdx[3], FALSE);

	if(AutoTeachOCV())
	{
		::SendMessage(m_hWnd, USER_MSG_SOCKET, 1, 4);	// Success
	}
	else
	{
		::SendMessage(m_hWnd, USER_MSG_SOCKET, 0, 4);	// Fail
	}
*/
	//AutoTeachOCV();
	/*
	m_strLotNo = "WWWWWWWB";
	m_strStartTime = "20141121";

	m_iPrcIdx[0] = 0;
	m_iBufIdx[0] = 0;
	m_iPrcDevIdx[0] = 1;


	m_VisFlagData[0].m_bFlag[10] = TRUE;
	*/
}
//
//Message Map Fucntions END
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//미 사용 코드
//
/*

void CFilmInsDlg::ScreenCapture(CString strPath)
{
	CRect rect;
	HDC h_screen_dc;
	m_hWnd = GetSafeHwnd();
	h_screen_dc = ::GetDC(m_hWnd);

	GetWindowRect(&rect);

	int width = 1280;
	int height = 1024;
	
	// DIB의 형식을 정의한다. 
	BITMAPINFO dib_define;
	dib_define.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	dib_define.bmiHeader.biWidth = width;
	dib_define.bmiHeader.biHeight = height;
	dib_define.bmiHeader.biPlanes = 1;
	dib_define.bmiHeader.biBitCount = 24;
	dib_define.bmiHeader.biCompression = BI_RGB;
	dib_define.bmiHeader.biSizeImage = (((width * 24 + 31) & ~31) >> 3) * height;
	dib_define.bmiHeader.biXPelsPerMeter = 0;
	dib_define.bmiHeader.biYPelsPerMeter = 0;
	dib_define.bmiHeader.biClrImportant = 0;
	dib_define.bmiHeader.biClrUsed = 0;
	
	// DIB의 내부 이미지 비트 패턴을 참조할 포인터 변수
	BYTE *p_image_data = NULL;
	// dib_define에 정의된 내용으로 DIB를 생성한다.
	HBITMAP h_bitmap = ::CreateDIBSection(h_screen_dc, &dib_define, DIB_RGB_COLORS, (void **)&p_image_data, 0, 0);
	// 이미지를 추출하기 위해서 가상 DC를 생성한다. 메인 DC에서는 직접적으로 비트맵에 접근하여
	// 이미지 패턴을 얻을 수 없기 때문이다.
	HDC h_memory_dc = ::CreateCompatibleDC(h_screen_dc);
	// 가상 DC에 이미지를 추출할 비트맵을 연결한다.
	HBITMAP h_old_bitmap = (HBITMAP)::SelectObject(h_memory_dc, h_bitmap);
	// 현재 스크린 화면을 캡쳐한다.
	::BitBlt(h_memory_dc, 0, 0, width, height, h_screen_dc, 0, 0, SRCCOPY);
	// 본래의 비트맵으로 복구한다.
	::SelectObject(h_memory_dc, h_old_bitmap); 
	// 가상 DC를 제거한다.
	DeleteDC(h_memory_dc);
	
	// DIB 파일의 내용을 구성한다.
	BITMAPFILEHEADER dib_format_layout;
	ZeroMemory(&dib_format_layout, sizeof(BITMAPFILEHEADER));
	dib_format_layout.bfType = *(WORD*)"BM";
	dib_format_layout.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dib_define.bmiHeader.biSizeImage;
	dib_format_layout.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	
	// DIB 파일을 생성한다.
	FILE *p_file = fopen("E:\\ScreenCapture.bmp", "wb");
	AfxMessageBox(strPath);
	if(p_file != NULL){
		fwrite(&dib_format_layout, 1, sizeof(BITMAPFILEHEADER), p_file);
		fwrite(&dib_define, 1, sizeof(BITMAPINFOHEADER), p_file);
		fwrite(p_image_data, 1, dib_define.bmiHeader.biSizeImage, p_file);
		fclose(p_file);
	}
	
	if(NULL != h_bitmap) DeleteObject(h_bitmap);
	// if(NULL != h_screen_dc) ::ReleaseDC(NULL, h_screen_dc); 
	if(NULL != h_screen_dc) ::ReleaseDC(m_hWnd, h_screen_dc); 
	// 캡쳐를 하기 위해서 감춘 화면을 다시 보여준다.
	fclose(p_file);
	//ShowWindow(SW_SHOW);
}

BOOL CFilmInsDlg::StartAutoDelete_Img()
{
	CString Delete_Img_dir,Delete_Data_dir,Delete_Ng_Data_dir,Delete_Tab_Data_dir;
	CString Delete_Product_dir,Delete_Spc_dir;
	CTime now_time = CTime::GetCurrentTime();

	m_strImgPath = m_strLogPath;
	for(int nYear = m_iDel_Year; nYear > 0; nYear--)		// 110930 ytlee
	{
		if(nYear != now_time.GetYear())
		{
			m_iDel_Month = 12;
			m_iDel_Day = m_iDel_RecentDay;		// 120104 ytlee
		}

		for(int nMonth = m_iDel_Month; nMonth > 0; nMonth--)
		{
			if(nMonth == 1 || nMonth == 3 || nMonth == 5 ||
				nMonth == 7 || nMonth == 8 || nMonth == 10)		// 120104 ytlee
			{
				m_iDel_Day = 31;
			}
			else if(nMonth == 4 || nMonth == 6 || nMonth == 9 || nMonth == 11)
			{
				m_iDel_Day = 30;
			}
			else if(nMonth == 2)
			{
				m_iDel_Day = 28;
			}
			if(nYear == now_time.GetYear() && (nMonth == now_time.GetMonth() || m_dlgSetup->m_SysPara.m_iPara[0] > now_time.GetDay()))		// 110901 ytlee
			{
				m_iDel_Day = m_iDel_RecentDay;
			}

			for(int nDay = 1; nDay <= m_iDel_Day; nDay++)
			{
				if(nMonth>9 && nDay>9)
					Delete_Img_dir.Format("%s\\%d%d%d",m_strImgPath, nYear, nMonth, nDay);
				else if(nMonth<10 && nDay>9)
					Delete_Img_dir.Format("%s\\%d0%d%d",m_strImgPath, nYear, nMonth, nDay);
				else if(nMonth>9 && nDay<10)
					Delete_Img_dir.Format("%s\\%d%d0%d",m_strImgPath, nYear, nMonth, nDay);
				else if(nMonth<10 && nDay<10)
					Delete_Img_dir.Format("%s\\%d0%d0%d",m_strImgPath, nYear, nMonth, nDay);

				CFileFind dbfile;

				if(dbfile.FindFile(Delete_Img_dir,0))
				{
					if(DeleteFolder(Delete_Img_dir))
						RemoveDirectory(Delete_Img_dir);
				}
			}
		}
	}

	return TRUE;
}


void CFilmInsDlg::ChangeStateLightValueEditBox()
{
	BOOL state;
	state = m_dlgSetup->m_SysPara.m_bPara[34];

	GetDlgItem(IDC_EDIT_IMG0_CH0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG0_CH1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG0_CH2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG0_CH3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG1_CH0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG1_CH1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG1_CH2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG1_CH3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG2_CH0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG2_CH1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG2_CH2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG2_CH3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG3_CH0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG3_CH1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG3_CH2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG3_CH3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG4_CH0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG4_CH1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG4_CH2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG4_CH3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG5_CH0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG5_CH1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG5_CH2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG5_CH3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG6_CH0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG6_CH1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG6_CH2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG6_CH3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG7_CH0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG7_CH1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG7_CH2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG7_CH3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG8_CH0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG8_CH1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG8_CH2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG8_CH3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG9_CH0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG9_CH1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG9_CH2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG9_CH3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG10_CH0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG10_CH1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG10_CH2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG10_CH3)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG11_CH0)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG11_CH1)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG11_CH2)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_IMG11_CH3)->EnableWindow(state);
}

*/
void CFilmInsDlg::OnNMRClickListTcp(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	
	m_list_tcp.DeleteAllItems();
	
}


void CFilmInsDlg::ClickBtnenhctrl2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다

	m_label_tot.SetCaption("0");
	m_label_good.SetCaption("0");
	m_label_ng.SetCaption("0");

}

void CFilmInsDlg::ClickBtnenhctrlLabelNgview()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CRect sys_rect;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &sys_rect, NULL);

	MoveWindow(sys_rect.Width() - NGVIEW_MOVE_OFFSET, 0, NGVIEW_MOVE_OFFSET, sys_rect.Height(), TRUE);
}

void CFilmInsDlg::ClickBtnenhctrlLabelMainview()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CRect sys_rect;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &sys_rect, NULL);
	MoveWindow(sys_rect.Width() - MAINVIEW_MOVE_OFFSET, 0, MAINVIEW_MOVE_OFFSET, sys_rect.Height(), TRUE);
}


//20150604 ngh Auto Delete
BOOL CFilmInsDlg::StartAutoDelete_Img()
{
	CString Delete_Img_dir,Delete_Data_dir,Delete_Ng_Data_dir,Delete_Tab_Data_dir;
	CString Delete_Product_dir,Delete_Spc_dir;
	CTime now_time = CTime::GetCurrentTime();
	BOOL bDelFolder[31];
	
	m_strImgPath = m_strLogPath;
	CheckDeleteDate();

	int nYear = m_iDel_Year;
	int nDelYear = now_time.GetYear();

	if(1 == now_time.GetMonth() && now_time.GetDay() < m_iDel_RecentDay)
	{
		nDelYear = m_iDel_Year;
	}
	else if(1 == now_time.GetMonth() && now_time.GetDay() > m_iDel_RecentDay)
	{
		nDelYear = m_iDel_Year - 1;
	}

	for(int nYear = m_iDel_Year; nYear >= nDelYear; nYear--)		// 110930 ytlee
	{
		if(nYear != now_time.GetYear())
		{
			m_iDel_Month = 12;
			m_iDel_Day = m_iDel_RecentDay;		// 120104 ytlee
		}

		for(int nMonth = m_iDel_Month; nMonth > 0; nMonth--)
		{
			if(nMonth == 1 || nMonth == 3 || nMonth == 5 ||
				nMonth == 7 || nMonth == 8 || nMonth == 10 || nMonth == 12)		// 120104 ytlee
			{
				m_iDel_Day = 31;
			}
			else if(nMonth == 4 || nMonth == 6 || nMonth == 9 || nMonth == 11)
			{
				m_iDel_Day = 30;
			}
			else if(nMonth == 2)
			{
				m_iDel_Day = 28;
			}
			if(nYear == now_time.GetYear() && (nMonth == now_time.GetMonth() || m_dlgSetup->m_SysPara.m_iPara[0] > now_time.GetDay()))		// 110901 ytlee
			{
				m_iDel_Day = m_iDel_RecentDay;
			}
			else
			{
				m_iDel_Day = 31;
			}

			for(int i = 0; i < 31; i++)
			{
				bDelFolder[i] = FALSE;
			}


			for(int nDay = 1; nDay <= m_iDel_Day; nDay++)
			{
				if(nMonth>9 && nDay>9)
					Delete_Img_dir.Format("%s\\%d%d%d",m_strImgPath, nYear, nMonth, nDay);
				else if(nMonth<10 && nDay>9)
					Delete_Img_dir.Format("%s\\%d0%d%d",m_strImgPath, nYear, nMonth, nDay);
				else if(nMonth>9 && nDay<10)
					Delete_Img_dir.Format("%s\\%d%d0%d",m_strImgPath, nYear, nMonth, nDay);
				else if(nMonth<10 && nDay<10)
					Delete_Img_dir.Format("%s\\%d0%d0%d",m_strImgPath, nYear, nMonth, nDay);

				CFileFind dbfile;

				if(dbfile.FindFile(Delete_Img_dir,0))
				{
					if(DeleteFolder(Delete_Img_dir))
						RemoveDirectory(Delete_Img_dir);
				}

				if(DeleteFolderSearch(Delete_Img_dir))
				{
					bDelFolder[nDay - 1] = TRUE;
				}
				else
				{
					bDelFolder[nDay - 1] = FALSE;
				}
			}

			for(int i = 0; i < m_iDel_Day; i++)
			{
				if(!bDelFolder[i])
				{
					return FALSE;
				}
			}
		}
	}

	return TRUE;
}


BOOL CFilmInsDlg::DeleteFolderSearch(CString strFolder)
{
	CFileFind ff;
	CString strTarget = strFolder;
	CStringArray strFileArray;

	if(strTarget.Right(1) != '\\')
	{
		strTarget += '\\';
	}
	
	strTarget += "*.*";
	if(ff.FindFile(strTarget)==TRUE)
	{
		CString Filename;
		BOOL bFlag = true;
		while(bFlag == TRUE)
		{
			bFlag = ff.FindNextFile();
			Filename = ff.GetFilePath();
			if(ff.IsDots())
			{
				continue;
			}
			if(ff.IsDirectory())
			{
				return FALSE;
			}
		}
	}
	ff.Close();

	return TRUE;

}