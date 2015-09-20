// SaveToFile.cpp: implementation of the CSaveToFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Netsurvey.h"
#include "NetsurveyDlg.h"
#include "SaveToFile.h"
#include "globals.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSaveToFile::CSaveToFile(CNetsurveyDlg *dlg, LPWSTR start, LPWSTR end, BOOL bSaveSelection, LPWSTR szFileName, int nFileFormat, BOOL bAppend)
{
	m_dlg = dlg;
	if (start)
		m_nIpStart = start;
	else
		m_nIpStart = _T("");

	if (end)
		m_nIpEnd = end;
	else
		m_nIpEnd = _T("");

	m_bAppend = bAppend;
	
	// Display this error message only if wasn't invoked via command-line
	if (!dlg->m_pSzDefaultFileName && dlg->m_nIpList.GetItemCount() == 0) 
	{
		m_dlg->NotScanedMessage();
		return;
	}

	m_saveselection = bSaveSelection;
	if (bSaveSelection && dlg->m_nIpList.GetSelectedCount()==0)
	{		
		return;
	}
	
	BOOL bResult;
	
	if (szFileName == NULL) 
	{
		bResult = queryFilename();
	}
	else
	{
		bResult = TRUE;
		m_filename = szFileName;
	}

	if (nFileFormat >= 0) 
		m_filetype = nFileFormat;
	
	if (bResult)
	{
		// Start saving!
		m_dlg->SetStatus(_T("Exporting..."));

		FILE *fileHandle;
		if (m_bAppend)
			fileHandle = _wfopen(m_filename,_T("at"));
		else
			fileHandle = _wfopen(m_filename,_T("wt"));
	
		if (!fileHandle) 
		{
			m_dlg->MessageBox(_T("Cannot open file for writing"), NULL, MB_OK | MB_ICONHAND);
			return;
		}	

		switch (m_filetype)
		{	
			case FILE_TYPE_CSV:
				saveToCSV(fileHandle);
				break;
			case FILE_TYPE_HTML:
				saveToHTML(fileHandle);
				break;
			case FILE_TYPE_XML:
				saveToXML(fileHandle);
				break;
			case FILE_TYPE_IPPORT_LIST:
				saveToIPPortList(fileHandle);
				break;
			default:
				saveToTXT(fileHandle);
				break;
		}		

		// At this point the file has been processed
		fclose(fileHandle);

		// Open the newly saved file only if run by user (not command-line)
		if (szFileName == NULL)
		{
			ShellExecute(0, _T("open"), m_filename, NULL, NULL, SW_SHOWNORMAL);
		}

		m_dlg->SetStatus(NULL);
	}

	m_dlg->FlashWindow(TRUE);
}

CSaveToFile::~CSaveToFile()
{
	
}

void CSaveToFile::saveToTXT(FILE *fileHandle)
{	
	if (!m_bAppend)
	{
	}

	fputs("\nScanned ", fileHandle);
	fwprintf(fileHandle, m_nIpStart);
	fputs(" - ", fileHandle);
	fwprintf(fileHandle, m_nIpEnd);
	
	char cstr[128];
	memset(cstr, 0x0, sizeof(cstr));
	wcstombs(cstr,  g_options->getCurrentDate(), sizeof(cstr));
	fputs("\n", fileHandle);
	fputs(cstr, fileHandle);
	fputs("\n\n", fileHandle);
	
	int i,j;
	int ws[100];

	CString tmp;
	LV_ITEM it;

	// Output column names
	
	for (i=0; i < g_scanner->getColumnCount(); i++) 
	{
		ws[i] = m_dlg->m_nIpList.GetColumnWidth(i) / 8 + 10;
		
		if (!m_bAppend)
		{			
			g_scanner->getColumnName(i, tmp);
			fwprintf(fileHandle,_T("%-*s"),ws[i],tmp);
		}
	}

	if (!m_bAppend)
		fputs("\n\n", fileHandle);
	

	// Output data
	for (i=0; i < m_dlg->m_nIpList.GetItemCount(); i++) 
	{
		it.mask = LVIF_STATE; 
		it.stateMask = LVIS_SELECTED;
		it.iItem = i; 
		it.iSubItem = CL_IP; 

		m_dlg->m_nIpList.GetItem(&it); 
		if (m_saveselection && it.state != LVIS_SELECTED) 
			continue;	// Skip not selected items

		for (j=0; j < g_scanner->getColumnCount(); j++) 
		{
			fwprintf(fileHandle, _T("%-*s"),ws[j], m_dlg->m_nIpList.GetItemText(i,j));
		}

		fputs("\n", fileHandle);
	}
}

void CSaveToFile::saveToHTML(FILE *fileHandle)
{

	if (m_bAppend)
	{
		m_dlg->MessageBox(_T("Appending to HTML files is not supported!"), NULL, MB_OK | MB_ICONHAND);
		return;
	}
	
	// Output HTML head
	fputs("<html>\n<head>\n", fileHandle);
	fputs("<title>SDR Free IP Scan: scanning results</title>\n", fileHandle);

	// some CSS magic ;-)
	fputs("<style>body, td { font-family: arial; font-size: 12px; }  </style>\n", fileHandle);

	char cstr[512]; memset(cstr, 0x0, sizeof(cstr));
	fputs("<meta name=\"Generator\" content=\"SDR Free IP Scan, ", fileHandle);
	fputs("\">", fileHandle);
	fputs("</head>\n", fileHandle);
	
	// The BODY!
	fputs("<body>\n", fileHandle);
	
	// beginning	
	fputs("<h4>Scanned ", fileHandle);	
	in_addr inAddr;	
	inAddr.S_un.S_addr = htonl(0/* TODO: fix g_nStartIP*/); fputs(inet_ntoa(inAddr), fileHandle);
	fputs(" - ", fileHandle);
	inAddr.S_un.S_addr = htonl(0/* TODO: fix g_nEndIP*/); fputs(inet_ntoa(inAddr), fileHandle);
	memset(cstr, 0x0, sizeof(cstr));

	fputs("</h4>\n", fileHandle);
	memset(cstr, 0x0, sizeof(cstr));
	wcstombs(cstr, g_options->getCurrentDate(), sizeof(g_options->getCurrentDate()));
	fputs(cstr, fileHandle);
	fputs("<br><br>\n", fileHandle);

	// the table
	fputs("<table border=\"1\" cellspacing=\"0\" cellpadding=\"2\">\n<tr>\n", fileHandle);
	
	int i,j;	

	CString tmp;
	CString szValue;
	LV_ITEM it;

	// Output column names
	for (i=0; i < g_scanner->getColumnCount(); i++) 
	{			
		g_scanner->getColumnName(i, tmp);			
		fprintf(fileHandle,"<td><b>%s</b></td>\n", tmp);
	}	

	// Output data
	for (i=0; i < m_dlg->m_nIpList.GetItemCount(); i++) 
	{
		fputs("</tr>\n<tr>\n", fileHandle);

		it.mask = LVIF_STATE; 
		it.stateMask = LVIS_SELECTED;
		it.iItem = i; 
		it.iSubItem = CL_IP; 

		m_dlg->m_nIpList.GetItem(&it); 
		if (m_saveselection && it.state != LVIS_SELECTED) 
			continue;	// Skip not selected items

		for (j=0; j < g_scanner->getColumnCount(); j++) 
		{
			szValue = m_dlg->m_nIpList.GetItemText(i,j);
			szValue.Replace(_T("&"), _T("&amp;"));
			szValue.Replace(_T("<"), _T("&lt;"));
			szValue.Replace(_T(">"), _T("&gt;"));
			fprintf(fileHandle, "<td>%s</td>\n", szValue);
		}	
	}

	// Finish the html
	fputs("</tr>\n", fileHandle);
	fputs("</table>\n", fileHandle);
	fputs("<br><br>Generated by <a href=\"", fileHandle);
	fputs("\">SDR Free IP Scan</a>\n", fileHandle);
	fputs("</body>\n</html>\n", fileHandle);
}

void CSaveToFile::saveToXML(FILE *fileHandle)
{
	if (m_bAppend)
	{
		m_dlg->MessageBox(_T("Appending to XML files is not supported!"), NULL, MB_OK | MB_ICONHAND);
		return;
	}
	
	// Output XML header
	fputs("<?xml version=\"1.0\"?>\n", fileHandle);
	fputs("<scanning_report>\n", fileHandle);
	
	// beginning	
	fputs("\t<options>\n", fileHandle);	
	in_addr inAddr;	
	inAddr.S_un.S_addr = htonl(0/* TODO: fix g_nStartIP*/); 
	fwprintf(fileHandle, _T("\t\t<start_ip>%s</start_ip>\n"), inet_ntoa(inAddr));	
	inAddr.S_un.S_addr = htonl(0/* TODO: fix g_nEndIP*/); 
	fwprintf(fileHandle, _T("\t\t<end_ip>%s</end_ip>\n"), inet_ntoa(inAddr));
	
	// Print time
	fprintf(fileHandle, "\t\t<time>%s</time>\n", g_options->getCurrentDate());
	
	fputs(
		"\t\t<generator>\n"
		"\t\t\t<name>SDR Free IP Scan</name>\n"
		"\t\t\t<url>", fileHandle);	
	fputs("</url>\n"
		"\t\t</generator>\n", fileHandle);	

	fputs("\t</options>\n", fileHandle);
	
	int i,j;	

	CString szTmp;
	CString szValue;
	LV_ITEM it;

	fputs("\t<ip_list>\n", fileHandle);	
	
	// Output data
	
	for (i=0; i < m_dlg->m_nIpList.GetItemCount(); i++) 
	{			
		it.mask = LVIF_STATE; 
		it.stateMask = LVIS_SELECTED;
		it.iItem = i; 
		it.iSubItem = CL_IP; 

		m_dlg->m_nIpList.GetItem(&it); 
		if (m_saveselection && it.state != LVIS_SELECTED) 
			continue;	// Skip not selected items

		fputs("\t\t<ip>\n", fileHandle);

		for (j=0; j < g_scanner->getColumnCount(); j++) 
		{
			// Prepare column name
			g_scanner->getColumnName(j, szTmp);
			szTmp.MakeLower(); 
			szTmp.Replace(_T("."), _T(""));
			for (int nTCHAR = 0; nTCHAR < szTmp.GetLength(); nTCHAR++)
				if (szTmp[nTCHAR] < 'a' || szTmp[nTCHAR] > 'z')
					szTmp.SetAt(nTCHAR, '_');
			// Prepare column value
			szValue = m_dlg->m_nIpList.GetItemText(i,j);
			szValue.Replace(_T("&"), _T("&amp;"));
			szValue.Replace(_T("<"), _T("&lt;"));
			szValue.Replace(_T(">"), _T("&gt;"));
			// Output both
			fprintf(fileHandle, "\t\t\t<%s>%s</%s>\n", szTmp, szValue, szTmp); 				
		}			

		fputs("\t\t</ip>\n", fileHandle);
	}

	

	// Finish the html
	fputs("\t</ip_list>\n", fileHandle);
	fputs("</scanning_report>\n", fileHandle);	
}

void CSaveToFile::saveToCSV(FILE *fileHandle)
{
	int i,j;	

	CString tmp;	
	CString szValue;
	LV_ITEM it;

	// Output data
	for (i=0; i < m_dlg->m_nIpList.GetItemCount(); i++) 
	{
		it.mask = LVIF_STATE; 
		it.stateMask = LVIS_SELECTED;
		it.iItem = i; 
		it.iSubItem = CL_IP; 

		m_dlg->m_nIpList.GetItem(&it); 
		if (m_saveselection && it.state != LVIS_SELECTED) 
			continue;	// Skip not selected items

		for (j=0; j < g_scanner->getColumnCount(); j++) 
		{
			szValue = m_dlg->m_nIpList.GetItemText(i,j);
			szValue.Replace(',', ';');		// No extra commas for CSV format :-)
			fprintf(fileHandle, "%s", szValue);
			if (j < g_scanner->getColumnCount() -1) fputs(",", fileHandle);
		}

		fputs("\n", fileHandle);
	}	
}

void CSaveToFile::saveToIPPortList(FILE *fileHandle)
{
	int i;	
	CString szIP;	
	CString szPorts;
	LV_ITEM it;

	// Output data
	for (i=0; i < m_dlg->m_nIpList.GetItemCount(); i++) 
	{
		it.mask = LVIF_STATE; 
		it.stateMask = LVIS_SELECTED;
		it.iItem = i; 
		it.iSubItem = CL_IP; 

		m_dlg->m_nIpList.GetItem(&it); 
		if (m_saveselection && it.state != LVIS_SELECTED) 
			continue;	// Skip not selected items

		// Get the IP
		szIP = m_dlg->m_nIpList.GetItemText(i, 0);				
		
		// If there are any ports
		if (szPorts.GetLength() > 0 && szPorts[0] != 'N')
		{
			// Funny code to prefix each port with and IP address
			szPorts.Replace(_T(","), '\n' + szIP + ':');
			// Now add an IP for the first one
			szPorts = szIP + ':' + szPorts + '\n'; 
			char cstr[512]; memset(cstr, 0x0, sizeof(cstr));
			wcstombs(cstr, szPorts.GetString(), sizeof(szPorts.GetString()));
			fputs(cstr, fileHandle);
		}
	}	
}


BOOL CSaveToFile::queryFilename()
{
	CFileDialog fd(FALSE,_T("txt"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Text files (*.txt)|*.txt|")
		_T("Comma-separated files (*.csv)|*.csv|")
		_T("XML files (*.xml)|*.xml|") 
		_T("IP-Port List files (*.lst)|*.lst||"), 
		m_dlg);

	if (fd.DoModal() == IDOK)
	{		
		// Get filter index for determining the file format
		int nFilterIndex = fd.m_ofn.nFilterIndex;

		if (nFilterIndex == 2)
		{
			m_filetype = FILE_TYPE_CSV;
		}
		else
		if (nFilterIndex == 3)
		{
			m_filetype = FILE_TYPE_HTML;
		}
		else 
		if (nFilterIndex == 4)
		{
			m_filetype = FILE_TYPE_XML;			
		}
		else 
		if (nFilterIndex == 5)
		{
			m_filetype = FILE_TYPE_IPPORT_LIST;			
		}
		else
		{
			m_filetype = FILE_TYPE_TXT;
		}

		m_filename = fd.GetPathName();

		return TRUE;
	}
	else
	{
		return FALSE;
	}	

}




