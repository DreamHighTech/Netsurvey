#pragma once


// CFavoriteRenameDlg dialog

class CFavoriteRenameDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFavoriteRenameDlg)

public:
	CFavoriteRenameDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFavoriteRenameDlg();

	CString			GetNewName();
	void			SetOriginalName(CString strOriginal);
// Dialog Data
	enum { IDD = IDD_DIALOG_FAVORITE_RENAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CString			m_strNewName;
};
