#pragma once

// CDisplayTABDlg dialog

class CDisplayTABDlg : public CDialog
{
	DECLARE_DYNAMIC(CDisplayTABDlg)

public:
	CDisplayTABDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDisplayTABDlg();

// Dialog Data
	enum { IDD = IDD_DISPLAYTAB_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_nDisplayOptions;
	virtual BOOL OnInitDialog();
	CString m_szNotAvailableValue;
	CString m_szNotScannedValue;
	BOOL m_bAskConfirm;
	BOOL m_bShowInfo;
	CBrush * m_nBrush;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
