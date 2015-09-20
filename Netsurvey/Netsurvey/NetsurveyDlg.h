
// NetsurveyDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "IPStyleDlgFactory.h"
#include "AbstractIPStyle.h"
#include "PreferencesDlg.h"
#include "ScanList.h"
#include "SelectFetcherDlg.h"
#include "afxwin.h"

// CNetsurveyDlg dialog
class CNetsurveyDlg : public CDialogEx
{
// Construction
public:
	CNetsurveyDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_Netsurvey_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CScanList m_nIpList;
	CIPStyleDlgFactory *m_pIPStyleFactory;
	CAbstractIPStyleDlg *m_pIPStyleDlg;
	CPreferencesDlg *m_pPreferenceDlg;
	CSelectFetcherDlg *m_pSelectFetcherDlg;
	CComboBox m_nComboIpStyleDlg;
	int m_nScanMode;
	unsigned long m_nTickCount;
	BOOL m_bScanningAborted;
	CString *m_pSzDefaultFileName;
	CString m_pSszCompleteInformation;
	int m_nCmdLineOptions;
	int m_nCmdLineFileFormat;
	int m_nStatusHeight;
	int m_nListOffset;
	CStatic	m_nStatus;
	CStatic	m_nThread;
	CProgressCtrl m_nProgressBar;


	void SetScanStart();
	void SelectIPStyleByType(const CString &szType);
	void SetRecreateIPFeed();
	void SetEnableMenuItems(BOOL bEnable);
	void SetStatus(LPCWSTR str);
	void SetKillAllRunningThreads();
	void ShowCompleteInformation();
	void SetRefreshFavouritesMenu();
	
	void NotScanedMessage();
	void NoIPSelectedMessage();
	void OnOptionsSavedimensions();
	
public:
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnSelchangeIpstyle();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnGotoNextalivehost();
	afx_msg void OnGotoNextdeadhost();
	afx_msg void OnGotoPreviousalivehost();
	afx_msg void OnGotoPreviousdeadhost();
	afx_msg void OnGotoFind();
	afx_msg void OnFavoritesAddcurrent();
	afx_msg void OnFavouritesDeletefavourite();
	afx_msg void OnExecuteFavouritesMenu(UINT nID);

private:
	afx_msg void OnOK();
	afx_msg void OnCancel();
	
public:
	afx_msg void OnScanQuit();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	HACCEL	m_hAccel;
public:
	afx_msg void OnCommandsShowdetails();
	afx_msg void OnScanExportall();
	afx_msg void OnScanExportselection();
	afx_msg void OnCommandsRescanip();
	afx_msg void OnCommandsDeleteip();
	afx_msg void OnCommandsCopyip();
	afx_msg void OnCommandsCopydetails();
	afx_msg void OnToolsPreferences();
	afx_msg void OnToolsFetchers();
	afx_msg void OnHelpAbout();
	afx_msg void OnBnClickedBtnPreferences();
	afx_msg void OnBnClickedBtnSelectfetchers();


public:
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
public:
	CButton m_button_start;
	CButton m_button_pref;
	CButton m_button_fetcher;
};
