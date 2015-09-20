#pragma once

#include "AbstractIPStyleDlg.h"
#include "NetsurveyDlg.h"

// CIPListDlg dialog

class CIPListDlg : public CAbstractIPStyleDlg
{
	DECLARE_DYNAMIC(CIPListDlg)

public:
	CIPListDlg(CNetsurveyDlg * dlg, CWnd* pParent = NULL);   // standard constructor
	CNetsurveyDlg *m_dlg;
	virtual ~CIPListDlg();

// Dialog Data
	enum { IDD = IDD_IPSTYPE_IPLIST };

protected:

	CString m_filename;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual CString serialize();
	virtual BOOL unserialize(const CString& szSettings);
	virtual BOOL processCommandLine(const CString& szCommandLine);
	CAbstractIPStyle * createIPFeed();
	BOOL queryFilename();
	BOOL Openfile();
	afx_msg void OnBnClickedListbtnBrowse();
};
