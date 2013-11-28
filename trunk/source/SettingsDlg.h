#pragma once

class CSettingsDlg;

#include "Port ScannerDlg.h"

// CSettingsDlg dialog

class CSettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(CSettingsDlg)

public:
	CSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingsDlg();
	CPortScannerDlg* dlg;
	BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString tcp_tim;
	CString udp_tim;
	CString tcp_con;
	afx_msg void OnBnClickedButton1();
};
