#include "StdAfx.h"
#include "IPStyleDlgFactory.h"
#include "IPListDlg.h"
#include "IPRangeDlg.h"
#include "IPRandomDlg.h"


CIPStyleDlgFactory::CIPStyleDlgFactory(void)
{
	// Specify number of feeders
	m_nIPFeeds = MAXIPSTYLEDLG;

	// Create all possible feeders (currently they are all built-in)
	m_paIPFeeds[0] = new CIPRangeDlg;
	m_paIPFeeds[0]->Create(CIPRangeDlg::IDD);
	m_szIPFeedNames[0] = "IP Range";
	m_szIPFeedTypes[0] = "range";

	m_paIPFeeds[1] = new CIPRandomDlg();
	m_paIPFeeds[1]->Create(CIPRandomDlg::IDD);
	m_szIPFeedNames[1] = "Random";
	m_szIPFeedTypes[1] = "random";

//	m_paIPFeeds[2] = new CIPListDlg();
//	m_paIPFeeds[2]->Create(CIPListDlg::IDD);
//	m_szIPFeedNames[2] = "IP List File";
//	m_szIPFeedTypes[2] = "list";
}

CIPStyleDlgFactory::~CIPStyleDlgFactory()
{
	for (int i = 0; i < m_nIPFeeds; i++)
	{
		delete m_paIPFeeds[i];
	}
}

int CIPStyleDlgFactory::getIPFeedCount()
{
	return m_nIPFeeds;
}

CAbstractIPStyleDlg * CIPStyleDlgFactory::getIPFeed(int nIndex)
{
	return m_paIPFeeds[nIndex];
}

CString & CIPStyleDlgFactory::getIPFeedName(int nIndex)
{
	return m_szIPFeedNames[nIndex];
}

int CIPStyleDlgFactory::getIndexByType(const CString & szType)
{
	for (int i = 0; i < m_nIPFeeds; i++)
	{
		if (szType == m_szIPFeedTypes[i])
			return i;
	}

	return -1;
}
