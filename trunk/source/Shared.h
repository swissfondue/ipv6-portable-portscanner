 
#pragma once

class CShared;
struct CTcpitam;

#define SOCK_BUFFER 32768

#include "stdafx.h"
#include "Port Scanner.h"
#include "Port ScannerDlg.h"
//#include "Windns.h"

class CShared
{
	public:
		CPortScannerDlg* Dlgmain;
};

struct thread_data
{
	CShared* shared;
	int test_id;
	CTcpitam* cur; // only for test 2
};

struct CTcpitam
{
	CTcpitam* prev;
	CTcpitam* next;
	int result; // 0 = not initialized, 1 = tcp closed, 2 = tcp open
	char* port;
	HANDLE thread;
	DWORD threadId;
	static char* server;
};

DWORD WINAPI RunTest(LPVOID lpParam);

char* ClearUnicode(char* src);

char* StringToChar(CString& src);

CString CharToString(char* src);

char* MidStr(char* src,int start,int end);

int FindStr(char* src,const char* to_find, int start = 0);

/// Ipv6

char* ShortToFull(char* src);

void Increment(char* src);

char* FullToShort(char* src);


void GetPortsToCheck(bool* ports_use,CString port_str, int* ports_num);

void ClearOneTcp(CPortScannerDlg* Dlgmain,  int* abpos);