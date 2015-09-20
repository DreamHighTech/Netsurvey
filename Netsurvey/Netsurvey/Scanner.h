
#pragma once

#include "AbstractIPStyle.h"

#include <afxtempl.h>	// For CArray
#include "ScanList.h"
#include "plugin.h"		// Structures and defines for plugins

// Well-known columns
#define CL_IP					0
#define CL_PING					1
#define CL_HOSTNAME				2
#define CL_HTTP					3
#define CL_FTP					4
#define CL_SMTP					5
#define CL_POP2					6
#define CL_POP3					7
#define CL_IMAP					8
#define CL_FINGER				9

#define CL_STATIC_COUNT			10

// Second parameter to ThreadProc
#define IP_IS_GIVEN		TRUE
#define	INDEX_IS_GIVEN	FALSE


// Structure to hold all functions of a service (plugin)

typedef struct
{
	TScanFunction *pScanFunction;
	TInfoFunction *pInfoFunction;
	TOptionsFunction *pOptionsFunction;
	TInitFunction *pInitFunction;
	TFinalizeFunction *pFinalizeFunction;
	BOOL bBuiltinColumn;
	CString *pszPluginName;		
} 
TScannerColumn;


class CScanner  
{
public:	
	void showColumnOptions(int nColumn, BOOL bAllColumns);
	void showColumnInfo(int nColumn, BOOL bAllColumns);
	void runFinalizeFunction(int nIndex, BOOL bAllFunctions = FALSE);
	void runInitFunction(int nIndex, BOOL bAllFunctions = FALSE);
	void saveSelectedColumns();
	void loadSelectedColumns();
	void loadAllPossibleColumns();
	int getColumnReference(int nItemIndex);
	BOOL getAllColumnName(int nIndex, CString &szColumnHeader);
	int getAllColumnsCount();
	BOOL runScanFunction(DWORD nIP, int nIndex, TCHAR *szBuffer, int nBufferLength, BOOL bGlobal=FALSE);
	void initMenuWithColumns(CMenu *pMenu);
	int m_nAliveHosts;
	int m_nOpenPorts;
	BOOL finalizeScanning();
	BOOL initScanning();
	void initListColumns(CScanList *cListCtrl);	
	int getColumnWidth(int nIndex);
	BOOL getColumnName(int nIndex, CString &szColumnHeader);
	int getColumnCount();
	BOOL doScanIP(DWORD nParam, BOOL bParameterIsIP, int nThreadIndex);
	CScanner();
	virtual ~CScanner();

	int m_Columns[256];		// It is updated by Select columns dialog box
	int m_nColumns;
	CArray<TScannerColumn, TScannerColumn&> m_AllColumns;			
	int m_nAllColumns;

protected:		
	
	CWinApp * m_app;
};

