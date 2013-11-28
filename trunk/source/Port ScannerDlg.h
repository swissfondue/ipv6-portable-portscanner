// Port ScannerDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class CPortScannerDlg;

void wresult(const wchar_t* str,CString& result,FILE* pFile);
CString IntToStr(int a);

#include "shared.h"
#include "SettingsDlg.h"

// CPortScannerDlg dialog
class CPortScannerDlg : public CDialog
{
// Construction
public:
	CPortScannerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PORTSCANNER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString ports;
	CListCtrl results;
	CString ip_from;
	CString ip_to;
	BOOL is_using_range_ip;
	CEdit edit_ip_to;
	afx_msg void OnBnClickedCheck1();
	CComboBox co_type;
	afx_msg void OnBnClickedButton2();
	HANDLE Thread;
	DWORD ThreadId;
	CShared* shared;
	afx_msg void OnBnClickedButton1();
	CImageList m_ImageList;
	int t_opt;
	CProgressCtrl progress;
	bool want_cancel;
	afx_msg void OnBnClickedButton5();
	CButton Scanbut;
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton3();
	CButton setbut;
	afx_msg void OnBnClickedButton6();
	long tcp_timeout;
	long udp_timeout;
	int tcp_conns;
	CTcpitam* first;
	CTcpitam* last;
};
