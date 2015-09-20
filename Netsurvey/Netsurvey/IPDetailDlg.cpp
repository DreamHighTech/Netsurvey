// IPDetailDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Netsurvey.h"
#include "IPDetailDlg.h"
#include "afxdialogex.h"


// CIPDetailDlg dialog

IMPLEMENT_DYNAMIC(CIPDetailDlg, CDialogEx)

CIPDetailDlg::CIPDetailDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CIPDetailDlg::IDD, pParent)
	, m_szScannedInfo(_T(""))
{
	m_szScannedInfo = _T("");
}

CIPDetailDlg::~CIPDetailDlg()
{
}

void CIPDetailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPDETAILEDIT, m_nEditDetail);
	DDX_Text(pDX, IDC_IPDETAILEDIT, m_szScannedInfo);
}


BEGIN_MESSAGE_MAP(CIPDetailDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


void CIPDetailDlg::addScannedInfo(LPWSTR szInfo)
{
	m_szScannedInfo += szInfo;
	m_szScannedInfo += _T("\r\n");
}


void CIPDetailDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	m_nEditDetail.SetSel(0,0);
}


BOOL CIPDetailDlg::PreTranslateMessage(MSG* pMsg)
{
	// Check for Enter key presses
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		EndDialog(IDOK);	// Close window on ENTER
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
