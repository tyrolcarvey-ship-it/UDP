#include "stdafx.h"
#include "NetUdp.h"
#include <Winsock2.h>

CNetUdp::CNetUdp()
{
	_UdpAddr.inaddr = __T("192.168.3.100");//本机
	_UdpAddr.inProt = 2001;//本机
	_UdpAddr.outaddr = __T("192.168.3.199");//目标机
	_UdpAddr.outProt = 2000;//目标机
	Datalen = 0;
}
//SOCKET sockSrv = socket(AF_INET, SOCK_DGRAM, 0);

CNetUdp::~CNetUdp()
{
	m_Send.Close();
	m_Receives.Close();
	WSACleanup();
}
void CNetUdp::Dupclose()
{
	m_Send.Close();
	m_Receives.Close();
	WSACleanup();
}
void CNetUdp::ZR_Dupclose()
{
	m_Send.Close();
	m_Receives.Close();
	WSACleanup();
	//因为软件发送关闭还会读取数据  清空防止报错
}
DWORD CNetUdp::ZR_Init_NetUdp(LPSTR ip)
{
	CString z_inaddr = _T("192.168.3.100");//本机IP地址
	CString z_outaddr(ip);
	UINT     z_inProt = 2001;//本机端口号
	UINT     z_outProt = 2000;//目标机端口号

	DWORD res;
	DWORD lp = 0;
	CString str;
	res = WSAStartup(MAKEWORD(1, 1), &wsadata);
	if (res != 0)
		return 1;
	//	bind(sockSrv, (SOCKADDR*)&_UdpAddr, sizeof(SOCKADDR));
	if (!m_Send.Create(z_outProt, SOCK_DGRAM, 0))//端口为目标机端口
	{
		lp = GetLastError();
		return lp;

	}
	///**/if (!m_Send.Bind(z_outProt, (LPCTSTR)z_outaddr))
	//{
	//	lp = GetLastError();
	//	return lp;
	//}
	if (!m_Receives.Create(z_inProt, SOCK_DGRAM, 0))//端口为本机端口
	{
		lp = GetLastError();
		return lp;
	}
	//if (!m_Receives.Bind(z_inProt, (LPCTSTR)z_inaddr))//本机端口和IP
	//{
	//	lp = GetLastError();
	//	return lp;
	//}

	return lp;
}



DWORD CNetUdp::Init_NetUdp(struct NET_UDPIPADD Addr)
{
	DWORD res;
	DWORD lp = 0;
	CString str;
	_UdpAddr = Addr;
	res = WSAStartup(MAKEWORD(1, 1), &wsadata);
	if (res != 0)
		return 1;
	//	bind(sockSrv, (SOCKADDR*)&_UdpAddr, sizeof(SOCKADDR));
	if (!m_Send.Create(Addr.outProt, SOCK_DGRAM, 0))//端口为目标机端口
	{
		lp = GetLastError();
		return lp;

	}
	/**/if (m_Send.Bind(Addr.outProt, Addr.outaddr))
	{
		lp = GetLastError();
		return lp;
	}
	if (!m_Receives.Create(Addr.inProt, SOCK_DGRAM, 0))//端口为本机端口
	{
		lp = GetLastError();
		return lp;
	}
	if (m_Receives.Bind(Addr.inProt, Addr.inaddr))//本机端口和IP
	{
		lp = GetLastError();
		return lp;
	}

	return lp;
}
DWORD CNetUdp::Send_Data(unsigned char* data, int lenData)
{
	DWORD lp = 0;
	if (lenData > 1000) return 0;
	Sleep(1);
	DWORD ret = m_Send.SendTo(data, lenData, _UdpAddr.outProt, _UdpAddr.outaddr, 0);
	if (ret != lenData)
	{
		lp = GetLastError();
		return lp;
	}
	return ret;
}
DWORD  CNetUdp::Receive_data(unsigned char* data, int len)
{
	if (len > 1000) return 0;
	DWORD lp = 0;
	DWORD ret;
	Sleep(1);
	ret = m_Receives.ReceiveFrom(data, len, _UdpAddr.inaddr, _UdpAddr.inProt, 0);
	if (ret != len)
	{
		lp = GetLastError();
		return lp;
	}
	return ret;
}


