// SelectFetcherDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetsurveyDlg.h"
#include "Netsurvey.h"
#include "SelectFetcherDlg.h"
#include "Scanner.h"
#include "globals.h"
#include "afxdialogex.h"


// CSelectFetcherDlg dialog

IMPLEMENT_DYNAMIC(CSelectFetcherDlg, CDialog)

CSelectFetcherDlg::CSelectFetcherDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectFetcherDlg::IDD, pParent)
{

}

CSelectFetcherDlg::~CSelectFetcherDlg()
{
}

void CSelectFetcherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AVAILABLEFETCHER_LIST, m_AllFetchers);
	DDX_Control(pDX, IDC_SELECTEDFETCHER_LIST, m_SelectedFetchers);
}


BEGIN_MESSAGE_MAP(CSelectFetcherDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_DOWN, &CSelectFetcherDlg::OnMoveDown)
	ON_BN_CLICKED(IDC_BTN_UP, &CSelectFetcherDlg::OnMoveUp)
	ON_BN_CLICKED(IDC_BTN_SELECT, &CSelectFetcherDlg::OnSelect)
	ON_BN_CLICKED(IDC_BTN_DESELECT, &CSelectFetcherDlg::OnDeselect)
	ON_BN_CLICKED(IDOK, &CSelectFetcherDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSelectFetcherDlg message handlers


BOOL CSelectFetcherDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	memset(&m_naSelColumns, 0, sizeof(m_naSelColumns));

	CString szTmp;
	int i;
	int nAllColumnCount = g_scanner->getAllColumnsCount();
	m_nSelectedColumns = g_scanner->getColumnCount();

	// Init list boxes	
	for (i=CL_STATIC_COUNT; i < nAllColumnCount; i++)
	{
		g_scanner->getAllColumnName(i, szTmp);
		m_AllFetchers.AddString(szTmp);
	}

	for (i=CL_STATIC_COUNT; i < m_nSelectedColumns; i++)
	{
		m_naSelColumns[i] = g_scanner->getColumnReference(i);
	}

	RepopulateSelectedColumns();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectFetcherDlg::RepopulateSelectedColumns()
{
	m_SelectedFetchers.ResetContent();
	CString szTmp;

	for (int i = CL_STATIC_COUNT; i < m_nSelectedColumns; i++)
	{
		g_scanner->getAllColumnName(m_naSelColumns[i], szTmp);
		m_SelectedFetchers.AddString(szTmp);
	}
}

void CSelectFetcherDlg::OnMoveDown()
{
	BOOL baSelection[128];
	memset(&baSelection, FALSE, sizeof(baSelection));

	for (int nItem = m_nSelectedColumns - 1; nItem >= CL_STATIC_COUNT; nItem--)
	{
		// If selected, then process it
		if (m_SelectedFetchers.GetSel(nItem - CL_STATIC_COUNT))
		{
			if (nItem >= m_nSelectedColumns - 1)
				return;	// Don't move items, if one of them is first item

			int nTmp = m_naSelColumns[nItem + 1];
			m_naSelColumns[nItem + 1] = m_naSelColumns[nItem];
			m_naSelColumns[nItem] = nTmp;

			baSelection[nItem+1] = TRUE;
		}		
	}

	RepopulateSelectedColumns();

	for (int nItem = CL_STATIC_COUNT; nItem < m_nSelectedColumns; nItem++)
	{
		if (baSelection[nItem])
		{
			m_SelectedFetchers.SetSel(nItem - CL_STATIC_COUNT);
		}
	}
}


void CSelectFetcherDlg::OnMoveUp()
{
	BOOL baSelection[128];
	memset(&baSelection, FALSE, sizeof(baSelection));

	for (int nItem = CL_STATIC_COUNT; nItem < m_nSelectedColumns; nItem++)
	{
		// If selected, then process it
		if (m_SelectedFetchers.GetSel(nItem - CL_STATIC_COUNT))
		{
			if (nItem <= CL_STATIC_COUNT)
				return;	// Don't move items, if one of them is first item

			int nTmp = m_naSelColumns[nItem - 1];
			m_naSelColumns[nItem - 1] = m_naSelColumns[nItem];
			m_naSelColumns[nItem] = nTmp;

			baSelection[nItem-1] = TRUE;
		}		
	}

	RepopulateSelectedColumns();

	for (int nItem = CL_STATIC_COUNT; nItem < m_nSelectedColumns; nItem++)
	{
		if (baSelection[nItem])
		{
			m_SelectedFetchers.SetSel(nItem - CL_STATIC_COUNT);
		}
	}
}


void CSelectFetcherDlg::OnSelect()
{
	for (int i = CL_STATIC_COUNT; i < g_scanner->getAllColumnsCount(); i++)
	{
		if (m_AllFetchers.GetSel(i - CL_STATIC_COUNT))
		{
			// check if it is already selected
			BOOL bFound = FALSE;
			for (int j = CL_STATIC_COUNT; j < m_nSelectedColumns; j++)
			{
				if (m_naSelColumns[j] == i)
				{
					bFound = TRUE;
					break;
				}
			}

			if (!bFound)
			{
				m_naSelColumns[m_nSelectedColumns] = i;
				m_nSelectedColumns++;
			}
		}
	}

	RepopulateSelectedColumns();
}


void CSelectFetcherDlg::OnDeselect()
{
	int naSelected[128];	

	for (int i = CL_STATIC_COUNT; i < m_nSelectedColumns; i++)
	{
		if (m_SelectedFetchers.GetSel(i - CL_STATIC_COUNT))
			naSelected[i] = TRUE;
		else
			naSelected[i] = FALSE;
	}

	for (int nItem = CL_STATIC_COUNT; nItem < m_nSelectedColumns; nItem++)
	{
		// If selected, then process it
		if (naSelected[nItem])
		{			
			for (int i = nItem; i < m_nSelectedColumns; i++)
			{
				m_naSelColumns[i] = m_naSelColumns[i+1];
				naSelected[i] = naSelected[i+1];
			}
			m_nSelectedColumns--;
			nItem--;
		}		
	}

	RepopulateSelectedColumns();
}


void CSelectFetcherDlg::OnBnClickedOk()
{
	for (int i=CL_STATIC_COUNT; i < m_nSelectedColumns; i++)
	{
		g_scanner->m_Columns[i] = m_naSelColumns[i];
	}

	g_scanner->m_nColumns = m_nSelectedColumns;

	g_scanner->initListColumns(&((CNetsurveyDlg*) AfxGetApp()->GetMainWnd())->m_nIpList);
	CDialog::OnOK();
}

