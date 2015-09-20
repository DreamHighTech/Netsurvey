// ScanUtilsPlugin.cpp: implementation of the CScanUtilsPlugin class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Netsurvey.h"
#include "ScanUtilsPlugin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScanUtilsPlugin::CScanUtilsPlugin()
{
}

CScanUtilsPlugin::~CScanUtilsPlugin()
{
}

void CScanUtilsPlugin::loadFromDir(CArray<TScannerColumn, TScannerColumn&> &columns, int &nColumnCount, CString &szDir)
{
	// This function loads plugins from the specified directory

	WIN32_FIND_DATA fileData;
	HANDLE hFind;

	hFind = FindFirstFile(szDir + _T("\\*.dll"), &fileData);
	
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			CString szFileName = fileData.cFileName;

			// Load plugin
			HMODULE hPlugin = LoadLibrary(szDir + _T("\\") + szFileName);
			
			if (hPlugin != INVALID_HANDLE_VALUE)
			{
				// Let's check the type of plugin
				TInfoFunction *pInfoFunction = (TInfoFunction*) GetProcAddress(hPlugin, "Info");
				TInfoStruct infoStruct;
				
				if (pInfoFunction == NULL)
				{
					MessageBox(0, _T("No \"Info\" function exported from plugin:\n") + szDir + _T("\\") + szFileName + _T("\nPlease move it to another directory if this is not a plugin for SDR Free IP Scan."), NULL, MB_ICONHAND | MB_OK);
					continue;
				}
				
				// Initialize info structure
				memset(&infoStruct, 0, sizeof(infoStruct));
				infoStruct.nStructSize = sizeof(infoStruct);
				infoStruct.nUniqueIndex = 0;	// TODO: a true index must be set here
		
				// Call plugin's info function
				pInfoFunction(&infoStruct);

				// Verify target SDR Free IP Scan versiion
				if (infoStruct.nAngryIPScannerVersion > 218)	// TODO: 218 is hardcoded
				{
					MessageBox(0, _T("Plugin ") + szFileName + _T(" requires a newer version of SDR Free IP Scan!"), NULL, MB_ICONHAND | MB_OK);
					continue;
				}

				if (infoStruct.nPluginType != PLUGIN_TYPE_COLUMN)
					continue;	// Skip unknown types
								
				columns[nColumnCount].pInfoFunction = pInfoFunction;
				
				// Lookup optional functions
				columns[nColumnCount].pInitFunction = (TInitFunction*) GetProcAddress(hPlugin, "Init");
				columns[nColumnCount].pFinalizeFunction = (TFinalizeFunction*) GetProcAddress(hPlugin,"Finalize");				
				columns[nColumnCount].pOptionsFunction = (TOptionsFunction*) GetProcAddress(hPlugin, "Options");

				// This is a mandatory function for PLUGIN_TYPE_COLUMN
				if (infoStruct.nPluginType == PLUGIN_TYPE_COLUMN)
				{
					columns[nColumnCount].pScanFunction = (TScanFunction*) GetProcAddress(hPlugin, "Scan");

					if (columns[nColumnCount].pScanFunction == NULL)
					{
						MessageBox(0, _T("No \"Scan\" function exported from plugin: ") + szFileName, NULL, MB_ICONHAND | MB_OK);
						continue;
					}
				}

				columns[nColumnCount].bBuiltinColumn = FALSE;				

				// Accept this plugin
				nColumnCount++;
			}
		}
		while (FindNextFile(hFind, &fileData));
	}

	FindClose(hFind);
}

void CScanUtilsPlugin::load(CArray<TScannerColumn, TScannerColumn&> &columns, int &nColumnCount)
{
	// This function loads plugins from the HDD and initializes structures passed as parameters

	// Load from default directory
	loadFromDir(columns, nColumnCount, g_options->m_szExecutablePath);

	// Load from dedicated directory
	loadFromDir(columns, nColumnCount, g_options->m_szExecutablePath + _T("\\") + PLUGINS_DIR);
}
