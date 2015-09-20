#pragma once

#include "stdafx.h"
#include <WinSock2.h>
#include <atlstr.h>

typedef int teDisplayOptions;
#define DISPLAY_ALL		0
#define DISPLAY_ALIVE	1
#define DISPLAY_OPEN	2

typedef struct {u_short nStartPort; u_short nEndPort; } tPortRange;

typedef	struct {CString szName; CString szContent; } tFavourite;

typedef	struct {CString szName; CString szExecute; CString szWorkDir; BOOL bCommandLine; } tOpener;

class COptions  
{
public:
	BOOL m_bSkipBroadcast;
	BOOL m_bAutoSave;
	void removeSettingsFromRegistry();
	void saveFavourites();
	void saveOpeners();
	void deleteFavourite();
	void addFavourite(const CString& szSerializedIPFeed);
	int m_nPortCount;
	BOOL m_bOptimizePorts;
	int m_nLanaNumber;
	CString getCurrentDate();
	int m_nPingCount;
	void saveDimensions();
	CString m_szExecutablePath;
	BOOL m_bScanHostIfDead;
	void setWindowPos();
	teDisplayOptions m_neDisplayOptions;
	int m_nPingTimeout;
	int m_nMaxThreads;
	int m_nTimerDelay;
	CString m_szNotAvailableValue;
	CString m_szNotScannedValue;
	BOOL m_bAskConfirm;
	BOOL m_bShowInfo;

	tPortRange *m_aParsedPorts;	// Array
	tFavourite m_aFavourites[250];
	tOpener m_aOpeners[100];
	BOOL setPortString(LPCWSTR szPortString);
	CString m_szPorts;
	COptions();
	virtual ~COptions();
	void load();
	void save();
	BOOL parsePortString();
	void initFavouritesMenu(CMenu *pMenu);
	void initOpenersMenu(CMenu *pMenu);
	
protected:
	void loadFavourites();
	void loadOpeners();
};

