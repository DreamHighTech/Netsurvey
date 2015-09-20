#pragma once


// CScanStatisticsDlg dialog

class CScanStatisticsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScanStatisticsDlg)

public:
	CScanStatisticsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CScanStatisticsDlg();
	void setMessageText(LPCWSTR szMessage);

// Dialog Data
	enum { IDD = IDD_SCANSTATISTIC };

protected:
	CString m_szMessage;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
