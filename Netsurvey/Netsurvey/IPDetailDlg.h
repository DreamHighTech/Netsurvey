#pragma once
#include "afxwin.h"


// CIPDetailDlg dialog

class CIPDetailDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CIPDetailDlg)

public:
	CIPDetailDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CIPDetailDlg();

// Dialog Data
	enum { IDD = IDD_SHOWDETAILDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_nEditDetail;
	CString m_szScannedInfo;

	void addScannedInfo(LPWSTR szInfo);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
