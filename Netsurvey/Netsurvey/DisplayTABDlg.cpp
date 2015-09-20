// DisplayTABDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Netsurvey.h"
#include "DisplayTABDlg.h"
#include "globals.h"
#include "afxdialogex.h"


// CDisplayTABDlg dialog

IMPLEMENT_DYNAMIC(CDisplayTABDlg, CDialog)

CDisplayTABDlg::CDisplayTABDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDisplayTABDlg::IDD, pParent)
{
	m_nDisplayOptions = false;
	m_szNotAvailableValue = _T("");
	m_szNotScannedValue = _T("");
	m_nBrush = NULL;
}

CDisplayTABDlg::~CDisplayTABDlg()
{
	if (NULL != m_nBrush)
		delete m_nBrush;
}

void CDisplayTABDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_ALLSCAN_OPTION, m_nDisplayOptions);
	DDX_Text(pDX, IDC_NOTAVAILABLE_TEXT, m_szNotAvailableValue);
	DDX_Text(pDX, IDC_UNKNOWN_TEXT, m_szNotScannedValue);
	DDX_Check(pDX, IDC_ASKCONFIRMATION_CHECKBOX, m_bAskConfirm);
	DDX_Check(pDX, IDC_SHOWINFODIALOG_CHECKBOX, m_bShowInfo);
}


BEGIN_MESSAGE_MAP(CDisplayTABDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDisplayTABDlg message handlers


BOOL CDisplayTABDlg::OnInitDialog()
{
	m_nDisplayOptions = g_options->m_neDisplayOptions;
	m_szNotAvailableValue = g_options->m_szNotAvailableValue;
	m_szNotScannedValue = g_options->m_szNotScannedValue;
	m_bAskConfirm = g_options->m_bAskConfirm;
	m_bShowInfo = g_options->m_bShowInfo;
	CDialog::OnInitDialog();
	
	if (!m_nBrush)
	{
		m_nBrush = new CBrush;
		m_nBrush->CreateSolidBrush(RGB(255,255,255));
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CDisplayTABDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	
	return CDialog::OnEraseBkgnd(pDC);
}


HBRUSH CDisplayTABDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	pDC->SetBkMode(TRANSPARENT);
	if (NULL != m_nBrush)
		hbr = (HBRUSH)m_nBrush->GetSafeHandle();
	// TODO:  Return a different brush if the default is not desired
	return hbr;
}
