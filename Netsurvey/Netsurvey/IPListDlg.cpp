// IPListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Netsurvey.h"
#include "IPListDlg.h"
#include "IPListStyle.h"
#include "afxdialogex.h"


// CIPListDlg dialog

IMPLEMENT_DYNAMIC(CIPListDlg, CAbstractIPStyleDlg)

CIPListDlg::CIPListDlg(CNetsurveyDlg* dlg, CWnd* pParent /*=NULL*/)
	: CAbstractIPStyleDlg(CIPListDlg::IDD, pParent)
{
	m_dlg = dlg;
}

CIPListDlg::~CIPListDlg()
{
}

void CIPListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CIPListDlg, CDialogEx)
	ON_BN_CLICKED(IDC_LISTBTN_BROWSE, &CIPListDlg::OnBnClickedListbtnBrowse)
END_MESSAGE_MAP()


// CIPListDlg message handlers
BOOL CIPListDlg::unserialize(const CString& szSettings)
{
	return FALSE;
}

BOOL CIPListDlg::processCommandLine(const CString& szCommandLine)
{

	return FALSE;
}

CAbstractIPStyle * CIPListDlg::createIPFeed()
{

	return new CIPListStyle();
}

CString CIPListDlg::serialize()
{
	CString szResult;

	return szResult;
}

void CIPListDlg::OnBnClickedListbtnBrowse()
{
	if (!queryFilename())
		return;

	
}

BOOL CIPListDlg::queryFilename()
{
	CFileDialog fd(FALSE,_T("*.*"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,NULL,m_dlg);

	if (fd.DoModal() == IDOK)
	{		
		m_filename = fd.GetPathName();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}