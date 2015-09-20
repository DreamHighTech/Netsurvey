// ScanUtilsInternal.cpp: implementation of the CScanUtilsInternal class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Netsurvey.h"
#include "ScanUtilsInternal.h"
#include "Scanner.h"
#include "ms_icmp.h"
#include "NetBIOSUtils.h"
#include "globals.h"
#include "clientSocket.h"
#include <stdio.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// For pinging

FARPROC lpfnIcmpCreateFile;
typedef BOOL (FAR WINAPI *TIcmpCloseHandle)(HANDLE IcmpHandle);
TIcmpCloseHandle lpfnIcmpCloseHandle;
typedef DWORD (FAR WINAPI *TIcmpSendEcho)(
	HANDLE IcmpHandle, 	/* handle returned from IcmpCreateFile() */
    u_long DestAddress, /* destination IP address (in network order) */
    LPVOID RequestData, /* pointer to buffer to send */
    WORD RequestSize,	/* length of data in buffer */
    IP_OPTION_INFORMATION* RequestOptns,  /* see Note 2 */
    LPVOID ReplyBuffer, /* see Note 1 */
    DWORD ReplySize, 	/* length of reply (must allow at least 1 reply) */
    DWORD Timeout 	/* time in milliseconds to wait for reply */
);
TIcmpSendEcho lpfnIcmpSendEcho;

char aPingDataBuf[32];

//////////////////////////////////////////////////////////////////////////////////
// PING
//////////////////////////////////////////////////////////////////////////////////

BOOL ScanIntInitPing()
{
	if (!lpfnIcmpCloseHandle) // if not already done
	{
		HMODULE hICMP = LoadLibrary(_T("ICMP.DLL"));
		if (!hICMP) 
		{
			CString szTmp;
			szTmp = _T("ICMP.DLL is not found. Program will not work.\n");
			MessageBox(0, szTmp,__T("Fatal Error"),MB_OK | MB_ICONHAND);
			exit(666);
		}

		lpfnIcmpCreateFile  = (FARPROC)GetProcAddress(hICMP,"IcmpCreateFile");
		lpfnIcmpCloseHandle = (TIcmpCloseHandle)GetProcAddress(hICMP,"IcmpCloseHandle");
		lpfnIcmpSendEcho    = (TIcmpSendEcho)GetProcAddress(hICMP,"IcmpSendEcho");

		// Fill data buffer for pinging
		for (int i=0; i < sizeof(aPingDataBuf); i++) aPingDataBuf[i]=i+65;
	}	

	return TRUE;
}

// Return values:
//  -1: dead
// >=0: alive (ping time)
int ScanIntDoPing(DWORD nIP, LPSTR szReturn, int nBufferLen)
{
	int nAlive = -1;	// Dead

	int nPingTime = 1000000; // A kind of infinity

	HANDLE hICMP = (HANDLE) lpfnIcmpCreateFile();

	unsigned char RepData[sizeof(IP_OPTION_INFORMATION*)+100];

	// Ping a few times
	for (int nPingCount = 1; nPingCount <= g_options->m_nPingCount; nPingCount++)
	{
		IP_OPTION_INFORMATION IPInfo;
		IPInfo.Ttl = 128;
		IPInfo.Tos = 0;
		IPInfo.Flags = 0;
		IPInfo.OptionsSize = 0;
		IPInfo.OptionsData = NULL;
		DWORD ReplyCount;
		ReplyCount = lpfnIcmpSendEcho(hICMP, nIP, &aPingDataBuf, sizeof(aPingDataBuf), 
			&IPInfo, RepData, sizeof(RepData), g_options->m_nPingTimeout);	

		if (ReplyCount) 	
		{
			ReplyCount = RepData[4]+RepData[5]*256+RepData[6]*65536+RepData[7]*256*65536;
			if (ReplyCount <= 0) 
			{					
				if (nPingTime < 1000000)	// 1000 secs, a kind of infinity
				{
					nPingTime = (nPingTime + *(u_long *) &(RepData[8])) / 2;	// Arithmetics average					
				}
				else
				{
					nPingTime = *(u_long *) &(RepData[8]);
				}
				
				nAlive = nPingTime;
			}
		}
	}

	char cstr[32];
	memset(cstr, 0x0, sizeof(cstr));
	wcstombs(cstr, g_options->m_szNotScannedValue, g_options->m_szNotScannedValue.GetLength());

	if (nBufferLen > 10)	// Check to not overflow the string buffer
	{
		if (nAlive >= 0)
		{
			if (nPingTime == 0)
				sprintf(szReturn,"%dms", nPingTime);
			else
				sprintf(szReturn,"%dms", nPingTime);
		}
		else
		{
			strcpy(szReturn, cstr);
		}
	}

	lpfnIcmpCloseHandle(hICMP);

	return nAlive;	// -1: dead, >=0: alive
}

BOOL ScanIntInfoPing(TInfoStruct *pInfoStruct)
{
	wcscpy((TCHAR*)&pInfoStruct->szPluginName, _T("Ping"));
	wcscpy((TCHAR*)&pInfoStruct->szDescription, _T("Shows the Ping time in milliseconds or \"Dead\" if host is not responding."));
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Ping TTL
//////////////////////////////////////////////////////////////////////////////////

BOOL ScanIntDoTTL(DWORD nIP, LPSTR szReturn, int nBufferLen)
{
	// This is just a slightly modified copy of Ping function

	BOOL bAlive = FALSE;

	HANDLE hICMP = (HANDLE) lpfnIcmpCreateFile();

	char RepData[sizeof(IP_OPTION_INFORMATION*)+100];

	// Issue one additional ping
	
	IP_OPTION_INFORMATION IPInfo;
	IPInfo.Ttl = 128;
	IPInfo.Tos = 0;
	IPInfo.Flags = 0;
	IPInfo.OptionsSize = 0;
	IPInfo.OptionsData = NULL;
	DWORD ReplyCount;
	ReplyCount = lpfnIcmpSendEcho(hICMP, nIP, &aPingDataBuf, sizeof(aPingDataBuf), 
		&IPInfo, RepData, sizeof(RepData), g_options->m_nPingTimeout);	

	if (ReplyCount)
	{
		ReplyCount = RepData[4]+RepData[5]*256+RepData[6]*65536+RepData[7]*256*65536;
		if (ReplyCount <= 0) 
		{								
			bAlive = (*(u_long *) &(RepData[8]) >= 0);
		}
	}

	lpfnIcmpCloseHandle(hICMP);

	if (nBufferLen > 10)	// Check to not overflow the string buffer
	{
		if (bAlive)
		{
			sprintf(szReturn,"%u", (unsigned)RepData[20]);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL ScanIntInfoTTL(TInfoStruct *pInfoStruct)
{
	wcscpy((TCHAR*)&pInfoStruct->szPluginName, _T("TTL"));
	wcscpy((TCHAR*)&pInfoStruct->szDescription, _T("Issues an additional \"ping\" and displays TTL value of the reply packet."));
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////
// DUMMY
/////////////////////////////////////////////////////////////////////////////////

BOOL ScanIntDoDummy(DWORD nIP, LPSTR szReturn, int nBufferLen)
{
	szReturn[0] = 0;
	return TRUE;
}

BOOL ScanIntInitDummy()
{
	return TRUE;
}

BOOL ScanIntInfoDummy(TInfoStruct *pInfoStruct)
{
	memset(pInfoStruct, 0, sizeof(TInfoStruct));
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// HOSTNAME
//////////////////////////////////////////////////////////////////////////////////

BOOL ScanIntDoHostname(DWORD nIP, LPSTR szReturn, int nBufferLen)
{
	hostent *he = gethostbyaddr((char*)&nIP, 4, 0);

	char cstr[32];
	memset(cstr, 0x0, sizeof(cstr));
	wcstombs(cstr, g_options->m_szNotScannedValue, g_options->m_szNotScannedValue.GetLength());

	if (he) 
	{
		if (nBufferLen > 0 && strlen(he->h_name) > (unsigned int) nBufferLen)
			he->h_name[nBufferLen-1] = 0;

		strcpy(szReturn, he->h_name);		
	} 
	else
	{
		strcpy(szReturn, cstr);
	}
	return TRUE;
}

BOOL ScanIntInfoHostname(TInfoStruct *pInfoStruct)
{
	wcscpy(pInfoStruct->szPluginName, _T("Hostname"));
	wcscpy(pInfoStruct->szDescription, _T("It is resolving hostnames of IP addresses using the DNS reverse lookup"));
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////
// NETBIOS
/////////////////////////////////////////////////////////////////////////////////

// Init / Finalization

// Computer Name

BOOL ScanIntDoNetBIOSComputerName(DWORD nIP, LPSTR szReturn, int nBufferLen)
{
	CString szComputerName;	
	CNetBIOSUtils::GetNames(nIP, NULL, &szComputerName, NULL, NULL);
	if (szComputerName.GetLength() > nBufferLen)
		szComputerName.SetAt(nBufferLen - 1, 0);
	char cstr[512]; memset(cstr, 0x0, sizeof(cstr));
	wcstombs(cstr, szComputerName.GetString(), sizeof(szComputerName.GetString()));
	strcpy(szReturn, cstr);
	return TRUE;
}

BOOL ScanIntInfoNetBIOSComputerName(TInfoStruct *pInfoStruct)
{
	wcscpy(pInfoStruct->szPluginName, _T("Comp. Name"));
	wcscpy(pInfoStruct->szDescription, _T("Gets the NetBIOS Computer Name (works mostly in LANs)"));
	return TRUE;	
}

// Group Name

BOOL ScanIntDoNetBIOSGroupName(DWORD nIP, LPSTR szReturn, int nBufferLen)
{
	CString szGroupName;	
	CNetBIOSUtils::GetNames(nIP, NULL, NULL, &szGroupName, NULL);
	if (szGroupName.GetLength() > nBufferLen)
		szGroupName.SetAt(nBufferLen - 1, 0);
	char cstr[512]; memset(cstr, 0x0, sizeof(cstr));
	wcstombs(cstr, szGroupName.GetString(), sizeof(szGroupName.GetString()));
	strcpy(szReturn, cstr);
	return TRUE;	
}

BOOL ScanIntInfoNetBIOSGroupName(TInfoStruct *pInfoStruct)
{
	wcscpy(pInfoStruct->szPluginName, _T("Group Name"));
	wcscpy(pInfoStruct->szDescription, _T("Gets NetBIOS Group Name (works mostly in LANs)"));
	return TRUE;		
}

// User Name

BOOL ScanIntDoNetBIOSUserName(DWORD nIP, LPSTR szReturn, int nBufferLen)
{
	CString szUserName;	
	CNetBIOSUtils::GetNames(nIP, &szUserName, NULL, NULL, NULL);
	if (szUserName.GetLength() > nBufferLen)
		szUserName.SetAt(nBufferLen - 1, 0);
	char cstr[512]; memset(cstr, 0x0, sizeof(cstr));
	wcstombs(cstr, szUserName.GetString(), sizeof(szUserName.GetString()));
	strcpy(szReturn, cstr);
	return TRUE;	
}

BOOL ScanIntInfoNetBIOSUserName(TInfoStruct *pInfoStruct)
{
	wcscpy(pInfoStruct->szPluginName, _T("User Name"));
	wcscpy(pInfoStruct->szDescription, _T("Gets NetBIOS User Name (just a guess, works mostly in LANs)"));
	return TRUE;		
}

// Mac Address

BOOL ScanIntDoNetBIOSMacAddress(DWORD nIP, LPSTR szReturn, int nBufferLen)
{
	CString szMacAddress;	
	CNetBIOSUtils::GetNames(nIP, NULL, NULL, NULL, &szMacAddress);
	if (szMacAddress.GetLength() > nBufferLen)
		szMacAddress.SetAt(nBufferLen - 1, 0);
	char cstr[512]; memset(cstr, 0x0, sizeof(cstr));
	wcstombs(cstr, szMacAddress.GetString(), sizeof(szMacAddress.GetString()));
	strcpy(szReturn, cstr);
	return TRUE;	
}

BOOL ScanIntInfoNetBIOSMacAddress(TInfoStruct *pInfoStruct)
{
	wcscpy(pInfoStruct->szPluginName, _T("MAC Address"));
	wcscpy(pInfoStruct->szDescription, _T("Gets Mac Address (works through NetBIOS and mostly in LANs)"));
	return TRUE;		
}

/* tcp connect */
#define HTTP_PORT 80
#define FTP_PORT 21
#define SMTP_PORT 25
#define POP2_PORT 109
#define POP3_PORT 110
#define IMAP_PORT 143
#define FINGER_PORT 79

BOOL ScanIntInfoHttpConnect(TInfoStruct *pInfoStruct)
{
	wcscpy(pInfoStruct->szPluginName, _T("HTTP(80)"));
	wcscpy(pInfoStruct->szDescription, _T("Gets HTTP Connect Status"));
	return TRUE;
}
BOOL ScanIntDoHttpConnect(DWORD nIP, LPSTR szReturn, int nBufferLen)
{
	strcpy(szReturn, "Alive");
	sockaddr_in addr;
	in_addr in;
	in.S_un.S_addr = nIP;
	addr.sin_addr = in;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(HTTP_PORT);

	clientSocket *nSocket = NULL;
	bool nConnect = false;
	nSocket = new clientSocket();
	nConnect = nSocket->CreateTCPClient(&addr);
	free(nSocket);
	return nConnect;
}

BOOL ScanIntInfoFtpConnect(TInfoStruct *pInfoStruct)
{
	wcscpy(pInfoStruct->szPluginName, _T("FTP(21)"));
	wcscpy(pInfoStruct->szDescription, _T("Gets FTP Connect Status"));
	return TRUE;
}
BOOL ScanIntDoFtpConnect(DWORD nIP, LPSTR szReturn, int nBufferLen)
{
	strcpy(szReturn, "Alive");
	sockaddr_in addr;
	in_addr in;
	in.S_un.S_addr = nIP;
	addr.sin_addr = in;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(FTP_PORT);

	clientSocket *nSocket = NULL;
	bool nConnect = false;
	nSocket = new clientSocket();
	nConnect = nSocket->CreateTCPClient(&addr);
	free(nSocket);
	return nConnect;
}

BOOL ScanIntInfoSmtpConnect(TInfoStruct *pInfoStruct)
{
	wcscpy(pInfoStruct->szPluginName, _T("SMTP(25)"));
	wcscpy(pInfoStruct->szDescription, _T("Gets SMTP Connect Status"));
	return TRUE;
}
BOOL ScanIntDoSmtpConnect(DWORD nIP, LPSTR szReturn, int nBufferLen)
{
	strcpy(szReturn, "Alive");
	sockaddr_in addr;
	in_addr in;
	in.S_un.S_addr = nIP;
	addr.sin_addr = in;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SMTP_PORT);

	clientSocket *nSocket = NULL;
	bool nConnect = false;
	nSocket = new clientSocket();
	nConnect = nSocket->CreateTCPClient(&addr);
	free(nSocket);
	return nConnect;
}

BOOL ScanIntInfoPop2Connect(TInfoStruct *pInfoStruct)
{
	wcscpy(pInfoStruct->szPluginName, _T("POP2(109)"));
	wcscpy(pInfoStruct->szDescription, _T("Gets POP2 Connect Status"));
	return TRUE;
}
BOOL ScanIntDoPop2Connect(DWORD nIP, LPSTR szReturn, int nBufferLen)
{
	strcpy(szReturn, "Alive");
	sockaddr_in addr;
	in_addr in;
	in.S_un.S_addr = nIP;
	addr.sin_addr = in;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(POP2_PORT);

	clientSocket *nSocket = NULL;
	bool nConnect = false;
	nSocket = new clientSocket();
	nConnect = nSocket->CreateTCPClient(&addr);
	free(nSocket);
	return nConnect;
}

BOOL ScanIntInfoPop3Connect(TInfoStruct *pInfoStruct)
{
	wcscpy(pInfoStruct->szPluginName, _T("POP3(110)"));
	wcscpy(pInfoStruct->szDescription, _T("Gets POP3 Connect Status"));
	return TRUE;
}
BOOL ScanIntDoPop3Connect(DWORD nIP, LPSTR szReturn, int nBufferLen)
{
	strcpy(szReturn, "Alive");
	sockaddr_in addr;
	in_addr in;
	in.S_un.S_addr = nIP;
	addr.sin_addr = in;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(POP3_PORT);

	clientSocket *nSocket = NULL;
	bool nConnect = false;
	nSocket = new clientSocket();
	nConnect = nSocket->CreateTCPClient(&addr);
	free(nSocket);
	return nConnect;
}

BOOL ScanIntInfoImapConnect(TInfoStruct *pInfoStruct)
{
	wcscpy(pInfoStruct->szPluginName, _T("IMAP(143)"));
	wcscpy(pInfoStruct->szDescription, _T("Gets IMAP Connect Status"));
	return TRUE;
}

BOOL ScanIntDoImapConnect(DWORD nIP, LPSTR szReturn, int nBufferLen)
{
	strcpy(szReturn, "Alive");
	sockaddr_in addr;
	in_addr in;
	in.S_un.S_addr = nIP;
	addr.sin_addr = in;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(IMAP_PORT);

	clientSocket *nSocket = NULL;
	bool nConnect = false;
	nSocket = new clientSocket();
	nConnect = nSocket->CreateTCPClient(&addr);
	free(nSocket);
	return nConnect;
}

BOOL ScanIntInfoFingerConnect(TInfoStruct *pInfoStruct)
{
	wcscpy(pInfoStruct->szPluginName, _T("FINGER(79)"));
	wcscpy(pInfoStruct->szDescription, _T("Gets FINGER Connect Status"));
	return TRUE;
}

BOOL ScanIntDoFingerConnect(DWORD nIP, LPSTR szReturn, int nBufferLen)
{
	strcpy(szReturn, "Alive");
	sockaddr_in addr;
	in_addr in;
	in.S_un.S_addr = nIP;
	addr.sin_addr = in;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(FINGER_PORT);

	clientSocket *nSocket = NULL;
	bool nConnect = false;
	nSocket = new clientSocket();
	nConnect = nSocket->CreateTCPClient(&addr);
	free(nSocket);
	return nConnect;
}
