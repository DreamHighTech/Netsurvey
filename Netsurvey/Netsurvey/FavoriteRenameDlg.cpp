// FavoriteRenameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Netsurvey.h"
#include "FavoriteRenameDlg.h"
#include "afxdialogex.h"


// CFavoriteRenameDlg dialog

IMPLEMENT_DYNAMIC(CFavoriteRenameDlg, CDialogEx)

CFavoriteRenameDlg::CFavoriteRenameDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFavoriteRenameDlg::IDD, pParent)
	, m_strNewName(_T(""))
{

}

CFavoriteRenameDlg::~CFavoriteRenameDlg()
{
}

void CFavoriteRenameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NEWNAME, m_strNewName);
}


BEGIN_MESSAGE_MAP(CFavoriteRenameDlg, CDialogEx)
END_MESSAGE_MAP()


// CFavoriteRenameDlg message handlers
CString CFavoriteRenameDlg::GetNewName()
{
	//UpdateData(true);
	return m_strNewName;
}

void CFavoriteRenameDlg::SetOriginalName(CString strOriginal)
{
	m_strNewName = strOriginal;
	//UpdateData(FALSE);
}