// ScanningTABDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Netsurvey.h"
#include "ScanningTABDlg.h"
#include "afxdialogex.h"
#include "globals.h"

// CScanningTABDlg dialog

IMPLEMENT_DYNAMIC(CScanningTABDlg, CDialog)

CScanningTABDlg::CScanningTABDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScanningTABDlg::IDD, pParent)
{

	m_nMaxThreads = 0;
	m_nTimerDelay = 0;
	m_nPingTimeout = 0;
	m_nPingCount = 0;
	m_bScanHostIfDead = FALSE;
	m_bSkipBroadcast = FALSE;
	m_nBrush = NULL;
}

CScanningTABDlg::~CScanningTABDlg()
{
	if (m_nBrush)
		delete m_nBrush;
}

void CScanningTABDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MAXIMUMTHREADS_TEXT, m_nMaxThreads);
	DDV_MinMaxUInt(pDX, m_nMaxThreads, 1, 1000);
	DDX_Text(pDX, IDC_DELAY_TEXT, m_nTimerDelay);
	DDV_MinMaxInt(pDX, m_nTimerDelay, 5, 10000);
	DDX_Text(pDX, IDC_PINGTIMEOUT_TEXT, m_nPingTimeout);
	DDV_MinMaxUInt(pDX, m_nPingTimeout, 500, 60000);
	DDX_Text(pDX, IDC_SENDPACKETCOUNT_TEXT, m_nPingCount);
	DDV_MinMaxInt(pDX, m_nPingCount, 1, 10);
	DDX_Check(pDX, IDC_SCANDEADHOSTS_CHECKBOX, m_bScanHostIfDead);
	DDX_Check(pDX, IDC_SKIP_CHECKBOX, m_bSkipBroadcast);
}


BEGIN_MESSAGE_MAP(CScanningTABDlg, CDialog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CScanningTABDlg message handlers


BOOL CScanningTABDlg::OnInitDialog()
{
	m_nMaxThreads = g_options->m_nMaxThreads;
	m_nPingTimeout = g_options->m_nPingTimeout;
	m_nPingCount = g_options->m_nPingCount;
	m_nTimerDelay = g_options->m_nTimerDelay;
	m_bScanHostIfDead = g_options->m_bScanHostIfDead;
	m_bSkipBroadcast = g_options->m_bSkipBroadcast;
	CDialog::OnInitDialog();
	if (!m_nBrush)
	{
		m_nBrush = new CBrush;
		m_nBrush->CreateSolidBrush(RGB(255,255,255));
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


//void CScanningTABDlg::OnOK()
//{
//	CDialog::OnOK();
	//g_options->m_nMaxThreads = m_nMaxThreads;
	//g_options->m_nPingTimeout = m_nPingTimeout;
	//g_options->m_nPingCount = m_nPingCount;
	//g_options->m_nTimerDelay = m_nTimerDelay;
	//g_options->m_bScanHostIfDead = m_bScanHostIfDead;
	//g_options->m_bSkipBroadcast = m_bSkipBroadcast;
//	
//}



HBRUSH CScanningTABDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	pDC->SetBkMode(TRANSPARENT);
	if (NULL != m_nBrush)
		hbr = (HBRUSH)m_nBrush->GetSafeHandle();

	return hbr;
}
