#pragma once

#include "AbstractIPStyleDlg.h"
#include "afxcmn.h"
#include "afxwin.h"

// CIPRandomDlg dialog

class CIPRandomDlg : public CAbstractIPStyleDlg
{
	DECLARE_DYNAMIC(CIPRandomDlg)

public:
	CIPRandomDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CIPRandomDlg();

// Dialog Data
	enum { IDD = IDD_IPSTYPE_IPRANDOM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual CString serialize();
	virtual BOOL unserialize(const CString& szSettings);
	virtual BOOL processCommandLine(const CString& szCommandLine);
	CAbstractIPStyle * createIPFeed();
	int m_nCount;
	CIPAddressCtrl m_nIpStart;
	virtual BOOL OnInitDialog();
	afx_msg void OnIpnFieldchangedIpranstart(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeRancount();
	afx_msg void OnDeltaposRanspin(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_nCountCtrl;
	CSpinButtonCtrl m_nCountSpin;
	CComboBox m_nNetMask;
	CButton m_button_ranip;
};
