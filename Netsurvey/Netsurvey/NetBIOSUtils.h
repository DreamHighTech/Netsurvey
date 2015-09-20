
#pragma once

class CNetBIOSUtils
{
protected:
	static int RetrieveData(LPCWSTR szIP, char *buf, int nBufSize);		

public:			
	CNetBIOSUtils();
	virtual ~CNetBIOSUtils();	
	static BOOL GetNames(LPCWSTR szIP, CString *szUserName, CString *szComputerName, CString *szGroupName, CString *szMacAddress);
	static BOOL GetNames(DWORD nIP, CString *szUserName, CString *szComputerName, CString *szGroupName, CString *szMacAddress);
};
