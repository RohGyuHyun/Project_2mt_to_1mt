
// FilmIns.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CFilmInsApp:
// �� Ŭ������ ������ ���ؼ��� FilmIns.cpp�� �����Ͻʽÿ�.
//

class CFilmInsApp : public CWinAppEx
{
public:
	CFilmInsApp();

// �������Դϴ�.
	public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CFilmInsApp theApp;