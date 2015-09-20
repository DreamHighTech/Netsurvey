#pragma once


// CAddFavoriteDlg dialog

class CAddFavoriteDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAddFavoriteDlg)

public:
	CAddFavoriteDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddFavoriteDlg();

	void		SetOriginalName(CString strOriginal);
	CString		GetNewFavorite();

// Dialog Data
	enum { IDD = IDD_DIALOG_FAVORITE_ADD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strNewFavorite;
};
