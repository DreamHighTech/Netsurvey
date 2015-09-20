#pragma once
#include "abstractipstyle.h"
class CIPRandomStyle :
	public CAbstractIPStyle
{
public:
	CIPRandomStyle(IPAddress nIPBase, IPAddress nIPMask, CString nIPOrgMask, int nIPs);
	~CIPRandomStyle(void);
protected:
	CString m_nIPOriginalMask;
	IPAddress m_nIPBitMask;
	IPAddress m_nIPBitAddition;

	IPAddress m_nCurrentIP;
	int m_nIPs;
	int m_nIPCounter;

public:	
	virtual void startFeeding();
	virtual void finishFeeding();
	virtual BOOL isNextIPAvailable();
	virtual int getPercentComplete();
	virtual IPAddress getNextIP();
	virtual CString getScanSummary();
};

