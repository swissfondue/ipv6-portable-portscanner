// SettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Port Scanner.h"
#include "SettingsDlg.h"


// CSettingsDlg dialog

IMPLEMENT_DYNAMIC(CSettingsDlg, CDialog)

CSettingsDlg::CSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsDlg::IDD, pParent)
	, tcp_tim(_T(""))
	, udp_tim(_T(""))
	, tcp_con(_T(""))
{

}

CSettingsDlg::~CSettingsDlg()
{
}

void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, tcp_tim);
	DDX_Text(pDX, IDC_EDIT4, udp_tim);
	DDX_Text(pDX, IDC_EDIT2, tcp_con);
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CSettingsDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CSettingsDlg message handlers

BOOL CSettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//_wtoi( 
	tcp_tim.Format(_T("%d"),dlg->tcp_timeout);
	udp_tim.Format(_T("%d"),dlg->udp_timeout);
	tcp_con.Format(_T("%d"),dlg->tcp_conns);
	UpdateData(FALSE);

	return TRUE;
}

void CSettingsDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	dlg->tcp_conns = _wtoi(tcp_con);
	dlg->tcp_timeout = _wtoi(tcp_tim);
	dlg->udp_timeout = _wtoi(udp_tim);

	OnOK();
}
