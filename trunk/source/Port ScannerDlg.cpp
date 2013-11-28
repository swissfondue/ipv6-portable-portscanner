// Port ScannerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Port Scanner.h"
#include "Port ScannerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPortScannerDlg dialog




CPortScannerDlg::CPortScannerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPortScannerDlg::IDD, pParent)
	, ports(_T(""))
	, ip_from(_T(""))
	, ip_to(_T(""))
	, is_using_range_ip(FALSE)
	, ThreadId(0)
	, shared(NULL)
	, t_opt(0)
	, want_cancel(false)
	, tcp_timeout(3)
	, udp_timeout(2)
	, tcp_conns(5)
	, first(NULL)
	, last(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPortScannerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT3, ports);
	DDX_Control(pDX, IDC_LIST1, results);
	DDX_Text(pDX, IDC_EDIT1, ip_from);
	DDX_Text(pDX, IDC_EDIT2, ip_to);
	DDX_Check(pDX, IDC_CHECK1, is_using_range_ip);
	DDX_Control(pDX, IDC_EDIT2, edit_ip_to);
	DDX_Control(pDX, IDC_COMBO1, co_type);
	DDX_CBIndex(pDX, IDC_COMBO1, t_opt);
	DDX_Control(pDX, IDC_PROGRESS1, progress);
	DDX_Control(pDX, IDC_BUTTON2, Scanbut);
	DDX_Control(pDX, IDC_BUTTON6, setbut);
}

BEGIN_MESSAGE_MAP(CPortScannerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHECK1, &CPortScannerDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_BUTTON2, &CPortScannerDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CPortScannerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON5, &CPortScannerDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON4, &CPortScannerDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON3, &CPortScannerDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON6, &CPortScannerDlg::OnBnClickedButton6)
END_MESSAGE_MAP()


// CPortScannerDlg message handlers

BOOL CPortScannerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	ports = _T("21-23,25,53,80,110,135,137-139,443,445,3306");

	ip_from = _T("::1");

	results.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

	co_type.SetCurSel(0);

	int index = 0;
	results.InsertColumn(index++,_T("IP Address"),LVCFMT_LEFT,200);
	results.InsertColumn(index++,_T("Port"),LVCFMT_LEFT,60);
	results.InsertColumn(index++,_T("Protocol"),LVCFMT_LEFT,60);
	results.InsertColumn(index++,_T("Port status"),LVCFMT_LEFT,80);

	shared = new CShared;
	memset(shared,0,sizeof(CShared));
	shared->Dlgmain = this;

	UpdateData(FALSE);

	m_ImageList.Create(16,16,ILC_COLOR16 | ILC_MASK, 2, 10);
	results.SetImageList(&m_ImageList, LVSIL_SMALL);

	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON1));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON2));

	setbut.SetIcon((HICON)LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON3), IMAGE_ICON, 16, 16, NULL));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPortScannerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPortScannerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CPortScannerDlg::OnBnClickedCheck1()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
	edit_ip_to.EnableWindow(is_using_range_ip);
}

void CPortScannerDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	Scanbut.EnableWindow(FALSE);

	want_cancel = false;

	thread_data* data = (thread_data*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                sizeof(thread_data));

	data->shared = shared;
	data->test_id = 1;

	Thread = CreateThread(NULL,0,RunTest,data,0,&ThreadId);

	if(!Thread)
		MessageBox(_T("FAILED TO CREATE TEST THREAD"));
}

void CPortScannerDlg::OnBnClickedButton1() // STOP
{
	// TODO: Add your control notification handler code here

	want_cancel = true;

	/*
	UpdateData(TRUE);

	char* ab = ShortToFull(StringToChar(ip_from));
	Increment(ab);
	ab = FullToShort(ab);
	MessageBox(CharToString(ab));*/

	// messagebox( expanded ipv6 source)
}

void CPortScannerDlg::OnBnClickedButton5()
{
	// TODO: Add your control notification handler code here
	results.DeleteAllItems();
}

void CPortScannerDlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
	CFileDialog FileSaveDialog(FALSE,_T("txt"),0,OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
	_T("Txt files (*.txt)|*.txt||"));

	CString temp;
	char* buff = NULL;

	FileSaveDialog.m_ofn.lpstrTitle = _T("Export to txt");
	
	if(FileSaveDialog.DoModal()==IDOK)
	{
		CString path = FileSaveDialog.GetPathName();

		FILE* pFile = fopen(StringToChar(path), "w");
		if(pFile!=NULL)
		{
			fwrite("Port Scanner results: \r\n\r\n",1,26,pFile);

			fwrite("-----------[ RESULTS ]----------\r\n\r\n",1,36,pFile);
			fwrite("Ip address     | Port  | Protocol | Status \r\n",1,45,pFile);

			CString gga;
			for(int i=0;i<results.GetItemCount();i++)
			{
				gga = results.GetItemText(i,0)+_T(" | ")+results.GetItemText(i,1)+_T(" | ")+results.GetItemText(i,2)+_T(" | ")+results.GetItemText(i,3);//+_T("\r\n");
				wresult(_T(" "),gga,pFile);
			}

			fclose(pFile);

			MessageBox(_T("Txt report succesfully saved."));
		}
	}
}

void wresult(const wchar_t* str,CString& result,FILE* pFile)
{
	char* buff = NULL;
	CString temp(str);
	temp+=result;
	temp+=_T("\r\n");
	buff = StringToChar(temp);
	fwrite(buff,1,strlen(buff),pFile);

	return;
}
void CPortScannerDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
	CFileDialog FileSaveDialog(FALSE,_T("html"),0,OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
	_T("Html files (*.html)|*.html||"));

	//CString temp;
	//char* buff = NULL;

	FileSaveDialog.m_ofn.lpstrTitle = _T("Export to html");

	CString report = _T("");
	
	if(FileSaveDialog.DoModal()==IDOK)
	{
		CString path = FileSaveDialog.GetPathName();

		if(true)
		{
			report = _T("");
			report+= _T("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\"><html><head><title>DNS Tool html report</title>");
			report+=_T("<style type=\"text/css\">	  .table {	clear:both;	border: 1px solid #000000; ");
			report+=_T("margin-top: 5px; margin-bottom: 5px;	background: #cccccc; border-spacing: 1px; ");
			report+=_T("	font-size: 12px;	font-family:\"Verdana\";	} ");
			report+=_T(" .first_tr {	background: #99ccff;	font-weight: bold;	height:25px;	font-size: 13px; } ");
			report+=_T(" td.first_td {	border: 1px solid #6699ff;	padding-left: 5px;	padding-right: 5px; } ");
			report+=_T(" td.left {	text-align: left;	padding-left: 5px;	padding-right: 5px; } ");
			report+=_T(" .tr_1 { background: #ffffff; } .tr_2 {	background: #eeeeee; } ");
			report+=_T(" .main2 {	border: 1px solid #333333;	background: #ddeeff; font-size: 1.25em;	text-align: left;	padding-left: 20px; margin-top: 30px; } ");
			report+=_T("</style></head><body>");

			report+= _T("<a name=\"top\"></a><h1>Port Scanner html report.</h1>");
			//report+= _T("<h3><a href=\"#1\">1. Results</a></h3>");
			report+= _T("<br><br>");

			//report+= _T("<a name=\"1\"></a><div class=\"main2\">1. Results</div><a href=\"#top\">top</a>");
			
			report+= _T("<table class=\"table\"><tr class=\"first_tr\" ><td class=\"first_td\">IP Address</td><td class=\"first_td\">Port</td><td class=\"first_td\">Protocol</td><td class=\"first_td\">Port Status</td></tr> ");

			for(int i=0;i<results.GetItemCount();i++)
			{
				/*
				<img src="data:image/png;base64,iVBORw0KGgoAAAANS..." />

				iVBORw0KGgoAAAANSUhEUgAAAA4AAAAOCAIAAACQKrqGAAAABnRSTlMA/wD/AP83WBt9AAAAYUlE
QVR4nIXQORKAMAxDUTlnD+f+FJDB66BxZb9GNkBmkgRqc65L0uVXnXsBAGyJZ3wkpH2W3y3r6AIN
urhMvU4OMFJxXy6eVtu3/8nUt/5k7Fu1Jlf1S1sXdPOBOeufnNx45gCE4RT68gAAAABJRU5ErkJg
gg==

				iVBORw0KGgoAAAANSUhEUgAAABAAAAAPCAIAAABiEdh4AAAABnRSTlMA/wD/AP83WBt9AAAAZElE
QVR4nJ2SQQ7AIAgEB7/NVd6NF5tIIVo78cAaNhBA3J0FMSHi6lEv0MHT6zHnkJ08nLOjp82+NTZu
YJS04s9SAOicRzLYVtYVtiSDbmVdQVMA2LPB27H+Xdz305Db43sbjgx7/rqn3//TDAAAAABJRU5E
rkJggg==*/
				report+= _T("<tr class=\"tr_")+IntToStr(i%2+1)+_T("\"><td class=\"left\">");
				if(results.GetItemText(i,3)!=_T("Open"))
					report+= _T("<img src=\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA4AAAAOCAIAAACQKrqGAAAABnRSTlMA/wD/AP83WBt9AAAAYUlEQVR4nIXQORKAMAxDUTlnD+f+FJDB66BxZb9GNkBmkgRqc65L0uVXnXsBAGyJZ3wkpH2W3y3r6AINurhMvU4OMFJxXy6eVtu3/8nUt/5k7Fu1Jlf1S1sXdPOBOeufnNx45gCE4RT68gAAAABJRU5ErkJggg==\" />");
				else
					report+= _T("<img src=\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAPCAIAAABiEdh4AAAABnRSTlMA/wD/AP83WBt9AAAAZElEQVR4nJ2SQQ7AIAgEB7/NVd6NF5tIIVo78cAaNhBA3J0FMSHi6lEv0MHT6zHnkJ08nLOjp82+NTZuYJS04s9SAOicRzLYVtYVtiSDbmVdQVMA2LPB27H+Xdz305Db43sbjgx7/rqn3//TDAAAAABJRU5ErkJggg==\" />");
				report+=results.GetItemText(i,0)+_T("</td><td class=\"left\">")+results.GetItemText(i,1)+_T("</td><td class=\"left\">")+results.GetItemText(i,2)+_T("</td><td class=\"left\">")+results.GetItemText(i,3)+_T("</td></tr>");
			}

			report+= _T("</table>");

			report+=_T("<br><br>");
			report+=_T("<a href=\"#top\">top</a><h2>End of report</h2></body>");
			
			//convert to cstr
			char* ta = StringToChar(report);

			// save
			FILE * pFile;
			int result;
			pFile = fopen(StringToChar(path), "w");
			if(pFile!=NULL)
			{
				result = fputs((const char*)ta,pFile);
				fclose(pFile);
			}

			MessageBox(_T("Html report succesfully saved."));
		}
	}
}

CString IntToStr(int a)
{
	CString t;
	t.Format(_T("%d"),a);
	return t;
}
void CPortScannerDlg::OnBnClickedButton6()
{
	// TODO: Add your control notification handler code here
	CSettingsDlg setdlg;
	setdlg.dlg = this;
	setdlg.DoModal();
}
