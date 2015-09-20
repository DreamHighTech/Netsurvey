// IPRangeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Netsurvey.h"
#include "AbstractIPStyleDlg.h"
#include "NetsurveyDlg.h"
#include "AbstractIPStyle.h"
//#include "IPRangeIPStyle.h"


IMPLEMENT_DYNAMIC(CAbstractIPStyleDlg, CDialogEx)
CAbstractIPStyleDlg::CAbstractIPStyleDlg(int nIDD, CWnd* pParent /*=NULL*/)
	: CDialogEx(nIDD, pParent)
{
	m_pToolTips = NULL;
}

BEGIN_MESSAGE_MAP(CAbstractIPStyleDlg, CDialogEx)
END_MESSAGE_MAP()

BOOL CAbstractIPStyleDlg::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	
	// Initialize tooltips
	m_pToolTips = new CToolTipCtrl;
	m_pToolTips->Create(this);
	
	return TRUE;  
}

CAbstractIPStyleDlg::~CAbstractIPStyleDlg() 
{
	if (m_pToolTips != NULL)
	{
		delete(m_pToolTips);
		m_pToolTips = NULL;
	}
}

BOOL CAbstractIPStyleDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (m_pToolTips != NULL)
		m_pToolTips->RelayEvent(pMsg);

	return CDialogEx::PreTranslateMessage(pMsg);
}

CWnd * CAbstractIPStyleDlg::SetFocus()
{
	GetNextDlgTabItem(this)->SetFocus();
	return this;
}
