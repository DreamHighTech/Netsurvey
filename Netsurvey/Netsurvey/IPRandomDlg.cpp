// IPRandomDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Netsurvey.h"
#include "IPRandomDlg.h"
#include "afxdialogex.h"
#include "IPRandomStyle.h"


#define  RANMAXMASK 7
CString g_nRANNetMask[7] = {
	_T("255...128"),
	_T("255...0"),
	_T("255..0.0"),
	_T("255.0.0.0"),
	_T("0.0.0.0"),
	_T("255..0.255"),
	_T("255.0.0.255"),
};

// CIPRandomDlg dialog

IMPLEMENT_DYNAMIC(CIPRandomDlg, CAbstractIPStyleDlg)

CIPRandomDlg::CIPRandomDlg(CWnd* pParent /*=NULL*/)
	: CAbstractIPStyleDlg(CIPRandomDlg::IDD, pParent)
	, m_nCount(0)
{
	m_nCount = 100;
}

CIPRandomDlg::~CIPRandomDlg()
{
}

void CIPRandomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPRANSTART, m_nIpStart);
	DDX_Control(pDX, IDC_RANCOUNT, m_nCountCtrl);
	DDX_Control(pDX, IDC_RANSPIN, m_nCountSpin);
	DDX_Control(pDX, IDC_RANMASK, m_nNetMask);
	DDX_Control(pDX, IDC_RANIP, m_button_ranip);
}


BEGIN_MESSAGE_MAP(CIPRandomDlg, CDialogEx)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPRANSTART, &CIPRandomDlg::OnIpnFieldchangedIpranstart)
	ON_EN_CHANGE(IDC_RANCOUNT, &CIPRandomDlg::OnEnChangeRancount)
	ON_NOTIFY(UDN_DELTAPOS, IDC_RANSPIN, &CIPRandomDlg::OnDeltaposRanspin)
END_MESSAGE_MAP()


// CIPRandomDlg message handlers

CString CIPRandomDlg::serialize()
{
	CString szResult;

	TCHAR szIP1[16], szIP2[16];
	m_nIpStart.GetWindowText((TCHAR *)&szIP1,16);
	m_nNetMask.GetWindowText((TCHAR *)&szIP2,16);

	szResult.Format(_T("%s|%s"), szIP1, szIP2);

	return szResult;
}

BOOL CIPRandomDlg::unserialize(const CString& szSettings)
{
	int nDelimeterPos = szSettings.Find(_T('|'));

	if (nDelimeterPos < 0)
		return FALSE;

	CString szIP = szSettings.Left(nDelimeterPos);
	m_nIpStart.SetWindowText(szIP);

	CString str = szSettings.Mid(nDelimeterPos + 1);
	nDelimeterPos = str.Find(_T('|'));
	szIP = str.Left(nDelimeterPos);
	m_nNetMask.SetWindowText(szIP);

	szIP = str.Mid(nDelimeterPos + 1);
	int nCount = _wtoi(szIP);
	m_nCountCtrl.SetWindowText(szIP);
	m_nCount = nCount;


	return FALSE;
}

BOOL CIPRandomDlg::processCommandLine(const CString& szCommandLine)
{

	return FALSE;
}

CString convert(CString str)
{
	if (str ==_T("255...128"))
		str = _T("255.255.255.128");
	else if (str == _T("255...0"))
		str = _T("255.255.255.0");
	else if (str == _T("255..0.0"))
		str = _T("255.255.0.0");
	else if (str == _T("255..0.255"))
		str = _T("255.255.0.255");

	return str;
}
CAbstractIPStyle * CIPRandomDlg::createIPFeed()
{
	TCHAR str[16];
	TCHAR netmask[16];
	char cstr[16];
	CString nstr;

	m_nIpStart.GetWindowText((TCHAR *)&str,16);
	wcstombs(cstr, str, sizeof(cstr));
	IPAddress nBaseIP = ntohl(inet_addr((char *)&cstr));

	m_nNetMask.GetWindowText((TCHAR *)&str,16);
	nstr = convert(str);
	wcstombs(cstr, nstr, sizeof(cstr));
	IPAddress nMaskIP = ntohl(inet_addr((char *)&cstr));
	m_nCountCtrl.GetWindowText((TCHAR *)&str,16);

	m_nNetMask.GetWindowText((TCHAR *)&netmask,16);

	return new CIPRandomStyle(nBaseIP, nMaskIP, netmask, _wtoi((TCHAR*)&str));
}


BOOL CIPRandomDlg::OnInitDialog()
{
	CAbstractIPStyleDlg::OnInitDialog();

	m_nIpStart.SetWindowText(_T("0.0.0.0"));
	TCHAR str[20];
	_itow(m_nCount, str, 10);
	m_nCountCtrl.SetWindowText(str);

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
	SetDlgItemText(IDC_RANHOSTNAME, szHostname);

	for(int i=0; i< RANMAXMASK; i++)
	{
		m_nNetMask.AddString(g_nRANNetMask[i]);		
	}
	m_nNetMask.SetCurSel(3);

	// Initialize tooltips
	m_pToolTips->AddTool(GetDlgItem(IDC_IPRANSTART), _T("Base IP address for generation of random IPs"));
	m_pToolTips->AddTool(GetDlgItem(IDC_RANMASK), _T("IP address mask for generation of random IPs"));
	m_pToolTips->AddTool(GetDlgItem(IDC_RANCOUNT), _T("Number of random IPs to generate."));
	m_pToolTips->Activate(TRUE);

	m_button_ranip.SetIcon((HICON)::LoadImage(AfxGetInstanceHandle(),
								MAKEINTRESOURCE(IDI_ICON_IPUP),
								IMAGE_ICON, 0, 0, 0));

	return TRUE; 
}


void CIPRandomDlg::OnIpnFieldchangedIpranstart(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

}


void CIPRandomDlg::OnEnChangeRancount()
{
	CString str = _T("");
	m_nCountCtrl.GetWindowTextW(str);
	int nCount = _wtoi(str);
	m_nCount = nCount;
	if (m_nCount > 100000)
		m_nCount = 100000;
	else if (m_nCount < 1)
		m_nCount = 1;
}


void CIPRandomDlg::OnDeltaposRanspin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	
	TCHAR str[20];
	m_nCount -= pNMUpDown->iDelta;
	if (m_nCount > 100000)
		m_nCount = 100000;
	else if (m_nCount < 1)
		m_nCount = 1;
	_itow(m_nCount, str, 10);
	m_nCountCtrl.SetWindowText(str);
	*pResult = 0;
}
