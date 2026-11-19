#pragma once
#include "afxSock.h"

#include "heads.h"

#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <windows.h>
class CNetUdp
{
public:
	CNetUdp();
	~CNetUdp();
public:
	DWORD      Init_NetUdp(struct NET_UDPIPADD Addr);
	DWORD      ZR_Init_NetUdp(LPSTR ip);
	DWORD      Send_Data(unsigned char* data, int lenData);
	DWORD      Receive_data(unsigned char* data,int len);
	void     Dupclose();
	void     ZR_Dupclose();

public:
	NET_UDPIPADD _UdpAddr;
	CAsyncSocket  m_Send;
	CAsyncSocket  m_Receives;
	WSADATA wsadata;
	int Datalen;
};

