// ScanStatisticsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Netsurvey.h"
#include "ScanStatisticsDlg.h"
#include "afxdialogex.h"


// CScanStatisticsDlg dialog

IMPLEMENT_DYNAMIC(CScanStatisticsDlg, CDialogEx)

CScanStatisticsDlg::CScanStatisticsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CScanStatisticsDlg::IDD, pParent)
{

}

CScanStatisticsDlg::~CScanStatisticsDlg()
{
}

void CScanStatisticsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CScanStatisticsDlg, CDialogEx)
END_MESSAGE_MAP()


// CScanStatisticsDlg message handlers


BOOL CScanStatisticsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetDlgItemText(IDC_MESSAGE_TEXT, m_szMessage);	

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CScanStatisticsDlg::setMessageText(LPCWSTR szMessage)
{
	m_szMessage = szMessage;
}