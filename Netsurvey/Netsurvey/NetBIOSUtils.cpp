// NetBIOSUtils.cpp: implementation of the CNetBIOSUtils class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NetBIOSUtils.h"
#include "Scanner.h"
#include "Netsurvey.h"
#include "globals.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define NETBIOS_REQUEST "\xa2\x48\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x20\x43\x4b\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x00\x00\x21\x00\x01"

#pragma pack(push, 1)
typedef struct _node_status_resp 
{
	unsigned short trn_id;
	unsigned short flags1;
	unsigned short flags2;
	unsigned short flags3;
	unsigned short flags4;
	unsigned short flags5;
	TCHAR rr_name[0x22];
	unsigned short nbstat;  /* 0x0021 */
	unsigned short in;  /* 0x0001 */
	unsigned long zilch; /* 0 */
	unsigned short rd_length;
	unsigned char num_names;
	struct {
		unsigned char nb_name[16];
		unsigned short name_flags;
	} name_array[1];
} NS_RESP;
#pragma pack(pop)

#define GROUP_NAME_FLAG		128
#define NAME_TYPE_DOMAIN	0x00
#define NAME_TYPE_MESSENGER	0x03


CNetBIOSUtils::CNetBIOSUtils()
{	
	// Nothings here now
}

CNetBIOSUtils::~CNetBIOSUtils()
{
	// Nothing's here
}

BOOL CNetBIOSUtils::GetNames(DWORD nIP, CString *szUserName, CString *szComputerName, CString *szGroupName, CString *szMacAddress)
{
	in_addr in;
	in.S_un.S_addr = nIP;
	TCHAR ipa[512];
	char* cipa = inet_ntoa(in);
	memset(ipa, 0x0, sizeof(ipa));
	mbstowcs(ipa, cipa, 15);

	return CNetBIOSUtils::GetNames(ipa, szUserName, szComputerName, szGroupName, szMacAddress);
}

BOOL CNetBIOSUtils::GetNames(LPCWSTR szIP, CString *szUserName, CString *szComputerName, CString *szGroupName, CString *szMacAddress)
{	    
	char buf[1000];    

	UINT nRetrievedLen = CNetBIOSUtils::RetrieveData(szIP, (char*) &buf, sizeof(buf));

	if (nRetrievedLen == 0)
		return FALSE;

	CString szTemp;

    NS_RESP *data = (NS_RESP *) &buf;    

    if (nRetrievedLen < sizeof(*data)) 
	{
		// response too small
        return FALSE;
    }
    
    if (nRetrievedLen < (sizeof (*data) + (data->num_names - 1) * sizeof (data->name_array[0]))) 
	{
        // response too small for num_names
        return FALSE;
    }

	//////////////////////////////////////////////////////////////////////////////////////////////////////    
	TCHAR szName[16];

	// get computer name
	if (szComputerName != NULL)
	{
		*szComputerName = "";
		memcpy(&szName, data->name_array[0].nb_name, 15); szName[15] = 0;
		*szComputerName = szName;
		szComputerName->TrimRight(' ');
	}

	// get group name
	if (szGroupName != NULL)
	{
		*szGroupName = "";
		for (int i = 0; i < data->num_names; i++)
		{			
			if ((data->name_array[i].name_flags & GROUP_NAME_FLAG) && data->name_array[i].nb_name[15] == NAME_TYPE_DOMAIN)
			{
				memcpy(&szName, data->name_array[i].nb_name, 15); szName[15] = 0;
				*szGroupName = szName;				
				break;
			}		
		}
		szGroupName->TrimRight(' ');
	}

	// get user name
	if (szUserName != NULL)
	{
		*szUserName = "";
		for (int i = data->num_names - 1; i >= 0; i--)
		{
			if (data->name_array[i].nb_name[15] == NAME_TYPE_MESSENGER)
			{
				memcpy(&szName, data->name_array[i].nb_name, 15); szName[15] = 0;
				*szUserName = szName;
				break;
			}
		}
		int nDollarIndex = szUserName->Find('$'); 
		if (nDollarIndex >= 0)
			szUserName->Delete(nDollarIndex);

		szUserName->TrimRight(' ');
	}
	
	// get mac address
	if (szMacAddress != NULL)
	{
   		szMacAddress->Format(_T("%02X-%02X-%02X-%02X-%02X-%02X"),
			data->name_array[data->num_names].nb_name[0],
			data->name_array[data->num_names].nb_name[1],
			data->name_array[data->num_names].nb_name[2],
			data->name_array[data->num_names].nb_name[3],
			data->name_array[data->num_names].nb_name[4],
			data->name_array[data->num_names].nb_name[5]);
	}

    return TRUE;
}

int CNetBIOSUtils::RetrieveData(LPCWSTR szIP, char *buf, int nBufSize)
{	
	struct sockaddr_in caddr;
	SOCKET hSocket;    	

	// Set socket to Non-Blocking mode
	u_long nNonBlocking = 1;	
	fd_set fd_read, fd_error;
	timeval timeout;	

	int nRetBufSize;

    if ((hSocket = socket (AF_INET, SOCK_DGRAM, 0)) < 0) 
	{
		// TODO: retry should be made if no more sockets can be opened
        return 0;
    }

	ioctlsocket(hSocket, FIONBIO, &nNonBlocking);	

    memset((TCHAR *) &caddr, 0, sizeof(caddr));
    caddr.sin_family = AF_INET;
	char cstr[512]; memset(cstr, 0x0, sizeof(cstr));
	wcstombs(cstr, szIP, sizeof(szIP));
	caddr.sin_addr.S_un.S_addr = inet_addr(cstr);
    caddr.sin_port = htons(137);

	nRetBufSize = sizeof(NETBIOS_REQUEST) - 1;	
	memcpy(buf, NETBIOS_REQUEST, nRetBufSize);
   
	int nSendToRet = 0;

    if ((nSendToRet = sendto(hSocket, buf, nRetBufSize, 0, (sockaddr *) &caddr, sizeof(caddr))) < 0) 
	{
		closesocket(hSocket);
        return 0;
    }    

	fd_read.fd_array[0] = hSocket; fd_read.fd_count = 1;
	fd_error.fd_array[0] = hSocket; fd_error.fd_count = 1;
	timeout.tv_sec = 0; 
	timeout.tv_usec = g_options->m_nPingTimeout * 1000;	

	if (select(0, &fd_read, 0, &fd_error, &timeout) > 0) 
	{
		if (fd_read.fd_count == 1)
		{
			nRetBufSize = recvfrom(hSocket, buf, nBufSize, 0, (struct sockaddr *) 0, (int *)0);

			closesocket(hSocket);

			if (nRetBufSize < 0) 
			{				
				return 0;
			} 
			else 
			{
				return nRetBufSize;
			} 			
		}
	}	

	closesocket(hSocket);
	return 0;
}
