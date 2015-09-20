#pragma once

#include "abstractipstyle.h"

class CIPListStyle :
	public CAbstractIPStyle
{
public:
	CIPListStyle(void);
	~CIPListStyle(void);
public:	
	virtual void startFeeding();
	virtual void finishFeeding();
	virtual BOOL isNextIPAvailable();
	virtual int getPercentComplete();
	virtual IPAddress getNextIP();
	virtual CString getScanSummary();
};

