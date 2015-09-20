// CommonDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Netsurvey.h"
#include "CommonDlg.h"
#include "afxdialogex.h"


// CCommonDlg dialog

IMPLEMENT_DYNAMIC(CCommonDlg, CDialogEx)

CCommonDlg::CCommonDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCommonDlg::IDD, pParent)
{
	m_nInitEditDlg = false;
}

CCommonDlg::~CCommonDlg()
{
}

void CCommonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMMON_EDIT, m_EditText);
}

BEGIN_MESSAGE_MAP(CCommonDlg, CDialogEx)
	ON_BN_CLICKED(ID_COMMON_CANCEL, &CCommonDlg::OnBnClickedCommonCancel)
	ON_BN_CLICKED(ID_COMMON_OK, &CCommonDlg::OnBnClickedCommonOk)
END_MESSAGE_MAP()


void CCommonDlg::OnBnClickedCommonCancel()
{	
	CDialogEx::OnCancel();
}

void CCommonDlg::OnBnClickedCommonOk()
{
	m_EditText.GetWindowText(m_szDefaultUserText);
	CDialogEx::OnOK();
}

BOOL CCommonDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(m_szCaption);
	SetDlgItemText(IDC_COMMON_TEXT, m_szQueryText);
	m_EditText.SetWindowText(m_szDefaultUserText);

	if (m_nInitEditDlg)
		m_EditText.SetFocus();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

CString CCommonDlg::doQuery()
{
	CString szReturn;

	if (DoModal() == IDOK)
		szReturn = m_szDefaultUserText;

	return szReturn;
}
