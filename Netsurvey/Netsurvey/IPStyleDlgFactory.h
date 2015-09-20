#pragma once

#include "AbstractIPStyleDlg.h"

class CIPStyleDlgFactory
{
	CAbstractIPStyleDlg * m_paIPFeeds[MAXIPSTYLEDLG];
	CString m_szIPFeedNames[MAXIPSTYLEDLG];
	CString m_szIPFeedTypes[MAXIPSTYLEDLG];
	int m_nIPFeeds;

public:
	CIPStyleDlgFactory(void);
	~CIPStyleDlgFactory(void);

	int getIPFeedCount();
	CAbstractIPStyleDlg * getIPFeed(int nIndex);
	CString & getIPFeedName(int nIndex);
	int getIndexByType(const CString & szType);
};

