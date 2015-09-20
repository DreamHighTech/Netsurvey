// Scanner.cpp: implementation of the CScanner class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Netsurvey.h"
#include "Scanner.h"
#include "ScanUtilsInternal.h"
#include "ScanUtilsPlugin.h"
#include "NetsurveyDlg.h"
#include <stdlib.h>

int g_threads[MAX_THREAD_COUNT + 1];
UINT g_nThreadCount = 0;
CDialogEx *g_dlg  = NULL;
CNetsurveyDlg *g_d = NULL; 
CScanner *g_scanner = NULL;
COptions *g_options = NULL;
CRITICAL_SECTION g_criticalSection;

#define DEFAULT_LOADED_COLUMN_COUNT	3

TScannerColumn g_BuiltInScannerColumns[] = 
{
	// IP is always 0!!
	{
		/*pScanFunction*/ &ScanIntDoDummy,
		/*pInfoFunction*/ &ScanIntInfoDummy,
		/*pSetuFunction*/ NULL,
		/*pInitFunction*/ NULL,
		/*pFinaFunction*/ NULL,
		/*bBuiltinColum*/ TRUE
	},
	// Ping is always 1!!
	{
		/*pScanFunction*/ &ScanIntDoPing, 
		/*pInfoFunction*/ &ScanIntInfoPing,
		/*pSetuFunction*/ NULL,
		/*pInitFunction*/ &ScanIntInitPing,
		/*pFinaFunction*/ NULL,
		/*bBuiltinColum*/ TRUE
	},
	// Hostname
	{
		/*pScanFunction*/ &ScanIntDoHostname,
		/*pInfoFunction*/ &ScanIntInfoHostname,
		/*pSetuFunction*/ NULL,
		/*pInitFunction*/ NULL,
		/*pFinaFunction*/ NULL,
		/*bBuiltinColum*/ TRUE
	},
	// Http connect
	{
		/*pScanFunction*/ &ScanIntDoHttpConnect,
		/*pInfoFunction*/ &ScanIntInfoHttpConnect,
		/*pSetuFunction*/ NULL,
		/*pInitFunction*/ NULL,
		/*pFinaFunction*/ NULL,
		/*bBuiltinColum*/ TRUE
	},
	// Ftp connect
	{
		/*pScanFunction*/ &ScanIntDoFtpConnect,
		/*pInfoFunction*/ &ScanIntInfoFtpConnect,
		/*pSetuFunction*/ NULL,
		/*pInitFunction*/ NULL,
		/*pFinaFunction*/ NULL,
		/*bBuiltinColum*/ TRUE
	},
	// Smtp connect
	{
		/*pScanFunction*/ &ScanIntDoSmtpConnect,
		/*pInfoFunction*/ &ScanIntInfoSmtpConnect,
		/*pSetuFunction*/ NULL,
		/*pInitFunction*/ NULL,
		/*pFinaFunction*/ NULL,
		/*bBuiltinColum*/ TRUE
	},
	// Pop2 connect
	{
		/*pScanFunction*/ &ScanIntDoPop2Connect,
		/*pInfoFunction*/ &ScanIntInfoPop2Connect,
		/*pSetuFunction*/ NULL,
		/*pInitFunction*/ NULL,
		/*pFinaFunction*/ NULL,
		/*bBuiltinColum*/ TRUE
	},
	// Pop3 connect
	{
		/*pScanFunction*/ &ScanIntDoPop3Connect,
		/*pInfoFunction*/ &ScanIntInfoPop3Connect,
		/*pSetuFunction*/ NULL,
		/*pInitFunction*/ NULL,
		/*pFinaFunction*/ NULL,
		/*bBuiltinColum*/ TRUE
	},
	// Imap connect
	{
		/*pScanFunction*/ &ScanIntDoImapConnect,
		/*pInfoFunction*/ &ScanIntInfoImapConnect,
		/*pSetuFunction*/ NULL,
		/*pInitFunction*/ NULL,
		/*pFinaFunction*/ NULL,
		/*bBuiltinColum*/ TRUE
	},
	// Finger connect
	{
		/*pScanFunction*/ &ScanIntDoFingerConnect,
		/*pInfoFunction*/ &ScanIntInfoFingerConnect,
		/*pSetuFunction*/ NULL,
		/*pInitFunction*/ NULL,
		/*pFinaFunction*/ NULL,
		/*bBuiltinColum*/ TRUE
	},
#if 0
	// NetBIOS Computer Name
	{
		/*pScanFunction*/ &ScanIntDoNetBIOSComputerName,
		/*pInfoFunction*/ &ScanIntInfoNetBIOSComputerName,
		/*pSetuFunction*/ NULL,
		/*pInitFunction*/ NULL,
		/*pFinaFunction*/ NULL,
		/*bBuiltinColum*/ TRUE
	},
	// NetBIOS Group Name
	{
		/*pScanFunction*/ &ScanIntDoNetBIOSGroupName,
		/*pInfoFunction*/ &ScanIntInfoNetBIOSGroupName,
		/*pSetuFunction*/ NULL,
		/*pInitFunction*/ NULL,
		/*pFinaFunction*/ NULL,
		/*bBuiltinColum*/ TRUE
	},
	// NetBIOS User Name
	{
		/*pScanFunction*/ &ScanIntDoNetBIOSUserName,
		/*pInfoFunction*/ &ScanIntInfoNetBIOSUserName,
		/*pSetuFunction*/ NULL,
		/*pInitFunction*/ NULL,
		/*pFinaFunction*/ NULL,
		/*bBuiltinColum*/ TRUE
	},
	// Mac Address
	{
		/*pScanFunction*/ &ScanIntDoNetBIOSMacAddress,
		/*pInfoFunction*/ &ScanIntInfoNetBIOSMacAddress,
		/*pSetuFunction*/ NULL,
		/*pInitFunction*/ NULL,
		/*pFinaFunction*/ NULL,
		/*bBuiltinColum*/ TRUE
	},
	// TTL
	{
		/*pScanFunction*/ &ScanIntDoTTL, 
		/*pInfoFunction*/ &ScanIntInfoTTL,
		/*pSetuFunction*/ NULL,
		/*pInitFunction*/ NULL,
		/*pFinaFunction*/ NULL,
		/*bBuiltinColum*/ TRUE
	}
#endif
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScanner::CScanner()
{
	m_app = AfxGetApp();	
	loadAllPossibleColumns();	
	loadSelectedColumns();
}

void CScanner::loadAllPossibleColumns()
{
	#ifdef DEBUG_MESSAGES
		AfxMessageBox("CScanner::loadAllPossibleColumns(): started ", 0, 0);
	#endif

	m_nAllColumns = sizeof(g_BuiltInScannerColumns) / sizeof(TScannerColumn);	

	m_AllColumns.SetSize(m_nAllColumns + 10, 10);
		
	// Load all possible columns
	for (int i=0; i < m_nAllColumns; i++)
	{
		m_AllColumns[i] = g_BuiltInScannerColumns[i];
	}

	// Load plugins
	CScanUtilsPlugin cPlugins;
	cPlugins.load(m_AllColumns, m_nAllColumns);

	TInfoStruct infoStruct;

	m_AllColumns[0].pszPluginName = new CString("IP");
	
	// Get names of all plugins/columns
	for (int i=1; i < m_nAllColumns; i++)
	{
		// Initialize infoStruct
		memset(&infoStruct, 0, sizeof(infoStruct));
		infoStruct.nStructSize = sizeof(infoStruct);
		infoStruct.nUniqueIndex = 0;	// TODO: true index must be set here

		m_AllColumns[i].pInfoFunction(&infoStruct);
		
		m_AllColumns[i].pszPluginName = new CString(infoStruct.szPluginName);		
	}
}

void CScanner::loadSelectedColumns()
{
	// Load selected columns from Registry
	CString szNull = _T("");
	CString szColumns = m_app->GetProfileString(szNull, _T("Columns"), _T("!"));	// "!" means that defaults must be loaded

	// add a non-digit to the end to make parsing easier
	szColumns += " ";	
	
	// Add the following columns - they always present
	m_nColumns = 0;	

	for (int i=0; i < CL_STATIC_COUNT; i++)
	{
		m_Columns[m_nColumns] = i;
		m_nColumns++;
	}

	if (szColumns.GetAt(0) == '!')
	{
		// Load defaults
		m_nColumns = CL_STATIC_COUNT;	

		for (int i=CL_STATIC_COUNT; i < DEFAULT_LOADED_COLUMN_COUNT; i++)
		{
			m_Columns[m_nColumns] = i;
			m_nColumns++;
		}
	}
	else
	{
		// Parse the string			
		TCHAR szCurCol[6];
		int nCurColLen = 0;	
		
		for (int i=0; i < szColumns.GetLength(); i++)
		{
			TCHAR chCur = szColumns.GetAt(i);
			
			if (chCur >= '0' && chCur <= '9')
			{			
				szCurCol[nCurColLen] = chCur;
				nCurColLen++;
			}
			else
			{
				if (nCurColLen == 0)
					continue;	// skip illegal TCHARacter

				szCurCol[nCurColLen] = 0;			
				nCurColLen = 0;

				int nCurColumn = _wtoi(szCurCol);

				if (nCurColumn < m_nAllColumns && nCurColumn >= CL_STATIC_COUNT) // Add only if there is a column with this index
				{			
					m_Columns[m_nColumns] = nCurColumn;
					m_nColumns++;
				}
			}
		}
	}
}

void CScanner::saveSelectedColumns()
{
	// Save selected columns to registry
	CString szColumns, szTmp;
	CString szNull = _T("");

	for (int i=CL_STATIC_COUNT; i < m_nColumns; i++)
	{
		szTmp.Format(_T("%d "), m_Columns[i]);
		szColumns += szTmp;
	}

	m_app->WriteProfileString(szNull, _T("Columns"), szColumns);
}

CScanner::~CScanner()
{
	for (int i = 0; i < m_nAllColumns; i++)
	{
		delete m_AllColumns[i].pszPluginName;
	}
}

int CScanner::getColumnCount()
{
	return m_nColumns;
}

int CScanner::getAllColumnsCount()
{
	return m_nAllColumns;
}

BOOL CScanner::getColumnName(int nIndex, CString &szColumnHeader)
{
	szColumnHeader = *m_AllColumns[m_Columns[nIndex]].pszPluginName;
	return TRUE;
}

BOOL CScanner::getAllColumnName(int nIndex, CString &szColumnHeader)
{
	szColumnHeader = *m_AllColumns[nIndex].pszPluginName;
	return TRUE;
}

int CScanner::getColumnWidth(int nIndex)
{
	CString szName;
	CString szNull = _T("");
	getColumnName(nIndex, szName);
	szName = _T("Col_") + szName;
	
	if (nIndex == 0)
		return m_app->GetProfileInt(szNull, szName, 125);
	else
		return m_app->GetProfileInt(szNull, szName, 100);
}

int CScanner::getColumnReference(int nItemIndex)
{
	return m_Columns[nItemIndex];
}

// Shows the information of column to the User by the column index
void CScanner::showColumnInfo(int nColumn, BOOL bAllColumns/* = TRUE*/)
{
	BOOL bNoInfo = FALSE;
	TInfoStruct infoStruct;	

	if (!bAllColumns)
		nColumn = m_Columns[nColumn];

	if (g_scanner->m_AllColumns[nColumn].pInfoFunction == NULL)
	{
		bNoInfo = TRUE;
	}
	else
	{		
		// Initialize infoStruct
		memset(&infoStruct, 0, sizeof(infoStruct));
		infoStruct.nStructSize = sizeof(infoStruct);
		infoStruct.nUniqueIndex = 0;	// TODO: a true index must be set here

		m_AllColumns[nColumn].pInfoFunction(&infoStruct);

		if (infoStruct.szDescription[0] == 0)
			bNoInfo = TRUE;
	}

	if (bNoInfo)
	{
		AfxMessageBox(_T("No info about this column"), MB_ICONINFORMATION | MB_OK, 0);
	}
	else
	{
		MessageBox(*AfxGetApp()->GetMainWnd(), infoStruct.szDescription, infoStruct.szPluginName, MB_OK | MB_ICONINFORMATION);
	}
}

void CScanner::showColumnOptions(int nColumn, BOOL bAllColumns/* = TRUE*/)
{
	if (!bAllColumns)
		nColumn = m_Columns[nColumn];

	if (m_AllColumns[nColumn].pOptionsFunction == NULL) 
	{
		// This column doesn't have options
		AfxMessageBox(_T("This column doesn't have any options."), MB_ICONINFORMATION | MB_OK, 0);
		return;
	}

	g_scanner->m_AllColumns[nColumn].pOptionsFunction(*AfxGetApp()->GetMainWnd());
}

void CScanner::initListColumns(CScanList *pListCtrl)
{
	int nCol, nWidth;

	pListCtrl->DeleteAllItems();

	int nCurrentColumnCount = pListCtrl->GetHeaderCtrl()->GetItemCount();
	for (nCol=0; nCol < nCurrentColumnCount; nCol++)
	{
		pListCtrl->DeleteColumn(0);
	}		
	
	for (nCol=0; nCol < m_nColumns; nCol++) 
	{					
		nWidth = getColumnWidth(nCol);
		LPCTSTR str = *m_AllColumns[m_Columns[nCol]].pszPluginName;
		pListCtrl->InsertColumn(nCol, *m_AllColumns[m_Columns[nCol]].pszPluginName, LVCFMT_LEFT, nWidth, nCol);
		pListCtrl->Update(nCol);
	}

	pListCtrl->SetColumnUpdate();
}

void CScanner::initMenuWithColumns(CMenu *pMenu)
{
	for (int nCol=CL_STATIC_COUNT; nCol < m_nAllColumns; nCol++) 
	{							
//		pMenu->InsertMenu(nCol-CL_STATIC_COUNT, MF_BYPOSITION, ID_MENU_SHOW_CMD_001 + nCol-CL_STATIC_COUNT, *m_AllColumns[nCol].pszPluginName);		
//		pMenu->EnableMenuItem(nCol-CL_STATIC_COUNT, MF_BYPOSITION | MF_ENABLED);
	}
}

void CScanner::runInitFunction(int nIndex, BOOL bAllFunctions)
{
	if (!bAllFunctions)
		nIndex = m_Columns[nIndex];

	if (m_AllColumns[nIndex].pInitFunction != NULL)
			m_AllColumns[nIndex].pInitFunction();
}

void CScanner::runFinalizeFunction(int nIndex, BOOL bAllFunctions)
{
	if (!bAllFunctions)
		nIndex = m_Columns[nIndex];

	if (m_AllColumns[nIndex].pFinalizeFunction != NULL)
			m_AllColumns[nIndex].pFinalizeFunction();
}

BOOL CScanner::initScanning()
{
	// Initialize all scanners
	for (int i=0; i < m_nColumns; i++)
	{
		runInitFunction(i);
	}

	m_nAliveHosts = 0;
	m_nOpenPorts = 0;	

	// Initialize the global dialog pointer
	g_d = (CNetsurveyDlg *) g_dlg;	

	// Initialize the critical section (used for synchronization of threads)
	InitializeCriticalSection(&g_criticalSection);	

	return TRUE;
}

BOOL CScanner::finalizeScanning()
{	
	for (int i=0; i < m_nColumns; i++)
	{
		runFinalizeFunction(i);
	}

	// Delete the critical section (used for synchronization of threads)
	DeleteCriticalSection(&g_criticalSection);	

	return TRUE;
}

#define WSA_VERSION		MAKEWORD(2,2)

BOOL CScanner::doScanIP(DWORD nParam, BOOL bParameterIsIP, int nThreadIndex)
{
	DWORD nItemIndex;
	DWORD nIP;
	CString szOpenPorts;			// Open ports string will be saved here
	BOOL bSomePortsOpen = FALSE;	// Will be TRUE if some ports are open

	// Known callers:
	// RescanIP() calls this function by item index, but
	// Thread is called by the IP address
	if (bParameterIsIP)
	{
		nIP = nParam;				// IP is passed as parameter
		nItemIndex = 0;				// Item index will be updated later
		nIP = htonl(nIP);			// Convert an IP to Network Byte Order
	}
	else
	{
		nItemIndex = nParam;					// ItemIndex is passed as parameter
		TCHAR szIP[16];
		g_d->m_nIpList.GetItemText(nItemIndex, CL_IP, (TCHAR*) &szIP, sizeof(szIP));
		char cstr[512]; memset(cstr, 0x0, sizeof(cstr));
		wcstombs(cstr, szIP, sizeof(szIP));
		nIP = inet_addr((char*)&cstr);
	}

	if (g_threads[nThreadIndex] == THREAD_MUST_DIE)	// Program asked to die ------------------------------------------------
		return FALSE;

	// At this place, the nIP is known, so let's initialize some stuff for inserting items
	in_addr structInAddr;
	structInAddr.S_un.S_addr = nIP;
	char *schr = inet_ntoa(structInAddr);
	TCHAR szIP[128];
	memset(szIP, 0x0, sizeof(szIP));
	mbstowcs(szIP, schr, 15);

	if (bParameterIsIP && g_options->m_neDisplayOptions == DISPLAY_ALL)
	{
		nItemIndex = g_d->m_nIpList.InsertItem(MAXINT, szIP, 2);
	}

	TCHAR szTmp[512];	// Temporary string. Scanning functions will return data using it.

	// Ping it! (column number 1), Check if it is alive
	char cstr[512]; memset(cstr, 0x0, sizeof(cstr));
	int nPingTime = m_AllColumns[CL_PING].pScanFunction(nIP, (char*) &cstr, sizeof(cstr));
	memset(szTmp, 0x0, sizeof(szIP));
	mbstowcs(szTmp, cstr, sizeof(cstr));
	BOOL bAlive = nPingTime >= 0;	// Negative value means "Dead"	

	if (g_threads[nThreadIndex] == THREAD_MUST_DIE)	// Program asked to die -------------------------------------
		return FALSE;
	
	if (bAlive)	// This If is needed to insert an item or finish scanning this IP, other processing will follow
	{
		if (bParameterIsIP && g_options->m_neDisplayOptions == DISPLAY_ALIVE)
		{
			// Insert an item if it is not RescanIP() who called us and scanning mode is DISPLAY_ALIVE			
			nItemIndex = g_d->m_nIpList.InsertItem(MAXINT, szIP, 2);
		}
	
		// Increment number of alive hosts
		m_nAliveHosts++;
	}
	else
	{
		if (g_options->m_neDisplayOptions != DISPLAY_ALL)
		{
			// If was chosen to display only alive IPs or only open ports, then we shouldn't do anything further
			return TRUE;
		}
	}

	if (g_threads[nThreadIndex] == THREAD_MUST_DIE)	// Program asked to die ------------------------------------------------
		return FALSE;

	if (bAlive)	// Update the item according to alive/dead status
	{
		// Change image to Alive
		g_d->m_nIpList.SetItem(nItemIndex, CL_IP, LVIF_IMAGE, NULL, 0, 0, 0, 0);
	}
	else
	{
		// Change image to Dead
		g_d->m_nIpList.SetItem(nItemIndex, CL_IP, LVIF_IMAGE, NULL, 1, 0, 0, 0);
	}

	// Set item text returned from pinging (Dead or X ms)
	g_d->m_nIpList.SetItemText(nItemIndex, CL_PING, (TCHAR*) &szTmp);
	
	// Scan other columns if scanning of dead hosts is enabled or host is Alive
	bool bScan = g_options->m_bScanHostIfDead || bAlive; 

	/*Tcp Connect*/
#ifdef _WINDOWS
	// init socket
	bool network_init = true;
	WSADATA		WSAData = { 0 };
	if (0 != WSAStartup(WSA_VERSION, &WSAData))
	{
		WSACleanup();
		network_init = false;
	}
#endif

	// Run other scans (besides ping and port scanning)
	for (int i = CL_HOSTNAME; i < m_nColumns; i++)
	{
		if (bScan)
		{
			if (m_AllColumns[m_Columns[i]].pInfoFunction != NULL)
			{
				szTmp[0] = 0;

				runScanFunction(nIP, i, (TCHAR*) &szTmp, sizeof(szTmp));
				
				// Returned an empty string
				if (szTmp[0] == 0)
					wcscpy((TCHAR*)&szTmp, g_options->m_szNotAvailableValue);
			}
			else
			{
				wcscpy((TCHAR*)&szTmp, _T("ERR!"));
			}
		}
		else
		{
			// Dead host, not scanned
			wcscpy((TCHAR*) &szTmp, g_options->m_szNotScannedValue);
		}

		if (g_threads[nThreadIndex] == THREAD_MUST_DIE)	// Program asked to die ----------------------------------------------------
			return FALSE;
		
		// Update the list with scanned info
		g_d->m_nIpList.SetItemText(nItemIndex, i, (TCHAR*) &szTmp);
	}

#ifdef _WINDOWS
	if (network_init == true) WSACleanup();
#endif

	return TRUE;
}

BOOL CScanner::runScanFunction(DWORD nIP, int nIndex, TCHAR *szBuffer, int nBufferLength, BOOL bGlobal /*=FALSE*/)
{
	TScanFunction *pScanFunction;

	if (bGlobal)
		pScanFunction = m_AllColumns[nIndex].pScanFunction;
	else
		pScanFunction = m_AllColumns[m_Columns[nIndex]].pScanFunction;

	char cstr[512]; memset(cstr, 0x0, sizeof(cstr));
	wcstombs(cstr, szBuffer, sizeof(szBuffer));

	if (pScanFunction(nIP, cstr, nBufferLength)) {
		mbstowcs(szBuffer, cstr, sizeof(cstr));
		return TRUE;
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////
//////////////////////////// THREAD ////////////////////////////////////
////////////////////////////////////////////////////////////////////////


UINT ThreadProcCallback(LPVOID nIP)
{	
	return ScanningThread((DWORD)nIP, IP_IS_GIVEN);
}

UINT ThreadProcCallbackRescan(LPVOID nItemIndex)
{	
	return ScanningThread((DWORD)nItemIndex, INDEX_IS_GIVEN);
}

UINT ScanningThread(DWORD nParam, BOOL bParameterIsIP)
{
	// Initialize thread //////////////////////////////////////////////////////	

	CString szTmp;	
		
	// Put thread's activity flag into global array (and find it's index)	
	int nThreadIndex;

	EnterCriticalSection(&g_criticalSection);	//////// BEGIN SYNCRONIZATION ////////////////////

	//g_nThreadCount++; This is incremented right before calling the thread

	for (nThreadIndex=0; nThreadIndex < MAX_THREAD_COUNT; nThreadIndex++) 
	{
		if (g_threads[nThreadIndex] == THREAD_DEAD) 
		{ 			
			g_threads[nThreadIndex] = THREAD_ALIVE;	// Thread is running
			break; 
		}
	}	

	// Display current number of threads
	szTmp.Format(_T("Threads : %d"), g_nThreadCount);
	g_d->m_nThread.SetWindowText(szTmp);

	LeaveCriticalSection(&g_criticalSection);	//////// END SYNCRONIZATION ////////////////////

	/////////////////////////////////////////////////////////////////////////////
	// Process scan /////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	// check if this IP is not broadcasting IP 
	// (just a guess right now, but in general, most X.X.X.0 and X.X.X.255 IPs are broadcasting)

	BOOL bProceed = TRUE;

	if (bParameterIsIP && g_options->m_bSkipBroadcast)
	{
		if (((nParam & 0xFF) == 0xFF) || ((nParam & 0xFF) == 0x00))
		{
			bProceed = FALSE;
		}
	}
	
	if (bProceed)
		g_scanner->doScanIP(nParam, bParameterIsIP, nThreadIndex);
	
	/////////////////////////////////////////////////////////////////////////////
	// Shutdown thread //////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	EnterCriticalSection(&g_criticalSection);	//////// BEGIN SYNCRONIZATION ////////////////////
	
	// Remove thread's handle	
	if (g_nThreadCount >=0) 
	{
//		g_d->SetDlgItemInt(IDC_NUMTHREADS, g_nThreadCount, FALSE);
	}	

	g_threads[nThreadIndex] = THREAD_DEAD;	// Thread is dead now

	if (g_nThreadCount > 0)	// For safety
		g_nThreadCount--;

	// Display current number of threads
	szTmp.Format(_T("Threads : %d"), g_nThreadCount);
	g_d->m_nThread.SetWindowText(szTmp);

	LeaveCriticalSection(&g_criticalSection);	//////// END SYNCRONIZATION ////////////////////
	
	return 0;

}

////////////////////////////////////////////////////////////////////////
//////////////////////////// THREAD ////////////////////////////////////
////////////////////////////////////////////////////////////////////////




