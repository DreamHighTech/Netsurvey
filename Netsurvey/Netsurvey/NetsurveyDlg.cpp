
// NetsurveyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Netsurvey.h"
#include "NetsurveyDlg.h"
#include "afxdialogex.h"
#include "globals.h"
#include "SaveToFile.h"
#include "CommandLine.h"
#include "ScanStatisticsDlg.h"

CWinApp *app;
CNetsurveyDlg* main_dlg;
unsigned long g_nStartItemIndex;
unsigned long g_nEndItemIndex;
unsigned long g_nCurrentItemIndex;
unsigned long g_nIPsScanned;
BOOL g_bScanExistingItems = FALSE;
CAbstractIPStyle * g_pIPStyle;

#define IPRANGEDLGID 0
#define IPRANDOMDLGID 1
#define IPLISTDLGID 2

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CNetsurveyDlg dialog

CNetsurveyDlg::CNetsurveyDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CNetsurveyDlg::IDD, pParent), m_nIpList(this)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pSzDefaultFileName = NULL;
	m_nStatusHeight = 19;
}
																		
void CNetsurveyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAINLIST, m_nIpList);
	DDX_Control(pDX, IDC_IPSTYLE, m_nComboIpStyleDlg);
	DDX_Control(pDX, IDC_PROGRESS1, m_nProgressBar);
	DDX_Control(pDX, IDC_STATUS, m_nStatus);
	DDX_Control(pDX, IDC_THREAD, m_nThread);
	DDX_Control(pDX, IDC_BTN_START, m_button_start);
	DDX_Control(pDX, IDC_BTN_PREFERENCES, m_button_pref);
	DDX_Control(pDX, IDC_BTN_SELECTFETCHERS, m_button_fetcher);
}

BEGIN_MESSAGE_MAP(CNetsurveyDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_IPSTYLE, &CNetsurveyDlg::OnSelchangeIpstyle)
	ON_BN_CLICKED(IDC_BTN_START, &CNetsurveyDlg::OnBnClickedBtnStart)
	ON_COMMAND(ID_GOTO_NEXTALIVEHOST, &CNetsurveyDlg::OnGotoNextalivehost)
	ON_COMMAND(ID_GOTO_NEXTDEADHOST, &CNetsurveyDlg::OnGotoNextdeadhost)
	ON_COMMAND(ID_GOTO_PREVIOUSALIVEHOST, &CNetsurveyDlg::OnGotoPreviousalivehost)
	ON_COMMAND(ID_GOTO_PREVIOUSDEADHOST, &CNetsurveyDlg::OnGotoPreviousdeadhost)
	ON_COMMAND(ID_GOTO_FIND, &CNetsurveyDlg::OnGotoFind)
	ON_COMMAND(ID_FAVORITES_ADDCURRENT, &CNetsurveyDlg::OnFavoritesAddcurrent)
	ON_COMMAND(ID_FAVOURITES_DELETEFAVOURITE, &CNetsurveyDlg::OnFavouritesDeletefavourite)
	ON_COMMAND(ID_SCAN_QUIT, &CNetsurveyDlg::OnScanQuit)
	ON_COMMAND(ID_COMMANDS_SHOWDETAILS, &CNetsurveyDlg::OnCommandsShowdetails)
	ON_COMMAND(ID_SCAN_EXPORTALL, &CNetsurveyDlg::OnScanExportall)
	ON_COMMAND(ID_SCAN_EXPORTSELECTION, &CNetsurveyDlg::OnScanExportselection)
	ON_COMMAND(ID_COMMANDS_RESCANIP, &CNetsurveyDlg::OnCommandsRescanip)
	ON_COMMAND(ID_COMMANDS_DELETEIP, &CNetsurveyDlg::OnCommandsDeleteip)
	ON_COMMAND(ID_COMMANDS_COPYIP, &CNetsurveyDlg::OnCommandsCopyip)
	ON_COMMAND(ID_COMMANDS_COPYDETAILS, &CNetsurveyDlg::OnCommandsCopydetails)
	ON_COMMAND(ID_TOOLS_PREFERENCES, &CNetsurveyDlg::OnToolsPreferences)
	ON_COMMAND(ID_TOOLS_FETCHERS, &CNetsurveyDlg::OnToolsFetchers)
	ON_COMMAND(ID_HELP_ABOUT, &CNetsurveyDlg::OnHelpAbout)
	ON_COMMAND_RANGE(ID_MENU_FAVOURITES_001, ID_MENU_FAVOURITES_249, OnExecuteFavouritesMenu)
	ON_BN_CLICKED(IDC_BTN_PREFERENCES, &CNetsurveyDlg::OnBnClickedBtnPreferences)
	ON_BN_CLICKED(IDC_BTN_SELECTFETCHERS, &CNetsurveyDlg::OnBnClickedBtnSelectfetchers)
END_MESSAGE_MAP()


// CNetsurveyDlg message handlers

BOOL CNetsurveyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	app = AfxGetApp();
	g_dlg = main_dlg = (CNetsurveyDlg*)app->m_pMainWnd;

	// Load default options
	g_options = new COptions();
	g_options->load();
	g_options->setWindowPos();

	g_scanner = new CScanner();	

	// Add columns to the list control
	g_scanner->initListColumns(&m_nIpList);

	m_pIPStyleFactory = new CIPStyleDlgFactory();
	m_pIPStyleDlg = m_pIPStyleFactory->getIPFeed(0);	
	m_pPreferenceDlg = new CPreferencesDlg;
	m_pSelectFetcherDlg = new CSelectFetcherDlg;

	for (int i = 0; i < m_pIPStyleFactory->getIPFeedCount(); i++)
	{		
		m_nComboIpStyleDlg.AddString(m_pIPStyleFactory->getIPFeedName(i));
	}

	m_nComboIpStyleDlg.SetCurSel(0);
	OnSelchangeIpstyle();
	m_nScanMode = SCAN_MODE_NOT_SCANNING;

	m_hAccel = LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINMENU));

	RECT rc; m_nProgressBar.GetWindowRect(&rc); m_nStatusHeight = rc.bottom-rc.top-2;

	SetRefreshFavouritesMenu();
	m_nIpList.InitPostCreateStuff();
	m_nIpList.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);

	m_button_start.SetIcon((HICON)::LoadImage(AfxGetInstanceHandle(),
								MAKEINTRESOURCE(IDI_ICON_START),
								IMAGE_ICON, 0, 0, 0));

	m_button_pref.SetIcon((HICON)::LoadImage(AfxGetInstanceHandle(),
								MAKEINTRESOURCE(IDI_ICON_PREF),
								IMAGE_ICON, 0, 0, 0));
	
	m_button_fetcher.SetIcon((HICON)::LoadImage(AfxGetInstanceHandle(),
								MAKEINTRESOURCE(IDI_ICON_FETCHER),
								IMAGE_ICON, 0, 0, 0));

	return TRUE;
}

void CNetsurveyDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNetsurveyDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNetsurveyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CNetsurveyDlg::OnSelchangeIpstyle()
{
	int nCurrentFeed = m_nComboIpStyleDlg.GetCurSel();

	m_pIPStyleDlg->ShowWindow(SW_HIDE);	
	m_pIPStyleDlg = m_pIPStyleFactory->getIPFeed(nCurrentFeed);
	m_pIPStyleDlg->ShowWindow(SW_SHOW);
	m_pIPStyleDlg->SetFocus();
}

void CNetsurveyDlg::SelectIPStyleByType(const CString &szType)
{
	int nIndex = m_pIPStyleFactory->getIndexByType(szType);

	if (nIndex < 0)
		AfxMessageBox(_T("Unknown IP Feed Type: ") + szType);
	else
	{
		m_nComboIpStyleDlg.SetCurSel(nIndex);
		OnSelchangeIpstyle();
	}
}

void CNetsurveyDlg::OnBnClickedBtnStart()
{
	SetScanStart();
}

void CNetsurveyDlg::SetScanStart()
{
	if (m_nScanMode == SCAN_MODE_NOT_SCANNING) 
	{
		if (!g_bScanExistingItems)
		{
			if ((m_nIpList.GetItemCount() > 0) && (g_options->m_bAskConfirm))
			{
				if (MessageBox(_T("Discard previous scanning results?"), NULL, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDNO)
				{
					return;
				}
			}

			m_nIpList.DeleteAllItems();

			SetRecreateIPFeed();

			if (!g_pIPStyle)
				return;	// An error occured - do nothing further

			g_pIPStyle->startFeeding();

		}
		else
		{
			// g_nStartItemIndex is set outside before calling this method
			g_nCurrentItemIndex = g_nStartItemIndex;
		}

		m_nProgressBar.SetRange(0,100);
		m_nProgressBar.SetPos(0);
		m_nTickCount = GetTickCount()/1000;

		m_nScanMode = SCAN_MODE_SCANNING;
//		m_nIpList.SetHeaderClicksDisabled(TRUE);
	
		((CButton*)GetDlgItem(IDC_BTN_START))->SetWindowTextW(_T("Stop"));
		m_button_start.SetIcon((HICON)::LoadImage(AfxGetInstanceHandle(),
								MAKEINTRESOURCE(IDI_ICON_STOP),
								IMAGE_ICON, 0, 0, 0));
		m_button_pref.EnableWindow(FALSE);
		m_button_fetcher.EnableWindow(FALSE);
		m_nComboIpStyleDlg.EnableWindow(FALSE);

		g_nThreadCount = 0;
		g_nIPsScanned = 0;
		m_bScanningAborted = FALSE;

		SetEnableMenuItems(FALSE);

		// Initialize scanning engine
		SetStatus(_T("Initializing..."));
		g_scanner->initScanning();

		SetTimer(1, g_options->m_nTimerDelay, NULL);

	} 
	else // m_nScanMode is SCAN_MODE_SCANNING or SCAN_MODE_FINISHING or SCAN_MODE_KILLING)
	{

		if (g_nThreadCount > 0) 
		{

			if (m_nScanMode == SCAN_MODE_FINISHING) 
			{
				//if (MessageBox(_T("Are you sure you want to interrupt scanning by killing all the threads?\nScanning results will be incomplete."),NULL,MB_YESNO | MB_ICONQUESTION) == IDNO) 
				//	return;

				// Kill threads
				SetStatus(_T("Wait for all thread terminate..."));
				SetKillAllRunningThreads();	

				//Sleep(1000);	// sleep 1 second and give some threads a chance to exit
				//status(NULL);

				m_nScanMode = SCAN_MODE_KILLING;				
			}
			else
				if (m_nScanMode == SCAN_MODE_KILLING)
				{
					MessageBox(_T("Please wait while killing active threads..."), NULL, MB_OK | MB_ICONINFORMATION);
				}
				else // SCAN_MODE_SCANNING
				{
					// Stop scanning (but wait for existing threads)

					m_bScanningAborted = TRUE;

					m_nProgressBar.SetPos(100);				

					SetStatus(_T("Wait for all threads to terminate..."));
					//((CButton*)GetDlgItem(IDC_BUTTON1))->SetBitmap((HBITMAP)m_bmpKill.m_hObject);
					((CButton*)GetDlgItem(IDC_BTN_START))->SetWindowTextW(_T("Stop!"));
					m_button_start.SetIcon((HICON)::LoadImage(AfxGetInstanceHandle(),
								MAKEINTRESOURCE(IDI_ICON_KILL),
								IMAGE_ICON, 0, 0, 0));
					m_nScanMode = SCAN_MODE_FINISHING;  // waiting can be interrupted
				}
		} 
		else // g_nThreadCount = 0
		{
			KillTimer(1);

			// Finalize IP feeder
			g_pIPStyle->finishFeeding();

			BOOL bShowScanInfo = (m_nScanMode != SCAN_MODE_KILLING) && !g_bScanExistingItems;

			m_nScanMode = SCAN_MODE_NOT_SCANNING;
//			m_nIpList.SetHeaderClicksDisabled(FALSE);

			SetStatus(_T("Finalizing..."));
			g_scanner->finalizeScanning();			

			//((CButton*)GetDlgItem(IDC_BUTTON1))->SetBitmap((HBITMAP)m_bmpStart.m_hObject); // start scan bitmap			
			((CButton*)GetDlgItem(IDC_BTN_START))->SetWindowTextW(_T("Start"));
			m_button_start.SetIcon((HICON)::LoadImage(AfxGetInstanceHandle(),
								MAKEINTRESOURCE(IDI_ICON_START),
								IMAGE_ICON, 0, 0, 0));

			m_button_pref.EnableWindow(TRUE);
			m_button_fetcher.EnableWindow(TRUE);
			m_nComboIpStyleDlg.EnableWindow(TRUE);

			SetEnableMenuItems(TRUE);			

			m_nProgressBar.SetPos(0);

			SetStatus(NULL);	// Ready

			if (m_pSzDefaultFileName)
			{
				// Program was invoked via command-line, so save data to file & exit

				CSaveToFile tmp(main_dlg, 0, 0, FALSE, m_pSzDefaultFileName->GetBuffer(255), m_nCmdLineFileFormat, m_nCmdLineOptions & CMDO_APPEND_FILE);

				if (!(m_nCmdLineOptions & CMDO_NOT_EXIT))
					ExitProcess(0);
			}
			else
			{
				// Display final message box with statistics				

				int nTotalTime = GetTickCount() / 1000 - m_nTickCount + 1;
				float fTimeForOneIP = (float) nTotalTime / g_nIPsScanned;

				CString szIPFeedInfo = g_pIPStyle->getScanSummary();

				m_pSszCompleteInformation.Format(
					_T("%u sec,  %.3f sec/host\r\n\r\n")
					_T("%s\r\n\r\n")						
					_T("IPs scanned:\t%u\r\n")
					_T("Alive hosts:\t%u\r\n"),
					nTotalTime, fTimeForOneIP, szIPFeedInfo, g_nIPsScanned, g_scanner->m_nAliveHosts);

				if (bShowScanInfo)
				{
					// Get attention of the user					
					FlashWindow(TRUE);
					//PlaySound("SystemExclamation", NULL, SND_ALIAS | SND_ASYNC);
					// Show info
					if (g_options->m_bShowInfo)
						ShowCompleteInformation();					
				}
			}
			g_bScanExistingItems = FALSE;	// Reset this stuff
		}
	}
}

void CNetsurveyDlg::SetRecreateIPFeed()
{
	if (g_pIPStyle)
	{
		delete g_pIPStyle;
		g_pIPStyle = NULL;
	}

	g_pIPStyle = m_pIPStyleDlg->createIPFeed();
}

void CNetsurveyDlg::SetEnableMenuItems(BOOL bEnable)
{
	UINT nEnable = bEnable ? MF_ENABLED : MF_GRAYED;

	CMenu *tmpMnu = GetMenu();

/*	tmpMnu->EnableMenuItem(ID_OPTIONS_OPTIONS, nEnable);
	tmpMnu->EnableMenuItem(ID_OPTIONS_SELECT_COLUMNS, nEnable);
	tmpMnu->EnableMenuItem(ID_OPTIONS_SELECTPORTS, nEnable);	

	tmpMnu->EnableMenuItem(ID_SCAN_SAVETOTXT, nEnable);
	tmpMnu->EnableMenuItem(ID_SCAN_SAVESELECTION, nEnable);

	tmpMnu->EnableMenuItem(ID_UTILS_DELETEFROMLIST_DEADHOSTS, nEnable);
	tmpMnu->EnableMenuItem(ID_UTILS_DELETEFROMLIST_ALIVEHOSTS, nEnable);
	tmpMnu->EnableMenuItem(ID_UTILS_DELETEFROMLIST_CLOSEDPORTS, nEnable);
	tmpMnu->EnableMenuItem(ID_UTILS_DELETEFROMLIST_OPENPORTS, nEnable);

	tmpMnu->EnableMenuItem(ID_OPTIONS_INSTALL_PROGRAM, nEnable);

	tmpMnu->EnableMenuItem(ID_COMMANDS_RESCANIP, nEnable);	
	tmpMnu->EnableMenuItem(ID_COMMANDS_DELETEIP, nEnable);	

	tmpMnu->EnableMenuItem(ID_SHOWNETBIOSINFO, nEnable);

	for (UINT i=0; i < tmpMnu->GetMenuItemCount(); i++)
	{
		tmpMnu->EnableMenuItem(ID_MENU_SHOW_CMD_001 + i, nEnable);
	}

	GetDlgItem(IDC_SELECT_COLUMNS)->EnableWindow(bEnable);
	GetDlgItem(IDC_SELECT_PORTS)->EnableWindow(bEnable);*/
}

void CNetsurveyDlg::SetStatus(LPCWSTR str) 
{
	if (str != NULL)
		SetDlgItemText(IDC_STATUS, str);
	else
		SetDlgItemText(IDC_STATUS, _T("Ready"));
}

void CNetsurveyDlg::SetKillAllRunningThreads()
{	
	//EnterCriticalSection(&g_criticalSection);				
	for (UINT i=0; i < sizeof(g_threads)/sizeof(g_threads[0]); i++) 
	{
		if (g_threads[i] != THREAD_DEAD) 
		{			
			//TerminateThread(g_threads[i], 0);
			//CloseHandle(g_threads[i]);

			// Tell the thread that it must commit a suicide
			g_threads[i] = THREAD_MUST_DIE;
		}
	}
	//LeaveCriticalSection(&g_criticalSection);
}

void CNetsurveyDlg::ShowCompleteInformation()
{
	// Show "Scanning complete" information box
	if (m_pSszCompleteInformation.GetLength() == 0)
		m_pSszCompleteInformation = "No last scan information yet.";

	CScanStatisticsDlg cMsgDlg;
	cMsgDlg.setMessageText(m_pSszCompleteInformation);
	cMsgDlg.DoModal();
}


void CNetsurveyDlg::OnTimer(UINT_PTR nIDEvent)
{
	int nItemIndex = 0;

	BOOL bCurrentLessThanEnd;

	if (g_bScanExistingItems)	
		bCurrentLessThanEnd = g_nCurrentItemIndex < g_nEndItemIndex;
	else
		bCurrentLessThanEnd = g_pIPStyle->isNextIPAvailable();

	if (m_bScanningAborted)
		bCurrentLessThanEnd = FALSE;	// Stop unfinished scanning

	if (bCurrentLessThanEnd) 
	{
		if ((int) g_nThreadCount >= g_options->m_nMaxThreads - 1) 
			return;

		IPAddress nCurrentIP = g_pIPStyle->getNextIP();

		in_addr in;
		TCHAR szIP[512];
		char * csxIP;
		in.S_un.S_addr = htonl(nCurrentIP);
		csxIP = inet_ntoa(in);
		memset(szIP, 0x0, sizeof(szIP));
		mbstowcs(szIP, csxIP, 15);
		SetStatus(szIP);		

		// This is decremented by each thread on exit
		g_nThreadCount++;	

		// Increase counter
		g_nIPsScanned++;

		if (g_bScanExistingItems)
		{
			AfxBeginThread(ThreadProcCallbackRescan, (LPVOID) g_nCurrentItemIndex);	
			g_nCurrentItemIndex++;

			if (g_nEndItemIndex != g_nStartItemIndex)	// To prevent division by 0 below
			{			
				m_nProgressBar.SetPos((g_nCurrentItemIndex - g_nStartItemIndex) * 100 / (g_nEndItemIndex - g_nStartItemIndex));
			}
		}
		else
		{
			AfxBeginThread(ThreadProcCallback, (LPVOID) nCurrentIP);	// Pass IP in Host byte order			

			m_nProgressBar.SetPos(g_pIPStyle->getPercentComplete());			
		}			

	} 
	else 
	{
		if (g_nThreadCount <= 0) 
		{
			if (g_bScanExistingItems)
				g_nEndItemIndex--;

			SetScanStart();	// Change current state (scan mode)
		} 
		else 
		{
			if (m_nScanMode == SCAN_MODE_SCANNING)
			{
				SetScanStart();	// Change current state (scan mode)				
			}
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CNetsurveyDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_nIpList.m_hWnd != NULL) 
	{
		if (m_nStatusHeight > 0)
		{
			m_nListOffset = 70;
			m_nIpList.MoveWindow(0, m_nListOffset, cx, cy-m_nListOffset-(m_nStatusHeight+2), TRUE);
			m_nStatus.MoveWindow(0, cy-m_nStatusHeight/*18*/, cx/3, /*18*/m_nStatusHeight, TRUE);
			m_nThread.MoveWindow(cx/3+1, cy-m_nStatusHeight/*18*/, cx/3-1, /*18*/m_nStatusHeight, TRUE);
			m_nProgressBar.MoveWindow(cx/3*2+1,cy-m_nStatusHeight,cx/3*2-1,m_nStatusHeight,TRUE);
		}
	}
}


void CNetsurveyDlg::OnDestroy()
{
	OnOptionsSavedimensions();

	CDialogEx::OnDestroy();

	if (g_options != NULL)
		delete(g_options);

	if (g_scanner != NULL)
		delete(g_scanner);

	if (g_pIPStyle != NULL)
		delete(g_pIPStyle);

	if (m_pSzDefaultFileName != NULL)
		delete(m_pSzDefaultFileName);
	
	if (m_pIPStyleFactory != NULL)
		delete (m_pIPStyleFactory);

	if (m_pPreferenceDlg != NULL)
		delete (m_pPreferenceDlg);

	if (m_pSelectFetcherDlg != NULL)
		delete (m_pSelectFetcherDlg);
}

void CNetsurveyDlg::OnGotoNextalivehost()
{
	m_nIpList.GoToNextAliveIP();
}


void CNetsurveyDlg::OnGotoNextdeadhost()
{
	m_nIpList.GoToNextDeadIP();
}


void CNetsurveyDlg::OnGotoPreviousalivehost()
{
	m_nIpList.GoToPreviousAliveIP();
}

void CNetsurveyDlg::OnClose()
{
	CDialogEx::OnCancel();
}

void CNetsurveyDlg::OnGotoPreviousdeadhost()
{
	m_nIpList.GoToPreviousDeadIP();
}


void CNetsurveyDlg::OnGotoFind()
{
	m_nIpList.GotoFind();
}


void CNetsurveyDlg::OnFavoritesAddcurrent()
{
	// Recreate IP Feed object (in case it is old or doesn't exist)
	SetRecreateIPFeed();

	// Add only if it was created without errors
	if (g_pIPStyle)
	{
		// Pass serialized IP Feed to the adder routine
		g_options->addFavourite(m_pIPStyleDlg->serialize());

		SetRefreshFavouritesMenu();
	}
}

void CNetsurveyDlg::SetRefreshFavouritesMenu()
{
	g_options->initFavouritesMenu(GetMenu()->GetSubMenu(INDEX_FAVOURITES_MENU));
}

void CNetsurveyDlg::NotScanedMessage()
{
	MessageBox(_T("No scanning results are available, please perform a scan first"), _T("Problem"), MB_OK | MB_ICONWARNING);
}

void CNetsurveyDlg::NoIPSelectedMessage()
{
	MessageBox(_T("No IP address selected"), _T("Problem"), MB_OK | MB_ICONWARNING);
}

void CNetsurveyDlg::OnFavouritesDeletefavourite()
{
	g_options->deleteFavourite();	

	SetRefreshFavouritesMenu();
}


void CNetsurveyDlg::OnExecuteFavouritesMenu(UINT nID)
{	
	if (m_nScanMode != SCAN_MODE_NOT_SCANNING)
		return;

	int nFavouriteIndex = nID - ID_MENU_FAVOURITES_001;
	int modeRnd = g_options->m_aFavourites[nFavouriteIndex].szName.Find(_T("Random"));
	int modeRan = g_options->m_aFavourites[nFavouriteIndex].szName.Find(_T("IP Range"));
	CString param;
	if (modeRnd != -1)
	{ /* Random */
		int nCurrentFeed = IPRANDOMDLGID;
		m_pIPStyleDlg->ShowWindow(SW_HIDE);	
		m_pIPStyleDlg = m_pIPStyleFactory->getIPFeed(nCurrentFeed);
		m_pIPStyleDlg->ShowWindow(SW_SHOW);
		m_pIPStyleDlg->SetFocus();
		m_nComboIpStyleDlg.SetCurSel(IPRANDOMDLGID);
		CString str = g_options->m_aFavourites[nFavouriteIndex].szName;
		int nDelimeterPos = str.Find(_T(':'));
		str = str.Mid(nDelimeterPos + 2);
		nDelimeterPos = str.Find(_T(':'));
		CString szIP = str.Left(nDelimeterPos);
		param = g_options->m_aFavourites[nFavouriteIndex].szContent + _T('|') + szIP;
	}
	else if (modeRan != -1)
	{ /* Range */
		int nCurrentFeed = IPRANGEDLGID;
		m_pIPStyleDlg->ShowWindow(SW_HIDE);	
		m_pIPStyleDlg = m_pIPStyleFactory->getIPFeed(nCurrentFeed);
		m_pIPStyleDlg->ShowWindow(SW_SHOW);
		m_pIPStyleDlg->SetFocus();
		m_nComboIpStyleDlg.SetCurSel(IPRANGEDLGID);
		param = g_options->m_aFavourites[nFavouriteIndex].szName;
	} else
	{ /* List */
		int nCurrentFeed = IPLISTDLGID;
		m_pIPStyleDlg->ShowWindow(SW_HIDE);	
		m_pIPStyleDlg = m_pIPStyleFactory->getIPFeed(nCurrentFeed);
		m_pIPStyleDlg->ShowWindow(SW_SHOW);
		m_pIPStyleDlg->SetFocus();
		m_nComboIpStyleDlg.SetCurSel(IPLISTDLGID);
		param = g_options->m_aFavourites[nFavouriteIndex].szContent;
	}

	m_pIPStyleDlg->unserialize(param);	

}
void CNetsurveyDlg::OnOK()
{

}

void CNetsurveyDlg::OnCancel()
{

}

BOOL CNetsurveyDlg::PreTranslateMessage(MSG* pMsg)
{
	BOOL bDoAccelerator = TRUE;

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DELETE)
	{
		if (pMsg->hwnd != m_nIpList.m_hWnd)
			bDoAccelerator = FALSE;	// Do not let accelerator capture Del key from editboxes
	}

	if (bDoAccelerator && TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) 
		return TRUE;

	//if (m_pToolTips != NULL)
	//	m_pToolTips->RelayEvent(pMsg);

	// Check for Enter key presses
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		// If this is a list control
		if (pMsg->hwnd == m_nIpList.m_hWnd)
		{
			if (m_nIpList.GetCurrentSelectedItem(FALSE) >= 0)
			{
				OnCommandsShowdetails();
				return TRUE;
			}
		}
	}


	return CDialogEx::PreTranslateMessage(pMsg);
}


void CNetsurveyDlg::OnScanQuit()
{
	CDialogEx::OnOK();
}


void CNetsurveyDlg::OnCommandsShowdetails()
{
	m_nIpList.ShowIPDetails();	
}


void CNetsurveyDlg::OnScanExportall()
{
	CString start=0, end=0;
	int nCurrentFeed = m_nComboIpStyleDlg.GetCurSel();

	if (g_pIPStyle == NULL)
	{
		NotScanedMessage();
		return;
	}

	CString portstr = g_pIPStyle->getScanSummary();

	int nDelimeterPos = portstr.Find(_T(" - "));

	if (nDelimeterPos >= 0)
	{
		start = portstr.Left(nDelimeterPos);
		end = portstr.Mid(nDelimeterPos + 3);
	}

	CSaveToFile tmp(main_dlg, (LPWSTR)start.GetString(), (LPWSTR)end.GetString());
}

void CNetsurveyDlg::OnScanExportselection()
{
	CString start=0, end=0;
	int nCurrentFeed = m_nComboIpStyleDlg.GetCurSel();

	if (g_pIPStyle == NULL)
	{
		NotScanedMessage();
		return;
	}

	CString portstr = g_pIPStyle->getScanSummary();

	int nDelimeterPos = portstr.Find(_T(" - "));

	if (nDelimeterPos >= 0)
	{
		start = portstr.Left(nDelimeterPos);
		end = portstr.Mid(nDelimeterPos + 3);
	}

	CSaveToFile tmp(main_dlg, (LPWSTR)start.GetString(), (LPWSTR)end.GetString(), TRUE);
}


void CNetsurveyDlg::OnCommandsRescanip()
{
	if (m_nScanMode != SCAN_MODE_NOT_SCANNING)
	{
		return;
	}

	if (m_nIpList.GetItemCount() == 0)
	{
		NotScanedMessage();
		return;
	}

	if (m_nIpList.GetCurrentSelectedItem(FALSE) == -1)
	{
		NoIPSelectedMessage();
		return;
	}

	POSITION pos = m_nIpList.GetFirstSelectedItemPosition();
	int nItemIndex;	

	g_bScanExistingItems = TRUE;
	g_nStartItemIndex = 0;
	g_nEndItemIndex = 1;	// We will process items manually

	SetScanStart();

	g_nEndItemIndex = g_nStartItemIndex;	// Timer will wait for completion only	

	while ((nItemIndex = m_nIpList.GetNextSelectedItem(pos)) >= 0)
	{				
		CString szIP = m_nIpList.GetItemText(nItemIndex, CL_IP);
		SetStatus(szIP);

		m_nIpList.ZeroResultsForItem(nItemIndex);

		g_nThreadCount++;	// This is decremented by threads on exit
		AfxBeginThread(ThreadProcCallbackRescan, (LPVOID) nItemIndex);

		Sleep(g_options->m_nTimerDelay);
	}
}


void CNetsurveyDlg::OnCommandsDeleteip()
{
	if (m_nScanMode != SCAN_MODE_NOT_SCANNING)
	{
		MessageBox(_T("Cannot delete while scanning"), NULL, MB_OK | MB_ICONHAND);
		return;
	}

	if (m_nIpList.GetCurrentSelectedItem(FALSE) == -1)
	{
		return;
	}	


	m_nIpList.DeleteSelectedItems();
}


void CNetsurveyDlg::OnCommandsCopyip()
{
	if (m_nIpList.GetItemCount() == 0)
	{
		NotScanedMessage();
		return;
	}
	
	if (m_nIpList.GetCurrentSelectedItem(FALSE) == -1)
	{
		NoIPSelectedMessage();
		return;
	}	

	m_nIpList.CopyIPToClipboard();
}


void CNetsurveyDlg::OnCommandsCopydetails()
{
	if (m_nIpList.GetItemCount() == 0)
	{
		NotScanedMessage();
		return;
	}

	if (m_nIpList.GetCurrentSelectedItem(FALSE) == -1)
	{
		NoIPSelectedMessage();
		return;
	}

	m_nIpList.CopyDetailToClipboard();
}


void CNetsurveyDlg::OnToolsPreferences()
{
	m_pPreferenceDlg->DoModal();
}


void CNetsurveyDlg::OnToolsFetchers()
{
	m_pSelectFetcherDlg->DoModal();
}


void CNetsurveyDlg::OnHelpAbout()
{
	CAboutDlg dlg;
	dlg.DoModal();	
}


void CNetsurveyDlg::OnBnClickedBtnPreferences()
{
	m_pPreferenceDlg->DoModal();
}


void CNetsurveyDlg::OnBnClickedBtnSelectfetchers()
{
	m_pSelectFetcherDlg->DoModal();
}

void CAboutDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
}


void CNetsurveyDlg::WinHelp(DWORD dwData, UINT nCmd)
{
	CAboutDlg dlg;
	dlg.DoModal();	
}

void CNetsurveyDlg::OnOptionsSavedimensions() 
{
	g_options->saveDimensions();
}