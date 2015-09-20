#pragma once

#include "resource.h"
#include "afxwin.h"

// CCommonDlg dialog

class CCommonDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCommonDlg)

public:
	CCommonDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCommonDlg();

// Dialog Data
	enum { IDD = IDD_COMMON_DIALOG };

	CString m_nCommonStr;
	CEdit m_EditText;
	bool m_nInitEditDlg;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCommonOk();
	afx_msg void OnBnClickedCommonCancel();
	virtual BOOL OnInitDialog();

public:
	CString m_szCaption;
	CString m_szQueryText;
	CString	m_szDefaultUserText;

	CString doQuery();
};
