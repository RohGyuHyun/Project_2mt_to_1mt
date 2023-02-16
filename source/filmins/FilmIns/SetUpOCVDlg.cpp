// SetUpOCVDlg.cpp : 구현 파일입니다.
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
#include "DialogModelMenu.h"

#include "SetUpBsaeDlg.h"
#include "DialogProcessing.h"

#include "FilmInsDlg.h"

#include "SetUpOCVDlg.h"


// CSetUpOCVDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSetUpOCVDlg, CDialog)

CSetUpOCVDlg::CSetUpOCVDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetUpOCVDlg::IDD, pParent)
{
	m_map0 = 0;
	m_pat_score = 70;
	m_pat_thrsh = 30;
	m_var_thrsh = 20;
	m_defect_area = 10;
	m_ppunch_minarea = 0;
	m_ppunch_maxarea = 0;
	m_ppunchbur_tol = 0;
	//}}AFX_DATA_INIT

	m_iRectIdx = 0;

	m_bShow = FALSE;

	m_map0 = 2;
}

CSetUpOCVDlg::~CSetUpOCVDlg()
{
}

void CSetUpOCVDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MAPNUM0_OCV, m_map0);
	DDX_Text(pDX, IDC_EDIT_SCORE_OCV, m_pat_score);
	DDX_Text(pDX, IDC_EDIT_TRSH_OCV, m_pat_thrsh);
	DDX_Text(pDX, IDC_EDIT_VAR_TRSH_OCV, m_var_thrsh);
	DDX_Text(pDX, IDC_EDIT_DEFACT_AREA_OCV, m_defect_area);
	DDX_Text(pDX, IDC_EDIT_PPUNCH_MINTOL_FILM, m_ppunch_minarea);
	DDX_Text(pDX, IDC_EDIT_PPUNCH_MAXTOL_FILM, m_ppunch_maxarea);
	DDX_Text(pDX, IDC_EDIT_PPUNCHBURR_TOL_FILM, m_ppunchbur_tol);
}


BEGIN_MESSAGE_MAP(CSetUpOCVDlg, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CSetUpOCVDlg 메시지 처리기입니다.

BOOL CSetUpOCVDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	set_draw(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("margin"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CSetUpOCVDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	UpdateData(TRUE);	

	CRect rect;

	if(bShow)
	{
		m_bShow = TRUE;
		disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], pdlg->m_iMapIdx[0]), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
		ChageStateInspParamInputBox();

		Update(TRUE);
	}
	else
	{
		m_bShow = FALSE;
		Update(FALSE);
	}	
}

HBRUSH CSetUpOCVDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

    pDC->SetTextColor(RGB(255, 255, 255));    
	pDC->SetBkColor(RGB(0, 0, 0));

	return (HBRUSH)pdlg->m_BrushBlack;

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	//return hbr;
}
BEGIN_EVENTSINK_MAP(CSetUpOCVDlg, CDialog)
	ON_EVENT(CSetUpOCVDlg, IDC_BTNENHCTRL_REGION_SET, DISPID_CLICK, CSetUpOCVDlg::ClickBtnenhctrlRegionSet, VTS_NONE)
	ON_EVENT(CSetUpOCVDlg, IDC_BTNENHCTRL_OCV_TEACH, DISPID_CLICK, CSetUpOCVDlg::ClickBtnenhctrlOcvTeach, VTS_NONE)
	ON_EVENT(CSetUpOCVDlg, IDC_BTNENHCTRL_OCV_TEST1, DISPID_CLICK, CSetUpOCVDlg::ClickBtnenhctrlOcvTest1, VTS_NONE)
	ON_EVENT(CSetUpOCVDlg, IDC_BTNENHCTRL_OCV_TEST2, DISPID_CLICK, CSetUpOCVDlg::ClickBtnenhctrlOcvTest2, VTS_NONE)
//	ON_EVENT(CSetUpOCVDlg, IDC_BTNENHCTRL_OCV_TEACH, 6, CSetUpOCVDlg::AnimDLLDoneBtnenhctrlOcvTeach, VTS_I4 VTS_BOOL)
END_EVENTSINK_MAP()

void CSetUpOCVDlg::ClickBtnenhctrlRegionSet()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CDialogCharRgnSet dlg;

	dlg.DoModal();
}

void CSetUpOCVDlg::ClickBtnenhctrlOcvTeach()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	OCVTeach(m_map0, m_pat_thrsh);
}

void CSetUpOCVDlg::ClickBtnenhctrlOcvTest1()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	Hobject modimage, shape, Region, ReducedImage, RegionDiff, TransImage, CropImage;
	HTuple HomMat2D;
	Hlong wd, ht;
	double rx, ry, ra, rs, rscore, dx, dy, char_dx[SUB_RGN_MAX_NUM][OCV_CHAR_MAX_NUM], char_dy[SUB_RGN_MAX_NUM][OCV_CHAR_MAX_NUM]; 
	double char_cx[SUB_RGN_MAX_NUM][OCV_CHAR_MAX_NUM], char_cy[SUB_RGN_MAX_NUM][OCV_CHAR_MAX_NUM];
	Hlong rn;
	int i, j;
	int idx;
	BOOL rslt[SUB_RGN_MAX_NUM][OCV_CHAR_MAX_NUM];
	Hobject CharRgn;
	HTuple CX, CY, Row, Col;

	double pat_score;

	UpdateData(TRUE);

	pat_score = (((double)m_pat_score) * 0.01);
	
	Herror herr;
	CString status;

	disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());

	set_check("~give_error");
	herr = get_image_pointer1(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_18!");
		pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
		return;
	}	
	
	rn = 0;

	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaBase[0]) && (i < SUB_RGN_MAX_NUM); i++)
	{

		dx = 0;
		dy = 0;
		if(0 < pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2])
		{
			//Total//////////////////////////////
			gen_rectangle1(&Region, pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[i][0].top, 
									pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[i][0].left, 
									pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[i][0].bottom, 
									pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[i][0].right);
			reduce_domain(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0),Region,&ReducedImage);

			idx = pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2];

			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "green");
			disp_obj(Region, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());

			modimage.Reset();
			shape.Reset();

			rslt[i][idx] = TRUE;

			if(pdlg->m_dlgSetup->m_InsPara.m_bCreatMdIDOCV[idx])
			{
				Hal_ShapePatternFind(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 
					&ReducedImage, 
					pdlg->m_dlgSetup->m_InsPara.m_PatIDOCV[idx], 
					pdlg->m_dlgSetup->m_InsPara.m_PatParaOCV[idx], 
					pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[idx], 
					wd, ht,
					pdlg->m_dlgSetup->m_InsPara.m_dCharCenterX[idx],
					pdlg->m_dlgSetup->m_InsPara.m_dCharCenterY[idx],
					pat_score,//Score
					&modimage, 
					&shape, 
					&rx, 
					&ry, 
					&ra, 
					&rs, 
					&rscore, 
					&rn);
			
				if(rn)
				{
					set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "cyan");
					dx = rx - pdlg->m_dlgSetup->m_InsPara.m_dCharCenterX[idx];
					dy = ry - pdlg->m_dlgSetup->m_InsPara.m_dCharCenterY[idx];
					disp_obj(modimage, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
				}
				else
				{
					rslt[i][idx] = FALSE;

					set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "red");
					set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10, 10);
					write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(HTuple("FAIL : can't find characters!")));
				}
			}
			else
			{
				rslt[i][idx] = FALSE;

				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "red");
				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 10, 10);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(HTuple("FAIL : no pattern!")));
			}
					
			for(j = 0; j < pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2] && (1 == rslt[i][idx]); j++)
			{
				rslt[i][j] = TRUE;
				modimage.Reset();
				shape.Reset();

				//Sub////////////////////////////////
				//세부영역조정
				gen_rectangle1(&Region, pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[j].top + dy, 
										pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[j].left + dx, 
										pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[j].bottom + dy, 
										pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[j].right + dx);
				reduce_domain(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0),Region,&ReducedImage);

				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "gray");
				disp_obj(Region, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());

				if(pdlg->m_dlgSetup->m_InsPara.m_bCreatMdIDOCV[j])
				{
					Hal_ShapePatternFind(0, 
										&ReducedImage, 
										pdlg->m_dlgSetup->m_InsPara.m_PatIDOCV[j], 
										pdlg->m_dlgSetup->m_InsPara.m_PatParaOCV[j], 
										pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[j], 
										wd, ht,
										pdlg->m_dlgSetup->m_InsPara.m_dCharCenterX[j],
										pdlg->m_dlgSetup->m_InsPara.m_dCharCenterY[j],
										pat_score,//Score
										&modimage, 
										&shape, 
										&rx, 
										&ry, 
										&ra, 
										&rs, 
										&rscore, 
										&rn);
					if(rn)
					{
						char_cx[i][j] = rx;
						char_cy[i][j] = ry;

						char_dx[i][j] = rx - pdlg->m_dlgSetup->m_InsPara.m_dCharCenterX[j];
						char_dy[i][j] = ry - pdlg->m_dlgSetup->m_InsPara.m_dCharCenterY[j];

						set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("blue"));
						disp_obj(shape, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());

						set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "green");
						disp_obj(modimage, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());

						set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), ry + 50, rx);
						write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple((int)(rscore * 100)));
					}
					else
					{
						rslt[i][j] = FALSE;
						char_dx[i][j] = 0;
						char_dy[i][j] = 0;

						set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "red");
						set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 40 * (i * 30), 10);
						write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(HTuple("FAIL : can't find character!")));
					}
				}
				else
				{
					rslt[i][j] = FALSE;
					char_dx[i][j] = 0;
					char_dy[i][j] = 0;

					set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), "red");
					set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 40 * (i * 30), 10);
					write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(HTuple("FAIL : no pattern!")));
					break;
				}
				
			}
		}
	}
}

void CSetUpOCVDlg::ClickBtnenhctrlOcvTest2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	if(!pdlg->m_bInspectStart)
	{

	}
	else
	{
		pdlg->WriteStatusList("WARRNING : 검사 진행 중 입니다!");
	}	
}


void CSetUpOCVDlg::Update(BOOL flag)
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	CRect rect;

	if(flag)
	{
		//0 : pat threshold, 1 : pat score, 2 : taught character num, 3 : var threshold, 4 : var tol
		m_pat_thrsh = pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[0];		
		m_pat_score = pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[1];
		//m_var_thrsh = pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[3]; 
		//m_defect_area = pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[4]; 

		//m_map0 = pdlg->m_dlgSetup->m_InsPara.m_iParaMapOCV[0];
		m_map0 = 2;

		//20100702
		m_ppunch_minarea = pdlg->m_dlgSetup->m_InsPara.m_iParaPPunch[0];
		m_ppunch_maxarea = pdlg->m_dlgSetup->m_InsPara.m_iParaPPunch[1];
		m_ppunchbur_tol = pdlg->m_dlgSetup->m_InsPara.m_iParaPPunch[2];
		
		disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], pdlg->m_iMapIdx[0]), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());

		UpdateData(FALSE);
	}
	else
	{
		UpdateData(TRUE);

		pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[0] = m_pat_thrsh;
		pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[1] = m_pat_score;
		//pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[3] = m_var_thrsh; 
		//pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[4] = m_defect_area; 

		pdlg->m_dlgSetup->m_InsPara.m_iParaMapOCV[0] = m_map0;
		//pdlg->m_dlgSetup->m_InsPara.m_iParaMapFilm[2] =  m_map0;

		//20100702
		pdlg->m_dlgSetup->m_InsPara.m_iParaPPunch[0] = m_ppunch_minarea;
		pdlg->m_dlgSetup->m_InsPara.m_iParaPPunch[1] = m_ppunch_maxarea;
		pdlg->m_dlgSetup->m_InsPara.m_iParaPPunch[2] = m_ppunchbur_tol;
	}
}


void CSetUpOCVDlg::RegionSeperate()
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);	
	
	CRect rect, rect_o;
	long i, j;
	long left, top, right, bottom;
	int rgn_num;

	disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], pdlg->m_iMapIdx[0]), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());

	//Get region & sperate
	rgn_num = pdlg->m_dlgImgRect[m_iRectIdx]->GetNum();
	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2]) && (i < rgn_num); i++)
	{
		pdlg->m_dlgImgRect[m_iRectIdx]->GetTracker(i, &left, &top, &right, &bottom);
		rect.left = left;
		rect.top = top;
		rect.right = right;
		rect.bottom = bottom;
						
		rect_o = pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i];
		
		if(rect != rect_o)
		{
			if( OCV_CHAR_MAX_NUM > (pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2] - 2) )
			{
				if(rect_o.left > rect.left)
				{
					//Until total region(added more 1 region lastly...)
					pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2]++;
					for(j = pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2]; j > i; j--)
					{
						//Shift
						pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[j] = pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[j - 1];
					}
					pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[j + 1] = CRect(rect.right, rect.top, rect_o.right, rect_o.bottom);
					pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[j] = rect;
				}
			}
		}
	}

	char temp[100];
	
	pdlg->m_dlgImgRect[m_iRectIdx]->DeleteAll();
	pdlg->m_dlgImgRect[m_iRectIdx]->SetActiveTracker(TRUE);
	
	//Char region
	for(i = 0; i < pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2]; i++)
	{
		rect = pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i];
		sprintf(temp, "%d", i + 1);
		pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom+1, RGB(0, 255, 255), temp);
		pdlg->m_dlgImgRect[m_iRectIdx]->Display();
	}
}


void CSetUpOCVDlg::RegionMerge()
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);	
	
	CRect rect;
	long i, j;
	long left, top, right, bottom;
	int rgn_num;

	disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());

	//Get region
	rgn_num = pdlg->m_dlgImgRect[m_iRectIdx]->GetNum();
	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2]) && (i < rgn_num); i++)
	{
		pdlg->m_dlgImgRect[m_iRectIdx]->GetTracker(i, &left, &top, &right, &bottom);
		rect.left = left;
		rect.top = top;
		rect.right = right;
		rect.bottom = bottom;
						
		pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i] = rect;
	}

	//Region merge
	Hobject Region0, Region1, InterRgn;
	HTuple Num, Area;
	HTuple Row0, Column0, Row1, Column1;
	for(i = 0; i < pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2]; i++)
	{
		if( i < (pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2] - 1) ) 
		{
			Region0.Reset();
			Region1.Reset();
			InterRgn.Reset();
			rect = pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i];
			gen_rectangle1(&Region0, rect.top, rect.left, rect.bottom, rect.right);
			
			rect = pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i + 1];
			gen_rectangle1(&Region1, rect.top, rect.left, rect.bottom, rect.right);
			
			//Intersection
			intersection(Region0, Region1, &InterRgn);
			area_center(InterRgn, &Area, _, _);
			count_obj(InterRgn, &Num);
			if(0 < (Hlong)Num[0])
			{
				if((Hlong)Area[0])
				{
					//Merge(shift)
					union2(Region0, Region1, &InterRgn);
					smallest_rectangle1(InterRgn,&Row0,&Column0,&Row1,&Column1);
					rect.top = (Hlong)Row0[0];
					rect.left = (Hlong)Column0[0];
					rect.bottom = (Hlong)Row1[0];
					rect.right = (Hlong)Column1[0];
					pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i] = rect;

					//Until total region(added more 1 region lastly...)
					for(j = i + 1; j < pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2]; j++)
					{
						//Shift
						pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[j] = pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[j + 1];
					}

					pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2]--;
				}
			}
		}
	}

	char temp[100];
	
	pdlg->m_dlgImgRect[m_iRectIdx]->DeleteAll();
	pdlg->m_dlgImgRect[m_iRectIdx]->SetActiveTracker(TRUE);
	
	//Char region
	for(i = 0; i < pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2]; i++)
	{
		rect = pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i];
		sprintf(temp, "%d", i + 1);
		pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(0, rect.left, rect.top, rect.right, rect.bottom+1, RGB(0, 255, 255), temp);
		pdlg->m_dlgImgRect[m_iRectIdx]->Display();
	}
}


void CSetUpOCVDlg::ExRegionMerge()//20141216 lss
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);	
	
	CRect rect;
	long i, j;
	long left, top, right, bottom;
	int rgn_num;

	//Get region
	rgn_num = pdlg->m_dlgImgRect[m_iRectIdx]->GetNum();
	for(i = 0; (i < pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2]) && (i < rgn_num); i++)
	{
		pdlg->m_dlgImgRect[m_iRectIdx]->GetTracker(i, &left, &top, &right, &bottom);
		rect.left = left;
		rect.top = top;
		rect.right = right;
		rect.bottom = bottom;
						
		pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i] = rect;
	}

	//Region merge
	Hobject Region0, Region1, InterRgn;
	HTuple Num, Area;
	HTuple Row0, Column0, Row1, Column1;
	for(i = 0; i < pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2]; i++)
	{
		if( i < (pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2] - 1) ) 
		{
			Region0.Reset();
			Region1.Reset();
			InterRgn.Reset();
			rect = pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i];
			gen_rectangle1(&Region0, rect.top, rect.left, rect.bottom, rect.right);
			
			rect = pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i + 1];
			gen_rectangle1(&Region1, rect.top, rect.left, rect.bottom, rect.right);
			
			//Intersection
			dilation_circle(Region0, &Region0, 2.5);
			dilation_circle(Region1, &Region1, 2.5);

			intersection(Region0, Region1, &InterRgn);
			area_center(InterRgn, &Area, _, _);
			count_obj(InterRgn, &Num);
			if(0 < (Hlong)Num[0])
			{
				if((Hlong)Area[0])
				{
					//Merge(shift)
					union2(Region0, Region1, &InterRgn);
					smallest_rectangle1(InterRgn,&Row0,&Column0,&Row1,&Column1);
					rect.top = (Hlong)Row0[0];
					rect.left = (Hlong)Column0[0];
					rect.bottom = (Hlong)Row1[0];
					rect.right = (Hlong)Column1[0];
					pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i] = rect;

					//Until total region(added more 1 region lastly...)
					for(j = i + 1; j < pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2]; j++)
					{
						//Shift
						pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[j] = pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[j + 1];
					}

					pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2]--;
				}
			}
		}
	}

}

void CSetUpOCVDlg::RegionSet()
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	RegionSet(m_map0, FALSE);
}

BOOL CSetUpOCVDlg::ExtRegionSet()
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	return RegionSet(pdlg->m_dlgSetup->m_InsPara.m_iParaMapOCV[0], TRUE);
}


BOOL CSetUpOCVDlg::ExtOCVTeach()
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	return OCVTeach(pdlg->m_dlgSetup->m_InsPara.m_iParaMapOCV[0], pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[0]);
}

BOOL CSetUpOCVDlg::OCVTeach(int map_idx, int thresh)
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);	

	BOOL rslt = TRUE;
	
	Hobject RegionHysteresis, RegionClosing, ConnectedRegions, RegionFillUp, RegionClosingRect;
	Hobject ReducedImage, Region, RegionRect, ModelImage, CropImage;
	HTuple Row0, Column0, Row1, Column1, Num;
	Hlong wd, ht;
	BOOL teach_ok = TRUE;
	int i;

	Herror herr;
	CString status;



	set_check("~give_error");
	herr = get_image_pointer1(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], map_idx), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_17!");
		pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	//20150123 lss 배경 제거 이미지 만들기
	Hlong min;
	HTuple Min, Max;
	Hobject ImageReduced, CharRgn, MaskImage;
	if(0 < pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2])
	{
		i = pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2];
		gen_rectangle1(&RegionRect, pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].top - 10, 
									pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].left - 10, 
									pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].bottom + 10, 
									pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].right + 10);

		min_max_gray(RegionRect, *pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], map_idx), HTuple(50), &Min, &Max, _);
		min = (Hlong)Min[0];
		min = min + thresh;
		if(0 > min)
		{
			min = 200;
		}
		if(255 < min)
		{
			min = 230;
		}
		reduce_domain(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], map_idx), RegionRect, &ImageReduced);
		threshold(ImageReduced, &CharRgn, min, 255);
		difference(RegionRect, CharRgn, &CharRgn);
		paint_region(CharRgn, *pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], map_idx), &MaskImage, 70, HTuple("fill"));
		disp_obj(MaskImage, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	}

	if(0 < pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2])
	{
		for(i = 0; i < pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2]; i++)
		{
			gen_rectangle1(&RegionRect, pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].top, 
										pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].left, 
										pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].bottom, 
										pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].right);

			//Teach
			if(Hal_ModelTeachingRect1(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 
										&MaskImage, 
										&pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i],
										wd,
										ht,
										pdlg->m_dlgSetup->m_InsPara.m_PatParaOCV[i], 
										thresh, //threshold
										thresh * 0.5, //min threshold
										&pdlg->m_dlgSetup->m_InsPara.m_dCharCenterX[i],
										&pdlg->m_dlgSetup->m_InsPara.m_dCharCenterY[i],
										&ModelImage, &pdlg->m_dlgSetup->m_InsPara.m_PatIDOCV[i]) )
			{
				pdlg->m_dlgSetup->m_InsPara.m_bCreatMdIDOCV[i] = TRUE;

				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));

				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].bottom + 10, pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].left);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(i));
			}
			else
			{
				pdlg->m_dlgSetup->m_InsPara.m_bCreatMdIDOCV[i] = FALSE;
				teach_ok = FALSE;

				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));

				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].bottom + 10, pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].left);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("X"));

				rslt = FALSE;
			}
			disp_obj(RegionRect, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
		}
		
		//Total teach
		gen_rectangle1(&RegionRect, pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].top, 
									pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].left, 
									pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].bottom, 
									pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].right);
		if(Hal_ModelTeachingRect1(0, 
									&MaskImage, 
									&pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i],
									wd,
									ht,
									pdlg->m_dlgSetup->m_InsPara.m_PatParaOCV[i], 
									thresh, //threshold
									thresh * 0.5, //min threshold
									&pdlg->m_dlgSetup->m_InsPara.m_dCharCenterX[i],
									&pdlg->m_dlgSetup->m_InsPara.m_dCharCenterY[i],
									&ModelImage, &pdlg->m_dlgSetup->m_InsPara.m_PatIDOCV[i]) )
		{
			pdlg->m_dlgSetup->m_InsPara.m_bCreatMdIDOCV[i] = TRUE;

			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("blue"));
		}
		else
		{
			pdlg->m_dlgSetup->m_InsPara.m_bCreatMdIDOCV[i] = FALSE;

			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));

			rslt = FALSE;
		}
		disp_obj(RegionRect, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	}	

	return rslt;
}

//20150130 이전. by
/*
BOOL CSetUpOCVDlg::OCVTeach(int map_idx, int thresh)
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	UpdateData(TRUE);	

	BOOL rslt = TRUE;
	
	Hobject RegionHysteresis, RegionClosing, ConnectedRegions, RegionFillUp, RegionClosingRect;
	Hobject ReducedImage, Region, RegionRect, ModelImage, CropImage;
	HTuple Row0, Column0, Row1, Column1, Num;
	Hlong wd, ht;
	BOOL teach_ok = TRUE;
	int i;

	Herror herr;
	CString status;

	disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], m_map0), pdlg->m_dlgImgRect[m_iRectIdx]->GetView());



	set_check("~give_error");
	herr = get_image_pointer1(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], map_idx), NULL, NULL, &wd, &ht);
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_17!");
		pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	



	if(0 < pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2])
	{
		for(i = 0; i < pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2]; i++)
		{
			gen_rectangle1(&RegionRect, pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].top, 
										pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].left, 
										pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].bottom, 
										pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].right);

			//Teach
			if(Hal_ModelTeachingRect1(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), 
										pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], map_idx), 
										&pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i],
										wd,
										ht,
										pdlg->m_dlgSetup->m_InsPara.m_PatParaOCV[i], 
										thresh, //threshold
										thresh * 0.5, //min threshold
										&pdlg->m_dlgSetup->m_InsPara.m_dCharCenterX[i],
										&pdlg->m_dlgSetup->m_InsPara.m_dCharCenterY[i],
										&ModelImage, &pdlg->m_dlgSetup->m_InsPara.m_PatIDOCV[i]) )
			{
				pdlg->m_dlgSetup->m_InsPara.m_bCreatMdIDOCV[i] = TRUE;

				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("green"));

				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].bottom + 10, pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].left);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple(i));
			}
			else
			{
				pdlg->m_dlgSetup->m_InsPara.m_bCreatMdIDOCV[i] = FALSE;
				teach_ok = FALSE;

				set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));

				set_tposition(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].bottom + 10, pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].left);
				write_string(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("X"));

				rslt = FALSE;
			}
			disp_obj(RegionRect, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
		}
		
		//Total teach
		gen_rectangle1(&RegionRect, pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].top, 
									pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].left, 
									pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].bottom, 
									pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i].right);
		if(Hal_ModelTeachingRect1(0, 
									pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], map_idx), 
									&pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i],
									wd,
									ht,
									pdlg->m_dlgSetup->m_InsPara.m_PatParaOCV[i], 
									thresh, //threshold
									thresh * 0.5, //min threshold
									&pdlg->m_dlgSetup->m_InsPara.m_dCharCenterX[i],
									&pdlg->m_dlgSetup->m_InsPara.m_dCharCenterY[i],
									&ModelImage, &pdlg->m_dlgSetup->m_InsPara.m_PatIDOCV[i]) )
		{
			pdlg->m_dlgSetup->m_InsPara.m_bCreatMdIDOCV[i] = TRUE;

			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("blue"));
		}
		else
		{
			pdlg->m_dlgSetup->m_InsPara.m_bCreatMdIDOCV[i] = FALSE;

			set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("red"));

			rslt = FALSE;
		}
		disp_obj(RegionRect, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	}	

	return rslt;
}
*/

void CSetUpOCVDlg::ChageStateInspParamInputBox()
{
	BOOL state;
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	
	state = pdlg->m_dlgSetup->m_SysPara.m_bPara[34];
	
	GetDlgItem(IDC_EDIT_PPUNCH_MINTOL_FILM)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_PPUNCH_MAXTOL_FILM)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_PPUNCHBURR_TOL_FILM)->EnableWindow(state);
	GetDlgItem(IDC_EDIT_TRSH_OCV)->EnableWindow(state);		// 110823 ytlee
	GetDlgItem(IDC_EDIT_SCORE_OCV)->EnableWindow(state);
}


BOOL CSetUpOCVDlg::RegionSet(int map_idx, BOOL auto_mode)
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	BOOL rslt = FALSE;

	Hobject RegionHysteresis, RegionClosing, ConnectedRegions, RegionFillUp, RegionClosingRect;
	Hobject ReducedImage, Region, RegionRect, ModelImage;
	HTuple Row0, Column0, Row1, Column1, Num, Min, Max;
	Hlong wd, ht;
	CRect rect, drect;
	CPoint point;
	HTuple Pointer;
	long i, max;
	int low_thresh, high_thresh;
	int rect_h;
	int idx;

	disp_obj(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], map_idx), pdlg->m_dlgImgRect[m_iRectIdx]->GetView()); //20110516 lee

	Herror herr;
	CString status;

	set_check("~give_error");
	herr = get_image_pointer1(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], map_idx), NULL, NULL, &wd, &ht); 
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at get_image_pointer1_19!");
		pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	gen_rectangle1(&Region, pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1].top + 10, 
							pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1].left + 10, 
							pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1].bottom - 10, 
							pdlg->m_dlgSetup->m_InsPara.m_InsRegionOCV[0][1].right - 10);

	//20150123 lss 배경 제거 이미지 만들기
	Hlong min;
	Hobject CharRgn, MaskImage, MaskReducedImage;
	int thresh = m_pat_thrsh;

	min_max_gray(Region, *pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], map_idx), HTuple(50), &Min, &Max, _);
	min = (Hlong)Min[0];
	min = min + thresh;
	if(0 > min)
	{
		min = 200;
	}
	if(255 < min)
	{
		min = 230;
	}
	reduce_domain(*pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], map_idx), Region, &ReducedImage);
	threshold(ReducedImage, &CharRgn, min, 255);
	connection(CharRgn,&CharRgn);
	closing_circle(CharRgn,&CharRgn,HTuple(8.0));
	select_shape(CharRgn, &CharRgn, HTuple("area"), HTuple("and"), HTuple(500),  HTuple("max"));
	difference(Region, CharRgn, &CharRgn);
	paint_region(CharRgn, *pdlg->m_VisMod->GetHalImage(0, pdlg->m_iBufIdx[0], map_idx), &MaskImage, 70, HTuple("fill"));
	disp_obj(MaskImage, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());

	reduce_domain(MaskImage, Region, &MaskReducedImage);

	//Find suitable threshold
	min_max_gray(Region, MaskImage, HTuple(0), _, &Max, _);//20110516 lee
	max = (Hlong)Max[0];

	low_thresh = max * 0.5;//0.5
	high_thresh = max * 0.6;//0.6

	if(low_thresh >= high_thresh)
	{
		low_thresh = 1;
		high_thresh = 255;
	}

	set_check("~give_error");
	herr = threshold(MaskReducedImage, &RegionHysteresis,HTuple(low_thresh),HTuple(255));
	set_check("give_error");
	if(H_MSG_TRUE == herr)
	{
	}
	else
	{
		status.Format("Halcon Error Occured at hysteresis_threshold_00!... %ld, %ld", (long)low_thresh, (long)high_thresh);
		pdlg->m_VisMod->WriteErrorList((LPCTSTR)status);
		return FALSE;
	}	

	dilation_rectangle1(RegionHysteresis, &RegionHysteresis,8,8);//2,2);20141216 lss
	closing_circle(RegionHysteresis,&RegionClosing,HTuple(8.0));//2.0));20141216 lss 
	connection(RegionClosing,&ConnectedRegions);
	sort_region(ConnectedRegions, &ConnectedRegions, "character", "true", "col");
	fill_up(ConnectedRegions,&RegionFillUp);
	closing_rectangle1(RegionFillUp,&RegionClosingRect,HTuple(50),HTuple(50));

	//select_shape(RegionClosingRect, &RegionClosingRect, HTuple("area"), HTuple("and"), HTuple(100),  HTuple("max"));
	select_shape(RegionClosingRect, &RegionClosingRect, HTuple("area"), HTuple("and"), HTuple(500),  HTuple("max"));

	smallest_rectangle1(RegionClosingRect,&Row0,&Column0,&Row1,&Column1);
	set_color(pdlg->m_dlgImgRect[m_iRectIdx]->GetView(), HTuple("cyan"));

	if(0 >= (Hlong)Row0.Num())
	{
		rslt = FALSE;
		return rslt;
	}

	gen_rectangle1(&RegionRect, Row0, Column0, Row1, Column1);
	disp_obj(RegionRect, pdlg->m_dlgImgRect[m_iRectIdx]->GetView());
	
	pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2] = (Hlong)Row0.Num();
	if( (OCV_CHAR_MAX_NUM - 1) < pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2])
	{
		pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2] = OCV_CHAR_MAX_NUM - 1;
	}
	if(0 < pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2])
	{
		idx = 0;
		for(i = 0; i < pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2]; i++)
		{
			rect.top = (Hlong)Row0[i];
			rect.left = (Hlong)Column0[i];
			rect.bottom = (Hlong)Row1[i];
			rect.right = (Hlong)Column1[i];

			/*
			//Divide
			if( (70 < rect.Width()) && (140 > rect.Width()) )
			{
				//2 divide
				point = rect.CenterPoint();
				drect.top = rect.top;
				drect.left = rect.left;
				drect.bottom = rect.bottom;
				drect.right = point.x;
				pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[idx] = drect;

				idx++;
				drect.left = point.x;
				drect.right = rect.right;
				pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[idx] = drect;
			}
			else if(140 < rect.Width())
			{
				//3 divide
				drect.top = rect.top;
				drect.left = rect.left;
				drect.bottom = rect.bottom;
				drect.right = rect.left + rect.Width() * 0.33;
				pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[idx] = drect;

				idx++;
				drect.left = drect.right;
				drect.right = rect.left + rect.Width() * 0.66;
				pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[idx] = drect;

				idx++;
				drect.left = drect.right;
				drect.right = rect.right;
				pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[idx] = drect;
			}
			else
			{
				rect_h = rect.Height();
				//작은것(하이픈)들은 검사영역을 크게 해준다.
				if(30 > rect_h)
				{
					rect.top = rect.top - rect_h;
					rect.bottom = rect.bottom + rect_h;
				}
				pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[idx] = rect;
			}
			*/

			pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[idx] = rect;
			idx++;
		}

		pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2] = idx;
	}
	
	//Total teach
	union1(RegionRect, &RegionRect);
	smallest_rectangle1(RegionRect,&Row0,&Column0,&Row1,&Column1);
	rect.top = (Hlong)Row0[0];
	rect.left = (Hlong)Column0[0];
	rect.bottom = (Hlong)Row1[0];
	rect.right = (Hlong)Column1[0];
	pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[idx] = rect;

	char temp[100];

	if(!auto_mode)
	{
		pdlg->m_dlgImgRect[m_iRectIdx]->DeleteAll();
		pdlg->m_dlgImgRect[m_iRectIdx]->SetActiveTracker(TRUE);
	}

	//Char region
	if(0 < pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2])
	{
		for(i = 0; i < pdlg->m_dlgSetup->m_InsPara.m_iParaOCV[2]; i++)
		{
			if(!auto_mode)
			{
				rect = pdlg->m_dlgSetup->m_InsPara.m_InsCharRegionOCV[i];
				sprintf(temp, "%d", i + 1);
				pdlg->m_dlgImgRect[m_iRectIdx]->AddTracker(3, rect.left, rect.top, rect.right, rect.bottom, RGB(0, 255, 255), temp);
				pdlg->m_dlgImgRect[m_iRectIdx]->Display();
			}

			rslt = TRUE;
		}
	}

	return rslt;
}

//void CSetUpOCVDlg::AnimDLLDoneBtnenhctrlOcvTeach(long iPicture, BOOL bResult)
//{
//	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
//}
