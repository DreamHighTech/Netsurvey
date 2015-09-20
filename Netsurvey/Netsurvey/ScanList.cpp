#include "StdAfx.h"
#include "ScanList.h"
#include "Scanner.h"
#include "globals.h"
#include "IPDetailDlg.h"
#include "NetsurveyDlg.h"

CScanList::CScanList(CNetsurveyDlg *dlg)
{
	m_dlg = dlg;
	m_bSortAscending = false;
	m_bHeaderClicksDisabled = FALSE;
}

CScanList::~CScanList(void)
{
}

void CScanList::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rcItem(lpDrawItemStruct->rcItem);
	int nItem = lpDrawItemStruct->itemID;
	CImageList* pImageList;

	// Save dc state
	int nSavedDC = pDC->SaveDC();

	// Get item image and state info
	LV_ITEM lvi;
	lvi.mask = LVIF_IMAGE | LVIF_STATE;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.stateMask = 0xFFFF;		// get all state flags
	GetItem(&lvi);

	// Should the item be highlighted
	BOOL bHighlight =((lvi.state & LVIS_DROPHILITED)
				|| ( (lvi.state & LVIS_SELECTED)
					&& ((GetFocus() == this)
						|| (GetStyle() & LVS_SHOWSELALWAYS)
						)
					)
				);


	// Get rectangles for drawing
	CRect rcBounds, rcLabel, rcIcon;
	GetItemRect(nItem, rcBounds, LVIR_BOUNDS);
	GetItemRect(nItem, rcLabel, LVIR_LABEL);
	GetItemRect(nItem, rcIcon, LVIR_ICON);
	CRect rcCol;

	CString sLabel = GetItemText(nItem, 0);

	// Labels are offset by a certain amount  
	// This offset is related to the width of a space TCHARacter
	int offset = pDC->GetTextExtent(_T(" "), 1 ).cx * 2;

	CRect rcHighlight;
	CRect rcWnd;	
	
	rcHighlight = rcBounds;
	rcHighlight.left = rcLabel.left;
	
	// Draw the background color
	DWORD nColor;

	if( bHighlight )
	{		
		pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));

		nColor = ::GetSysColor(COLOR_HIGHLIGHT);		
	}
	else
	{
		nColor = ::GetSysColor(COLOR_WINDOW);

		if (nItem % 2 == 1) // Even
		{
			nColor -= 0x050505;		// subtract 0x20 from Blue byte to make it more "yellow"
		}				
	}

	pDC->FillRect(rcHighlight, &CBrush(nColor));

	if (bHighlight)
	{
		pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
	}
	else
	{
		pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
	}
	
	// Set clip region
	rcCol = rcBounds;
	rcCol.right = rcCol.left + GetColumnWidth(0);
	CRgn rgn;
	rgn.CreateRectRgnIndirect(&rcCol);
	pDC->SelectClipRgn(&rgn);
	rgn.DeleteObject();

	/*
	// Draw state icon
	if (lvi.state & LVIS_STATEIMAGEMASK)
	{
		int nImage = ((lvi.state & LVIS_STATEIMAGEMASK) >> 12) - 1;
		pImageList = GetImageList(LVSIL_STATE);
		if (pImageList)
		{
			pImageList->Draw(pDC, nImage,
				CPoint(rcCol.left, rcCol.top), ILD_TRANSPARENT);
		}
	}*/
	
	// Draw normal and overlay icon
	pImageList = GetImageList(LVSIL_SMALL);
	if (pImageList)
	{
		UINT nOvlImageMask=lvi.state & LVIS_OVERLAYMASK;
		pImageList->DrawEx(pDC, lvi.iImage, 
			CPoint(rcIcon.left-10, rcIcon.top), CSize(16,16), CLR_NONE, CLR_NONE, ILD_TRANSPARENT|ILD_SCALE);
		//pImageList->Draw(pDC, lvi.iImage,
		//	CPoint(rcIcon.left-10, rcIcon.top), ILD_NORMAL);
	}

	
	// Draw item label - Column 0
	rcLabel.left += offset/2;
	rcLabel.right -= offset;

	// Draw an IP address
	rcLabel.top += 2;
	pDC->DrawText(sLabel,-1,rcLabel,DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP 
				| DT_TOP | DT_END_ELLIPSIS);
	
	// Draw labels for remaining columns
	
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH;

	rcBounds.right = rcHighlight.right > rcBounds.right ? rcHighlight.right :
							rcBounds.right;
	rgn.CreateRectRgnIndirect(&rcBounds);
	pDC->SelectClipRgn(&rgn);
	rgn.DeleteObject();
			   	
	for(int nColumn = 1; GetColumn(nColumn, &lvc); nColumn++)
	{
		rcCol.left = rcCol.right;
		rcCol.right += lvc.cx;

		sLabel = GetItemText(nItem, nColumn);
		if (sLabel.GetLength() == 0)
			continue;

		// Get the text justification
		UINT nJustify = DT_LEFT;
		switch(lvc.fmt & LVCFMT_JUSTIFYMASK)
		{
		case LVCFMT_RIGHT:
			nJustify = DT_RIGHT;
			break;
		case LVCFMT_CENTER:
			nJustify = DT_CENTER;
			break;
		default:
			break;
		}

		rcLabel = rcCol;
		rcLabel.top += 2;
		rcLabel.left += offset;
		rcLabel.right -= offset;

		pDC->DrawText(sLabel, -1, rcLabel, nJustify | DT_SINGLELINE | 
					DT_NOPREFIX | DT_TOP | DT_END_ELLIPSIS);
	}

	// Draw focus rectangle if item has focus
	if (lvi.state & LVIS_FOCUSED && (GetFocus() == this))
		pDC->DrawFocusRect(rcHighlight);

	
	// Restore dc
	pDC->RestoreDC( nSavedDC );
}

void CScanList::GotoFind()
{
	CCommonDlg cCommonDlg;
	cCommonDlg.m_szCaption = _T("Find");
	cCommonDlg.m_szQueryText = _T("Enter the text to search for");
	cCommonDlg.m_nInitEditDlg = true;
	if (cCommonDlg.DoModal() == IDCANCEL) 
		return;
	m_szSearchFor = cCommonDlg.m_szDefaultUserText;
	
	SetFocus();	
	for (int i = 0; i < GetItemCount(); i++) 
	{
		for (int nCol = 0; nCol < GetColumnCount(); nCol++)
		{
			if (GetItemText(i, nCol).Find(m_szSearchFor) != -1) 
			{
				SetSelectedItem(i);
				return;
			}
		}
		
	}
	
	MessageBox(_T("Nothing was found."), _T("Find"), MB_OK | MB_ICONINFORMATION);
}

int CScanList::GetColumnCount()
{
	return GetHeaderCtrl()->GetItemCount();
}

void CScanList::GoToNextAliveIP()
{
	SetFocus();	
	int i = GetCurrentSelectedItem();
	if (i == -1)
		return;

	for (i++; i < GetItemCount(); i++) 
	{
		if (!(GetItemText(i, CL_PING) == g_options->m_szNotAvailableValue)) 
		{
			SetSelectedItem(i);
			break;
		}
	}
}

void CScanList::GoToNextDeadIP()
{
	SetFocus();	
	int i = GetCurrentSelectedItem();
	if (i == -1)
		return;

	for (i++; i < GetItemCount(); i++) 
	{
		if (GetItemText(i, CL_PING) == g_options->m_szNotAvailableValue) 
		{
			SetSelectedItem(i);
			break;
		}
	}
}

void CScanList::GoToPreviousAliveIP()
{
	SetFocus();	
	int i = GetCurrentSelectedItem();
	if (i == -1)
		return;

	for (i--; i >= 0; i--) 
	{
		if (!(GetItemText(i, CL_PING) == g_options->m_szNotAvailableValue)) 
		{
			SetSelectedItem(i);
			break;
		}
	}
}

void CScanList::GoToPreviousDeadIP()
{
	SetFocus();	
	int i = GetCurrentSelectedItem();
	if (i == -1)
		return;

	for (i--; i >= 0; i--) 
	{
		if (GetItemText(i, CL_PING) == g_options->m_szNotAvailableValue) 
		{
			SetSelectedItem(i);
			break;
		}
	}
}

int CScanList::GetCurrentSelectedItem(BOOL bShowError)
{
	POSITION pos = GetFirstSelectedItemPosition();
	int nCurrentItem = GetNextSelectedItem(pos);
	if (nCurrentItem < 0 && bShowError) 
	{ 
		ShowErrorNothingSelected(); 		
	}

	return nCurrentItem;
}

void CScanList::ShowErrorNothingSelected()
{
	
}

void CScanList::SetSelectedItem(int nItem)
{
	SetItemState(-1, 0, LVIS_SELECTED);
	SetItemState(nItem, LVIS_SELECTED | LVIS_FOCUSED,LVIS_SELECTED | LVIS_FOCUSED);
	EnsureVisible(nItem, FALSE);
}

void CScanList::ZeroResultsForItem(int nItemIndex)
{
	for (int i=CL_PING; i < GetColumnCount(); i++)
	{
		SetItem(nItemIndex, i, LVIF_TEXT, _T(""), 0, 0, 0, 0);
	}

	SetItem(nItemIndex, CL_IP, LVIF_IMAGE, NULL, 2, 0, 0, 0);	

	RedrawWindow();
}

void CScanList::DeleteSelectedItems()
{		
	int nItemIndex, nFirstItemIndex;
	POSITION pos;

	pos = GetFirstSelectedItemPosition();
	nFirstItemIndex = GetNextSelectedItem(pos);

	SetRedraw(FALSE);

	do 
	{
		pos = GetFirstSelectedItemPosition();
		nItemIndex = GetNextSelectedItem(pos);
		DeleteItem(nItemIndex);		
	}
	while (nItemIndex >= 0);

	SetSelectedItem(nFirstItemIndex);

	SetRedraw(TRUE);

	Invalidate();
	UpdateWindow();
}

void CScanList::ShowIPDetails()
{
	if (m_dlg->m_nIpList.GetItemCount() == 0)
	{
		m_dlg->NotScanedMessage();
		return;
	}

	int nCurrentItem = GetCurrentSelectedItem(FALSE);

	if (m_dlg->m_nIpList.GetCurrentSelectedItem(FALSE) == -1)
	{
		m_dlg->NoIPSelectedMessage();
		return;
	}	

	CIPDetailDlg cDlg(this);

	// Set columns
	int nColumns = g_scanner->getColumnCount();
	CString szInfoLine;

	for (int i=0; i < nColumns; i++)
	{
		g_scanner->getColumnName(i, szInfoLine);
		szInfoLine += _T(":");
		if (szInfoLine.GetLength() <= 8) szInfoLine += _T("\t");
		if (szInfoLine.GetLength() <= 20) szInfoLine += _T("\t");
		szInfoLine += GetItemText(nCurrentItem, i);

		cDlg.addScannedInfo((LPWSTR)szInfoLine.GetString());
	}

	cDlg.DoModal();
}

BEGIN_MESSAGE_MAP(CScanList, CListCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, &CScanList::OnLvnColumnclick)
END_MESSAGE_MAP()


void CScanList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	ShowIPDetails();

	CListCtrl::OnLButtonDblClk(nFlags, point);
}

void CScanList::InitPostCreateStuff()
{
	m_nImgList.Create(16, 16, ILC_COLOR24 | ILC_MASK, 2, 2);
	m_nImgList.Add(AfxGetApp()->LoadIcon(IDI_ICONSTATUSOK));
	m_nImgList.Add(AfxGetApp()->LoadIcon(IDI_ICONSTATUSFAIL));
	m_nImgList.Add(AfxGetApp()->LoadIcon(IDI_ICONSTATUSUNKNOWN));
	m_nImgList.Add(AfxGetApp()->LoadIcon(IDI_ICONSTATUS));

	// Add image list to the listbox (in case is not added yet)
	SetImageList(&m_nImgList, LVSIL_SMALL);	
}

void CScanList::SetColumnUpdate()
{
	int nCol = g_scanner->getColumnCount();	// Last column index

	if (GetColumnCount() > nCol)
	{
		// Remember the width
		AfxGetApp()->WriteProfileInt(_T(""), _T("Col_!OP!"), GetColumnWidth(nCol));

		// Now remove
		DeleteColumn(nCol);	// Delete the special column
	}
}

int CALLBACK SortCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) 
{
	CScanList *pList = (CScanList*) lParamSort;
	CHeaderCtrl *pListHeader = (CHeaderCtrl*) pList->GetHeaderCtrl();
	int nSortedColumn = pList->m_nSortColumn;

	CString strItem1 = pList->GetItemText(lParam1, nSortedColumn);
	CString strItem2 = pList->GetItemText(lParam2, nSortedColumn);   

	int nRet, n1, n2;
	char cstr[512];

	switch (nSortedColumn) 
	{
	case CL_IP:
		memset(cstr, 0x0, sizeof(cstr));
		wcstombs(cstr, strItem1, strItem1.GetLength());
		n1 = ntohl(inet_addr(cstr));
		memset(cstr, 0x0, sizeof(cstr));
		wcstombs(cstr, strItem2, strItem2.GetLength());
		n2 = ntohl(inet_addr(cstr));
		if (n1 > n2) nRet = 1; else if (n1 < n2) nRet = -1; else nRet = 0;
		break;
	case CL_PING:
		if (strItem1 == g_options->m_szNotAvailableValue)
			nRet = 1;
		else 
			if (strItem2 == g_options->m_szNotAvailableValue)
				nRet = -1;
			else
			{
				n1 = _wtoi(strItem1);
				n2 = _wtoi(strItem2);
				if (n1 > n2) nRet = 1; else if (n1 < n2) nRet = -1; else nRet = 0;
			}
			break;
	default:

		if (strItem1 == g_options->m_szNotAvailableValue || strItem1 == g_options->m_szNotScannedValue)
			strItem1 = "\xFF";	// Move it to the end

		if (strItem2 == g_options->m_szNotAvailableValue || strItem2 == g_options->m_szNotScannedValue)
			strItem2 = "\xFF";  // Move it to the end

		nRet = strItem1.CompareNoCase(strItem2);
		break;
	}

	return nRet * (pList->IsSortingAscending() ? 1 : -1);
}

BOOL CScanList::IsSortingAscending()
{
	return m_bSortAscending;
}

void CScanList::SetSortingAscending(bool flag)
{
	m_bSortAscending = flag;
}

void CScanList::CopyIPToClipboard()
{
	int nCurrentItem = GetCurrentSelectedItem();

	if (nCurrentItem < 0)
		return;

	CString szIP = GetItemText(nCurrentItem, CL_IP);
	int cch = szIP.GetLength();
	TCHAR *temp = new TCHAR[cch + 1];
	_tcscpy(temp, szIP.GetBuffer());

	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (cch + 1) * sizeof(TCHAR)); 
	if (hglbCopy == NULL)
		return;

	LPTSTR lp = (LPTSTR)GlobalLock(hglbCopy);
	memcpy(lp, temp, cch * sizeof(TCHAR));	
	lp[cch] = (TCHAR)0;
	GlobalUnlock(hglbCopy);

	OpenClipboard();
	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT,hglbCopy);
	CloseClipboard();
}

void CScanList::CopyDetailToClipboard()
{
	int nCurrentItem = GetCurrentSelectedItem();

	if (nCurrentItem < 0)
		return;

	// Set columns
	int nColumns = g_scanner->getColumnCount();
	CString szInfoLine, szInfoTotal;

	for (int i=0; i < nColumns; i++)
	{
		g_scanner->getColumnName(i, szInfoLine);
		szInfoTotal += szInfoLine + _T(":");
		if (szInfoLine.GetLength() <= 8) szInfoTotal += _T("\t");
		if (szInfoLine.GetLength() <= 20) szInfoTotal += _T("\t");
		szInfoTotal += GetItemText(nCurrentItem, i) + _T('\n');
	}

	int cch = szInfoTotal.GetLength();
	TCHAR *temp = new TCHAR[cch + 1];
	_tcscpy(temp, szInfoTotal.GetBuffer());

	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (cch + 1) * sizeof(TCHAR)); 
	if (hglbCopy == NULL)
		return;

	LPTSTR lp = (LPTSTR)GlobalLock(hglbCopy);
	memcpy(lp, temp, cch * sizeof(TCHAR));	
	lp[cch] = (TCHAR)0;
	GlobalUnlock(hglbCopy);

	OpenClipboard();
	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT,hglbCopy);
	CloseClipboard();
}

void CScanList::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	CHeaderCtrl *m_ctlHeader = (CHeaderCtrl*) GetHeaderCtrl();

	if (m_bHeaderClicksDisabled)
		return;		// Do not show anything during scanning

	m_nSortColumn = pNMLV->iSubItem;

	// Get the current on-screen mouse position
	POINT mousePos;
	GetCursorPos(&mousePos);

	// Translate mouse pos to the header pos
	RECT rcWin;
	m_ctlHeader->GetWindowRect(&rcWin);
	mousePos.x -= rcWin.left;	// Only horizontal coordinate is interesting for us

	// Get the rect of clicked item
	m_ctlHeader->GetItemRect(pNMLV->iSubItem, &rcWin);	

	// Detect which icon was clicked
	if (rcWin.right < mousePos.x)
	{
		if (rcWin.right > mousePos.x)
		{
			// Options icon			
			g_scanner->showColumnOptions(pNMLV->iSubItem, FALSE);
		}
		else
		{
			// Info icon
			g_scanner->showColumnInfo(pNMLV->iSubItem, FALSE);			
		}

		return;	// Do not proceed with sorting stuff
	}	

	// Quit there is no items to sort
	if (GetItemCount() <= 0)
	{
		//m_ctlHeader->SetSortArrow(-1, 0);	// Ensure that this is switched off
		return;
	}

	BOOL bSortAscending = IsSortingAscending();
	int nSortedColumn = m_nSortColumn;

	if(pNMLV->iSubItem == nSortedColumn)
		bSortAscending = !bSortAscending;
	else
		bSortAscending = TRUE;

	nSortedColumn = pNMLV->iSubItem;

	for (int i=0;i < GetItemCount();i++) 
	{
		SetItemData(i, i);
	}		

	//m_ctlHeader->SetSortArrow(nSortedColumn, bSortAscending);

	SortItems(&SortCompareFunc, (DWORD) this);
	SetSortingAscending(bSortAscending==TRUE);

	*pResult = 0;
}
