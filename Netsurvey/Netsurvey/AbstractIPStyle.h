
#pragma once

typedef DWORD IPAddress;

class CAbstractIPStyle  
{
public:
	CAbstractIPStyle();
	virtual ~CAbstractIPStyle();	

	// Start feeding of IP addresses
	virtual void startFeeding() = 0;

	// Finish feeding of IP addresses
	virtual void finishFeeding() = 0;

	// Whether we should continue scanning
	virtual BOOL isNextIPAvailable() = 0;

	// Returns status for a progress bar (from 0 to 100)
	virtual int getPercentComplete() = 0;

	// Returns the next IP address
	virtual IPAddress getNextIP() = 0;

	// Returns the IP feed settings short human-readable summary
	virtual CString getScanSummary() = 0;

};
