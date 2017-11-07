#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CObjectsDetecterApp:
// See ObjectsDetecter.cpp for the implementation of this class
//

class CObjectsDetecterApp : public CWinApp
{
public:
	CObjectsDetecterApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CObjectsDetecterApp theApp;
