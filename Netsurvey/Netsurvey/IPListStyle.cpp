#include "StdAfx.h"
#include "IPListStyle.h"


CIPListStyle::CIPListStyle(void)
{
}


CIPListStyle::~CIPListStyle(void)
{
}

void CIPListStyle::startFeeding()
{
}

void CIPListStyle::finishFeeding()
{
	// no finishing necessary
}

BOOL CIPListStyle::isNextIPAvailable()
{
	return 0;
}

int CIPListStyle::getPercentComplete()
{	
	return 0;
}

IPAddress CIPListStyle::getNextIP()
{
	return 0;
}

CString CIPListStyle::getScanSummary()
{
	return NULL;
}