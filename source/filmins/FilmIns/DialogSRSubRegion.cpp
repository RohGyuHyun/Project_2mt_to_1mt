// DialogSRSubRegion.cpp : ���� �����Դϴ�.
//

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

#include "DialogSRSubRegion.h"


// CDialogSRSubRegion ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDialogSRSubRegion, CDialog)

CDialogSRSubRegion::CDialogSRSubRegion(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogSRSubRegion::IDD, pParent)
	, m_check_large(FALSE)
{
	int i, j;
	m_iCurListCtrlIndex = 0;
	
	for(i = 0; i < SR_SUBREGION_MAX_NUM; i++)
	{
		for(j = 0; (j < SR_SUBREGION_POINT_MAX_NUM); j++)
		{
			m_iSmallSubRgnX[i][j] = 0;
			m_iSmallSubRgnY[i][j] = 0;
		}
	}

	for(j = 0; (j < SR_SUBREGION_MAX_NUM); j++)
	{
		m_iSmallSubRgnType[j] = 0;
		m_iSmallSubRgnPointIdx[j] = 0;
	}

}

CDialogSRSubRegion::~CDialogSRSubRegion()
{

}

void CDialogSRSubRegion::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_REGION_SMALL, m_list_sr_small);
	DDX_Check(pDX, IDC_CHECK_LARGE, m_check_large);
}

BEGIN_MESSAGE_MAP(CDialogSRSubRegion, CDialog)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_CLICK, IDC_LIST_REGION_SMALL, &CDialogSRSubRegion::OnNMClickListRegionSmall)
	ON_BN_CLICKED(IDC_CHECK_LARGE, &CDialogSRSubRegion::OnBnClickedCheckLarge)
END_MESSAGE_MAP()

// CDialogSRSubRegion �޽��� ó�����Դϴ�.
BEGIN_EVENTSINK_MAP(CDialogSRSubRegion, CDialog)
	ON_EVENT(CDialogSRSubRegion, IDC_BTNENHCTRL_START_SUBSR_SMALL, DISPID_CLICK, CDialogSRSubRegion::ClickBtnenhctrlStartSubsrSmall, VTS_NONE)
	ON_EVENT(CDialogSRSubRegion, IDC_BTNENHCTRL_P_SUBSR_SMALL, DISPID_CLICK, CDialogSRSubRegion::ClickBtnenhctrlPSubsrSmall, VTS_NONE)
	ON_EVENT(CDialogSRSubRegion, IDC_BTNENHCTRL_M_SUBSR_SMALL, DISPID_CLICK, CDialogSRSubRegion::ClickBtnenhctrlMSubsrSmall, VTS_NONE)
	ON_EVENT(CDialogSRSubRegion, IDC_BTNENHCTRL_V_SUBSR_SMALL, DISPID_CLICK, CDialogSRSubRegion::ClickBtnenhctrlVSubsrSmall, VTS_NONE)
	ON_EVENT(CDialogSRSubRegion, IDC_BTNENHCTRL_SUBSR_OK, DISPID_CLICK, CDialogSRSubRegion::ClickBtnenhctrlSubsrOk, VTS_NONE)
	ON_EVENT(CDialogSRSubRegion, IDC_BTNENHCTRL_ADDRGN_SMALL, DISPID_CLICK, CDialogSRSubRegion::ClickBtnenhctrlAddrgnSmall, VTS_NONE)
	ON_EVENT(CDialogSRSubRegion, IDC_BTNENHCTRL_COMP_SUBSR_SMALL, DISPID_CLICK, CDialogSRSubRegion::ClickBtnenhctrlCompSubsrSmall, VTS_NONE)
	ON_EVENT(CDialogSRSubRegion, IDC_BTNENHCTRL_SUBSR_CANCEL, DISPID_CLICK, CDialogSRSubRegion::ClickBtnenhctrlSubsrCancel, VTS_NONE)
	ON_EVENT(CDialogSRSubRegion, IDC_BTNENHCTRL_DELRGN_SMALL, DISPID_CLICK, CDialogSRSubRegion::ClickBtnenhctrlDelrgnSmall, VTS_NONE)
END_EVENTSINK_MAP()

BOOL CDialogSRSubRegion::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	int i, j;
	char temp[DAT_STR_LENG];
	CString strtemp;

	LV_COLUMN m_lvColumn;

	m_lvColumn.mask= LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	m_lvColumn.fmt=LVCFMT_LEFT;
	m_lvColumn.pszText="No.";
	m_lvColumn.iSubItem=0;
	m_lvColumn.cx=85;
	m_list_sr_small.InsertColumn(0, &m_lvColumn);
	//m_list_sr_large.InsertColumn(0, &m_lvColumn);

	for(i = 1; i <= SR_SUBREGION_POINT_MAX_NUM; i++)
	{
		sprintf(temp, "Pt.#%d", i);
		m_lvColumn.pszText = temp;
		m_lvColumn.cx=85;
		m_list_sr_small.InsertColumn(i, &m_lvColumn);
		//m_list_sr_large.InsertColumn(i, &m_lvColumn);
	}
	set_color(pdlg->m_dlgImgRect[0]->GetView(), "red");
	//20150119 lss SR sub region
	m_iSmallSubRgnIdx = pdlg->m_dlgSetup->m_InsPara.m_iSmallSubRgnIdx;

	m_check_large = pdlg->m_dlgSetup->m_InsPara.m_iSmallSubRgnType[0];

	for(i = 0; (i < m_iSmallSubRgnIdx) && (i < SR_SUBREGION_MAX_NUM); i++)
	{
		m_iSmallSubRgnPointIdx[i] = pdlg->m_dlgSetup->m_InsPara.m_iSmallSubRgnPointIdx[i];
		m_iSmallSubRgnType[i] = pdlg->m_dlgSetup->m_InsPara.m_iSmallSubRgnType[i];

		if(0 == m_iSmallSubRgnType[i])
		{
			strtemp.Format("%d(��������)", i+1);
			m_list_sr_small.InsertItem(m_iSmallSubRgnIdx, strtemp);
		}
		else
		{
			strtemp.Format("%d(��������)", i+1);
			m_list_sr_small.InsertItem(m_iSmallSubRgnIdx, strtemp);
		}

		for(j = 0; (j < m_iSmallSubRgnPointIdx[i]) && (j < SR_SUBREGION_POINT_MAX_NUM); j++)
		{
			m_iSmallSubRgnX[i][j] = pdlg->m_dlgSetup->m_InsPara.m_iSmallSubRgnX[i][j];
			m_iSmallSubRgnY[i][j] = pdlg->m_dlgSetup->m_InsPara.m_iSmallSubRgnY[i][j];

			sprintf(temp, "(%d, %d)", m_iSmallSubRgnX[i][j], m_iSmallSubRgnY[i][j]);
			m_list_sr_small.SetItemText(i, j + 1, temp);

			sprintf(temp, "%d_%d", i + 1, j + 1);
			pdlg->m_dlgImgRect[0]->AddTracker(3, m_iSmallSubRgnX[i][j] - 50, m_iSmallSubRgnY[i][j] - 50, m_iSmallSubRgnX[i][j] + 50,  m_iSmallSubRgnY[i][j] +50, RGB(0, 255, 255), temp);
			pdlg->m_dlgImgRect[0]->Display();
		}
	}

	ClickBtnenhctrlVSubsrSmall();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDialogSRSubRegion::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

}

//���� ���� ���� ���θ����.
//�����Ǿ� �ִ� ��� ������ �ʱ�ȭ �մϴ�.
void CDialogSRSubRegion::ClickBtnenhctrlStartSubsrSmall()
{
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	int i, j;

	if(IDYES == MessageBox("��� ������ �ʱ�ȭ �մϱ�?", "���", MB_YESNO))
	{
		m_iCurListCtrlIndex = 0;

		m_iSmallSubRgnIdx = 0;	// Region Index �ʱ�ȭ.
		
		for(int i = 0; i < SR_SUBREGION_MAX_NUM; i++)
		{
			m_iSmallSubRgnPointIdx[i] = 0;	// Point per Region Index �ʱ�ȭ
			m_iSmallSubRgnType[i] = 0;

			for(j = 0; j < SR_SUBREGION_POINT_MAX_NUM; j++)
			{
				//Point �ʱ�ȭ
				m_iSmallSubRgnX[i][j] = 0;
				m_iSmallSubRgnY[i][j] = 0;
			}
		}

		m_list_sr_small.DeleteAllItems();
		pdlg->m_dlgImgRect[0]->DeleteAll();
	}
}

//�ϳ��� ������ �߰��մϴ�.
void CDialogSRSubRegion::ClickBtnenhctrlAddrgnSmall()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	CString strtemp;
	char temp[100];
	int i, j;

	if(SR_SUBREGION_MAX_NUM > (m_iSmallSubRgnIdx - 1) )
	{
		pdlg->m_dlgImgRect[0]->DeleteAll();

		m_iCurListCtrlIndex = m_iSmallSubRgnIdx;
		m_iSmallSubRgnIdx++;

		m_iSmallSubRgnPointIdx[m_iCurListCtrlIndex] = 0;
		m_iSmallSubRgnType[m_iCurListCtrlIndex] = 0;

		sprintf(temp, "%d", m_iSmallSubRgnIdx);
		m_list_sr_small.InsertItem(m_iSmallSubRgnIdx, temp);

	}
	else
	{
		//���� ���� �ѵ� ����
		AfxMessageBox("������ �ִ� 10�� ���� ���� �� �� �ֽ��ϴ�.");
	}
}

//���� �� �ϳ��� ����Ʈ�� �߰��մϴ�.
void CDialogSRSubRegion::ClickBtnenhctrlPSubsrSmall()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	char temp[100];
	int l, t, r, b;

	if(0 < m_iSmallSubRgnIdx)
	{
		//m_iCurListCtrlIndex = m_iSmallSubRgnIdx - 1;

		if(SR_SUBREGION_POINT_MAX_NUM > m_iSmallSubRgnPointIdx[m_iCurListCtrlIndex])
		{
			//�⺻ ���� ��ġ�� ȭ�� �߾�.
			l = (WIDTH / 2) - 50;
			t = (HEIGHT / 2) - 50;
			r = (WIDTH / 2) + 50;
			b = (HEIGHT / 2) + 50;
			
			sprintf(temp, "%d_%d", m_iSmallSubRgnIdx, m_iSmallSubRgnPointIdx[m_iCurListCtrlIndex] + 1);
			pdlg->m_dlgImgRect[0]->AddTracker(3, l, t, r, b, RGB(0, 155, 155), temp);
			pdlg->m_dlgImgRect[0]->Display();

			m_iSmallSubRgnX[m_iCurListCtrlIndex][m_iSmallSubRgnPointIdx[m_iCurListCtrlIndex]] = l + 50;
			m_iSmallSubRgnY[m_iCurListCtrlIndex][m_iSmallSubRgnPointIdx[m_iCurListCtrlIndex]] = t + 50;

			sprintf(temp, "(%d, %d)", l + 50, t + 50);
			m_list_sr_small.SetItemText(m_iCurListCtrlIndex, m_iSmallSubRgnPointIdx[m_iCurListCtrlIndex] + 1, temp);

			m_iSmallSubRgnPointIdx[m_iCurListCtrlIndex]++;
		}
		else
		{
			AfxMessageBox("�� ���� �� ����Ʈ�� 20�� ���� ���� �� �ֽ��ϴ�.");
		}
	}
	else
	{
		AfxMessageBox("������ ������ �����ϴ�.");
	}
}

void CDialogSRSubRegion::ClickBtnenhctrlMSubsrSmall()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	CString strtemp;
	char temp[100];
	int i, j;

	if(0 < m_iSmallSubRgnPointIdx[m_iCurListCtrlIndex])
	{
		m_iSmallSubRgnPointIdx[m_iCurListCtrlIndex]--;
		//Point �ʱ�ȭ
		m_iSmallSubRgnX[m_iCurListCtrlIndex][m_iSmallSubRgnPointIdx[m_iCurListCtrlIndex]] = 0;
		m_iSmallSubRgnY[m_iCurListCtrlIndex][m_iSmallSubRgnPointIdx[m_iCurListCtrlIndex]] = 0;
	}

	m_list_sr_small.DeleteAllItems();
	pdlg->m_dlgImgRect[0]->DeleteAll();


	for(i = 0; (i < m_iSmallSubRgnIdx) && (i < SR_SUBREGION_MAX_NUM); i++)
	{

		if(0 == m_iSmallSubRgnType[i])
		{
			strtemp.Format("%d(��������)", i+1);
			m_list_sr_small.InsertItem(m_iSmallSubRgnIdx, strtemp);
		}
		else
		{
			strtemp.Format("%d(��������)", i+1);
			m_list_sr_small.InsertItem(m_iSmallSubRgnIdx, strtemp);
		}

		for(j = 0; (j < m_iSmallSubRgnPointIdx[i]) && (j < SR_SUBREGION_POINT_MAX_NUM); j++)
		{
			sprintf(temp, "(%d, %d)", m_iSmallSubRgnX[i][j], m_iSmallSubRgnY[i][j]);
			m_list_sr_small.SetItemText(i, j + 1, temp);

			sprintf(temp, "%d_%d", i + 1, j + 1);
			pdlg->m_dlgImgRect[0]->AddTracker(3, m_iSmallSubRgnX[i][j] - 50, m_iSmallSubRgnY[i][j] - 50, m_iSmallSubRgnX[i][j] + 50,  m_iSmallSubRgnY[i][j] +50, RGB(0, 255, 255), temp);
			pdlg->m_dlgImgRect[0]->Display();
		}
	}

	ClickBtnenhctrlVSubsrSmall();
}

void CDialogSRSubRegion::ClickBtnenhctrlVSubsrSmall()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	int i, j;
	char temp[100];
	HTuple Sub_Rgn_Num_X, Sub_Rgn_Num_Y;
	Hobject sr_sub_rgn;

	::SendMessage(pdlg->m_hWnd, USER_MSG_IMG_RECT, 0, 0);

	//Sub region �����///////////////////////////////////////////////////////////////////////////////////////
	for(i = 0; (i < m_iSmallSubRgnIdx) && (i < SR_SUBREGION_MAX_NUM); i++)
	{
		Sub_Rgn_Num_X.Reset();
		Sub_Rgn_Num_Y.Reset();

		for(j = 0; (j < m_iSmallSubRgnPointIdx[i]) && (j < SR_SUBREGION_POINT_MAX_NUM); j++)
		{
			tuple_concat(Sub_Rgn_Num_X, m_iSmallSubRgnX[i][j], &Sub_Rgn_Num_X);
			tuple_concat(Sub_Rgn_Num_Y, m_iSmallSubRgnY[i][j], &Sub_Rgn_Num_Y);
		}

		gen_region_polygon_filled(&sr_sub_rgn, Sub_Rgn_Num_Y, Sub_Rgn_Num_X);

		set_color(pdlg->m_dlgImgRect[0]->GetView(), HTuple("green"));
		disp_obj(sr_sub_rgn, pdlg->m_dlgImgRect[0]->GetView());
	}

	pdlg->m_dlgImgRect[0]->DeleteAll();

	for(i = 0; (i < m_iSmallSubRgnIdx) && (i < SR_SUBREGION_MAX_NUM); i++)
	{
		for(j = 0; (j < m_iSmallSubRgnPointIdx[i]) && (j < SR_SUBREGION_POINT_MAX_NUM); j++)
		{
			sprintf(temp, "%d_%d", i + 1, j + 1);
			pdlg->m_dlgImgRect[0]->AddTracker(3, m_iSmallSubRgnX[i][j] - 50, m_iSmallSubRgnY[i][j] - 50, m_iSmallSubRgnX[i][j] + 50,  m_iSmallSubRgnY[i][j] +50, RGB(0, 255, 255), temp);
			pdlg->m_dlgImgRect[0]->Display();
		}
	}
	
}

void CDialogSRSubRegion::ClickBtnenhctrlSubsrOk()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	int i, j;

	//20150119 lss SR sub region
	pdlg->m_dlgSetup->m_InsPara.m_iSmallSubRgnIdx = m_iSmallSubRgnIdx;
	
	for(i = 0; (i < m_iSmallSubRgnIdx) && (i < SR_SUBREGION_MAX_NUM); i++)
	{
		pdlg->m_dlgSetup->m_InsPara.m_iSmallSubRgnType[i] = m_iSmallSubRgnType[i];
		pdlg->m_dlgSetup->m_InsPara.m_iSmallSubRgnPointIdx[i] = m_iSmallSubRgnPointIdx[i];

		for(j = 0; j < SR_SUBREGION_POINT_MAX_NUM; j++)
		{
			if(j < m_iSmallSubRgnPointIdx[i])
			{
				pdlg->m_dlgSetup->m_InsPara.m_iSmallSubRgnX[i][j] = m_iSmallSubRgnX[i][j];
				pdlg->m_dlgSetup->m_InsPara.m_iSmallSubRgnY[i][j] = m_iSmallSubRgnY[i][j];
			}
			else
			{
				pdlg->m_dlgSetup->m_InsPara.m_iSmallSubRgnX[i][j] = 0;
				pdlg->m_dlgSetup->m_InsPara.m_iSmallSubRgnY[i][j] = 0;
			}
		}
	}

	EndDialog(0);
}


void CDialogSRSubRegion::OnNMClickListRegionSmall(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�
	if( 0 > pNMItemActivate->iItem )
		return;

	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	m_list_sr_small.SetExtendedStyle(LVS_EX_FULLROWSELECT);	//����Ʈ ������ Ŭ�� �� �� �� ��� ����.

	int i;
	char temp[100];

	if(m_iSmallSubRgnIdx <= m_iCurListCtrlIndex)
	{
		return;
	}

	m_iCurListCtrlIndex = pNMItemActivate->iItem;	//���� ���õ� ������ ��ȣ
	i = m_iCurListCtrlIndex;

	pdlg->m_dlgImgRect[0]->DeleteAll();

	for(int j = 0; (j < m_iSmallSubRgnPointIdx[i]) && (j < SR_SUBREGION_POINT_MAX_NUM); j++)
	{
		sprintf(temp, "%d_%d", i + 1, j + 1);
		pdlg->m_dlgImgRect[0]->AddTracker(3, m_iSmallSubRgnX[i][j] - 50, m_iSmallSubRgnY[i][j] - 50, m_iSmallSubRgnX[i][j] + 50,  m_iSmallSubRgnY[i][j] +50, RGB(0, 255, 255), temp);	
	}

	m_check_large = m_iSmallSubRgnType[m_iCurListCtrlIndex];
	UpdateData(FALSE);

	::SendMessage(pdlg->m_hWnd, USER_MSG_IMG_RECT, 0, 0);

	*pResult = 0;
}

//���� ���� ������ ������ ����Ʈ�� �����Ѵ�.
//
void CDialogSRSubRegion::ClickBtnenhctrlCompSubsrSmall()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;
	int i, j;
	long left, top, right, bottom;
	char temp[100];
	int max_num = 0;
	CString strtemp;

	for(i = 0; (i < m_iSmallSubRgnPointIdx[m_iCurListCtrlIndex]) && (i < SR_SUBREGION_POINT_MAX_NUM); i++)
	{
		pdlg->m_dlgImgRect[0]->GetTracker(i, &left, &top, &right, &bottom);

		m_iSmallSubRgnX[m_iCurListCtrlIndex][i] = left + 50;
		m_iSmallSubRgnY[m_iCurListCtrlIndex][i] = top + 50;

		sprintf(temp, "(%d, %d)", m_iSmallSubRgnX[m_iCurListCtrlIndex][i], m_iSmallSubRgnY[m_iCurListCtrlIndex][i]);
		m_list_sr_small.SetItemText(m_iCurListCtrlIndex, i + 1, temp);

	}

	for(i = 0; (i < m_iSmallSubRgnIdx) && (i < SR_SUBREGION_MAX_NUM); i++)
	{
		if(0 == m_iSmallSubRgnType[i])
		{
			strtemp.Format("%d(��������)", i+1);
			m_list_sr_small.SetItemText(i, 0, strtemp);
		}
		else
		{
			strtemp.Format("%d(��������)", i+1);
			m_list_sr_small.SetItemText(i, 0, strtemp);
		}
	}

	ClickBtnenhctrlVSubsrSmall();
}

void CDialogSRSubRegion::ClickBtnenhctrlSubsrCancel()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	EndDialog(0);
}

void CDialogSRSubRegion::ClickBtnenhctrlDelrgnSmall()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
CFilmInsDlg *pdlg = (CFilmInsDlg *)AfxGetApp()->m_pMainWnd;

	int i, j;
	CString strtemp;
	char temp[100];

	if(IDYES == MessageBox("���� ������ �����Ͻðڽ��ϱ�?", "���", MB_YESNO))
	{

		m_iSmallSubRgnPointIdx[m_iCurListCtrlIndex] = 0;	
		m_iSmallSubRgnType[m_iCurListCtrlIndex] = 0;

		for(i = 0; i < SR_SUBREGION_POINT_MAX_NUM; i++)
		{
			m_iSmallSubRgnX[m_iCurListCtrlIndex][i] = 0;
			m_iSmallSubRgnY[m_iCurListCtrlIndex][i] = 0;
		}

		for(i = m_iCurListCtrlIndex; (i < (m_iSmallSubRgnIdx - 1) ) && (i < SR_SUBREGION_MAX_NUM); i++)
		{
			m_iSmallSubRgnPointIdx[i] = m_iSmallSubRgnPointIdx[i + 1];	
			m_iSmallSubRgnType[i] = m_iSmallSubRgnType[i + 1];

			for(j = 0; j < SR_SUBREGION_POINT_MAX_NUM; j++)
			{
				//Point �ʱ�ȭ
				m_iSmallSubRgnX[i][j] = m_iSmallSubRgnX[i + 1][j];
				m_iSmallSubRgnY[i][j] = m_iSmallSubRgnY[i + 1][j];
			}
		}

		m_list_sr_small.DeleteAllItems();
		pdlg->m_dlgImgRect[0]->DeleteAll();
		m_iCurListCtrlIndex = 0;
		m_iSmallSubRgnIdx--;	
		
		for(i = 0; (i < m_iSmallSubRgnIdx) && (i < SR_SUBREGION_MAX_NUM); i++)
		{

			if(0 == m_iSmallSubRgnType[i])
			{
				strtemp.Format("%d(��������)", i+1);
				m_list_sr_small.InsertItem(m_iSmallSubRgnIdx, strtemp);
			}
			else
			{
				strtemp.Format("%d(��������)", i+1);
				m_list_sr_small.InsertItem(m_iSmallSubRgnIdx, strtemp);
			}

			for(j = 0; (j < m_iSmallSubRgnPointIdx[i]) && (j < SR_SUBREGION_POINT_MAX_NUM); j++)
			{
				sprintf(temp, "(%d, %d)", m_iSmallSubRgnX[i][j], m_iSmallSubRgnY[i][j]);
				m_list_sr_small.SetItemText(i, j + 1, temp);

				sprintf(temp, "%d_%d", i + 1, j + 1);
				pdlg->m_dlgImgRect[0]->AddTracker(3, m_iSmallSubRgnX[i][j] - 50, m_iSmallSubRgnY[i][j] - 50, m_iSmallSubRgnX[i][j] + 50,  m_iSmallSubRgnY[i][j] +50, RGB(0, 255, 255), temp);
				pdlg->m_dlgImgRect[0]->Display();
			}
		}

		ClickBtnenhctrlVSubsrSmall();

	}
}

void CDialogSRSubRegion::OnBnClickedCheckLarge()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strtemp;

	UpdateData(TRUE);
	m_iSmallSubRgnType[m_iCurListCtrlIndex] = m_check_large;

	for(int i = 0; (i < m_iSmallSubRgnIdx) && (i < SR_SUBREGION_MAX_NUM); i++)
	{
		if(0 == m_iSmallSubRgnType[i])
		{
			strtemp.Format("%d(��������)", i+1);
			m_list_sr_small.SetItemText(i, 0, strtemp);
		}
		else
		{
			strtemp.Format("%d(��������)", i+1);
			m_list_sr_small.SetItemText(i, 0, strtemp);
		}
	}
}
