#include "StdAfx.h"
#include "IPRandomStyle.h"


CIPRandomStyle::CIPRandomStyle(IPAddress nIPBase, IPAddress nIPMask, CString nIPOrgMask, int nIPs)
{
	// Remember the mask
	m_nIPOriginalMask = nIPOrgMask;

	// "Parse" the mask (bytewise)
	m_nIPBitAddition = 0;
	m_nIPBitMask = 0;

	// Note: 32bit IPv4 is hardcoded here

	m_nIPBitMask = nIPMask;
	m_nIPBitAddition = nIPBase;

	// Remember number of runs
	m_nIPs = nIPs;
}

CIPRandomStyle::~CIPRandomStyle()
{

}

void CIPRandomStyle::startFeeding()
{
	// Initialize IP counter
	m_nIPCounter = 0;

	// Init random seed
	srand((unsigned int)time(NULL));
}

void CIPRandomStyle::finishFeeding()
{
	// no finishing necessary
}

BOOL CIPRandomStyle::isNextIPAvailable()
{
	return m_nIPCounter < m_nIPs;
}

int CIPRandomStyle::getPercentComplete()
{	
	return m_nIPCounter * 100L / m_nIPs;
}

IPAddress CIPRandomStyle::getNextIP()
{
	// Generate new random IP byte-wise
	// Note: this is hardcoded for 32bit IPv4
	m_nCurrentIP = 0;
	int i = 0;
	for (DWORD nCurBitMask = 1; nCurBitMask; nCurBitMask <<=1)
	{
		if ((m_nIPBitMask & nCurBitMask) == nCurBitMask)
		{			
			m_nCurrentIP += m_nIPBitAddition & nCurBitMask;
		}
		else
		{
			m_nCurrentIP += ((rand() << i) & nCurBitMask);
		}
		i ++;
	}

	m_nIPCounter++;

	return m_nCurrentIP;
}

CString CIPRandomStyle::getScanSummary()
{
	CString szResult;

	// Output something like: Rnd 100x (192.0.0.0)

	TCHAR ipp[512];
	memset(ipp, 0x0, sizeof(ipp));
	char *cipp;

	in_addr in;
	in.S_un.S_addr = htonl(m_nIPBitAddition);
	cipp = inet_ntoa(in);
	memset(ipp, 0x0, sizeof(ipp));
	mbstowcs(ipp, cipp, 15);
	szResult.Format(_T("Random: %d: (%s/%s)"), m_nIPs, ipp, m_nIPOriginalMask);

	return szResult;
}
