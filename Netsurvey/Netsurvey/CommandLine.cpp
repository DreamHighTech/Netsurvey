// CommandLine.cpp: implementation of the CCommandLine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Netsurvey.h"
#include "CommandLine.h"
#include "SaveToFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommandLine::CCommandLine()
{
	m_nOptions = 0;
}

CCommandLine::~CCommandLine()
{

}

BOOL CCommandLine::process()
{	
	if (__argc!=1) 
	{
	
		// too few or too many parameters
		if (__argc < 3) 
		{
			displayHelp();
			exit(1);
		}

		m_nOptions = 0;
		int nParameter = 0;

		// parse parameters
		for (int i=1; i < __argc; i++)
		{
			if (__targv[i][0] == '-' || __targv[i][0] == '/')
			{
				// this is an option
				for (int j=1; __targv[i][j] != 0; j++)
				{
					switch (__targv[i][j])
					{
						case 's': 
							m_nOptions |= CMDO_START_SCAN; 
							break;						

						case 'e': 
							m_nOptions |= CMDO_NOT_EXIT; 
							break;

						case 'a': 
							m_nOptions |= CMDO_APPEND_FILE; 
							break;

						case 'h':
							m_nOptions |= CMDO_HIDE_WINDOW;
							break;

						case 'f': 
							if (j == 1)	// Accept "f" only as a first TCHARacter
							{
								switch (__targv[i][j+2])	// "-f:X" - check the X TCHARacter
								{
									case 'c':
										m_nFileFormat = FILE_TYPE_CSV; break;
									case 'h': 
										m_nFileFormat = FILE_TYPE_HTML; break;
									case 'x': 
										m_nFileFormat = FILE_TYPE_XML; break;
									case 'l': 
										m_nFileFormat = FILE_TYPE_IPPORT_LIST; break;
									default:
										m_nFileFormat = FILE_TYPE_TXT; break;
								}
								__targv[i][j+1] = 0;	// To exit from for loop
							}							
							break;

						case 't':
							if (j == 1)	// Accept "t" only as a first TCHARacter in group
							{
								// Copy IP feed identificator (type)
								wcsncpy(m_szIPFeedType.GetBuffer(64), &__targv[i][j+2], 64);
								m_szIPFeedType.ReleaseBuffer();
								__targv[i][j+1] = 0;	// To proceed with next parameter
							}
							break;

						default:
							CString err = _T("Unknown option: ");
							err += __targv[i][j];
							//+ __targv[i][j];
							MessageBox(0, err, NULL, MB_OK | MB_ICONHAND);
							break;
					}
				}
			}
			else
			{
				// this is a parameter
				nParameter++;

				m_szIPFeedParams += __targv[i];
				m_szIPFeedParams += "|";
			}
		}		

		// Delete trailing pipe
		m_szIPFeedParams.Delete(m_szIPFeedParams.GetLength() - 1);

		// Now get filename out of m_szIPFeedParams (it should be the last one)
		if (nParameter > 2)
		{				
			// Find the last pipe
			int nLastPipe = m_szIPFeedParams.ReverseFind('|');
			
			// Remember filename
			m_szFilename = m_szIPFeedParams.Mid(nLastPipe + 1);
			m_szIPFeedParams.Delete(nLastPipe, m_szIPFeedParams.GetLength() - nLastPipe);
		}

		
		if (m_szFilename.GetLength() > 0) 
		{			
			m_nOptions |= CMDO_SAVE_TO_FILE;
			m_nOptions |= CMDO_START_SCAN;
		}
		else
		{
			if (m_nOptions & CMDO_HIDE_WINDOW)
			{
				MessageBox(0, _T("Filename is not given, will not hide the window"), _T("Warning"), MB_OK | MB_ICONWARNING);
			}
		}

		// Select IP range by default
		if (m_szIPFeedType.GetLength() == 0)
			m_szIPFeedType = "range";	

		return TRUE;
	}
	
	return FALSE;

}

void CCommandLine::displayHelp()
{
	MessageBox(0, _T("Command-line parameters:\n\n")
				_T("[OPTIONS] [-t:IPFEED] [IPFEED_PARAMETERS] [FILENAME]\n\n")
				_T("IPFEED:\n")
				_T("\tThe way you want to choose IP addresses for scanning, look below for values.\n")
				_T("IPFEED_PARAMETERS:\n")
				_T("\trange:\n")
				_T("\t\t<START_IP> <END_IP>\t- starting and ending IPs of a range\n")
				_T("\trandom:\n")
				_T("\t\t<IP_COUNT> <BASE_IP>\t- number of IPs and the base IP for randomization\n")
				_T("FILENAME:\n")
				_T("\tfilename to save listing to. If it is given, then program will close\n")
				_T("\tafter saving data to a file\n")
				_T("OPTIONS:\n")
				_T("\t-s\tautomatically start scanning (if filename is not given)\n")			
				_T("\t-e\tdo not exit after saving data\n")
				_T("\t-a\tappend to the file, do not overwrite\n")
				_T("\t-h\thide main window while scanning; be careful with this!\n")
				_T("\t  \trun only from batch files with this option.\n")
				_T("\t-f:X\tfile format. X can be 'csv', 'html', 'txt', 'xml' or 'lst'.\n")
			   ,_T("SDR Free IP Scan Command-line"),MB_OK | MB_ICONINFORMATION);
}
