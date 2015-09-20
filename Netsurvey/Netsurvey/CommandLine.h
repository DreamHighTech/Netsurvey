
#pragma once


// Command-line options
#define CMDO_START_SCAN		1
#define CMDO_NOT_EXIT		2
#define CMDO_SAVE_TO_FILE	4
#define CMDO_APPEND_FILE	16
#define CMDO_HIDE_WINDOW	32


class CCommandLine 
{
public:
	int m_nFileFormat;
	int m_nOptions;
	CString m_szFilename;
	CString m_szIPFeedType;
	CString m_szIPFeedParams;
	static void displayHelp();
	BOOL process();
	CCommandLine();
	virtual ~CCommandLine();

};

