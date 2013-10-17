
// App11.h : main header file for the App11 application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CApp11App:
// See App11.cpp for the implementation of this class
//

class CApp11App : public CWinApp
{
public:
	CApp11App();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CApp11App theApp;
