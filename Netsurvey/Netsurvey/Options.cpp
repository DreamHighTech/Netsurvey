// Options.cpp: implementation of the COptions class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Netsurvey.h"
#include "Options.h"
#include "NetsurveyDlg.h"
#include "DeleteFavoriteDlg.h"
#include "AddFavoriteDlg.h"
#include "globals.h"


COptions::COptions()
{
	m_aParsedPorts = 0;
}

COptions::~COptions()
{
	if (m_aParsedPorts != NULL)
		delete m_aParsedPorts;
}

BOOL COptions::setPortString(LPCWSTR szPortString)
{
	m_szPorts = szPortString;
	return parsePortString();
}

BOOL COptions::parsePortString()
{
	
	if (m_aParsedPorts != NULL)
		delete m_aParsedPorts;

	// Pre-process
	int nCommas = 0;
	m_szPorts += ',';
	LPCWSTR szPorts = m_szPorts;
	for (int i=0; szPorts[i] != 0; i++)
	{
		if (szPorts[i] == ',') nCommas++;
	}

	m_aParsedPorts = new tPortRange[nCommas + 10];
	memset(m_aParsedPorts, 0, (nCommas + 10) * sizeof(tPortRange));

	m_nPortCount = 0;

	// Process!!!
	TCHAR szCurPort[6];
	int nCurPortLen = 0;
	int nCurPortIndex = 0;	
	
	for (int i=0; szPorts[i] != 0; i++)
	{
		if (szPorts[i] >= '0' && szPorts[i] <= '9')
		{			
			if (nCurPortLen >= 5)
			{			
				AfxMessageBox(_T("A port number cannot be greater than 65535"), MB_ICONHAND | MB_OK, 0);
				return FALSE;
			}
			else
			if (nCurPortLen == 0 && szPorts[i] == '0')
			{
				AfxMessageBox(_T("A port number cannot start with 0"), MB_ICONHAND | MB_OK, 0);
				return FALSE;
			}

			szCurPort[nCurPortLen] = szPorts[i];
			nCurPortLen++;
		}
		else
		{
			szCurPort[nCurPortLen] = 0;
			u_short nCurPort = (u_short) _wtoi(szCurPort);
			nCurPortLen = 0;

			if (szPorts[i] == '-')
			{
				if (m_aParsedPorts[nCurPortIndex].nStartPort != 0)
				{
					AfxMessageBox(_T("Unexpected \"-\" in the port string"), MB_ICONHAND | MB_OK, 0);
					return FALSE;
				}

				m_aParsedPorts[nCurPortIndex].nStartPort = nCurPort;
			}
			else
			if (szPorts[i] == ',')
			{
				if (szPorts[i+1] != 0 && szCurPort[0] == 0)
				{
					AfxMessageBox(_T("Port cannot be 0 or unexpected comma in the port string"), MB_ICONHAND | MB_OK, 0);
					return FALSE;
				}							

				if (m_aParsedPorts[nCurPortIndex].nStartPort == 0)
				{
					m_aParsedPorts[nCurPortIndex].nStartPort = nCurPort;
				}
				
				m_aParsedPorts[nCurPortIndex].nEndPort = nCurPort;

				m_nPortCount += m_aParsedPorts[nCurPortIndex].nEndPort - m_aParsedPorts[nCurPortIndex].nStartPort + 1;

				nCurPortIndex++;								
			}
		}
	}	

	// Delete the comma added above
	m_szPorts.Delete(m_szPorts.GetLength()-1);

	return TRUE;	
}


void COptions::save()
{
	CWinApp *app = AfxGetApp();

	CString szNull = _T("");
	app->WriteProfileInt(szNull,_T("Delay"),m_nTimerDelay);
	app->WriteProfileInt(szNull,_T("MaxThreads"),m_nMaxThreads);
	app->WriteProfileInt(szNull,_T("Timeout"),m_nPingTimeout);	
	app->WriteProfileInt(szNull,_T("PingCount"),m_nPingCount);	
	app->WriteProfileInt(szNull,_T("DisplayOptions"),m_neDisplayOptions);	
	app->WriteProfileString(szNull, _T("PortString"), m_szPorts);
	app->WriteProfileInt(szNull, _T("ScanHostIfDead"), m_bScanHostIfDead);
	app->WriteProfileInt(szNull, _T("OptimizePorts"), m_bOptimizePorts);
	app->WriteProfileInt(szNull, _T("AutoSave"), m_bAutoSave);
	app->WriteProfileInt(szNull, _T("SkipBroadcast"), m_bSkipBroadcast);

	g_scanner->saveSelectedColumns();
}

void COptions::saveDimensions()
{
	CString szNull = _T("");
	CWinApp *app = AfxGetApp();

	BOOL bMaximized = app->GetMainWnd()->IsZoomed();
	app->WriteProfileInt(_T(""), _T("Maximized"), bMaximized);

	// Save window pos only if window is not maximized
	if (!bMaximized)
	{
		RECT rc;
		app->GetMainWnd()->GetWindowRect(&rc);
		app->WriteProfileInt(szNull,_T("Left"),rc.left);
		app->WriteProfileInt(szNull,_T("Top"),rc.top);
		app->WriteProfileInt(szNull,_T("Bottom"),rc.bottom);
		app->WriteProfileInt(szNull,_T("Right"),rc.right);
	}
	
	// Save column widths
	CString szTmp;
	CNetsurveyDlg *cDlg = (CNetsurveyDlg *) app->GetMainWnd();	
	for (int i=0; i < g_scanner->getColumnCount(); i++) 
	{
		g_scanner->getColumnName(i, szTmp);
		szTmp = _T("Col_") + szTmp;
		app->WriteProfileInt(szNull, szTmp, cDlg->m_nIpList.GetColumnWidth(i));
	}
}


void COptions::load()
{
	CWinApp *app = AfxGetApp();
	CString szNull = _T("");
	
	m_nTimerDelay = app->GetProfileInt(szNull,_T("Delay"),20);
	m_nMaxThreads = app->GetProfileInt(szNull,_T("MaxThreads"),64);
	m_nPingTimeout = app->GetProfileInt(szNull,_T("Timeout"),3000);
	m_nPingCount = app->GetProfileInt(szNull,_T("PingCount"),3);
	m_neDisplayOptions = app->GetProfileInt(szNull,_T("DisplayOptions"),FALSE);
	m_bScanHostIfDead = app->GetProfileInt(szNull, _T("ScanHostIfDead"), FALSE);
	m_bOptimizePorts = app->GetProfileInt(szNull,_T( "OptimizePorts"), TRUE);
	m_bAutoSave = app->GetProfileInt(szNull, _T("AutoSave"), FALSE);
	m_bSkipBroadcast = app->GetProfileInt(szNull, _T("SkipBroadcast"), TRUE);
	m_szNotAvailableValue = app->GetProfileString(szNull,_T("NotAvailableValue"),_T("[n/a]"));
	m_szNotScannedValue = app->GetProfileString(szNull,_T("NotScannedValue"),_T("[n/s]"));
	m_bAskConfirm = app->GetProfileInt(szNull,_T("AskConfirm"),FALSE);
	m_bShowInfo = app->GetProfileInt(szNull,_T("ShowInfo"),FALSE);

	setPortString(app->GetProfileString(szNull, _T("PortString"), szNull));	// also parses it

	// Load Favourites
	loadFavourites();

	// Load Openers
	loadOpeners();

	// Path, where the SDR Free IP Scan resides
	m_szExecutablePath = __targv[0];
	
	int nTmp = m_szExecutablePath.ReverseFind('\\');
#if 0
	if (nTmp < 0)
	{
		TCHAR szCurDir[256];
		::GetCurrentDirectory(sizeof(szCurDir), (TCHAR*) szCurDir);
		m_szExecutablePath = szCurDir;	// The program is run from a current directory	
	}
	else
	{	
		m_szExecutablePath.Delete(nTmp, m_szExecutablePath.GetLength() - nTmp);			
	}
#endif
}

void COptions::loadFavourites()
{
	CWinApp *app = AfxGetApp();

	CString szKey;
	CString szNull = _T("");

	for (int i=0; i < 99; i++)
	{
		szKey.Format(_T("FavouriteName_%d"), i);
		m_aFavourites[i].szName = app->GetProfileString(szNull, szKey);
		
		if (m_aFavourites[i].szName.GetLength() == 0)
			break;

		// Try to load the new-style favourite first
		szKey.Format(_T("FavouriteContent_%d"), i);
		m_aFavourites[i].szContent = app->GetProfileString(szNull, szKey);

		if (m_aFavourites[i].szContent.GetLength() == 0)
		{
			// TODO: remove this code after a few releases from 2.50
			// Let's make an attempt to load old-style favourite (backward compatibility)
			szKey.Format(_T("FavouriteIP1_%d"), i);
			IPAddress nIP1 = app->GetProfileInt(szNull, szKey, 0);
			szKey.Format(_T("FavouriteIP2_%d"), i);
			IPAddress nIP2 = app->GetProfileInt(szNull, szKey, 0);

			// Convert to a new style 
			in_addr inaddr;		
			inaddr.S_un.S_addr = nIP1;
			m_aFavourites[i].szContent = inet_ntoa(inaddr);	
			m_aFavourites[i].szContent += "|";
			inaddr.S_un.S_addr = nIP2;
			m_aFavourites[i].szContent += inet_ntoa(inaddr);	
		}
	}

}

void COptions::saveFavourites()
{
	CWinApp *app = AfxGetApp();

	CString szKey;
	CString szNull = _T("");

	for (int i=0; i < 99; i++)
	{
		szKey.Format(_T("FavouriteName_%d"), i);
		app->WriteProfileString(szNull, szKey, m_aFavourites[i].szName);		

		// This check should be here, because empty last string should be written above
		if (m_aFavourites[i].szName.GetLength() == 0)
			break;

		szKey.Format(_T("FavouriteContent_%d"), i);
		app->WriteProfileString(szNull, szKey, m_aFavourites[i].szContent);
	}
}

void COptions::loadOpeners()
{
	CWinApp *app = AfxGetApp();

	CString szKey;
	CString szNull =_T("");

	for (int i=0; i < 99; i++)
	{
		szKey.Format(_T("OpenerName_%d"), i);
		m_aOpeners[i].szName = app->GetProfileString(szNull, szKey);
		
		if (m_aOpeners[i].szName.GetLength() == 0)
			break;
		
		szKey.Format(_T("OpenerString_%d"), i);
		m_aOpeners[i].szExecute = app->GetProfileString(szNull, szKey);

		szKey.Format(_T("OpenerDir_%d"), i);
		m_aOpeners[i].szWorkDir = app->GetProfileString(szNull, szKey);

		szKey.Format(_T("OpenerCmdLine_%d"), i);
		m_aOpeners[i].bCommandLine = app->GetProfileInt(szNull, szKey, 0);
	}

	// Add default openers, if no openers were loaded
	if (m_aOpeners[0].szName.GetLength() == 0)
	{
		m_aOpeners[0].szName = "In explorer";
		m_aOpeners[0].szExecute = "\\\\%s";
		m_aOpeners[0].bCommandLine = FALSE;

		m_aOpeners[1].szName = "Web Browser (HTTP)";
		m_aOpeners[1].szExecute = "http://%s/";
		m_aOpeners[1].bCommandLine = FALSE;

		m_aOpeners[2].szName = "FTP";
		m_aOpeners[2].szExecute = "ftp://%s/";
		m_aOpeners[2].bCommandLine = FALSE;

		m_aOpeners[3].szName = "Telnet";
		m_aOpeners[3].szExecute = "telnet://%s/";
		m_aOpeners[3].bCommandLine = FALSE;

		m_aOpeners[4].szName = "Ping";
		m_aOpeners[4].szExecute = "ping %s";
		m_aOpeners[4].bCommandLine = TRUE;

		m_aOpeners[5].szName = "Traceroute";
		m_aOpeners[5].szExecute = "tracert %s";
		m_aOpeners[5].bCommandLine = TRUE;

		m_aOpeners[6].szName = "GeoLocate (Website)";
		m_aOpeners[6].szExecute = "http://www.geobutton.com/IpLocator.htm?GetLocation&ipaddress=%s";
		m_aOpeners[6].bCommandLine = FALSE;
	}

}

void COptions::saveOpeners()
{
	CWinApp *app = AfxGetApp();

	CString szKey;
	CString szNull = _T("");

	for (int i=0; i < 99; i++)
	{
		szKey.Format(_T("OpenerName_%d"), i);
		app->WriteProfileString(szNull, szKey, m_aOpeners[i].szName);

		if (m_aOpeners[i].szName.GetLength() == 0)
			break;

		szKey.Format(_T("OpenerString_%d"), i);
		app->WriteProfileString(szNull, szKey, m_aOpeners[i].szExecute);

		szKey.Format(_T("OpenerDir_%d"), i);
		app->WriteProfileString(szNull, szKey, m_aOpeners[i].szWorkDir);

		szKey.Format(_T("OpenerCmdLine_%d"), i);
		app->WriteProfileInt(_T(""), szKey, m_aOpeners[i].bCommandLine);
	}
}

void COptions::setWindowPos()
{
	CWinApp *app = AfxGetApp();

	RECT rc;
	CString szNull = _T("");

	rc.left = app->GetProfileInt(szNull,_T("Left"),0);
	rc.top = app->GetProfileInt(szNull,_T("Top"),0);
	rc.bottom = app->GetProfileInt(szNull,_T("Bottom"),0);
	rc.right = app->GetProfileInt(szNull,_T("Right"),0);	

	// Fix restoring from saved maximized state

	if (rc.left < 0)
	{
		rc.right += rc.left;
		rc.left = 0;		
	}

	if (rc.top < 0)
	{
		rc.bottom += rc.top;
		rc.top = 0;
	}

	CWnd *d = app->GetMainWnd();

    if (rc.right!=0) 
	{
		d->SetWindowPos(NULL,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,SWP_NOZORDER);
	} 
	else 
	{
		d->SetWindowPos(NULL,0,0,602,350,SWP_NOMOVE | SWP_NOZORDER);
	}

	if (app->GetProfileInt(szNull, _T("Maximized"), FALSE))
		d->ShowWindow(SW_SHOWMAXIMIZED);

	// Column widths are restored in CScanner::initListColumns()
}

CString COptions::getCurrentDate()
{
	TCHAR szTime[128];

	CString szDateTime;

	GetDateFormat(LOCALE_USER_DEFAULT, 0, NULL, NULL, (LPWSTR)&szTime, sizeof(szTime) / sizeof(TCHAR));
	szDateTime = szTime;

	szDateTime += TCHAR(' ');	
	GetTimeFormat(LOCALE_USER_DEFAULT, 0, NULL, NULL, (LPWSTR)&szTime, sizeof(szTime) / sizeof(TCHAR));
	szDateTime += szTime;	 
	
	return szDateTime;
}

void COptions::initFavouritesMenu(CMenu *pMenu)
{	
	// Delete items first
	for (int i=0; i < 250; i++) 
	{
		if (!pMenu->DeleteMenu(3, MF_BYPOSITION))
			break;
	}

	// Add menu items for favourites
	for (int i=0; i < 250; i++) 
	{
		if (m_aFavourites[i].szName.GetLength() == 0)
			break;

		pMenu->InsertMenu(i+3, MF_BYPOSITION, ID_MENU_FAVOURITES_001 + i, m_aFavourites[i].szName);				
	}
}

void COptions::initOpenersMenu(CMenu *pMenu)
{	
	// Delete items first
	for (int i=0; i < 99; i++) 
	{
		if (!pMenu &&!pMenu->DeleteMenu(2, MF_BYPOSITION))
			break;
	}

	CString szTmp;

	// Add menu items for openers
	for (int i=0; i < 99; i++) 
	{
		if (m_aOpeners[i].szName.GetLength() == 0)
			break;
		
		if (i <= 9)
			szTmp.Format(_T("%s\tCtrl+%d"), m_aOpeners[i].szName, i);
		else
			szTmp = m_aOpeners[i].szName;

//		pMenu->InsertMenu(i+2, MF_BYPOSITION, ID_MENU_OPEN_CMD_001 + i, szTmp);
	}
}


void COptions::addFavourite(const CString& szSerializedIPFeed)
{
	CAddFavoriteDlg dlg(AfxGetApp()->GetMainWnd());
// 
// 	dlg.m_szCaption = "Add Favorite";
// 	dlg.m_szQueryText = "Enter the name of current favorite:";	

	CString szTmp;	

	dlg.SetOriginalName(g_pIPStyle->getScanSummary());
// 	dlg.m_szDefaultUserText = g_pIPStyle->getScanSummary();
// 
// 	szTmp = dlg.doQuery();
	
	if(dlg.DoModal() == IDOK)
	{
		szTmp = dlg.GetNewFavorite();
		if (szTmp.GetLength() == 0)
			return;
	}
	

	// Find the empty slot and populate it
	for (int i=0; i < 250; i++)
	{
		if (m_aFavourites[i].szName.GetLength() == 0)
		{
			m_aFavourites[i].szName = szTmp;
			m_aFavourites[i].szContent = szSerializedIPFeed;
			
			break;
		}
	}

	saveFavourites();

}

void COptions::deleteFavourite()
{

	DeleteFavoriteDlg dlg;
	dlg.SetFavorites(m_aFavourites);
	
	if (dlg.DoModal() == IDOK)
	{

// 		for (int i = dlg.m_nFavouriteIndex; i < 250; i++)
// 		{			
// 			m_aFavourites[i] = m_aFavourites[i+1];
// 
// 			if (m_aFavourites[i].szName.GetLength() == 0)
// 				break;
// 		}
	}

	saveFavourites();
}


void COptions::removeSettingsFromRegistry()
{
	CWinApp *app = AfxGetApp();

	if (MessageBox(app->GetMainWnd()->m_hWnd, 
		_T("So, you want to wipe traces and remove SDR Free IP Scan's settings from registry?\n\n")
		_T("Do you know that all your choosen options and settings will be lost?\n\n")
		_T("Are you sure?"), _T("Wipe Traces"), MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2) == IDYES)
	{
		// Remove settings
		RegDeleteKey(HKEY_CURRENT_USER, _T("Software\\IP_Scanner"));
		
		MessageBox(app->GetMainWnd()->m_hWnd, 
			_T("Settings are now deleted from registry.\n\n")
			_T("They will be reset next time you run SDR Free IP Scan\n")
			_T("until that you can restore settings by selecting both\n")
			_T("\"Save\" menu items in Options menu."), _T("Info"), MB_ICONINFORMATION | MB_OK);

		g_options->m_bAutoSave = FALSE;
	}
}
