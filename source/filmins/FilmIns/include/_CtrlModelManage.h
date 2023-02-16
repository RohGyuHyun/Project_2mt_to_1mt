#pragma once

#include "afxcmn.h"
#include "Resource.h"
#include "BtnEnh.h"


//#define IDD_CTRLMODELMANAGE_DIALOG      10001
#define MAX_MOD_NAME					100
#define MAX_MOD_DESC					100

#define MOD_MODEL_PATH					"D:\\DATABASE\\MODEL"
#define MOD_MODEL_REGI   				"SOFTWARE\\CANINS\\MODEL"

//제품관리
typedef struct Model
{
	char szName[MAX_MOD_NAME];
	char szDesc[MAX_MOD_DESC];
	char szPath[MAX_PATH];
}typeModel;


// C_CtrlModelManageDlg 대화 상자입니다.

class AFX_EXT_CLASS C_CtrlModelManageDlg : public CDialog
{
	DECLARE_DYNAMIC(C_CtrlModelManageDlg)

public:
	C_CtrlModelManageDlg(CWnd* pParent = NULL, CString path = "", CString regi = "");   // 표준 생성자입니다.
	virtual ~C_CtrlModelManageDlg();

	BOOL RemoteModel(CString name, CString desc);
	void Open();
	void Model();
	void SetMainFolder(char* path);
	void SetMainRegister(char* path);
	char *GetName();
	char *GetDesc();
	char *GetPath();
	char *GetConversionName();
	char *GetConversionDesc();
	char *GetConversionPath();


// 대화 상자 데이터입니다.
	//enum { IDD = IDD_CTRLMODELMANAGE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	int m_iDlgType;
	int m_iModelIdx;
	char m_szMainPath[MAX_PATH];
	char m_szMainRegi[MAX_PATH];
	typeModel m_CurrentModel;
	typeModel m_ConversionModel;

	virtual BOOL OnInitDialog();

	void Conversion();
	void Select();
	void Remove();
	void Add();
	BOOL AddList(typeModel mod);
	BOOL SelectList(int idx, BOOL on_off);
	BOOL SetRegString(LPCTSTR lpstrKey, LPCTSTR lpstrEntry, LPCTSTR lpintValue);
	BOOL GetRegString(LPCTSTR lpstrKey, LPCTSTR lpstrEntry, BYTE *lpintValue);
	int RegDeleteSubKey(HKEY hKey, LPCTSTR lpSubKey);

	void RemoteSelect(CString modelname);
public:
	CBtnEnh m_Name;
	CBtnEnh m_Desc;
	CListCtrl m_list_model;
	CString m_edit_name;
	CString m_edit_desc;
	void ClickBtnenhctrlNew();
	void ClickBtnenhctrlSelect();
	void ClickBtnenhctrlDel();
	void ClickBtnenhctrlOk();
	void ClickBtnenhctrlCancel();
	afx_msg void OnEnChangeEditName();
	afx_msg void OnPaint();
	virtual INT_PTR DoModal(int type);

	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()

	
	afx_msg void OnNMDblclkListModel(NMHDR *pNMHDR, LRESULT *pResult);
};
