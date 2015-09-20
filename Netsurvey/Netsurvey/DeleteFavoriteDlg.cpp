// DeleteFavoriteDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Netsurvey.h"
#include "DeleteFavoriteDlg.h"
#include "afxdialogex.h"
#include "globals.h"
#include "FavoriteRenameDlg.h"

// DeleteFavoriteDlg dialog

IMPLEMENT_DYNAMIC(DeleteFavoriteDlg, CDialogEx)

DeleteFavoriteDlg::DeleteFavoriteDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(DeleteFavoriteDlg::IDD, pParent)
{

}

DeleteFavoriteDlg::~DeleteFavoriteDlg()
{
}

void DeleteFavoriteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listFavorite);
}


BEGIN_MESSAGE_MAP(DeleteFavoriteDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_UP, &DeleteFavoriteDlg::OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, &DeleteFavoriteDlg::OnBnClickedButtonDown)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &DeleteFavoriteDlg::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_RENAME, &DeleteFavoriteDlg::OnBnClickedButtonRename)
	ON_LBN_DBLCLK(IDC_LIST1, &DeleteFavoriteDlg::OnDblclkFavorite)
END_MESSAGE_MAP()


// DeleteFavoriteDlg message handlers
BOOL DeleteFavoriteDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	for (int i=0; i < 99; i++)
	{
		if (g_options->m_aFavourites[i].szName.GetLength() == 0)
			break;

		//m_listFavorite.AddString((LPCTSTR)"TTT");
		m_listFavorite.AddString(g_options->m_aFavourites[i].szName);
	}

	m_listFavorite.SetCurSel(0);

	return TRUE;  
}


void DeleteFavoriteDlg::OnBnClickedButtonUp()
{
	// TODO: Add your control notification handler code here
	int nCurrenCell = m_listFavorite.GetCurSel();
	if(nCurrenCell != 0)
	{
		nCurrenCell --;
		m_listFavorite.SetCurSel(nCurrenCell);
	}
}


void DeleteFavoriteDlg::OnBnClickedButtonDown()
{
	// TODO: Add your control notification handler code here
	int nCurrenCell = m_listFavorite.GetCurSel();
	if(nCurrenCell != (m_listFavorite.GetCount() - 1))
	{
		nCurrenCell ++;
		m_listFavorite.SetCurSel(nCurrenCell);
	}
}


void DeleteFavoriteDlg::OnBnClickedButtonDelete()
{
	if (m_listFavorite.GetCurSel() == -1)
		return;

	if(m_listFavorite.GetCount() == 0)
		return;

	//TODO: Add your control notification handler code here
	for (int i = m_listFavorite.GetCurSel(); i < 250; i++)
	{			
		m_pFavorite[i] = m_pFavorite[i+1];

		if (m_pFavorite[i].szName.GetLength() == 0)
			break;
	}
	m_listFavorite.DeleteString(m_listFavorite.GetCurSel());
}

void DeleteFavoriteDlg::SetFavorites(tFavourite* pAddress)
{
	m_pFavorite = pAddress;
}

void DeleteFavoriteDlg::OnBnClickedButtonRename()
{
	// TODO: Add your control notification handler code here
	if(m_listFavorite.GetCount() == 0)
		return;

	int nCurSel = m_listFavorite.GetCurSel();

	CString strOriginal;
	m_listFavorite.GetText(nCurSel, strOriginal);

	CFavoriteRenameDlg dlg;
	dlg.SetOriginalName(strOriginal);
	if(dlg.DoModal())
	{
		CString strNew = dlg.GetNewName();
		if(strNew.IsEmpty())
			return;

		m_pFavorite[nCurSel].szName = strNew;
		m_listFavorite.DeleteString(nCurSel);
		m_listFavorite.InsertString(nCurSel, strNew);
	}
}


void DeleteFavoriteDlg::OnDblclkFavorite()
{
	// TODO: Add your control notification handler code here
	OnBnClickedButtonRename();
}
