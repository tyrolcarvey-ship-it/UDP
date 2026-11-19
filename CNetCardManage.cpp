#include "stdafx.h"
#include "CNetCardManage.h"


CNetCardManage::CNetCardManage()
{
	for (auto &x : m_CardList)
	{
		x.reset(nullptr);
	}
}

CNetCardManage::~CNetCardManage()
{
}

DWORD CNetCardManage::Init_NetUdp(short CardId, struct NET_UDPIPADD Addr)
{
	ASSERT(CardId < MAX_CARD_NUMBER);

	if (m_CardList[CardId] == nullptr)
	{
		m_CardList[CardId].reset(new CNetUdp());
	}

	return m_CardList[CardId]->Init_NetUdp(Addr);
}

DWORD CNetCardManage::ZR_Init_NetUdp(short CardId, LPSTR ip)
{
	ASSERT(CardId < MAX_CARD_NUMBER);

	if (m_CardList[CardId] == nullptr)
	{
		m_CardList[CardId].reset(new CNetUdp());
	}

	return m_CardList[CardId]->ZR_Init_NetUdp(ip);
}

DWORD CNetCardManage::Send_Data(short CardId, unsigned char* data, int lenData)
{
	ASSERT(CardId < MAX_CARD_NUMBER);
	ASSERT(m_CardList[CardId] != nullptr);
	return m_CardList[CardId]->Send_Data(data, lenData);
}

DWORD CNetCardManage::Receive_data(short CardId, unsigned char* data, int len)
{
	ASSERT(CardId < MAX_CARD_NUMBER);
	ASSERT(m_CardList[CardId] != nullptr);
	return m_CardList[CardId]->Receive_data(data, len);
}

void CNetCardManage::Dupclose(short CardId)
{
	ASSERT(CardId < MAX_CARD_NUMBER);
	if (m_CardList[CardId] == nullptr)
	{
		return;
	}

	m_CardList[CardId]->Dupclose();
}