#pragma once
#include "NetUdp.h"
#include <memory>

#define MAX_CARD_NUMBER 250

class CNetCardManage
{
public:
	CNetCardManage();
	~CNetCardManage();

	DWORD    Init_NetUdp(short CardId, struct NET_UDPIPADD Addr);
	DWORD    ZR_Init_NetUdp(short CardId, LPSTR ip);
	DWORD    Send_Data(short CardId, unsigned char* data, int lenData);
	DWORD    Receive_data(short CardId, unsigned char* data, int len);
	void     Dupclose(short CardId);

private:
	std::unique_ptr<CNetUdp> m_CardList[MAX_CARD_NUMBER];
	//std::map<int, BOOL> m_CardState;     //控制卡状态，key表示控制卡ID，value表示是否初始化
};

