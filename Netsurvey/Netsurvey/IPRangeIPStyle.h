#pragma once
#include "abstractipstyle.h"
class CIPRangeIPStyle :
	public CAbstractIPStyle
{
public:
	CIPRangeIPStyle(IPAddress nStartIP, IPAddress nEndIP);
	~CIPRangeIPStyle(void);

protected:
	IPAddress m_nEndIP;
	IPAddress m_nStartIP;
	IPAddress m_nCurrentIP;

public:	
	virtual void startFeeding();
	virtual void finishFeeding();
	virtual BOOL isNextIPAvailable();
	virtual int getPercentComplete();
	virtual IPAddress getNextIP();
	virtual CString getScanSummary();
};

