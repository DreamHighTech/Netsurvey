#include "StdAfx.h"
#include "IPRangeIPStyle.h"


CIPRangeIPStyle::CIPRangeIPStyle(IPAddress nStartIP, IPAddress nEndIP)
{
	m_nStartIP = nStartIP;
	m_nEndIP = nEndIP;	
}


CIPRangeIPStyle::~CIPRangeIPStyle(void)
{
}

void CIPRangeIPStyle::startFeeding()
{
	// Initialize current IP
	m_nCurrentIP = m_nStartIP;
}

void CIPRangeIPStyle::finishFeeding()
{
	// no finishing necessary
}

BOOL CIPRangeIPStyle::isNextIPAvailable()
{
	return m_nCurrentIP <= m_nEndIP;
}

int CIPRangeIPStyle::getPercentComplete()
{	
	return (m_nCurrentIP - m_nStartIP) * 100L / (m_nEndIP - m_nStartIP + 1);
}

IPAddress CIPRangeIPStyle::getNextIP()
{
	// Return current IP and then increase it
	m_nCurrentIP++;
	return m_nCurrentIP-1;
}

CString CIPRangeIPStyle::getScanSummary()
{
	CString szResult;

	// Convert IPs to strings
	TCHAR ipps[512];
	TCHAR ippe[512];
	char *cipp;

	in_addr in;
	in.S_un.S_addr = htonl(m_nStartIP);
	cipp = inet_ntoa(in);
	memset(ipps, 0x0, sizeof(ipps));
	mbstowcs(ipps, cipp, 15);

	in.S_un.S_addr = htonl(m_nEndIP);
	cipp = inet_ntoa(in);
	memset(ippe, 0x0, sizeof(ippe));
	mbstowcs(ippe, cipp, 15);

	szResult.Format(_T("IP Range: (%s-%s)"), ipps, ippe);

	return szResult;
}
