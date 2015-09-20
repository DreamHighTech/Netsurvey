#pragma once
#include "afxwin.h"
#include "globals.h"

// DeleteFavoriteDlg dialog

class DeleteFavoriteDlg : public CDialogEx
{
	DECLARE_DYNAMIC(DeleteFavoriteDlg)

public:
	DeleteFavoriteDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~DeleteFavoriteDlg();

	void			SetFavorites(tFavourite* pAddress);

// Dialog Data
	enum { IDD = IDD_DELETE_FAVORITE_DIALOG };

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


public:
	virtual BOOL	OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	CListBox		m_listFavorite;
	tFavourite		*m_pFavorite;

public:
	afx_msg void	OnBnClickedButtonUp();
	afx_msg void	OnBnClickedButtonDown();
	afx_msg void	OnBnClickedButtonDelete();
	afx_msg void	OnBnClickedButtonRename();
	afx_msg void OnDblclkFavorite();
};
