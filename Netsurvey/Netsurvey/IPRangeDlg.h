#pragma once

#include "AbstractIPStyleDlg.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "afxbutton.h"

// CIPRangeDlg dialog

class CIPRangeDlg : public CAbstractIPStyleDlg
{
	DECLARE_DYNAMIC(CIPRangeDlg)

public:
	CIPRangeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CIPRangeDlg();

// Dialog Data
	enum { IDD = IDD_IPSTYPE_IPRANGE };

	CButton	m_btnIPUp;
	CString	m_szHostname;
	BOOL m_bIp2Virgin;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual CString serialize();
	virtual BOOL unserialize(const CString& szSettings);
	virtual BOOL processCommandLine(const CString& szCommandLine);
	CAbstractIPStyle * createIPFeed();
//	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	CIPAddressCtrl m_nIpStart;
	CIPAddressCtrl m_nIpEnd;
	afx_msg void OnBnClickedLocalip();
	afx_msg void OnIpnFieldchangedIpaddress1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnIpnFieldchangedIpaddress2(NMHDR *pNMHDR, LRESULT *pResult);
	CComboBox m_nComboNetMask;
	afx_msg void OnCbnSelchangeNetmask();
	CButton m_button_ip;
};