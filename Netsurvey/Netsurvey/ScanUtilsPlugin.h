
#pragma once

#include "Scanner.h"
#include "globals.h"

#define PLUGINS_DIR		_T("plugins")

class CScanUtilsPlugin  
{
public:
	CScanUtilsPlugin();
	virtual ~CScanUtilsPlugin();
	void load(CArray<TScannerColumn, TScannerColumn&> &columns, int &nColumnCount);
protected:
	void loadFromDir(CArray<TScannerColumn, TScannerColumn&> &columns, int &nColumnCount, CString &szDir);
};

