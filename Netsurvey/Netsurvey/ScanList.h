#pragma once
#include "afxcmn.h"
#include "CommonDlg.h"

class CNetsurveyDlg;

class CScanList :
	public CListCtrl
{
public:
	CScanList(CNetsurveyDlg *dlg);
	~CScanList(void);
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);

	CString m_szSearchFor;
	CNetsurveyDlg *m_dlg;
	CImageList m_nImgList;
	bool m_bSortAscending;
	BOOL m_bHeaderClicksDisabled;
	int	m_nSortColumn;

public:
	void GotoFind();
	void GoToNextAliveIP();
	void GoToNextDeadIP();
	void GoToPreviousAliveIP();
	void GoToPreviousDeadIP();
	int GetCurrentSelectedItem(BOOL bShowError = TRUE);
	void ShowErrorNothingSelected();
	void SetSelectedItem(int nItem);
	int GetColumnCount();
	void ZeroResultsForItem(int nItemIndex);
	void DeleteSelectedItems();
	void ShowIPDetails();
	void InitPostCreateStuff();
	void SetColumnUpdate();
	BOOL IsSortingAscending();
	void SetSortingAscending(bool flag);
	void CopyIPToClipboard();
	void CopyDetailToClipboard();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
};

