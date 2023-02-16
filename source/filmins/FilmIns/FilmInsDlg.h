
// FilmInsDlg.h : 헤더 파일
//

#pragma once
#include "BtnEnh.h"
#include "AreaCamControl.h"
#include "Comi/Comidas.h"
#include "afxcmn.h"
#include "afxwin.h"


#define	IMAGE_RECT_DIALOG	10000

#define SAVE_MAIN_PATH			"D:\\Insp_Result\\"
#define	LOG_PATH				"E:\\Insp_Result"

#define LIST_MONI_NUM			50

#define USER_MSG_VIS			WM_USER+10
#define USER_MSG_SOCKET			WM_USER+11 

#define	CONTROL_PC_IP_ADDRESS	"127.0.0.1"//"1.1.1.1"

#define SOCKET_PORT_NUM1		3000
#define SOCKET_PORT_NUM2		3001
#define SOCKET_PORT_NUM3		3002
#define SOCKET_PORT_NUM4		3003

#define NG_MAX_NUM				20   //최대 불량 저장 개수(큰것 기준) 	
#define LIST_MONI_NUM			50   //List line
#define SUB_NG_VIEW_INNER		25
#define NG_DISP_MAX_NUM			3

#define LIGHTING_MAX_NUM		4

#define ALIGN_FAILED_DATA		9999
#define ALIGN_OFFSET_DATA		5000
#define MOT_VISION_FLAG_DATA_MAX_NUM		30

#define NGVIEW_MOVE_OFFSET		770
#define MAINVIEW_MOVE_OFFSET	390

#define GOOD					0    //Good
#define LFPT					1   // LeadNFilmPulltionTop //Lead 변색 & Film 오염 Top Lead  //변경 
#define SDRD					2   // SolderResistDefect      //Solder Resist 관련 불량
#define IKCD					3   // InkChipDefect           //INK Chip
#define OSCD					4   // OtherSideChipDefect     //Chip 이면 결함
#define OSCO					5   // OtherSideChipOut        //이면 Chip Out
#define NBDD					6   // NoBondDefect            //No Bond
#define ESAD					7   // ExceedSpreadAreaDefect  //도포범위 초과
#define LCOD					8   // LeadCropOutDefect       //Lead 노출
#define PCOD					9   // POTCropOutDefect        //미충진
#define ROFD					10   // ResinOverfullDefect    //수지유출
#define RSKD					11   // ResinSprinkleDefect    //수지날림
#define RSTD					12   // ResinStainDefect       //수지묻음
#define BUBD					13   // BubbleDefect           //기포불량
#define NPTD					14   // NoPottingDefect        //No Potting
#define DPTD					15   // DoublePottingDefect    //이중 Potting
#define PPCD					16   // POTParticleDefect      //POT Particle
#define MKND					17   // MarkingNgDefect        //인쇄불량(Marking)
#define TPED					18   // TapeEditNgDefect       //편집테이프 불량
#define PFHD					19   // PFHoleNgDefect         //P.F Hole 찢김
#define PCND					20   // PunchNgDefect          //Punching불량
#define FPCD					21   // FilmPunchDefect        //Film 찍힘
#define FPKD					22   // FilmPuckerDefect       //Film 구겨짐
#define AMIF 					23   // Align Mark Insp fail   //Align Mark 인식 실패
#define NOCP					24   // No Chip                 //No Chip
#define LFPB					25   // LeadNFilmPulltionBottom //Lead 변색 & Film 오염 Bottom Lead   //추가
#define BRKN					26   // Broken
#define SRMP					27   //SR면 metal particle
#define AMSC					37   //AM scratch


#define	STDF					40   // Setup Data Fail	       //2D No TeachData		//변경 
#define AMPL                    41   // Align Mark Limit Over  //Align Pos Limit Over   //변경 
#define PNCH                    42   // Punch				   // Punch chip
#define SKIP					43


typedef struct VisionFlagData
{
	BOOL m_bFlag[MOT_VISION_FLAG_DATA_MAX_NUM];//0 : first chip start, 1 : first chip complete, 2 : first chip result(0 : not found, 1 : find)
	int m_iData[MOT_VISION_FLAG_DATA_MAX_NUM];
	double m_dData[MOT_VISION_FLAG_DATA_MAX_NUM];//0 : first chip offset

	int m_iVisGrabIdx;
	int m_iVisProcessIdx;

	BOOL m_bVisGrabEnable;
	BOOL m_bVisInsEnable;

	int m_iInsRsltType[SUB_RGN_MAX_NUM];
	char m_sInsRsltType[SUB_RGN_MAX_NUM][100];
	int m_iInsErrNum[SUB_RGN_MAX_NUM];
	int m_iInsFindChip[SUB_RGN_MAX_NUM];//1 : 칩찾음, 2 : 펀치칩, 3 : 펀치칩이면서 PF불량

	double m_dAlignOffset;

	char m_sSendPacket[1000];

	BOOL m_bInspectStartNGView;
}typeVisionFlagData;

typedef struct LOTInfo
{
	char m_sProductName[100];
	char m_sSawonNo[100];
	char m_sName[100];
	char m_sLOTNo[100];
	char m_sLOTType[100];
	int m_iPFCnt;
	char m_sFilmNo[100];
	char m_sManageNo[100];
	char m_sDeviceNo[100];
	int m_iInputCnt;
	char m_sCustomer[100];
	char m_sStatus[100];

	//For working
	BOOL m_bLOTStarted;
	BOOL m_bLOTApproved;
	BOOL m_bLOTTest;

	char m_sModelName[100];
}typeLOTInfo;


// CFilmInsDlg 대화 상자
class CFilmInsDlg : public CDialog
{
// 생성입니다.
public:
	CFilmInsDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	~CFilmInsDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_FILMINS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

public:
	//20150223 by
	//COMIZOA DIO Handle
	HANDLE m_hComiDevice32, m_hComiDevice64;

	//Live
	BOOL m_bLiveOverlayDisp;

	BOOL m_bCheckLoadMaster;
	BOOL m_bCheckGrab;

	CBrush m_BrushBlack;

	CString m_strImgPath;

	//NG view////////////////////////////////////
	int m_iNGViewIdx[IMG_RECT_MAX_NUM];//NG view index
	int m_iNGSubViewIdx[IMG_RECT_MAX_NUM];//NG sub view for scroll
	//NG scroll//////////////////////////////////
	int m_iNGScrollIdx[IMG_RECT_MAX_NUM];
	int m_iNGScrollLastNum[IMG_RECT_MAX_NUM];
	//NG review data/////////////////////////////
	char m_sNGLotID[IMG_RECT_MAX_NUM][MAX_PATH];
	char m_sNGDate[IMG_RECT_MAX_NUM][MAX_PATH];
	int m_iNGDeviceIdx[IMG_RECT_MAX_NUM];

	int m_RectIdx;
	int m_iMapIdx[IMG_RECT_MAX_NUM];

	int m_iGrbIdx[IMG_RECT_MAX_NUM];//Grab index
	int m_iPrcIdx[IMG_RECT_MAX_NUM];//Process index
	int m_iBufIdx[IMG_RECT_MAX_NUM];//Buffer index
	int m_iRslt[IMG_RECT_MAX_NUM];//Result

	int m_iPrcDevIdx[IMG_RECT_MAX_NUM];//디바이스 인덱스
	int m_iGrbDevIdx[IMG_RECT_MAX_NUM];//디바이스 인덱스
	
	CString m_strLotNo;					//lotnumber
	CString m_strStartTime;			//start time
	CString m_strLogPath;			//저장 경로


	//Inspection flag////////////////////////////
	BOOL m_bFlagGrabStart[IMG_RECT_MAX_NUM];

	BOOL m_bChkNGKind[IMG_RECT_MAX_NUM][SUB_RGN_MAX_NUM];

	long m_lProfileMode; //20110418 lee
	long m_lColorMode;

	//Time
	long m_lGrabTime;
	long m_lProcessTime;

	//테스트시 사용
	long m_lTest;

	//Auto Teaching Data //20100729 lee
	double m_offset_x, m_offset_y;
	int m_iDel_Year, m_iDel_Month, m_iDel_Day, m_iDel_RecentDay;

	short m_iLightChIdx[LIGHTING_MAX_NUM];		 //카메라별 조명 채널인덱스
	short m_iLightOnOffIdx[LIGHTING_MAX_NUM]; //카메라별 조명 OnOff 인덱스
	BOOL m_bIsColor;							//컬러 카메라 유무

	//모델
	CString m_strDBPath;						//데이터베이스 경로
	CString m_strRegiPath;						//레지스트리 경로

	Hlong m_lWidth[IMG_RECT_MAX_NUM], m_lHeight[IMG_RECT_MAX_NUM];
	Hlong m_lWindowID[IMAGE_MAP_MAX_NUM];
	Hlong m_lNGWindowID[IMAGE_NG_NUM];

	CWinThread   *m_pThread[IMG_RECT_MAX_NUM];
 	CWinThread   *m_pInspectThread[IMG_RECT_MAX_NUM];

	//20150604 ngh Auto Delete
	CWinThread *m_pDeleSearchThread;
	BOOL m_bDeleteFolderStart;
	BOOL m_bDeleteFolder;
	
	//IMSI 이미지 저장 스레드
	/*
	CWinThread	 *m_pSaveImageThread;
	BOOL m_bSaveImageStart;
	UINT static SaveImageThread(LPVOID pParam);
	*/

	//Inspection/////////////////////////////////
	BOOL m_bInspectStart[IMG_RECT_MAX_NUM];//Auto inspection flag
	BOOL m_bInspectStartNGView[IMG_RECT_MAX_NUM];//Auto inspection flag for NG view
	BOOL m_bInspectProcStartCheck[IMG_RECT_MAX_NUM];//20101007 lee

	//CAreaCamControl
	CAreaCamControl m_cCam;
	//Vision module
	C_VisionModule *m_VisMod;
	//Rect Image
	C_CtrlImageRect *m_dlgImgRect[IMG_RECT_MAX_NUM];
	//소켓통신을위한 클라이언트 객체
	CClientSock *m_Client;

	//Vision Flag
	typeVisionFlagData m_VisFlagData[IMG_RECT_MAX_NUM];

	CSetUpBsaeDlg *m_dlgSetup;
	CDialogProcessing m_dlgProcessing;

public:
	BOOL WriteLogAVI(CString path, CString file, CString bak_file, CString log, long limit_line);
	BOOL WriteLogStatus(CString path, CString file, CString bak_file, CString log, long limit_line);
	BOOL DeleteFolder(CString strFolder);
	BOOL Test();
	BOOL ScrollCheck(int cur_scroll_idx);
	BOOL WriteLogPot(CString path, CString file, CString bak_file, CString log, long limit_line);
	BOOL CheckDeleteDate();
	BOOL StartInspectThread();
	BOOL StopInspectThread();
	BOOL RemoteControlModel(short menu_select, CString model_name = NULL);
	BOOL AutoTeachOCV();
	
	//Socket Send
	void OnSend(CString data);
	void WriteStatusList(LPCTSTR lpszItem);
	void UpdateLight(BOOL update);
	void UpdateMapButton(int idx);
	//void ChangeStateLightValueEditBox();
	void ModelSave();
	void GrabProcess(int cam_idx, int grab_idx, int img_cnt);
	void LotDisp(typeLotInfo lot);
	void WriteLot(typeLotInfo lot);
	void DoEvents();
	void NGViewDisp();
	void ScrollCheck();
	void WriteTCPList(LPCTSTR lpszItem, BOOL IsRCVorSND);
	void ResultLog(int cam_idx, int buf_idx, int ng_view_idx, int cof_idx, int defect_idx, int defect_type, 
		long area, double cx, double cy, double sizex, double sizey, BOOL save);
	void ResultPotLog(int cam_idx, int buf_idx, int ng_view_idx, int device_idx, Hlong area);
	
	//20150223 by
	void SetLight(int ch, int var);
	//void SetLight(HANDLE hdev, int ch, int var);		// Digital Light
	
	//Inspect result
	void CheckNGKind(int grab_proc_type, int buf_idx);
	void InspectResult(int grab_proc_type, int buf_idx);
	void ResultCheck_Display(int cam_idx, int buf_idx, int ng_view_idx, BOOL auto_mode, BOOL main_view, BOOL chk_ng_type, BOOL chk_ng_Disp, BOOL ng_view_data_save); //20110509 lee	C_CtrlImageRectDlg *m_dlgImgRect[IMG_RECT_MAX_NUM];
	void SetLightIndex(int camidx);
	void InspectProcess(int cam_idx, int prc_idx, int map_Idx);
	void SetStaticMapType(int vis_idx);
	void ResetCount();
	
	int HandleAllMessage();
	int CalculateGrayValues();
	double CalculateContrastValues();
	
	UINT static GrabThreadCam0(LPVOID pParam);
    UINT static InspectThread(LPVOID pParam);

	//20150604 ngh Auto Delete
	UINT static DeleteSearchThread(LPVOID pParam);
	BOOL DeleteFolderSearch(CString path);
	BOOL StartAutoDelete_Img();


	//Image display
	LRESULT OnImageRectMessage(WPARAM para0, LPARAM para1);
	LRESULT OnVisMessage(WPARAM para0, LPARAM para1);
	LRESULT OnSocketMessage(WPARAM para0, LPARAM para1);

	//BOOL StartAutoDelete_Img();	// 미사용
	//void ScreenCapture(CString strPath); //미사용

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg LRESULT OnConnect(WPARAM, LPARAM);
	afx_msg LRESULT OnReceive(WPARAM, LPARAM);
	afx_msg LRESULT OnClose(WPARAM, LPARAM);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnRectImgMap0();
	afx_msg void OnRectImgMap1();
	afx_msg void OnRectImgMap2();
	afx_msg void OnRectImgMap3();
	afx_msg void OnRectImgMap4();
	afx_msg void OnRectImgMap5();
	afx_msg void OnRectImgMap6();
	afx_msg void OnRectImgMap7();
	afx_msg void OnRectImgMap8();
	afx_msg void OnRectImgMap9();
	afx_msg void OnRectImgMap10();
	afx_msg void OnRectImgMap11();
	afx_msg void OnNMRClickListTcp(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	DECLARE_MESSAGE_MAP()
public:
	CBtnEnh m_label_online;
	CBtnEnh m_label_tot;
	CBtnEnh m_label_good;
	CBtnEnh m_label_ng;
	CBtnEnh m_label_gtime;
	CBtnEnh m_label_ptime;
	CBtnEnh m_label_model;
	CBtnEnh m_ctrlMainTitle;
	CBtnEnh m_label_mode;
	CBtnEnh m_map0;
	CBtnEnh m_map1;
	CBtnEnh m_map2;
	CBtnEnh m_map3;
	CBtnEnh m_map4;
	CBtnEnh m_map5;
	CBtnEnh m_map6;
	CBtnEnh m_map7;
	CBtnEnh m_btnStartStop;
	CBtnEnh m_btnSetup;
	CBtnEnh m_label_ngview;
	
	CScrollBar m_scroll_ng;
	CListCtrl m_list_rslt;
	CListCtrl m_list_tcp;
	
	int m_img0_ch0;
	int m_img0_ch1;
	int m_img0_ch2;
	int m_img0_ch3;
	int m_img1_ch0;
	int m_img1_ch1;
	int m_img1_ch2;
	int m_img1_ch3;
	int m_img2_ch0;
	int m_img2_ch1;
	int m_img2_ch2;
	int m_img2_ch3;
	int m_img3_ch0;
	int m_img3_ch1;
	int m_img3_ch2;
	int m_img3_ch3;
	int m_img4_ch0;
	int m_img4_ch1;
	int m_img4_ch2;
	int m_img4_ch3;
	int m_img5_ch0;
	int m_img5_ch1;
	int m_img5_ch2;
	int m_img5_ch3;
	int m_img6_ch0;
	int m_img6_ch1;
	int m_img6_ch2;
	int m_img6_ch3;
	int m_img7_ch0;
	int m_img7_ch1;
	int m_img7_ch2;
	int m_img7_ch3;
	int m_img8_ch0;
	int m_img8_ch1;
	int m_img8_ch2;
	int m_img8_ch3;
	int m_img9_ch0;
	int m_img9_ch1;
	int m_img9_ch2;
	int m_img9_ch3;
	int m_img10_ch0;
	int m_img10_ch1;
	int m_img10_ch2;
	int m_img10_ch3;
	int m_img11_ch0;
	int m_img11_ch1;
	int m_img11_ch2;
	int m_img11_ch3;

	DECLARE_EVENTSINK_MAP()

	void ClickBtnenhctrlRunstop();
	void ClickBtnenhctrlModel();
	void ClickBtnenhctrlSetup();
	void ClickBtnenhctrlExit();
	void ClickBtnenhctrlLabelSelProfilemode();
	void ClickBtnenhctrlLabelSelColormode();
	void ClickBtnenhctrlLabelResult4();
	void ClickBtnenhctrlLabelMapTest();
	void ClickBtnenhctrlLabelMapSave();
	void ClickBtnenhctrlLabelMapLoad();
	void ClickBtnenhctrlLabelMapLoad3();
	void ClickBtnenhctrlLabelMap0();
	void ClickBtnenhctrlLabelMap1();
	void ClickBtnenhctrlLabelMap2();
	void ClickBtnenhctrlLabelMap3();
	void ClickBtnenhctrlLabelMap4();
	void ClickBtnenhctrlLabelMap5();
	void ClickBtnenhctrlLabelMap6();
	void ClickBtnenhctrlLabelMap7();
	void ClickBtnenhctrlLabelMap8();
	void ClickBtnenhctrlLabelMap9();
	void ClickBtnenhctrlLabelMap10();
	void ClickBtnenhctrlLabelMap11();
	void DisplayView(int cam_idx, int buf_idx, int img_idx = 0);

	int m_edit_test;
	
	void ClickBtnenhctrl2();
	void ClickBtnenhctrlLabelNgview();
	void ClickBtnenhctrlLabelMainview();
};
