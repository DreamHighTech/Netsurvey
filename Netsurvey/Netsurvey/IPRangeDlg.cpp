// IPRangeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Netsurvey.h"
#include "NetsurveyDlg.h"
#include "IPRangeDlg.h"
#include "afxdialogex.h"
#include "IPRangeIPStyle.h"
#include "globals.h"

#define  MAXMASK 7
CString g_nNetMask[7] = {
	_T("/24"),
	_T("/16"),
	_T("255...192"),
	_T("255...128"),
	_T("255...0"),
	_T("255..0.0"),
	_T("255.0.0.0"),
};
// CIPRangeDlg dialog

IMPLEMENT_DYNAMIC(CIPRangeDlg, CAbstractIPStyleDlg)

CIPRangeDlg::CIPRangeDlg(CWnd* pParent /*=NULL*/)
	: CAbstractIPStyleDlg(CIPRangeDlg::IDD, pParent)
{
	m_szHostname = _T("");
}

CIPRangeDlg::~CIPRangeDlg()
{
}

void CIPRangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_nIpStart);
	DDX_Control(pDX, IDC_IPADDRESS2, m_nIpEnd);
	DDX_Text(pDX, IDC_HOSTNAME, m_szHostname);
	DDX_Control(pDX, IDC_NETMASK, m_nComboNetMask);
	DDX_Control(pDX, IDC_LOCALIP, m_button_ip);
}


BEGIN_MESSAGE_MAP(CIPRangeDlg, CDialogEx)
//	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_LOCALIP, &CIPRangeDlg::OnBnClickedLocalip)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS1, &CIPRangeDlg::OnIpnFieldchangedIpaddress1)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS2, &CIPRangeDlg::OnIpnFieldchangedIpaddress2)
	ON_CBN_SELCHANGE(IDC_NETMASK, &CIPRangeDlg::OnCbnSelchangeNetmask)
END_MESSAGE_MAP()


// CIPRangeDlg message handlers



CAbstractIPStyle * CIPRangeDlg::createIPFeed()
{
	TCHAR str[16];
	char cstr[16];

	m_nIpStart.GetWindowText((TCHAR *)&str,16);

	wcstombs(cstr, str, sizeof(cstr));
	IPAddress nStartIP = ntohl(inet_addr((char *)&cstr));

	m_nIpEnd.GetWindowText((TCHAR *)&str,16);
	wcstombs(cstr, str, sizeof(cstr));
	IPAddress nEndIP = ntohl(inet_addr((char *)&cstr));

	if (nEndIP < nStartIP) 
	{
		MessageBox(_T("Ending IP address is lower than starting."), NULL, MB_OK | MB_ICONHAND);
		return NULL;
	}

	return new CIPRangeIPStyle(nStartIP, nEndIP);
}

CString CIPRangeDlg::serialize()
{
	CString szResult;

	TCHAR szIP1[16], szIP2[16];
	m_nIpStart.GetWindowText((TCHAR *)&szIP1,16);
	m_nIpEnd.GetWindowText((TCHAR *)&szIP2,16);

	szResult.Format(_T("%s|%s"), szIP1, szIP2);

	return szResult;
}

BOOL CIPRangeDlg::unserialize(const CString& szSettings)
{
	int nDelimeterPos = szSettings.Find(_T('|'));

	if (nDelimeterPos < 0)
		return FALSE;

	CString szIP = szSettings.Left(nDelimeterPos);
	m_nIpStart.SetWindowText(szIP);

	szIP = szSettings.Mid(nDelimeterPos + 1);
	m_nIpEnd.SetWindowText(szIP);

	m_bIp2Virgin = FALSE;

	return TRUE;
}

BOOL CIPRangeDlg::processCommandLine(const CString& szCommandLine)
{
	// For now, the same code can be used :-)
	return unserialize(szCommandLine);
}

//void CIPRangeDlg::OnPaint()
//{
//	CPaintDC dc(this); // device context for painting
//	// TODO: Add your message handler code here
//	// Do not call CAbstractIPStyleDlg::OnPaint() for painting messages
//}


BOOL CIPRangeDlg::OnInitDialog()
{
	CAbstractIPStyleDlg::OnInitDialog();

	// Initialize controls
	m_bIp2Virgin = TRUE;
	m_nIpStart.SetWindowText(_T("0.0.0.0"));

	// Init hostname
	hostent *he;
	char cszHostname[256];
	TCHAR szHostname[128];
	TCHAR wstr[128];
	in_addr in;

	gethostname((char *)&cszHostname, 100);
	he = gethostbyname(cszHostname);
	memcpy(&in.S_un.S_addr,*he->h_addr_list,sizeof(long));
	char* addr = inet_ntoa(in);
	
	memset(wstr, 0x0, sizeof(wstr));
	mbstowcs(wstr, addr, 15);
	m_nIpStart.SetWindowText(wstr);

	memset(wstr, 0x0, sizeof(szHostname));
	mbstowcs(szHostname, cszHostname, sizeof(cszHostname));
	SetDlgItemText(IDC_HOSTNAME, szHostname);

	// Process change event
	OnIpnFieldchangedIpaddress1(NULL, NULL);

	// Initialize tooltips
	m_pToolTips->AddTool(GetDlgItem(IDC_LOCALIP), _T("Use the IP address of the specified hostname"));
	m_pToolTips->Activate(TRUE);

	for(int i=0; i< MAXMASK; i++)
	{
		m_nComboNetMask.AddString(g_nNetMask[i]);
	}
	m_nComboNetMask.SetWindowTextW(_T("NetMask"));

	m_button_ip.SetIcon((HICON)::LoadImage(AfxGetInstanceHandle(),
								MAKEINTRESOURCE(IDI_ICON_IPUP),
								IMAGE_ICON, 0, 0, 0));

	return TRUE;
}

void CIPRangeDlg::OnBnClickedLocalip()
{
	((CNetsurveyDlg*)g_dlg)->SetStatus(_T("Getting IP..."));

	hostent *he;
	char str[100];
	TCHAR wstr[100];
	char *addr;
	in_addr in;
	GetDlgItemText(IDC_HOSTNAME,wstr,100);
	wcstombs(str, wstr, sizeof(wstr));
	he = gethostbyname(str);

	((CNetsurveyDlg*)g_dlg)->SetStatus(NULL);

	if (!he) 
	{
		MessageBox(_T("No DNS entry"),NULL,MB_OK | MB_ICONHAND);
		return;
	}
	memcpy(&in.S_un.S_addr,*he->h_addr_list,sizeof(long));
	addr = inet_ntoa(in);
	memset(wstr, 0x0, sizeof(wstr));
	mbstowcs(wstr, addr, 15);
	m_nIpStart.SetWindowText(wstr);
	m_nIpEnd.SetWindowText(wstr);
	m_bIp2Virgin = TRUE;
}


void CIPRangeDlg::OnIpnFieldchangedIpaddress1(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (m_bIp2Virgin) 
	{
		TCHAR str[16];	
		m_nIpStart.GetWindowText((TCHAR*)&str,sizeof(str));
		m_nIpEnd.SetWindowText((TCHAR*)&str);
	}

	if (pResult)
		*pResult = 0;
}


void CIPRangeDlg::OnIpnFieldchangedIpaddress2(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_bIp2Virgin = FALSE;

	if (pResult)
		*pResult = 0;
}


void CIPRangeDlg::OnCbnSelchangeNetmask()
{
	int curentindex = m_nComboNetMask.GetCurSel();
	DWORD ipstart, ipend;
	char *ipcstart = (char*)&ipstart;
	char *ipcend = (char*)&ipend;
	m_nIpStart.GetAddress(ipstart);
	m_nIpEnd.GetAddress(ipend);

	switch(curentindex)
	{
	case 0:
	case 4:
		ipcstart[0] = (TCHAR) 0;
		ipcend[0] = (TCHAR) 255;
		break;
	case 1:
	case 5:
		ipcstart[0] = (TCHAR) 0;
		ipcstart[1] = (TCHAR) 0;
		ipcend[0] = (TCHAR) 255;
		ipcend[1] = (TCHAR) 255;
		break;
	case 2:
		ipcstart[0] = (TCHAR) 192;
		ipcend[0] = (TCHAR) 255;
		break;
	case 3:
		ipcstart[0] = (TCHAR) 128;
		ipcend[0] = (TCHAR) 255;
		break;
	case 6:
		ipcstart[0] = (TCHAR) 0;
		ipcstart[1] = (TCHAR) 0;
		ipcstart[2] = (TCHAR) 0;
		ipcend[0] = (TCHAR) 255;
		ipcend[1] = (TCHAR) 255;
		ipcend[2] = (TCHAR) 255;
		break;
	}

	m_nIpStart.SetAddress(ipstart);
	m_nIpEnd.SetAddress(ipend);
}
