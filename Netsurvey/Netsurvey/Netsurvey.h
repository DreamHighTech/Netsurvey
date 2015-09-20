
// Netsurvey.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CNetsurveyApp:
// See Netsurvey.cpp for the implementation of this class
//
#define APP_REG		_T("SDR Free IP Scan")

class CNetsurveyApp : public CWinApp
{
public:
	CNetsurveyApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()

};

extern CNetsurveyApp theApp;