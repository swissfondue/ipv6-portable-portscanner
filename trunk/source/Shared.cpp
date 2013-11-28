
#include "Shared.h"

char* CTcpitam::server = "";

DWORD WINAPI RunTest(LPVOID lpParam)
{
	thread_data* data = (thread_data*)lpParam;
	CShared* shared = data->shared;
	CPortScannerDlg* Dlgmain = shared->Dlgmain;
	
	int test_type = data->test_id; 
	CString msg;
	CTcpitam* cur = data->cur;

	if(test_type==1) /////////////////////////////////////////////////////////////////////////
	{
		bool ports_use[65536];
		int ports_num = 0;

		GetPortsToCheck(ports_use,Dlgmain->ports,&ports_num);

		int t_opt = Dlgmain->t_opt;

		bool use_tcp = ((t_opt==0)||(t_opt==2));
		bool use_udp = ((t_opt==1)||(t_opt==2));

		Dlgmain->progress.SetRange(0,ports_num*(1+(t_opt==2)));

		int abpos = 0;


		char sbuff[8096+1];
		int ibuff;

		char AddrName[NI_MAXHOST];

		char *Server = NULL; // ip
		int Family = PF_INET6;
		//int Family = PF_INET;
		int SocketType = SOCK_STREAM;
		char *Port = "19976";

		WSADATA wsaData;

		int i, RetVal, AddrLen;//, AmountToSend;
		//int ExtraBytes = 0;
		//unsigned int Iteration, MaxIterations = 1;
		BOOL RunForever = FALSE;

		ADDRINFO Hints, *AddrInfo, *AI;
		SOCKET ConnSocket = INVALID_SOCKET;
		struct sockaddr_storage Addr;

		if ((RetVal = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
		{
			//fprintf(stderr, "WSAStartup failed with error %d: %s\n",
					//RetVal, PrintError(RetVal));
			Dlgmain->MessageBox(_T("error 1"));
			WSACleanup();
			return -1; 
		}

	Server = StringToChar(Dlgmain->ip_from);
	char* Full = ShortToFull(Server);
	char* Short = FullToShort(Full);

	char* LastServer = StringToChar(Dlgmain->ip_to);
	LastServer = ShortToFull(LastServer);

	CTcpitam::server = Server;
	
	// petla servers

	for(;;)
	{
		//Sleep(500);

	// petla TCP/UDP
	for(int tcpudp = 0; tcpudp<2; tcpudp++)
	{
		if((tcpudp==0)&&(!use_tcp))
			continue;
		if((tcpudp==1)&&(!use_udp))
			continue;

		if(tcpudp==0)
			SocketType = SOCK_STREAM;
		if(tcpudp==1)
		{
			while(Dlgmain->first)
			{
				ClearOneTcp(Dlgmain,&abpos);
				Sleep(100);
			}

			SocketType = SOCK_DGRAM;
		}

	for(int et3 = 0;et3<65536;et3++)
		if(ports_use[et3])
		{
			if(Dlgmain->want_cancel)
			{
				while(Dlgmain->first)
				{
					ClearOneTcp(Dlgmain,&abpos);
					Sleep(100);
				}
				break;
			}

			ClearOneTcp(Dlgmain,&abpos);

			// na koncu tej petli niech dodaje jeden do kolejki jesli mozna + sleep(200ms)
			// albo sleep   TIMEOUT/MAX_TCP

			// set current port
			//delete[] Port;
			CString erh;
			erh.Format(_T("%d"),et3);
			Port = StringToChar(erh);

			memset(&Hints, 0, sizeof (Hints));
			Hints.ai_family = Family;
			Hints.ai_socktype = SocketType;
			RetVal = getaddrinfo(Server, Port, &Hints, &AddrInfo);
			if (RetVal != 0)
			{
				Dlgmain->MessageBox(_T("Cannot resolve address."));
				break;
			}

			for (AI = AddrInfo; AI != NULL; AI = AI->ai_next)
			{
				ConnSocket = socket(AI->ai_family, AI->ai_socktype, AI->ai_protocol);
				int bu6 = 1;
				setsockopt(ConnSocket,SOL_SOCKET,SO_DONTLINGER,(const char*)(&bu6),4);
				int bu7 = 0;
				setsockopt(ConnSocket,SOL_SOCKET,SO_LINGER,(const char*)(&bu7),4);
				
				if (ConnSocket == INVALID_SOCKET)
				{
					//Dlgmain->MessageBox(_T("error 3"));
					continue;
				}

				if(tcpudp==0) //tcp
				{
					CTcpitam* newe = new CTcpitam;
					newe->next = NULL;
					newe->prev = Dlgmain->last;
					newe->port = Port;
					newe->result = 0;

					if(Dlgmain->last)
					{
						Dlgmain->last->next = newe;
						Dlgmain->last = newe;
					}
					else
						Dlgmain->last = newe;

					if(!Dlgmain->first)
						Dlgmain->first = newe;

					thread_data* data = (thread_data*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
					sizeof(thread_data));

					data->shared = shared;
					data->test_id = 2;
					data->cur = newe;

					newe->thread = CreateThread(NULL,0,RunTest,data,0,&(newe->threadId));

					DWORD slp = Dlgmain->tcp_timeout*1000.0/Dlgmain->tcp_conns;

					Sleep(slp);
				}
				else // udp
				{
					unsigned long iMode = 0;
					ioctlsocket(ConnSocket,FIONBIO,&iMode);

					if (connect(ConnSocket, AI->ai_addr, (int) AI->ai_addrlen) != SOCKET_ERROR)
						break;
				}
			}

			if (AI == NULL)
			{
				continue;
			}

			if(tcpudp==1) // UDP
			{
				//SOCKET rawsock = socket(AI->ai_family,SOCK_RAW,IPPROTO_ICMP);
				SOCKET rawsock = socket(AI->ai_family,SOCK_RAW,IPPROTO_ICMPV6);
				connect(rawsock, AI->ai_addr, (int) AI->ai_addrlen);
				
				if (rawsock<0)
				{
					Dlgmain->MessageBox(_T("Error: can't scan UDP, need administrator privilages"));
					return 0;
				}

				
				bool afeee = false;

				//Dlgmain->MessageBox(_T("sending..."));
				for(int gg=0;gg<3;gg++)
				{
					if(gg>0)
						Sleep(200);

					send(ConnSocket,"",0,0);
					
					fd_set fds2 ;
					int n2;
					timeval tv2;
					FD_ZERO(&fds2);
					FD_SET(ConnSocket, &fds2);
					tv2.tv_sec = Dlgmain->udp_timeout;
					tv2.tv_usec = 0;

					n2 = select(0, &fds2, NULL, NULL, &tv2 ) ;
					// n=0 timeout    n=-1  error
					if((n2 == 0)||( n2 == -1 ))
					{
						afeee = true;
					}
					else
					{
						ibuff = recv(ConnSocket,sbuff,8096,0);

						int indax = Dlgmain->results.InsertItem(Dlgmain->results.GetItemCount(),CharToString(Short),1);
						int gg = et3;
						CString temp;
						temp.Format(_T("%d"),gg);
						Dlgmain->results.SetItemText(indax,1,temp);
						Dlgmain->results.SetItemText(indax,2,_T("UDP"));
						Dlgmain->results.SetItemText(indax,3,_T("Open"));
						afeee = false;

						break;
					}
				}

				if(afeee)
				{
					for(int gg=0;gg<2;gg++)
					{
						send(ConnSocket,"",0,0);
						Sleep(200);
					}

					fd_set fds ;
					int n;
					timeval tv;
					FD_ZERO(&fds);
					FD_SET(rawsock, &fds);
					tv.tv_sec = Dlgmain->udp_timeout;
					tv.tv_usec = 0;

					n = select(0, &fds, NULL, NULL, &tv ) ;
					// n=0 timeout    n=-1  error
					if((n == 0)||( n == -1 ))
					{
						/*
						if(n==0)
							Dlgmain->MessageBox(_T("timeout"));
						else if(n==-1)
							Dlgmain->MessageBox(_T("sel err"));
							*/
						int indax = Dlgmain->results.InsertItem(Dlgmain->results.GetItemCount(),CharToString(Short),0);
						int gg = et3;
						CString temp;
						temp.Format(_T("%d"),gg);
						Dlgmain->results.SetItemText(indax,1,temp);
						Dlgmain->results.SetItemText(indax,2,_T("UDP"));
						Dlgmain->results.SetItemText(indax,3,_T("Open/Filtered"));
					}
					else
					{
						ibuff = recv(rawsock,sbuff,8096,0);
						/*
						if(ibuff==SOCKET_ERROR)
						{
							Dlgmain->MessageBox(_T("recv sock err"));
							CString w4hw;
							unsigned aggg = unsigned(WSAGetLastError());
							int aaf = aggg;
							w4hw.Format(_T("%u"),aggg);
							Dlgmain->MessageBox(w4hw);
						}
						sbuff[ibuff] = '\0';
						Dlgmain->MessageBox(CharToString(sbuff));*/
						int indax = Dlgmain->results.InsertItem(Dlgmain->results.GetItemCount(),CharToString(Short),0);
						int gg = et3;
						CString temp;
						temp.Format(_T("%d"),gg);
						Dlgmain->results.SetItemText(indax,1,temp);
						Dlgmain->results.SetItemText(indax,2,_T("UDP"));
						Dlgmain->results.SetItemText(indax,3,_T("Closed"));
					}
				}
				closesocket(rawsock);
				closesocket(ConnSocket);
			}

			// We are done with the address info chain, so we can free it.
			freeaddrinfo(AddrInfo);

			closesocket(ConnSocket);

		} // port end

		} // tcpudp loop end

		while(Dlgmain->first)
		{
			ClearOneTcp(Dlgmain,&abpos);
			Sleep(100);
		}

		Dlgmain->progress.SetPos(0);
		abpos = 0;

		if(Dlgmain->is_using_range_ip)
		{
			Server = ShortToFull(Server);

			if(strcmp(Server,LastServer)!=-1)
				break;
			else
			{
				Increment(Server);
				Server = FullToShort(Server);
				strcpy(Short,Server);

				ConnSocket = INVALID_SOCKET;

				CTcpitam::server = Server;

				Sleep(1000);
			}
		}
		else break;

		} // servers loop end

		Dlgmain->Scanbut.EnableWindow(TRUE);

	} // TEST 1 End

	if(test_type==2) // tcp conn test
	{
		ADDRINFO Hints, *AddrInfo, *AI;
		SOCKET ConnSocket = INVALID_SOCKET;
		struct sockaddr_storage Addr;
		int RetVal, AddrLen;

			memset(&Hints, 0, sizeof (Hints));
			Hints.ai_family = AF_INET6;
			Hints.ai_socktype = SOCK_STREAM;
			RetVal = getaddrinfo(cur->server, cur->port, &Hints, &AddrInfo);
			if (RetVal != 0)
			{
				Dlgmain->MessageBox(_T("Cannot resolve address."));
				cur->result = 3;
				return 0;
			}

			for (AI = AddrInfo; AI != NULL; AI = AI->ai_next)
			{
				ConnSocket = socket(AI->ai_family, AI->ai_socktype, AI->ai_protocol);
				int bu6 = 1;
				setsockopt(ConnSocket,SOL_SOCKET,SO_DONTLINGER,(const char*)(&bu6),4);
				int bu7 = 0;
				setsockopt(ConnSocket,SOL_SOCKET,SO_LINGER,(const char*)(&bu7),4);
				
				if (ConnSocket == INVALID_SOCKET)
				{
					continue;
				}

				if(true) //tcp
				{
					unsigned long iMode = 1;
					ioctlsocket(ConnSocket,FIONBIO,&iMode);

					if((connect(ConnSocket, AI->ai_addr, (int) AI->ai_addrlen) != SOCKET_ERROR)||(WSAGetLastError()==WSAEWOULDBLOCK))
					{
						fd_set fds;
						int n;
						timeval tv;
						FD_ZERO(&fds);
						FD_SET(ConnSocket, &fds);
						tv.tv_sec = Dlgmain->tcp_timeout;
						tv.tv_usec = 0;

						n = select(0, NULL,&fds, NULL, &tv ) ;
						// n=0 timeout    n=-1  error
						if((n == 0)||( n == -1 ))
						{
							cur->result = 1; // closed
							closesocket(ConnSocket);
							return 0;
						}
						else
						{
							AddrLen = sizeof (Addr);
							if (getpeername(ConnSocket, (LPSOCKADDR) & Addr, (int *) &AddrLen) == SOCKET_ERROR)
							{
								cur->result = 1; // closed
								closesocket(ConnSocket);
								return 0;
							}
							else
							{
								cur->result = 2; // open
								closesocket(ConnSocket);
								return 0;
							}
						}
					}
					else
					{
							// maybe disp error ?!!
						///////////////////////////////////////////////////////////////////////////////
						cur->result = 1; // closed
						closesocket(ConnSocket);
						return 0;
					}

					iMode = 0;
					ioctlsocket(ConnSocket,FIONBIO,&iMode);
				}
			}
	}

	return 0;
}

char* ClearUnicode(char* src)
{
	if(!src)
	{
		src = new char[1];
		src[0] = '\0';
	}
	else
	{
		int i = 0;
		char a;
		while(src[i]!='\0')
		{
			a = src[i];
			if(!(((a>=32)&&(a<=126))||(a==13)||(a==10)))
				src[i] = ' ';

			i++;
		}
	}

	return src;
}

char* StringToChar(CString& src)
{
	long len = src.GetLength();//wcslen (src);
	char* Resultt = new char[len+1];
	//wcstombs(Resultt,src,len);
	size_t chars;
	errno_t err;
	err = wcstombs_s(&chars,Resultt,len+1,src,len+1);
	//Resultt[len] = '\0';

	if(err)
		exit(err);

	return Resultt;
}

CString CharToString(char* src)
{
	//CString temp(src);
	CString temp;

	if(!src)
		temp = _T("");
	else
	{
		LPWSTR a = temp.GetBuffer(2048);

		size_t chars;
		errno_t err;
		err = mbstowcs_s(&chars,a,2048,src,2048);

		if(err)
			exit(err);

		temp.ReleaseBuffer();
	}

	return temp;
}

char* MidStr(char* src,int start,int end)
{
	char* data = NULL;

	if(src==NULL)
		return data;

	if(strlen(src)<end||start<0||end<=start)
		return data;

	data = new char[end-start+1];
	memcpy(data,&(src[start]),end-start);
	data[end-start] = '\0';

	return data;
}

char* ShortToFull(char* src)
{
	char dec[8][4];

	for(int g3=0;g3<8;g3++)
		for(int g4=0;g4<4;g4++)
			dec[g3][g4] = '0';

	int tm = FindStr(src,"::");
	bool DoubleColoned = (tm!=-1);

	if(tm == -1)
		tm = 100;

	// double loop with dir 1 and dir -1

	bool started = false;
	int s_pos, e_pos;
	char* mg;

	int block = 0;

	int cur_seg = 0;

	int dir = 1;
	
	int i = 0;
	while(true)
	{
		if(!started)
		{
			if(src[i]!=':')
			{
				started = true;
				s_pos = i;
			}
		}
		else
		{
			if((src[i]==':')||(i==strlen(src)))
			{
				started = false;
				e_pos = i;

				if(dir==1)
					mg = MidStr(src,s_pos,e_pos);
				else
					mg = MidStr(src,e_pos+1,s_pos+1);
				int oh = 4-strlen(mg);
				for(int af=0;af<4;af++)
				{
					dec[cur_seg][af] = (af<oh)? '0' : mg[af-oh];
				}

				cur_seg = cur_seg+dir;
			}
		}

		if(DoubleColoned)
		{
			if(i==tm+1)
			{
				if(dir==1)
				{
					dir = -1;
					i = strlen(src)-1;
					started = false;
					cur_seg = 7;
					continue;
				}
				else
					break;
			}
		}
		else if(i==strlen(src))
			break;

		i = i+dir;
	}

	char* gk = new char[4*8+7];
	int curr = 0;
	for(int g3=0;g3<8;g3++)
	{
		for(int g4=0;g4<4;g4++)
		{
			gk[curr++] = dec[g3][g4];
		}
		
		if(g3<7)
			gk[curr++] = ':';
	}

	return gk;
}

int FindStr(char* src,const char* to_find, int start)
{
	bool found;
	int src_char = start;
	int to_len = strlen(to_find);
	while(src[src_char]!='\0')
	{
		int to_find_char = 0;
		found = true;
		while(to_find[to_find_char]!='\0')
		{
			if(src[src_char+to_find_char]=='\0')
				return -1;

			if(src[src_char+to_find_char]!=to_find[to_find_char])
			{
				found = false;
				break;
			}
			to_find_char++;
		}

		if(found)
			return src_char;

		src_char++;
	}

	return -1;
}

void Increment(char* src)
{
	bool again = false;

	int af = strlen(src)-1;
	for(int f=af;f>=0;f--)
	{
		if(src[f]==':')
			continue;

		again = false;

		char kg = src[f];
		int val = 0;

		if((kg>=48)&&(kg<58))
			val = kg-48;

		if((kg>=65)&&(kg<71))
			val = kg-65+10;

		if((kg>=97)&&(kg<103))
			val = kg-97+10;

		val++;
		if(val==16)
		{
			val = 0;
			again = true;
		}

		if((val>=0)&&(val<10))
			kg = val+48;

		if((val>=10)&&(val<16))
			kg = val-10+97;

		src[f] = kg;

		if(!again)
			break;
	}
}

char* FullToShort(char* src)
{
	char out[32+7+1];
	char out2[32+7+1];

	bool gp[8]; // 2001:0:0:1:0:0:0:1
	memset(&gp,0,8);
	int gp_cur = 0;

	int cursor = 0;
	int cursor2 = 0;

	bool ga = true;
	int x = 1;

	for(int i=0;i<strlen(src)-0;i++)
	{
		if((src[i]!='0')||x==4)
		{
			if((ga)&&(x==4)&&(src[i]=='0'))
				gp[gp_cur++] = true;
			if(((!ga)||(src[i]!='0'))&&(x==4))
				gp_cur++;
			ga = false;
		}

		if(src[i]==':')
		{
			ga = true;
			out[cursor++] = ':';
			x = 0;
		}
		else
		{
			if(i==strlen(src)-0)
			{
				ga = true;
				x = 0;
			}

			if(ga&& (src[i]=='0'))
				;
			else
				out[cursor++] = src[i];
		}

		x++;
	}

	out[cursor++] = '\0';

	int max_a = 1;
	bool is_p = false;
	bool p_prev = false;
	int ag_pos, ag_pos_end;

	int a_start, a_end;

	/*
		char* gw = new char[9];
		for(int i=0;i<8;i++)
		{
			if(gp[i])
				gw[i] = '1';
			else
				gw[i] = '0';
		}
		gw[8] = '\0';

		return gw;
*/
	for(int i=0;i<8;i++)
	{
		if(gp[i])
		{
			if(!p_prev)
			{
				a_start = i;
			}
		}
		else
		{
			if(p_prev)
			{
				a_end = i;
				int lenn = a_end-a_start;

				if(lenn>=2)
				{
					if(lenn>max_a)
					{
						max_a = lenn;
						ag_pos = a_start;
						ag_pos_end = a_end;
						is_p = true;
					}
				}
			}
		}
		
		p_prev = gp[i];
	}

	if(is_p==false)
	{
		char* gw = new char[cursor];
		memcpy(gw,&out,cursor);

		return gw;
	}
	else
	{
		int ca = 0;
		bool fo = true;
		bool f3 = true;
		for(int i=0;i<cursor;i++)
		{
			if(fo)
			{
				if(ca==ag_pos)
				{
					if(cursor2>0)
						cursor2--;
					f3 = false;
					out2[cursor2++] = ':';
					out2[cursor2++] = ':';
				}
				if(ca==ag_pos_end)
				{
					f3 = true;
				}
			}

			fo = false;

			if((out[i]==':'))
			{
				ca++;
				fo = true;
			}

			if(f3)
				out2[cursor2++] = out[i];
		}

		char* gw = new char[cursor2];
		memcpy(gw,&out2,cursor2);

		return gw;
	}
}

void GetPortsToCheck(bool* ports_use,CString port_str,int* ports_num)
{
	*ports_num = 0;

	char* ag = StringToChar(port_str);

	int aaf = strlen(ag);

	int posS = 0;
	int posE = 0;
	bool started = true;

	int from= -1;
	int to = -1;

	memset(ports_use,0,65536);

	char cur;
	char* tamp;

	//DWORD timout = 2;

	for(int i=0;i<aaf;i++)
	{
		cur = ag[i];

		if(started)
		{
			if(cur=='-')
			{
				posE = i;
				tamp = MidStr(ag,posS,posE);
				from = atoi(tamp);

				posS = i+1;
			}

			if((cur==',')||(i==aaf-1))
			{
				if(cur==',')
					posE = i;
				else
					posE = i+1;

				tamp = MidStr(ag,posS,posE);

				if(from==-1)
				{
					from = atoi(tamp);
					ports_use[from] = true;
					(*ports_num)++;

					from = to = -1;
					posS = i+1;
				}
				else
				{
					to = atoi(tamp);
					for(int o = from;o<=to;o++)
					{
						ports_use[o] = true;
						(*ports_num)++;
					}

					from = to = -1;
					posS = i+1;
				}
			}
		}
		else if(!started)
		{
			
		}
	}
}

void ClearOneTcp(CPortScannerDlg* Dlgmain, int* abpos)
{
				int how_m_used = 0;
			CTcpitam* prev = NULL;
			CTcpitam* curr = Dlgmain->first;
			while(curr)
			{
				if(curr->result==0)
				{
					how_m_used++;
					prev = curr;
					curr = curr->next;
				}
				else if(curr==Dlgmain->first)
				{
					Dlgmain->progress.SetPos((*abpos)++);

					if(curr->result==1)
					{
						int indax = Dlgmain->results.InsertItem(Dlgmain->results.GetItemCount(),CharToString(curr->server),0);
						Dlgmain->results.SetItemText(indax,1,CharToString(curr->port));
						Dlgmain->results.SetItemText(indax,2,_T("TCP"));
						Dlgmain->results.SetItemText(indax,3,_T("Closed"));
					}
					else if(curr->result==2)
					{
						int indax = Dlgmain->results.InsertItem(Dlgmain->results.GetItemCount(),CharToString(curr->server),1);
						Dlgmain->results.SetItemText(indax,1,CharToString(curr->port));
						Dlgmain->results.SetItemText(indax,2,_T("TCP"));
						Dlgmain->results.SetItemText(indax,3,_T("Open"));
					}


					//if(prev)
						//prev->next = curr->next;

					if(curr->next)
						curr->next->prev = curr->prev;

					Dlgmain->first = curr->next;

					if(Dlgmain->last==curr)
					{
						Dlgmain->last = curr->prev;
						if(Dlgmain->first==curr)
							Dlgmain->first = NULL;
					}

					prev = curr->prev;

					CTcpitam* ga3 = curr;
					curr = curr->next;
					delete ga3;
					//curr = NULL;

					
					
				}
				else
				{
					prev = curr;
					curr = curr->next;
				}

			}
}