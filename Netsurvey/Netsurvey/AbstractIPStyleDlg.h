
#pragma once

#include "AbstractIPStyle.h"
#include "resource.h"

#define MAXIPSTYLEDLG 2

class CAbstractIPStyleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAbstractIPStyleDlg)
// Construction
public:
	virtual CAbstractIPStyle * createIPFeed() = 0;
	
	CAbstractIPStyleDlg(int nIDD, CWnd* pParent = NULL);   // standard constructor
	~CAbstractIPStyleDlg();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIPRangeDlg)
	protected:
	public:
	virtual CWnd * SetFocus();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:

	CToolTipCtrl * m_pToolTips;

	// Generated message map functions
	//{{AFX_MSG(CIPRangeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	// Returns serialized settings as a string (for implementing Favorites)
	virtual CString serialize() = 0;

	// Imports previously serialized data and restores it's state
	virtual BOOL unserialize(const CString& szSettings) = 0;

	// Imports command-line options, to initialize options from command-line
	virtual BOOL processCommandLine(const CString& szCommandLine) = 0;

};
