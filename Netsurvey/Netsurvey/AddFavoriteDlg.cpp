// AddFavoriteDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Netsurvey.h"
#include "AddFavoriteDlg.h"
#include "afxdialogex.h"


// CAddFavoriteDlg dialog

IMPLEMENT_DYNAMIC(CAddFavoriteDlg, CDialogEx)

CAddFavoriteDlg::CAddFavoriteDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAddFavoriteDlg::IDD, pParent)
	, m_strNewFavorite(_T(""))
{

}

CAddFavoriteDlg::~CAddFavoriteDlg()
{
}

void CAddFavoriteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strNewFavorite);
}


BEGIN_MESSAGE_MAP(CAddFavoriteDlg, CDialogEx)
END_MESSAGE_MAP()


// CAddFavoriteDlg message handlers
void CAddFavoriteDlg::SetOriginalName(CString strOriginal)
{
	m_strNewFavorite = strOriginal;
}

CString CAddFavoriteDlg::GetNewFavorite()
{
	return m_strNewFavorite;
}