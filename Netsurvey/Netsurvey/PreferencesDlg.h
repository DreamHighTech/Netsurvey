#pragma once
#include "afxcmn.h"
#include "ScanningTABDlg.h"
#include "DisplayTABDlg.h"

// CPreferencesDlg dialog

class CPreferencesDlg : public CDialog
{
	DECLARE_DYNAMIC(CPreferencesDlg)

public:
	CPreferencesDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPreferencesDlg();

// Dialog Data
	enum { IDD = IDD_PREFERENCES_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_preferencestabctrl;
	CScanningTABDlg *m_pScanningTABDlg;
	CDisplayTABDlg *m_pDisplayTABDlg;
	virtual BOOL OnInitDialog();

	void AddTab(CWnd* pWnd, LPTSTR lpszCaption, int iImage);
	afx_msg void OnSelchangePreferencesTabctrl(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
};
