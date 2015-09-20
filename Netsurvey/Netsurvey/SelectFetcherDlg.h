#pragma once


// CSelectFetcherDlg dialog

class CSelectFetcherDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectFetcherDlg)

public:
	CSelectFetcherDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectFetcherDlg();

// Dialog Data
	enum { IDD = IDD_SELECTFETCHERS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	char m_naSelColumns[128];
	int m_nSelectedColumns;
	void RepopulateSelectedColumns();

	virtual BOOL OnInitDialog();
	CListBox m_AllFetchers;
	CListBox m_SelectedFetchers;
	afx_msg void OnMoveDown();
	afx_msg void OnMoveUp();
	afx_msg void OnSelect();
	afx_msg void OnDeselect();
	afx_msg void OnBnClickedOk();

};
