
#pragma once

#include "NetsurveyDlg.h"

// File type constants

#define FILE_TYPE_TXT			0
#define FILE_TYPE_CSV			1
#define FILE_TYPE_XML			2
#define FILE_TYPE_HTML			3
#define FILE_TYPE_IPPORT_LIST	4

class CSaveToFile
{
public:	
	BOOL m_bAppend;
	BOOL m_saveselection;
	CString m_filename;
	int m_filetype;	
	CNetsurveyDlg *m_dlg;

	void saveToCSV(FILE *fileHandle);
	void saveToTXT(FILE *fileHandle);
	void saveToHTML(FILE *fileHandle);
	void saveToXML(FILE *fileHandle);
	void saveToIPPortList(FILE *fileHandle);
	BOOL queryFilename();

	CSaveToFile(CNetsurveyDlg *dlg, LPWSTR start=NULL, LPWSTR end=NULL, BOOL bSaveSelection = FALSE, LPWSTR szFileName = NULL, int nFileFormat = -1, BOOL bAppend = FALSE);
	virtual ~CSaveToFile();

	CString m_nIpStart;
	CString m_nIpEnd;

};

