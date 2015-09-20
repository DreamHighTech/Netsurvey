#pragma once


// CScanningTABDlg dialog

class CScanningTABDlg : public CDialog
{
	DECLARE_DYNAMIC(CScanningTABDlg)

public:
	CScanningTABDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CScanningTABDlg();

// Dialog Data
	enum { IDD = IDD_SCANNINGTAB_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	UINT m_nMaxThreads;
	int m_nTimerDelay;
	UINT m_nPingTimeout;
	int m_nPingCount;
	BOOL m_bScanHostIfDead;
	BOOL m_bSkipBroadcast;
	virtual BOOL OnInitDialog();
	CBrush * m_nBrush;
//	virtual void OnOK();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
