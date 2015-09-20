// PreferencesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Netsurvey.h"
#include "PreferencesDlg.h"
#include "globals.h"
#include "afxdialogex.h"


// CPreferencesDlg dialog

IMPLEMENT_DYNAMIC(CPreferencesDlg, CDialog)

CPreferencesDlg::CPreferencesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPreferencesDlg::IDD, pParent)
{
	m_pScanningTABDlg = NULL;
	m_pDisplayTABDlg = NULL;	
}

CPreferencesDlg::~CPreferencesDlg()
{
	if (m_pScanningTABDlg != NULL)
		delete m_pScanningTABDlg;

	if (m_pDisplayTABDlg != NULL)
	{
		delete m_pDisplayTABDlg;
	}
}

void CPreferencesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PREFERENCES_TABCTRL, m_preferencestabctrl);
}


BEGIN_MESSAGE_MAP(CPreferencesDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_PREFERENCES_TABCTRL, &CPreferencesDlg::OnSelchangePreferencesTabctrl)
	ON_BN_CLICKED(IDOK, &CPreferencesDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CPreferencesDlg message handlers


BOOL CPreferencesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_pScanningTABDlg == NULL)
	{
		m_pScanningTABDlg = new CScanningTABDlg;
	}
	m_pScanningTABDlg->Create(m_pScanningTABDlg->IDD, &m_preferencestabctrl);	
	AddTab(m_pScanningTABDlg, _T("Scanning"), 0);
	

	if (m_pDisplayTABDlg == NULL)
	{
		m_pDisplayTABDlg = new CDisplayTABDlg;
	}

	m_pDisplayTABDlg->Create(m_pDisplayTABDlg->IDD, &m_preferencestabctrl);
	AddTab(m_pDisplayTABDlg, _T("Display"), 1);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CPreferencesDlg::AddTab(CWnd* pWnd, LPTSTR lpszCaption, int iImage)
{
	ASSERT_VALID(pWnd);

	TCITEM item;
	item.mask = TCIF_TEXT|TCIF_PARAM|TCIF_IMAGE;
	item.lParam = (LPARAM) pWnd;
	item.pszText = lpszCaption;
	item.iImage = iImage;
	int iIndex = m_preferencestabctrl.GetItemCount();
	m_preferencestabctrl.InsertItem(iIndex, &item);

	pWnd->SetWindowPos(NULL, 4, 28, 0, 0,
						SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOZORDER);
	
	pWnd->ShowWindow(iIndex ? SW_HIDE : SW_SHOW);
}

void CPreferencesDlg::OnSelchangePreferencesTabctrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int tabIndex = m_preferencestabctrl.GetCurSel();
	if (tabIndex == 0)
	{
		m_pScanningTABDlg->ShowWindow(SW_SHOW);
		m_pDisplayTABDlg->ShowWindow(SW_HIDE);
	} else if(tabIndex == 1){
		m_pDisplayTABDlg->ShowWindow(SW_SHOW);
		m_pScanningTABDlg->ShowWindow(SW_HIDE);
	}
	*pResult = 0;
}


void CPreferencesDlg::OnBnClickedOk()
{
	m_pScanningTABDlg->UpdateData(1);
	m_pDisplayTABDlg->UpdateData(1);
	g_options->m_nMaxThreads = m_pScanningTABDlg->m_nMaxThreads;
	g_options->m_nPingTimeout = m_pScanningTABDlg->m_nPingTimeout;
	g_options->m_nPingCount = m_pScanningTABDlg->m_nPingCount;
	g_options->m_nTimerDelay = m_pScanningTABDlg->m_nTimerDelay;
	g_options->m_bScanHostIfDead = m_pScanningTABDlg->m_bScanHostIfDead;
	g_options->m_bSkipBroadcast = m_pScanningTABDlg->m_bSkipBroadcast;
	g_options->m_neDisplayOptions = m_pDisplayTABDlg->m_nDisplayOptions;
	g_options->m_szNotAvailableValue = m_pDisplayTABDlg->m_szNotAvailableValue;
	g_options->m_szNotScannedValue = m_pDisplayTABDlg->m_szNotScannedValue;
	g_options->m_bAskConfirm = m_pDisplayTABDlg->m_bAskConfirm;
	g_options->m_bShowInfo = m_pDisplayTABDlg->m_bShowInfo;
	CDialog::OnOK();
}
