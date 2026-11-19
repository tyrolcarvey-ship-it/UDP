// NetUdpDll.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "NetUdpDll.h"
#include "NetUdp.h"
#include "DataCrc.h"
#include <Windows.h>
//#include <iomanip>
//#include <chrono>
//#include <ctime>
#include <string>
#include <fstream>
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CNetUdpDllApp, CWinApp)
END_MESSAGE_MAP()
#define DATALEN 64

// CNetUdpDllApp 构造

CNetUdpDllApp::CNetUdpDllApp()
{
	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CNetUdpDllApp 对象

CNetUdpDllApp theApp;

CNetUdp udpl;
MotorParaData Motdata;
// CNetUdpDllApp 初始化

// 定义全局互斥锁
CRITICAL_SECTION g_criticalSection;

//// 初始化互斥锁
//void InitializeLock()
//{
//	InitializeCriticalSection(&g_criticalSection);
//}
//
//// 销毁互斥锁
//void DestroyLock()
//{
//	DeleteCriticalSection(&g_criticalSection);
//}
//
//
//// 在需要加锁的地方调用这个函数
//void Lock()
//{
//	// 获取互斥锁
//	EnterCriticalSection(&g_criticalSection);
//}
//
//// 在不再需要锁的地方调用这个函数
//void Unlock()
//{
//	// 释放互斥锁
//	LeaveCriticalSection(&g_criticalSection);
//}
//

BOOL CNetUdpDllApp::InitInstance()
{
	CWinApp::InitInstance();
	return TRUE;
}


MotorParaData MData;
int startx, starty, startz, startq, starts;
int Xtemp, Ytemp, Ztemp;
BOOL tempstatey = 0, tempstatez = 0;
int DOstate;
int direction_c;
double Xposition, Yposition, Zposition;
int data[10]; unsigned char Input[65];
int iflag = 0;
int state;
unsigned char errorpoint[64];
int pointerror = 0;
int count = 1;
int temp[10];
int nm = 0;
int Xstate = 0, Ystate = 0, Zstate = 0;
int lockread = 1;
int speeda = 0, accelerationa = 0;
int returnzero = 0;
BOOL Xsign = TRUE, Ysign = TRUE, Zsign = TRUE;
short connectstate = 0;
short Xnow = 0, Ynow = 0, Znow = 0;
BOOL startjoy = 0;
LPSTR iptemp;
extern "C" __declspec(dllexport)void PASCAL EXPORT SetMotorStartVal(int x, int y, int z)
{
	startx = x;
	starty = y;
	startz = z;
}
extern "C" __declspec(dllexport)void PASCAL EXPORT SetMotorStartValQ(int q)
{
	startq = q;
}
extern "C" __declspec(dllexport)void PASCAL EXPORT SetMotorStartValS(int s)
{
	starts = s;
}
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT InitNetUdp(struct NET_UDPIPADD Addr)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	DOstate = 0;
	//InitializeLock();
	return (udpl.Init_NetUdp(Addr));
}
extern "C" __declspec(dllexport)void   PASCAL EXPORT  Dupclose()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//DestroyLock();
	return (udpl.Dupclose());
}
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_InitNetUdp(int* val, LPSTR ip)//修改后初始化网络
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//InitializeLock();
	int res = udpl.ZR_Init_NetUdp(ip);
	iptemp = ip;
	int mcd;
	*val = 1;
	//	DOstate = 0;
	return res;
}
extern "C" __declspec(dllexport)DWORD  PASCAL EXPORT  ZR_Dupclose()//修改后关闭网络
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//DestroyLock();
	udpl.ZR_Dupclose();
	return (0);
}

extern "C" __declspec(dllexport)DWORD SetMotorParasNet(motionpara mpara)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	unsigned char data[64], val;
	int speedneed = (600000000000) / (mpara.speed * 2);
	if (mpara.acceleration > 6)  mpara.acceleration = 6;
	if (mpara.acceleration < 0)  mpara.acceleration = 0;
	//mpara.speed = 50000000 / (mpara.speed * 2);
	mpara.speed = mpara.speed;
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 26;
	data[4] = mpara.speed >> 24;
	data[5] = mpara.speed >> 16;
	data[6] = mpara.speed >> 8;
	data[7] = mpara.speed;
	data[8] = 1000 >> 24;//空走速度直接写成1000
	data[9] = 1000 >> 16;
	data[10] = 1000 >> 8;
	data[11] = 1000;
	data[16] = 0;
	data[17] = 0;
	data[18] = mpara.acceleration;
	data[19] = mpara.acceleration;
	data[20] = speedneed >> 24;
	data[21] = speedneed >> 16;
	data[22] = speedneed >> 8;
	data[23] = speedneed;
	unsigned char chv = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	if (val != 25)
	{
		int retrn;
		ZR_InitNetUdp(&retrn, iptemp);
		val = udpl.Send_Data(data, 25);
		return val;
	}
	return (0);
}

//extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_Interpolate_move(short card_no, short axis1, short axis2, double x_distance, double y_distance, double start_vel, double max_vel, double t_acc, double t_dec)
//{
//	count = 0;
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
//	int dirx = 1, diry = 1;
//	int Xm = 0, Ym = 0;
//	if (max_vel == 0) { return 0; }
//	if (x_distance < 0)
//	{
//		dirx = 0;
//	}
//	if (y_distance < 0)
//	{
//		diry = 0;
//	}
//	motionpara mpa;
//	mpa.laserSpace = 10;
//	mpa.motiontype = 1;
//	mpa.motorEn = 1;
//	mpa.railspace = 10;
//	mpa.xrailspace = 10;
//	mpa.yrailspace = 10;
//	mpa.zrailspace = 10;
//	mpa.speed = max_vel;
//	mpa.acceleration = int(t_acc * 10);
//	mpa.weekpul = 5000;
//	mpa.xweekpul = 5000;
//	mpa.yweekpul = 5000;
//	mpa.zweekpul = 5000;
//	Xm = int(abs(x_distance));
//	Ym = int(abs(y_distance));
//	mpa.z = 0;
//	unsigned char data[64];
//	int val;
//	unsigned char chv = 0;
//	int laserSpace = mpa.laserSpace;
//	SetMotorParasNet(mpa);
//	Sleep(5);
//	data[0] = 0;
//	data[1] = 0;
//	data[2] = 0;
//	data[3] = 29;
//	data[4] = Xm >> 24;
//	data[5] = Xm >> 16;
//	data[6] = Xm >> 8;
//	data[7] = Xm;
//	data[8] = Ym >> 24;
//	data[9] = Ym >> 16;
//	data[10] = Ym >> 8;
//	data[11] = Ym;
//	data[12] = 0;
//	data[13] = 0;
//	data[14] = 0;
//	data[15] = 0;
//	data[16] = 0;
//	data[17] = 1;
//	data[18] = laserSpace;//激光触发间隔;
//	data[19] = (dirx << 2) + (diry << 1) + 0;//,占1位是否出激光-占2位马达顺序-占3位马达方向,
//	data[20] = (1 << 5) + (1 << 4) + (1 << 3)
//		+ (1 << 2) + (1 << 1) + (1);
//	data[21] = 0;
//	data[22] = 0;
//	data[23] = 0;
//	for (int i = 0; i < 24; i++) chv += data[i];
//	data[24] = chv;
//	val = udpl.Send_Data(data, 25);
//	if (val != 25)
//	{
//		int retrn;
//		ZR_InitNetUdp(&retrn, iptemp);
//		val = udpl.Send_Data(data, 25);
//		return val;
//	}
//	count = 1;
//	return 0;
//}


extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_Interpolate_move(short card_no, short axis1, short axis2, double x_distance, double y_distance, double start_vel, double max_vel, double t_acc, double t_dec)
{
	count = 0;
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	int dirx = 1, diry = 1;
	int Xm = 0, Ym = 0;
	if (max_vel == 0) { return 0; }
	if (x_distance < 0)
	{
		dirx = 0;
	}
	if (y_distance < 0)
	{
		diry = 0;
	}
	motionpara mpa;
	mpa.laserSpace = 10;
	mpa.motiontype = 1;
	mpa.motorEn = 1;
	mpa.railspace = 10;
	mpa.xrailspace = 10;
	mpa.yrailspace = 10;
	mpa.zrailspace = 10;
	mpa.speed = max_vel;
	mpa.acceleration = int(t_acc * 10);
	mpa.weekpul = 5000;
	mpa.xweekpul = 5000;
	mpa.yweekpul = 5000;
	mpa.zweekpul = 5000;
	Xm = int(abs(x_distance));
	Ym = int(abs(y_distance));
	mpa.z = 0;
	unsigned char data[64];
	int val;
	unsigned char chv = 0;
	int laserSpace = mpa.laserSpace;
	int speedneed = (600000000000) / (mpa.speed * 2);
	if (mpa.acceleration > 6)  mpa.acceleration = 6;
	if (mpa.acceleration < 0)  mpa.acceleration = 0;
	//mpara.speed = 50000000 / (mpara.speed * 2);
	mpa.speed = mpa.speed;
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 26;
	data[4] = mpa.speed >> 24;
	data[5] = mpa.speed >> 16;
	data[6] = mpa.speed >> 8;
	data[7] = mpa.speed;
	data[8] = 1000 >> 24;//空走速度直接写成1000
	data[9] = 1000 >> 16;
	data[10] = 1000 >> 8;
	data[11] = 1000;
	data[16] = 0;
	data[17] = 0;
	data[18] = mpa.acceleration;
	data[19] = mpa.acceleration;
	data[20] = speedneed >> 24;
	data[21] = speedneed >> 16;
	data[22] = speedneed >> 8;
	data[23] = speedneed;
	data[24] = 0;
	data[25] = 0;
	data[26] = 0;
	data[27] = 29;
	data[28] = Xm >> 24;
	data[29] = Xm >> 16;
	data[30] = Xm >> 8;
	data[31] = Xm;
	data[32] = Ym >> 24;
	data[33] = Ym >> 16;
	data[34] = Ym >> 8;
	data[35] = Ym;
	data[40] = 0;
	data[41] = 1;
	data[42] = laserSpace;//激光触发间隔;
	data[43] = (dirx << 2) + (diry << 1) + 0;//,占1位是否出激光-占2位马达顺序-占3位马达方向,
	data[44] = (1 << 5) + (1 << 4) + (1 << 3)
		+ (1 << 2) + (1 << 1) + (1);
	data[45] = 0;
	data[46] = 0;
	data[47] = 0;
	for (int i = 0; i < 48; i++) chv += data[i];
	data[48] = chv;
	val = udpl.Send_Data(data, 49);
	if (val != 49)
	{
		int retrn;
		ZR_InitNetUdp(&retrn, iptemp);
		val = udpl.Send_Data(data, 49);
		return val;
	}
	count = 1;
	return 0;
}



extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_Interpolation_move(short card_no, short axis1, short axis2, double x_distance, double y_distance, double start_vel, double max_vel, double t_acc, double t_dec)//修改后插补运动
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//if (x_distance > 0) { x_distance += 0.5; }
	//else if (x_distance < 0) { x_distance -= 0.5; }
	//if (y_distance > 0) { y_distance += 0.5; }
	//else if (y_distance < 0) { y_distance -= 0.5; }
	motionpara mpa;
	mpa.laserSpace = 10;
	mpa.motiontype = 1;
	mpa.motorEn = 1;
	mpa.railspace = 10;
	mpa.xrailspace = 10;
	mpa.yrailspace = 10;
	mpa.zrailspace = 10;
//	mpa.speed = start_vel;
	mpa.speed = max_vel;
	mpa.acceleration = int(t_acc * 10);
	mpa.weekpul = 5000;
	mpa.xweekpul = 5000;
	mpa.yweekpul = 5000;
	mpa.zweekpul = 5000;
	mpa.linerazero[0] = 0;
	mpa.linerazero[1] = 0;
	mpa.linerazero[2] = 0;
	if (axis1 == 0)
	{
		mpa.x = x_distance;
		if (axis2 == 1)
		{
			mpa.y = y_distance;
		}
		else if (axis2 == 2)
		{
			mpa.z = y_distance;
		}
	}
	else if (axis1 == 1)
	{
		mpa.y = x_distance;
		if (axis2 == 0)
		{
			mpa.x = y_distance;
		}
		else if (axis2 == 2)
		{
			mpa.z = y_distance;
		}
	}
	else if (axis1 == 2)
	{
		mpa.z = x_distance;
		if (axis2 == 0)
		{
			mpa.x = y_distance;
		}
		else if (axis2 == 1)
		{
			mpa.y = y_distance;
		}
	}
	mpa.x *= 0.0005;
	mpa.y *= 0.0005;
	mpa.z *= 0.0005;
	SendMotorDataNet(mpa);
	return 0;
}


extern "C" __declspec(dllexport)DWORD PASCAL EXPORT SendMotorDataNet(motionpara mparas)
{
	count = 0;
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	unsigned char data[128];
	int val;
	unsigned char chv = 0;
	int tradex = mparas.x, tradey = mparas.y, tradez = mparas.z, laserSpace = mparas.laserSpace, motiontype = mparas.motiontype, motorEn = mparas.motorEn;
	int tradexf = abs(mparas.x - tradex) * 1000, tradeyf = abs(mparas.y - tradey) * 1000, tradezf = abs(mparas.z - tradez) * 1000;
	if ((mparas.x < 0) && (tradex == 0))
	{
		tradex = 0xFFFF;
	}
	if ((mparas.y < 0) && (tradey == 0))
	{
		tradey = 0xFFFF;
	}
	if ((mparas.z < 0) && (tradez == 0))
	{
		tradez = 0xFFFF;
	}

	SetMotorParasNet(mparas);
	Sleep(5);
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 8;
	data[4] = tradex >> 8;
	data[5] = tradex;
	data[6] = tradexf >> 8;
	data[7] = tradexf;
	data[8] = tradey >> 8;
	data[9] = tradey;
	data[10] = tradeyf >> 8;
	data[11] = tradeyf;
	data[12] = tradez >> 8;
	data[13] = tradez;
	data[14] = tradezf >> 8;
	data[15] = tradezf;
	data[16] = 0;
	data[17] = motorEn;
	data[18] = laserSpace;//激光出发间隔
	data[19] = motiontype;//马达使能和是否出激光
	data[20] = (mparas.motorrst[2] << 5) + (mparas.motorrst[1] << 4) + (mparas.motorrst[0] << 3)
		+ (mparas.linerazero[2] << 2) + (mparas.linerazero[1] << 1) + (mparas.linerazero[0]);
	data[21] = 0;
	data[22] = 0;
	data[23] = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	//	while (pointerror != 62464)
	//	{
	val = udpl.Send_Data(data, 25);
	if (val != 25)
	{
		int retrn;
		ZR_InitNetUdp(&retrn, iptemp);
		val = udpl.Send_Data(data, 25);
		return val;
	}
	//		Sleep(10);
	//		val = udpl.Receive_data(errorpoint, 2);
	//		pointerror = (errorpoint[0] << 8) + errorpoint[1];
	//	}
	count = 1;
//	pointerror = 0;
	return 0;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_Absolute_move(short axis, double distance, double start_vel, double max_vel, double t_acc, double t_dec)
{
	ZR_relative_move(axis,distance, start_vel, max_vel, t_acc, t_dec);
	return 0;
}

//extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_Absolute_move(short axis, double distance, double start_vel, double max_vel, double t_acc, double t_dec)
//{
//	count = 0;
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
//	if (distance >= 1000000) distance = 1000000;
//	int idistance = distance;
//	motionpara mpa;
//	mpa.speed = max_vel;
//	mpa.acceleration = int(t_acc * 10);
//	mpa.laserSpace = 10;
//	mpa.motiontype = 1;
//	mpa.motorEn = 1;
//	mpa.railspace = 10;
//	mpa.xrailspace = 10;
//	mpa.yrailspace = 10;
//	mpa.zrailspace = 10;
////	mpa.speed = start_vel;
//	mpa.weekpul = 5000;
//	mpa.xweekpul = 5000;
//	mpa.yweekpul = 5000;
//	mpa.zweekpul = 5000;
//	mpa.linerazero[0] = 0;
//	mpa.linerazero[1] = 0;
//	mpa.linerazero[2] = 0;
//	mpa.motorrst[0] = 1;
//	mpa.motorrst[1] = 1;
//	mpa.motorrst[2] = 1;
//	mpa.z = 0;
//	mpa.y = 0;
//	mpa.x = 0;
//	if (axis == 0)
//	{
//		mpa.x = idistance;
//	}
//	else if (axis == 1)
//	{
//		mpa.y = idistance;
//	}
//	else if (axis == 2)
//	{
//		mpa.z = idistance;
//	}
//	mpa.x *= 0.0005;
//	mpa.y *= 0.0005;
//	mpa.z *= 0.0005;
//	unsigned char data[128];
//	int val;
//	unsigned char chv = 0;
//	int tradex = mpa.x, tradey = mpa.y, tradez = mpa.z, laserSpace = mpa.laserSpace, motiontype = mpa.motiontype, motorEn = mpa.motorEn;
//	int tradexf = abs(mpa.x - tradex) * 1000, tradeyf = abs(mpa.y - tradey) * 1000, tradezf = abs(mpa.z - tradez) * 1000;
//	if (axis == 2)
//	{
//		SetMotorParasNetQ(mpa);
//		Sleep(10);
//		data[0] = 0;
//		data[1] = 0;
//		data[2] = 0;
//		data[3] = ??;
//		data[4] = tradex >> 8;
//		data[5] = tradex;
//		data[6] = tradexf >> 8;
//		data[7] = tradexf;
//		data[8] = tradey >> 8;
//		data[9] = tradey;
//		data[10] = tradeyf >> 8;
//		data[11] = tradeyf;
//		data[12] = tradez >> 8;
//		data[13] = tradez;
//		data[14] = tradezf >> 8;
//		data[15] = tradezf;
//		data[16] = unsigned char(axis + 1);
//		data[17] = motorEn;
//		data[18] = laserSpace;//激光出发间隔
//		data[19] = motiontype;//马达使能和是否出激光
//		data[20] = (mpa.motorrst[2] << 5) + (mpa.motorrst[1] << 4) + (mpa.motorrst[0] << 3)
//			+ (mpa.linerazero[2] << 2) + (mpa.linerazero[1] << 1) + (mpa.linerazero[0]);
//		data[21] = 0;
//		data[22] = 0;
//		data[23] = 0;
//		for (int i = 0; i < 24; i++) chv += data[i];
//		data[24] = chv;
//		//	while (pointerror != 62464)
//		//	{
//		val = udpl.Send_Data(data, 25);
//		if (val != 25)
//		{
//			int retrn;
//			ZR_InitNetUdp(&retrn, iptemp);
//			val = udpl.Send_Data(data, 25);
//			return val;
//		}
//	}
//	else
//	{
//		SetMotorParasNet(mpa);
//		Sleep(10);
//		data[0] = 0;
//		data[1] = 0;
//		data[2] = 0;
//		data[3] = 30;
//		data[4] = tradex >> 8;
//		data[5] = tradex;
//		data[6] = tradexf >> 8;
//		data[7] = tradexf;
//		data[8] = tradey >> 8;
//		data[9] = tradey;
//		data[10] = tradeyf >> 8;
//		data[11] = tradeyf;
//		data[12] = tradez >> 8;
//		data[13] = tradez;
//		data[14] = tradezf >> 8;
//		data[15] = tradezf;
//		data[16] = unsigned char(axis + 1);
//		data[17] = motorEn;
//		data[18] = laserSpace;//激光出发间隔
//		data[19] = motiontype;//马达使能和是否出激光
//		data[20] = (mpa.motorrst[2] << 5) + (mpa.motorrst[1] << 4) + (mpa.motorrst[0] << 3)
//			+ (mpa.linerazero[2] << 2) + (mpa.linerazero[1] << 1) + (mpa.linerazero[0]);
//		data[21] = 0;
//		data[22] = 0;
//		data[23] = 0;
//		for (int i = 0; i < 24; i++) chv += data[i];
//		data[24] = chv;
//		//	while (pointerror != 62464)
//		//	{
//		val = udpl.Send_Data(data, 25);
//		if (val != 25)
//		{
//			int retrn;
//			ZR_InitNetUdp(&retrn, iptemp);
//			val = udpl.Send_Data(data, 25);
//			return val;
//		}
//	}
//	//		Sleep(20);
//	//		val = udpl.Receive_data(errorpoint, 2);
//	//		pointerror = (errorpoint[0] << 8) + errorpoint[1];
//	//	}
//	count = 1;
//	pointerror = 0;
//	return 0;
//}
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_Uniaxialmove(short axis, double start_vel, double max_vel, double t_acc)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	int dir = 1;
	if (start_vel < 0)
	{
		dir = 0; start_vel = abs(start_vel);
	}
	if (max_vel < 0)
	{
		dir = 0; max_vel = abs(max_vel);
	}
	motionpara mpa;
	mpa.speed = max_vel;
	mpa.acceleration = int(t_acc * 10);
	mpa.laserSpace = 10;
	mpa.motiontype = 1;
	mpa.motorEn = 1;
	mpa.railspace = 10;
	mpa.xrailspace = 10;
	mpa.yrailspace = 10;
	mpa.zrailspace = 10;
//	mpa.speed = start_vel;
	mpa.weekpul = 5000;
	mpa.xweekpul = 5000;
	mpa.yweekpul = 5000;
	mpa.zweekpul = 5000;
	mpa.linerazero[0] = 0;
	mpa.linerazero[1] = 0;
	mpa.linerazero[2] = 0;
	mpa.x = 1000000;
	mpa.y = 1000000;
	mpa.z = 1000000;
	if (axis == 2)
	{
		SendMotorDataNetQ(mpa, dir);//暂且Q轴当Z轴
	}
	else 
	{
		SendMotorMove(mpa, dir, axis);
	}
	return 0;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_relative_move(short axis, double distance, double start_vel, double max_vel, double t_acc, double t_dec)
{
	count = 0;
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	int dir = 1;
	if(axis == 2)
	{
		if (abs(distance) < 1) { return 0; }
	}
	if (distance < 0)
	{
		dir = 0; distance = abs(distance);
	}
	int idistance = distance;
	motionpara mpa;
	mpa.speed = max_vel;
	mpa.acceleration = int(t_acc * 10);
	mpa.laserSpace = 10;
	mpa.motiontype = 1;
	mpa.motorEn = 1;
	mpa.railspace = 10;
	mpa.xrailspace = 10;
	mpa.yrailspace = 10;
	mpa.zrailspace = 10;
	mpa.weekpul = 5000;
	mpa.xweekpul = 5000;
	mpa.yweekpul = 5000;
	mpa.zweekpul = 5000;
	mpa.linerazero[0] = 0;
	mpa.linerazero[1] = 0;
	mpa.linerazero[2] = 0; 
	mpa.x = idistance;
	mpa.y = idistance;
	mpa.z = idistance;
	if (axis == 2) 
	{
		SendMotorDataNetQ(mpa, dir);
	}
	else if (axis == 1)
	{
		SendMotorDataNetY(mpa, dir);
	}
	else { SendMotorMove(mpa, dir, axis); }
//	else { SendMotorMove(mpa, dir, axis); }
	count = 1;
	return 0;
}


extern "C" __declspec(dllexport)DWORD PASCAL EXPORT SendMotorDataNetY(motionpara mparas, int dir)
{
	int val = 0, num = 0, datanum = 0;
	unsigned char data[128], rdata[10];
	unsigned char motororEn = mparas.motorEn;//马达使能;
	unsigned char motororstop = mparas.motorstop;//马达停止;
	unsigned char motiontype = mparas.motiontype;//是否出激光;
	unsigned char laserSpace = mparas.laserSpace;//激光触发间距;
	int tradey = mparas.y * 2;
	for (int i = 0; i < 24; i++)
	{
		data[i] = 0;
	}
	SetMotorParasNetY(mparas);
	Sleep(5);
aa:	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 21;
	data[4] = tradey >> 24;
	data[5] = tradey >> 16;
	data[6] = tradey >> 8;
	data[7] = tradey;
	data[8] = 0;
	data[9] = 0;
	data[10] = 0;
	data[11] = 0;
	data[12] = 0;
	data[13] = 0;
	data[14] = 0;
	data[15] = 0;
	data[16] = 0;
	data[17] = 0;
	int motororder = 0;
	data[18] = laserSpace;//激光触发间隔;
	data[19] = (motororEn << 6) + (1 << 5) + (motororder << 4) + (motororder << 3) + (0 << 2) + (dir << 1) + 0;//,占1位是否出激光-占2位马达顺序-占3位马达方向,
	data[20] = 0;
	data[21] = 0;
	data[22] = 0;
	data[23] = (mparas.motorrst[2] << 5) + (mparas.motorrst[1] << 4) + (mparas.motorrst[0] << 3)
		+ (mparas.linerazero[2] << 2) + (mparas.linerazero[1] << 1) + (mparas.linerazero[0]);
	unsigned char chv = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	if (val != 25)
	{
		return val;
	}
	return (val);
}
extern "C" __declspec(dllexport)DWORD SetMotorParasNetY(motionpara mpara)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	unsigned char data[64], val;
	int speedneed = (600000000000) / (mpara.speed * 2);
	if (mpara.acceleration > 6)  mpara.acceleration = 6;
	if (mpara.acceleration < 0)  mpara.acceleration = 0;
	//	mpara.speed = 50000000 / (mpara.speed * 2);
	mpara.speed = mpara.speed;
aa: data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 22;
	data[4] = mpara.speed >> 24;
	data[5] = mpara.speed >> 16;
	data[6] = mpara.speed >> 8;
	data[7] = mpara.speed;
	data[8] = 1000 >> 24;//空走速度设置1000
	data[9] = 1000 >> 16;
	data[10] = 1000 >> 8;
	data[11] = 1000;
	data[16] = 0;
	data[17] = 0;
	data[18] = mpara.acceleration;
	data[19] = mpara.acceleration;
	data[20] = speedneed >> 24;
	data[21] = speedneed >> 16;
	data[22] = speedneed >> 8;
	data[23] = speedneed;
	unsigned char chv = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	if (val != 25)
	{
		int retrn;
		ZR_InitNetUdp(&retrn, iptemp);
		val = udpl.Send_Data(data, 25);
		return val;
	}
	return (0);
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT SendMotorMove(motionpara mparas, int dir, int index)
{
	int val = 0, num = 0;
	int motororder = 0;
	unsigned char data[128];
	unsigned char rdata[10];
	unsigned char motororEn = mparas.motorEn;//马达使能;
	unsigned char motororstop = mparas.motorstop;//马达停止;
	unsigned char motiontype = mparas.motiontype;//是否出激光;
	unsigned char laserSpace = mparas.laserSpace;//激光触发间距;
	int tradex = mparas.x;// *(mparas.xweekpul / mparas.xrailspace);
	int tradey = mparas.y;// *(mparas.yweekpul / mparas.yrailspace);
	int tradez = mparas.z;// *(mparas.zweekpul / mparas.zrailspace);
	mparas.linerazero;
	SetMotorParasNet(mparas);
	Sleep(5);
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 20;
aa:	switch (index)
{
case 0:
	data[4] = tradex >> 24;
	data[5] = tradex >> 16;
	data[6] = tradex >> 8;
	data[7] = tradex;//最长轴马达长度--127:96

	data[8] = 0;
	data[9] = 0;

	data[10] = 0;
	data[11] = 0;
	data[12] = 0;
	data[13] = 0;

	data[14] = 0;
	data[15] = 0;
	data[16] = 0;
	data[17] = 0;
	data[18] = laserSpace;//激光触发间隔;
	motororder = 0;
	data[19] = (motororEn << 6) + (motiontype << 5) + (motororder << 4) + (motororder << 3) + (dir << 2) + (0 << 1) + 0;//,占1位是否出激光-占2位马达顺序-占3位马达方向,--15:0
	break;
case 1:
	data[4] = tradey >> 24;
	data[5] = tradey >> 16;
	data[6] = tradey >> 8;
	data[7] = tradey;
	data[8] = 0;
	data[9] = 0;
	data[10] = 0;
	data[11] = 0;
	data[12] = 0;
	data[13] = 0;
	data[14] = 0;
	data[15] = 0;
	data[16] = 0;
	data[17] = 0;
	motororder = 2;
	data[18] = laserSpace;//激光触发间隔;
	data[19] = (motororEn << 6) + (motiontype << 5) + (motororder << 4) + (motororder << 3) + (0 << 2) + (dir << 1) + 0;//,占1位是否出激光-占2位马达顺序-占3位马达方向,
	break;
case 2:
	data[4] = tradez >> 24;
	data[5] = tradez >> 16;
	data[6] = tradez >> 8;
	data[7] = tradez;
	data[8] = 0;
	data[9] = 0;
	data[10] = 0;
	data[11] = 0;
	data[12] = 0;
	data[13] = 0;
	data[14] = 0;
	data[15] = 0;
	data[16] = 0;
	data[17] = 0;
	motororder = 3;
	data[18] = laserSpace;//激光触发间隔;
	data[19] = (motororEn << 6) + (motiontype << 5) + (motororder << 4) + (motororder << 3) + (0 << 2) + (0 << 1) + dir;//,占1位是否出激光-占2位马达顺序-占3位马达方向,
	break;
}
unsigned char chv = 0;
data[20] = 0;
data[21] = 0;
data[22] = 0;
data[23] = (mparas.motorrst[2] << 5) + (mparas.motorrst[1] << 4) + (mparas.motorrst[0] << 3)
+ (mparas.linerazero[2] << 2) + (mparas.linerazero[1] << 1) + (mparas.linerazero[0]);
for (int i = 0; i < 24; i++) chv += data[i];
data[24] = chv;
//	while (pointerror != 62464)
//	{
val = udpl.Send_Data(data, 25);
//		Sleep(20);
//		val = udpl.Receive_data(errorpoint, 2);
//		pointerror = (errorpoint[0] << 8) + errorpoint[1];
//	}
pointerror = 0;
if (val != 25)
{
	int retrn;
	ZR_InitNetUdp(&retrn, iptemp);
	val = udpl.Send_Data(data, 25);
	return val;
}
return 0;
}
extern "C" __declspec(dllexport)DWORD SetMotorParasNetQ(motionpara mpara)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	unsigned char data[64], val;
	int speedneed = (600000000000) / (mpara.speed * 2);
	if (mpara.acceleration > 6)  mpara.acceleration = 6;
	if (mpara.acceleration < 0)  mpara.acceleration = 0;
//	mpara.speed = 50000000 / (mpara.speed * 2);
	mpara.speed = mpara.speed;
aa: data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 28;
	data[4] = mpara.speed >> 24;
	data[5] = mpara.speed >> 16;
	data[6] = mpara.speed >> 8;
	data[7] = mpara.speed;
	data[8] = 1000 >> 24;//空走速度设置1000
	data[9] = 1000 >> 16;
	data[10] = 1000 >> 8;
	data[11] = 1000;
	data[16] = 0;
	data[17] = 0;
	data[18] = mpara.acceleration;
	data[19] = mpara.acceleration;
	data[20] = speedneed >> 24;
	data[21] = speedneed >> 16;
	data[22] = speedneed >> 8;
	data[23] = speedneed;
	unsigned char chv = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	if (val != 25)
	{
		int retrn;
		ZR_InitNetUdp(&retrn, iptemp);
		val = udpl.Send_Data(data, 25);
		return val;
	}
	return (0);
}
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT SendMotorDataNetQ(motionpara mparas,int dir)
{
	int val = 0, num = 0, datanum = 0;
	unsigned char data[128], rdata[10];
	unsigned char motororEn = mparas.motorEn;//马达使能;
	unsigned char motororstop = mparas.motorstop;//马达停止;
	unsigned char motiontype = mparas.motiontype;//是否出激光;
	unsigned char laserSpace = mparas.laserSpace;//激光触发间距;
	int tradez = mparas.z * 2;
	for (int i = 0; i < 24; i++)
	{
		data[i] = 0;
	}
	SetMotorParasNetQ(mparas);
	Sleep(5);
aa:	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 12;
	data[4] = tradez >> 24;
	data[5] = tradez >> 16;
	data[6] = tradez >> 8;
	data[7] = tradez;
	data[8] = 0;
	data[9] = 0;
	data[10] = 0;
	data[11] = 0;
	data[12] = 0;
	data[13] = 0;
	data[14] = 0;
	data[15] = 0;
	data[16] = 0;
	data[17] = 0;
	int motororder = 0;
	data[18] = laserSpace;//激光触发间隔;
	data[19] = (motororEn << 6) + (motiontype << 5) + (motororder << 4) + (motororder << 3) + (0 << 2) + (0 << 1) + dir;//,占1位是否出激光-占2位马达顺序-占3位马达方向,
	data[20] = 0;
	data[21] = 0;
	data[22] = 0;
	data[23] = (mparas.motorrst[2] << 5) + (mparas.motorrst[1] << 4) + (mparas.motorrst[0] << 3)
		+ (mparas.linerazero[2] << 2) + (mparas.linerazero[1] << 1) + (mparas.linerazero[0]);
	unsigned char chv = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	if (val != 25)
	{
		return val;
	}
	return (val);
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT SendMotorDataNetS(motionpara mparas)
{
	int val = 0, num = 0, datanum = 0;
	unsigned char data[128], rdata[10];
	int ss = mparas.s * (mparas.weekpul / mparas.railspace);
	int s = abs(ss - starts);
	unsigned char dirs;
	for (int i = 0; i < 24; i++)
	{
		data[i] = 0;
	}
	if (starts < s) dirs = 1;
	else dirs = 0;
aa:	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 13;
	data[4] = s >> 24;
	data[5] = s >> 16;
	data[6] = s >> 8;
	data[7] = s;
	data[8] = mparas.speed >> 8;
	data[9] = mparas.speed;
	data[10] = mparas.acceleration >> 8;
	data[11] = mparas.acceleration;
	data[12] = mparas.speed >> 8;
	data[13] = mparas.speed;
	data[14] = mparas.acceleration >> 8;
	data[15] = mparas.acceleration;
	data[16] = 0;
	data[17] = 0;
	data[18] = 0;
	data[19] = dirs;
	data[20] = 0;
	data[21] = 0;
	data[22] = 0;
	data[23] = 0;
	unsigned char chv = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	//	while (pointerror != 62464)
	//	{
	val = udpl.Send_Data(data, 25);
	//		Sleep(20);
	//		val = udpl.Receive_data(errorpoint, 2);
	//		pointerror = (errorpoint[0] << 8) + errorpoint[1];
	//	}
	pointerror = 0;
	if (val != 25)
	{
		return val;
	}
	//Sleep(5);
	//val =udpl.Receive_data(rdata, 2);
	starts = s;
	return (val);
}
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_get_counter(short axis, int* cmd)
{
	int data[10];
	GetProtNet(data);
	if (axis == 0)
	{
		*cmd = data[0];
	}
	else if (axis == 1)
	{
		*cmd = data[1];
	}
	else if (axis == 2)
	{
		*cmd = data[2];
	}
	return 0;
}
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_get_encoder(short axis, double* postion)//, int* adj)
{
	double ABC = 10000000.0000, CDE = 100000000.0000, EFG = 149946368.0000, XYZ = 16777216.0000, HIJ = 149946368.0000;
	GetProtNet(data);
	Xposition = data[5];
	Yposition = data[6];
	Zposition = data[7];
	if (axis == 0)
	{
		if ((Xposition >= ABC) && (Xposition <= XYZ))
		{
			Xposition = Xposition - XYZ;
		}
		if (Xposition <= -850000000)
		{
			Xposition = Xtemp;
		}
		Xtemp = Xposition;
		*postion = Xposition;
	}
	else if (axis == 1)
	{
		if ((Yposition >= CDE) && (Yposition <= EFG))
		{
			Yposition = Yposition - EFG;
		}
		if (Yposition <= -850000000)
		{
			Yposition = Ytemp;
		}
		Ytemp = Yposition;
		*postion = Yposition;
	}
	else if (axis == 2)
	{ 
		if ((Zposition >= 20000000.0000) && (Zposition <= HIJ))
		{
			Zposition = Zposition - HIJ;
		}
		if (Zposition <= -850000000)
		{
			Zposition = Ztemp;
		}
		Ztemp = Zposition;
		*postion = Zposition;
	}
	return 0;
}


extern "C" __declspec(dllexport) DWORD PASCAL EXPORT GetIntArray(int* portsta, int length) 
{
	int val = 0, datanum = 0;
	unsigned char data0[128], rdata[128], nouse[128], data[128];
	val = udpl.Receive_data(nouse, 40);
	if (val == 10040) {
		unsigned char tmp[2048];
		int tmpRet = udpl.Receive_data(tmp, sizeof(tmp));
		if (tmpRet == 10040) {
			unsigned char big[65507];
			udpl.Receive_data(big, sizeof(big));
		}
	}
	for (int i = 0; i < 24; i++)
	{
		data[i] = 0;
	}
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 15;
	unsigned char chv = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	if (val != 25)
	{
		int retrn;
		ZR_InitNetUdp(&retrn, iptemp);
		val = udpl.Send_Data(data, 25);
		return val;
	}
	Sleep(1);
	val = udpl.Receive_data(rdata, 40);
	if (val != 40)
	{
		return val;
	}
	for (int i = 0; i < 40; i += 4)
	{
		if ((i == 32) || (i == 28) || (i == 24))
			portsta[i / 4] = (rdata[(i + 3)] << 20) + (rdata[(i + 2)] << 16) + (rdata[(i + 1)] << 8) + rdata[(i)];
		else if (i == 36)
			portsta[i / 4] = (rdata[(i + 3)] << 24) + (rdata[(i + 2)] << 16) + (rdata[(i + 1)] << 8) + rdata[(i)];
		else portsta[i / 4] = (rdata[(i + 2)] << 16) + (rdata[(i + 1)] << 8) + rdata[(i)];
	}
	return 0;
}

//void log_with_time(const CString& message) {
//	// 获取当前时间
//	CTime time = CTime::GetCurrentTime();
//
//	// 第一步：转换时间部分为ANSI字符串
//	CString timeStr = time.Format(_T("%Y-%m-%d %H:%M:%S"));
//	CT2A convertedTime(timeStr.GetString());
//	const char* timeAnsi = convertedTime.m_psz;
//
//	// 第二步：转换消息部分为ANSI字符串
//	CT2A convertedMsg(message.GetString());
//	const char* msgAnsi = convertedMsg.m_psz;
//
//	auto now = std::chrono::system_clock::now();
//	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
//		now.time_since_epoch()) % 1000;
//
//	// 第三步：格式化完整日志行
//	CStringA logEntryA;
//	logEntryA.Format("[%s.%03d] %s\r\n",
//		timeAnsi,
//		ms.count(),
//		msgAnsi);
//
//	// 第四步：创建按日期命名的日志文件
//	CString logFileName;
//	logFileName.Format(_T("Log_%s.txt"), time.Format(_T("%Y%m%d")));
//
//
//	std::ofstream logFile(logFileName, std::ios::app);
//	if (logFile.is_open()) {
//		logFile << logEntryA;
//		logFile.flush();
//	}
//	else {
//		OutputDebugString(_T("日志文件打开失败\r\n"));
//	}
//}


extern "C" __declspec(dllexport)DWORD PASCAL EXPORT GetProtNet(int* portsta)
{
	/*if ((count == 1) || (startjoy == 1))*/
	if (count == 1)
	{
		int val = 0, datanum = 0;
		unsigned char data[128], rdata[100];
		for (int i = 0; i < 24; i++)
		{
			data[i] = 0;
		}
		data[0] = 0;
		data[1] = 0;
		data[2] = 0;
		data[3] = 15;
		data[20] = 0;
		data[21] = 0;
		data[22] = 0;
		data[23] = 0;
		unsigned char chv = 0;
		for (int i = 0; i < 24; i++) chv += data[i];
		data[24] = chv;
		val = udpl.Send_Data(data, 25);
		if (val != 25)
		{
			int retrn;
			ZR_InitNetUdp(&retrn, iptemp);
			val = udpl.Send_Data(data, 25);
			return val;
		}
		Sleep(10);
		val = udpl.Receive_data(rdata, 40);
		if (val != 40)
		{
			return val;
		}
//		int error = 1;
//		while (error != 0)
//		{
//			data[0] = 0;
//			data[1] = 0;
//			data[2] = 0;
//			data[3] = 15;
//			data[20] = 0;
//			data[21] = 0;
//			data[22] = 0;
//			data[23] = 0;
//			unsigned char chv = 0;
//			for (int i = 0; i < 24; i++) chv += data[i];
//			data[24] = chv;
//			val = udpl.Send_Data(data, 25);
//			if (val != 25)
//			{
//				return val;
//			}
//			Sleep(10);
//			//Lock();
//			val = udpl.Receive_data(rdata, 40);
//			//Unlock();
//			if (val != 40)
//			{
//				return val;
//			}
//			error = (rdata[35] << 16) + (rdata[34] << 8) + rdata[33];
//		}
		for (int i = 0; i < 40; i += 4)
		{
			if ((i == 32) || (i == 28) || (i == 24))
				portsta[i / 4] = (rdata[(i + 3)] << 20) + (rdata[(i + 2)] << 16) + (rdata[(i + 1)] << 8) + rdata[(i)];
			else if (i == 36)
				portsta[i / 4] = (rdata[(i + 3)] << 24) + (rdata[(i + 2)] << 16) + (rdata[(i + 1)] << 8) + rdata[(i)];
			else portsta[i / 4] = (rdata[(i + 2)] << 16) + (rdata[(i + 1)] << 8) + rdata[(i)];
		}
		for (int i = 0; i < 10; i++)
		{
			temp[i] = portsta[i];
		}
	}
	else
	{
		for (int i = 0; i < 10; i++)
		{
			portsta[i] = temp[i];
		}
	}
	return 1;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT SetProtNet(int portsta)
{
	count = 0;
	int val = 0, datanum = 0;
	unsigned char data[128], rdata[10];
	for (int i = 0; i < 24; i++)
	{
		data[i] = 0;
	}
aa:	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 10;
	data[4] = portsta >> 24;
	data[5] = portsta >> 16;
	data[6] = portsta >> 8;;

	data[7] = portsta;;
	data[20] = 0;
	data[21] = 0;
	data[22] = 0;
	data[23] = 0;
	unsigned char chv = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	if (val != 25)
	{
		return val;
	}
	Sleep(10);
	count = 1;
	//val =udpl.Receive_data(rdata, 2);
	return val;
}
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT MotorStop(unsigned char* data)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	unsigned char val;
aa:	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 40;//马达停止命令
	for (int i = 8; i < 24; i++) data[i] = 0;
	unsigned char chv = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	if (val != 25)
	{
		return val;
	}
	//Sleep(5);
	//udpl.Receive_data(rdata, 2);
	return val;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT MotornoStop()
{

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	unsigned char data[50], val;
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 40;//马达停止命令
	data[7] = (0 << 5) + (0 << 4) + (0 << 3) + (0 << 2) + (1 << 1) + 0;
	for (int i = 7; i < 24; i++) data[i] = 0;
	data[20] = 0;
	data[21] = 0;
	data[22] = 0;
	data[23] = 0;
	unsigned char chv = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	if (val != 25)
	{
		return val;
	}
	Sleep(10);
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 40;//马达停止命令
	data[7] = (0 << 5) + (0 << 4) + (0 << 3) + (0 << 2) + (0 << 1) + 0;
	for (int i = 7; i < 24; i++) data[i] = 0;
	data[20] = 0;
	data[21] = 0;
	data[22] = 0;
	data[23] = 0;
	chv = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	if (val != 25)
	{
		return val;
	}
	return val;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_qk_stop(short axis)
{
	unsigned char data[32] = {0}, data2[32] = {0};
	data[7] = (0 << 5) + (0 << 4) + (0 << 3) + (0 << 2) + (1 << 1) + 0;
	if (axis == 0)
	{
		MotorStop(data);
	}
	if (axis == 1)
	{
		MotorStopY(data);
	}
	if (axis == 2)
	{
		MotorStopQ(data);
	}
	//Sleep(10);
	//data2[7] = (0 << 5) + (0 << 4) + (0 << 3) + (0 << 2) + (0 << 1) + 0;
	//MotorStop(data2);
	return 0;
}
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT MotorStopQ(unsigned char* data)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	unsigned char val;
aa:	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 56;//马达停止命令
	for (int i = 8; i < 24; i++) data[i] = 0;
	unsigned char chv = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	if (val != 25)
	{
		return val;
	}
	//Sleep(5);
	//udpl.Receive_data(rdata, 2);
	return val;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT MotorStopY(unsigned char* data)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	unsigned char val;
aa:	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 23;//马达停止命令
	for (int i = 8; i < 24; i++) data[i] = 0;
	unsigned char chv = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	if (val != 25)
	{
		return val;
	}
	//Sleep(5);
	//udpl.Receive_data(rdata, 2);
	return val;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_axis_stop(short card, short emg_enable, short emg_logic)
{

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	unsigned char data[50], val, datanum = 0;
aa:	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 40;//马达停止命令
	if (emg_enable == 1)
	{
		data[7] = (1 << 5) + (0 << 4) + (1 << 3) + (0 << 2) + (1 << 1) + 0;
	}
	else if (emg_enable == 0)
	{
		data[7] = (0 << 5) + (0 << 4) + (0 << 3) + (0 << 2) + (0 << 1) + 0;
	}
	for (int i = 8; i < 24; i++) data[i] = 0;
	data[20] = 0;
	data[21] = 0;
	data[22] = 0;
	data[23] = 0;
	unsigned char chv = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	if (val != 25)
	{
		return val;
	}
	return 0;
}


extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_axis_state(short axis)
{
//	lockread = 0;
//	count = 1;
	//unsigned char data[128], rdata[40];
	//int val = 0;
	//for (int i = 0; i < 24; i++)
	//{
	//	data[i] = 0;
	//}
	//data[3] = 15;
	//data[24] = 15;
	//val = udpl.Send_Data(data, 25);
	//if (val != 25)
	//{
	//	return 0;
	//}
	//Sleep(20);
	//val = udpl.Receive_data(rdata, 40);
	//if (val != 40)
	//{
	//	return 0;
	//}
	//if (axis == 2)
	//{
	//	state = (rdata[39] >> 2) & 1;
	//}
	//else
	//{
	//	state = (rdata[39] >> 3) & 1;
	//}
	if (axis == 2)
	{
		GetProtNet(data);
		state = (data[9] >> 27) & 1;
	}
	else if(axis == 1)
	{	
		GetProtNet(data);
		state = (data[9] >> 26) & 1;
	}
	else
	{
		GetProtNet(data);
		state = (data[9] >> 28) & 1;
	}
	//if (axis == 0)
	//{
	//	GetProtNet(data);
	//	state = (data[9] >> 27) & 1;
	//	tempstatey = state;
	//	tempstatez = (data[9] >> 28) & 1;
	//}
	//if (axis == 1) 
	//{
	//	state = tempstatey;
	//}
	//if (axis == 2) 
	//{ 
	//	state = tempstatez;
	//}
//	lockread = 1;
	return state;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_get_axis_di(short card, short io, int* io_level)
{
	//	GetProtNet(data);
	//	for (int i = 0; i < 16; i++)
	//	{
	//		Input[i] = (data[9] >> i) & 1;
	//	}
	*io_level = 1;//Input[io];
	return 0;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_set_axis_do(short card_no, short io_number, short io_level)
{
	int sd;
	int temp;
	if (io_level == 1)
	{
		sd = (io_level << (io_number));
		DOstate |= sd;
	}
	else if (io_level == 0)
	{
		io_level = 1;
		temp = (io_level << (io_number));
		DOstate &= ~temp;
	}
	SetProtNet(DOstate);
	return 0;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_get_axis_do(short card_no, short io_number, int* io_level)
{
	*io_level = (DOstate >> io_number) & 1;
	return 0;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_get_axis_io(short axis, int* axis_status)
{
	//	int data[10];
	if (lockread != 0)
	{
		GetProtNet(data);
	}
	//	unsigned char Input[65];
	for (int i = 0; i < 26; i++)
	{
		Input[i] = (data[9] >> i) & 1;//16开始X+ X-
	}
	for (int i = 16; i < 24; i++)
	{
		Input[i] = (Input[i] + 1) & 1;//限位取反,1激活
	}
	if ((data[5] <= -850000000) || (data[6] <= -850000000) || (data[7] <= -850000000))
	{
		for (int i = 0; i < 3; i++)
		{
			Input[i] = 0;
		}
	}
	Input[26] = (data[9] >> 28) & 1;
	if (axis == 0)
	{
		*axis_status = (Input[3] << 6) + (Input[16] << 0) + (Input[17] << 1) + (Input[0] << 2);
	}
	else if (axis == 1)
	{
		*axis_status = (Input[18] << 0) + (Input[19] << 1) + (Input[1] << 2);
	}
	else if (axis == 2)
	{
		*axis_status = (Input[22] << 0) + (Input[23] << 1) + (Input[2] << 2);
	}
	return 0;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_set_encoder(short axis, double postion)
{
	count = 0;
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	unsigned char data[128];
	int val;
	unsigned char chv = 0;
	double tradex = 0, tradey = 0, tradez = 0;
	if (axis == 0)
	{
		tradex = postion;
		tradex *= 0.0005;
	}
	else if (axis == 1)
	{
		tradey = postion;
		tradey *= 0.0005;
	}
	else if (axis == 2)
	{
		tradez = postion;
		tradez *= 0.0005;
	}
	int tradexf = abs(tradex - int(tradex)) * 1000, tradeyf = abs(tradey - int(tradey)) * 1000, tradezf = abs(tradez - int(tradez)) * 1000;
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 31;
	data[4] = int(tradex) >> 8;
	data[5] = int(tradex);
	data[6] = tradexf >> 8;
	data[7] = tradexf;
	data[8] = int(tradey) >> 8;
	data[9] = int(tradey);
	data[10] = tradeyf >> 8;
	data[11] = tradeyf;
	data[12] = int(tradez) >> 8;
	data[13] = int(tradez);
	data[14] = tradezf >> 8;
	data[15] = tradezf;
	data[16] = unsigned char(axis + 1);;
	data[17] = 0;
	data[18] = 0;
	data[19] = 0;
	data[20] = 0;
	data[21] = 0;
	data[22] = 0;
	data[23] = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	Sleep(10);
	count = 1;
	return 0;

}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_set_counter(short axis, int cmd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	unsigned char data[128];
	int val;
	unsigned char chv = 0;
	int tradex = 0, tradey = 0, tradez = 0;
	if (axis == 0)
	{
		tradex = cmd;
	}
	else if (axis == 1)
	{
		tradey = cmd;
	}
	else if (axis == 2)
	{
		tradez = cmd;
	}
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 33;
	data[4] = tradex >> 8;
	data[5] = tradex;
	data[6] = 0;
	data[7] = 0;
	data[8] = tradey >> 8;
	data[9] = tradey;
	data[10] = 0;
	data[11] = 0;
	data[12] = tradez >> 8;
	data[13] = tradez;
	data[14] = 0;
	data[15] = 0;
	data[16] = unsigned char(axis + 1);;
	data[17] = 0;
	data[18] = 0;
	data[19] = 0;
	data[20] = 0;
	data[21] = 0;
	data[22] = 0;
	data[23] = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	return 0;

}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_axis_limit(short axis, short el_mode)
{
	return 0;
}
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_set_Change_Axis(short axis, short pulse_mode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	unsigned char data[128];
	int val;
	unsigned char chv = 0;
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 1;
	data[4] = axis;
	data[5] = pulse_mode;
	data[6] = 0;
	data[7] = 0;
	data[8] = 0;
	data[9] = 0;
	data[10] = 0;
	data[11] = 0;
	data[12] = 0;
	data[13] = 0;
	data[14] = 0;
	data[15] = 0;
	data[16] = 0;
	data[17] = 0;
	data[18] = 0;
	data[19] = 0;
	data[20] = 0;
	data[21] = 0;
	data[22] = 0;
	data[23] = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	Sleep(10);
	val = udpl.Send_Data(data, 25);
	return 0;
}
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_axis_enable(short axis, short on_off)
{
	switch (axis)
	{
	case 0:
		ZR_set_axis_do(0, 0, on_off);
		break;
	case 1:
		ZR_set_axis_do(0, 1, on_off);
		break;
	case 2:
		ZR_set_axis_do(0, 2, on_off);
		break;
	}
	return 0;
}
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_axis_alarm(short axis, short alm_logic, short alm_mode)
{
	return 0;
}
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_slow_stop(short axis, double t_dec)
{
	unsigned char data[32] = { 0 }, data2[32] = { 0 };
	int vl = t_dec * 200000;
	if (vl >= 60000) { vl = 60000; }
	if (vl <= 0) { vl = 0; }
	data[4] = vl >> 16;
	data[5] = vl >> 8;
	data[6] = vl;
	data[7] = (0 << 5) + (0 << 4) + (0 << 3) + (0 << 2) + (1 << 1) + 0;
	if (axis == 0)
	{
		MotorStop(data);
	}
	if (axis == 1)
	{
		MotorStopY(data);
	}
	if (axis == 2)
	{
		MotorStopQ(data);
	}
	//Sleep(10);
	//data2[7] = (0 << 5) + (0 << 4) + (0 << 3) + (0 << 2) + (0 << 1) + 0;
	//MotorStop(data2);
	return 0;
}
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_ruler_direction(short axis, short direction)
{
	if (direction == 0)
		direction_c = 0;
	else if (direction == 1)
		direction_c = 1;
	return 0;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_ReturnZero(short axis, double start_vel, double max_vel, double t_acc, bool flag)
{
	motionpara mpa;
	mpa.speed = abs(int(max_vel));
	mpa.acceleration = int(t_acc * 10);
	mpa.laserSpace = 10;
	mpa.motiontype = 1;
	mpa.motorEn = 1;
	mpa.railspace = 10;
	mpa.xrailspace = 10;
	mpa.yrailspace = 10;
	mpa.zrailspace = 10;
	mpa.weekpul = 5000;
	mpa.xweekpul = 5000;
	mpa.yweekpul = 5000;
	mpa.zweekpul = 5000;
	if (flag == 0)
	{
		mpa.linerazero[0] = 0;
		mpa.linerazero[1] = 0;
		mpa.linerazero[2] = 0;
	}
	else if (flag == 1)
	{
		mpa.linerazero[0] = 1;
		mpa.linerazero[1] = 1;
		mpa.linerazero[2] = 1;
	}
	mpa.motorrst[0] = 0;
	mpa.motorrst[1] = 0;
	mpa.motorrst[2] = 0;
	mpa.x = mpa.y = mpa.z = 2000000;
	int dir = 0;
	if (axis == 0)
	{
		Xsign = 1;
		if (max_vel > 0) { dir = 1; }
		else { dir = 0; };
		mpa.speed = abs(int(max_vel));
		SendMotorMove(mpa, dir, 0);
	}
	else if (axis == 1)
	{
		Ysign = 1;
		if (max_vel > 0) { dir = 1; }
		else { dir = 0; };
		mpa.speed = abs(int(max_vel));
		SendMotorDataNetY(mpa, dir);
	}
	else if (axis == 2)
	{
		Zsign = 1;
		if (max_vel > 0) { dir = 1; }
		else { dir = 0; };
		mpa.speed = abs(int(max_vel));
		SendMotorDataNetQ(mpa, dir);
	}
	return 0;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_ReturnZero2(double start_vel0, double start_vel1, double start_vel2, double t_acc, bool flag)
{
	motionpara mpa;
	mpa.speed = abs(int(start_vel0));
	mpa.acceleration = int(t_acc * 10);
	mpa.laserSpace = 10;
	mpa.motiontype = 1;
	mpa.motorEn = 1;
	mpa.railspace = 10;
	mpa.xrailspace = 10;
	mpa.yrailspace = 10;
	mpa.zrailspace = 10;
	mpa.weekpul = 5000;
	mpa.xweekpul = 5000;
	mpa.yweekpul = 5000;
	mpa.zweekpul = 5000;
	if (flag == 0)
	{
		mpa.linerazero[0] = 0;
		mpa.linerazero[1] = 0;
		mpa.linerazero[2] = 0;
	}
	else if (flag == 1)
	{
		mpa.linerazero[0] = 1;
		mpa.linerazero[1] = 1;
		mpa.linerazero[2] = 1;
	}
	mpa.motorrst[0] = 0;
	mpa.motorrst[1] = 0;
	mpa.motorrst[2] = 0;
	Xsign = Ysign = Zsign = 1;
	int dir0 = 0, dir1 = 0, dir2 = 0;
	mpa.x = mpa.y = mpa.z = 2000000;
	if (start_vel0 > 0) {dir0 = 1;}
	else { dir0 = 0; };
	mpa.speed = abs(int(start_vel0));
	SendMotorMove(mpa, dir0, 0);
	if (start_vel1 > 0) {dir1 = 1; }
	else { dir1 = 0; };
	Sleep(10);
	mpa.speed = abs(int(start_vel1));
	SendMotorDataNetY(mpa, dir1);
	Sleep(50);
	if (start_vel2 > 0) { dir2 = 1; }
	else { dir2 = 0; };
	mpa.speed = abs(int(start_vel2));
	SendMotorDataNetQ(mpa,dir2);
	return 0;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_set_ruler(short axis, short ez_logic, short ez_mode)
{
	return 0;
}
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_clear(short card_no)
{
	return 0;
}
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_stick_able(short card_no, int enable)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	unsigned char data[128];
	int val;
	unsigned char chv = 0;
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 100;
	data[4] = enable;
	data[5] = 0;
	data[6] = 0;
	data[7] = 0;
	data[8] = 0;
	data[9] = 0;
	data[10] = 0;
	data[11] = 0;
	data[12] = 0;
	data[13] = 0;
	data[14] = 0;
	data[15] = 0;
	data[16] = 0;
	data[17] = 0;
	data[18] = 0;
	data[19] = 0;
	data[20] = 0;
	data[21] = 0;
	data[22] = 0;
	data[23] = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	return 0;
}
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_pls_iptmode(short axis, short mode, short logic)
{
	return 0;
}
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_get_latch_flag(short card_no, int* flag)
{
	//double xs, ys, zs;
	//*flag = 0;
	//ZR_get_encoder(0, &xs);
	//ZR_get_encoder(1, &ys);
	//ZR_get_encoder(2, &zs);
	//if ((xs >= -1000.0000) && (xs <= 1000.0000)&&(ys >= -1000.0000) && (ys <= 1000.0000)&&(zs >= -1000.0000) && (zs <= 1000.0000))
	//{
	//	*flag |= 1 << 12;
	//	*flag |= 1 << 13;
	//	*flag |= 1 << 14;
	//}
	int data[10];
	*flag = 0;
	GetProtNet(data);
	if(((data[9] >> 29) & 1) == 0)  Xsign = 0;
	if(((data[9] >> 30) & 1) == 0)  Ysign = 0;
	if(((data[9] >> 31) & 1) == 0)  Zsign = 0;
	if((Xsign == 0)&&(Ysign == 0)&& (Zsign == 0))
	{
		*flag |= 1 << 12;
		*flag |= 1 << 13;
		*flag |= 1 << 14;
	} 

	return 0;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_Changeip(BYTE tempip[])
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	unsigned char data[128];
	int val;
	unsigned char chv = 0;
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 32;
	data[4] = tempip[0];
	data[5] = tempip[1];
	data[6] = tempip[2];
	data[7] = tempip[3];
	data[8] = tempip[4];
	data[9] = tempip[5];
	data[10] = tempip[6];
	data[11] = tempip[7];
	data[12] = 0;
	data[13] = 0;
	data[14] = 0;
	data[15] = 0;
	data[16] = 0;
	data[17] = 0;
	data[18] = 0;
	data[19] = 0;
	data[20] = 0;
	data[21] = 0;
	data[22] = 0;
	data[23] = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	return 0;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_AxisCofig(short card_no, short axis, double pulse_per_revolution, double pitch, double encoder_resolution, double encoder_direction)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	int MC = int(pulse_per_revolution);//单位脉冲
	int LJ = int(pitch);//螺距
	int FBL = int(encoder_resolution * 10000);//分辨率*10000
	int DIR = int(encoder_direction);//方向
	unsigned char data[128];
	int val;
	unsigned char chv = 0;
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 9;
	data[4] = 0;
	data[5] = 0;
	data[6] = 0;
	data[7] = axis;
	data[8] = MC >> 24;
	data[9] = MC >> 16;
	data[10] = MC >> 8;
	data[11] = MC;
	data[12] = LJ >> 24;
	data[13] = LJ >> 16;
	data[14] = LJ >> 8;
	data[15] = LJ;
	data[16] = FBL >> 24;
	data[17] = FBL >> 16;
	data[18] = FBL >> 8;
	data[19] = FBL;
	data[20] = 0;
	data[21] = 0;
	data[22] = 0;
	data[23] = DIR;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	return 0;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_emg_stop()
{
	ZR_qk_stop(0);
	return 0;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_GetControllerState(short card_no, int* state)
{
	state = 0;
	return 0;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_set_joy_emg(short card_no, short enable, short logic)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	unsigned char data[128];
	int val;
	unsigned char chv = 0;
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 101;
	data[4] = 0;
	data[5] = 0;
	data[6] = 0;
	data[7] = logic;
	data[8] = 0;
	data[9] = 0;
	data[10] = 0;
	data[11] = 0;
	data[12] = 0;
	data[13] = 0;
	data[14] = 0;
	data[15] = 0;
	data[16] = 0;
	data[17] = 0;
	data[18] = 0;
	data[19] = 0;
	data[20] = 0;
	data[21] = 0;
	data[22] = 0;
	data[23] = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	return 0;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_joybox_set_profile(short card_no, double low_speed[3], double mid_speed[3], double high_speed[3], double acc[3])
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	unsigned char data[128];
	int val;
	unsigned char chv = 0;
	int xl = low_speed[0], xm = mid_speed[0], xh = high_speed[0],
		yl = low_speed[1], ym = mid_speed[1], yh = high_speed[1],
		zl = low_speed[2], zm = mid_speed[2], zh = high_speed[2],
		xa = acc[0], ya = acc[1], za = acc[2];
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 102;
	data[4] = xl >> 24;
	data[5] = xl >> 16;
	data[6] = xl >> 8;
	data[7] = xl;
	data[8] = xm >> 24;
	data[9] = xm >> 16;
	data[10] = xm >> 8;
	data[11] = xm;
	data[12] = xh >> 24;
	data[13] = xh >> 16;
	data[14] = xh >> 8;
	data[15] = xh;
	data[16] = xa >> 24;
	data[17] = xa >> 16;
	data[18] = xa >> 8;
	data[19] = xa;
	data[20] = 0;
	data[21] = 0;
	data[22] = 0;
	data[23] = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	Sleep(20);
	chv = 0;
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 103;
	data[4] = yl >> 24;
	data[5] = yl >> 16;
	data[6] = yl >> 8;
	data[7] = yl;
	data[8] = ym >> 24;
	data[9] = ym >> 16;
	data[10] = ym >> 8;
	data[11] = ym;
	data[12] = yh >> 24;
	data[13] = yh >> 16;
	data[14] = yh >> 8;
	data[15] = yh;
	data[16] = ya >> 24;
	data[17] = ya >> 16;
	data[18] = ya >> 8;
	data[19] = ya;
	data[20] = 0;
	data[21] = 0;
	data[22] = 0;
	data[23] = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	Sleep(20);
	chv = 0;
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 104;
	data[4] = zl >> 24;
	data[5] = zl >> 16;
	data[6] = zl >> 8;
	data[7] = zl;
	data[8] = zm >> 24;
	data[9] = zm >> 16;
	data[10] = zm >> 8;
	data[11] = zm;
	data[12] = zh >> 24;
	data[13] = zh >> 16;
	data[14] = zh >> 8;
	data[15] = zh;
	data[16] = za >> 24;
	data[17] = za >> 16;
	data[18] = za >> 8;
	data[19] = za;
	data[20] = 0;
	data[21] = 0;
	data[22] = 0;
	data[23] = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	return 0;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_joybox_set_allconfig(short card_no, short min_value[3], short mid_value[3], short max_value[3], short reserve_flag[3])
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	unsigned char data[128];
	int val;
	unsigned char chv = 0;
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 105;
	data[4] = min_value[0] >> 8;
	data[5] = min_value[0];
	data[6] = min_value[1] >> 8;
	data[7] = min_value[1];
	data[8] = min_value[2] >> 8;
	data[9] = min_value[2];
	data[10] = mid_value[0] >> 8;
	data[11] = mid_value[0];
	data[12] = mid_value[1] >> 8;
	data[13] = mid_value[1];
	data[14] = mid_value[2] >> 8;
	data[15] = mid_value[2];
	data[16] = max_value[0] >> 8;
	data[17] = max_value[0];
	data[18] = max_value[1] >> 8;
	data[19] = max_value[1];
	data[20] = max_value[2] >> 8;
	data[21] = max_value[2];
	data[22] = 0;
	data[23] = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	return 0;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_joybox_set_deadband(short card_no, double dead_band[3])
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	unsigned char data[128];
	int dead0 = dead_band[0],dead2 = dead_band[2], dead1 = dead_band[1];
	int val;
	unsigned char chv = 0;
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 106;
	data[4] = dead0 >> 24;
	data[5] = dead0 >> 16;
	data[6] = dead0 >> 8;
	data[7] = dead0;
	data[8] = dead1 >> 24;
	data[9] = dead1 >> 16;
	data[10] = dead1 >> 8;
	data[11] = dead1;
	data[12] = dead2 >> 24;
	data[13] = dead2 >> 16;
	data[14] = dead2 >> 8;
	data[15] = dead2;
	data[16] = 0;
	data[17] = 0;
	data[18] = 0;
	data[19] = 0;
	data[20] = 0;
	data[21] = 0;
	data[22] = 0;
	data[23] = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	return 0;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_joybox_get_if_connceted(short card_no, short* connect_flag)
{
	startjoy = 1;
	unsigned char data[128], rdata[40];
	int val;
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 107;
	unsigned char chv = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	if (val != 25)
	{
		return val;
	}
	Sleep(50);
	val = udpl.Receive_data(rdata, 40);
	Sleep(50);
	if (rdata[0] != 107)
	{
		*connect_flag = connectstate;
	}
	else
	{
		connectstate = *connect_flag = rdata[1];
	}
	//if (*connect_flag == 0)
	//{
	//	int i = 0;
	//}
	return 0;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_joybox_get_checkdata(short card_no, short id, short* xdata, short* ydata, short* zdata)
{
	startjoy = 1;
	unsigned char data0[128], rdata[40];
	int val;
	data0[0] = 0;
	data0[1] = 0;
	data0[2] = 0;
	data0[3] = 108;
	unsigned char chv = 0;
	for (int i = 0; i < 24; i++) chv += data0[i];
	data0[24] = chv;
	val = udpl.Send_Data(data0, 25);
	if (val != 25)
	{
		return val;
	}
	Sleep(50);
	val = udpl.Receive_data(rdata, 40);
	Sleep(50);
	if (rdata[0] != 108)
	{
		*xdata = Xnow;
		*ydata = Ynow;
		*zdata = Znow;
	}
	else
	{
		Xnow = *xdata = (rdata[2] << 8) + rdata[1];
		Ynow = *ydata = (rdata[4] << 8) + rdata[3];
		Znow = *zdata = (rdata[6] << 8) + rdata[5];
	}
	return 0;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_joybox_set_joglen(short card_no, short low_distance[3], short mid_distance[3], short high_distance[3])
{
	unsigned char data[128];
	int val;
	unsigned char chv = 0;
	short xl = low_distance[0], xm = mid_distance[0], xh = high_distance[0],
		yl = low_distance[1], ym = mid_distance[1], yh = high_distance[1],
		zl = low_distance[2], zm = mid_distance[2], zh = high_distance[2];
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 112;
	data[4] = xl >> 8;
	data[5] = xl;
	data[6] = xm >> 8;
	data[7] = xm;
	data[8] = xh >> 8;
	data[9] = xh;
	data[10] = yl >> 8;
	data[11] = yl;
	data[12] = ym >> 8;
	data[13] = ym;
	data[14] = yh >> 8;
	data[15] = yh;
	data[16] = zl >> 8;
	data[17] = zl;
	data[18] = zm >> 8;
	data[19] = zm;
	data[20] = zh >> 8;
	data[21] = zh;
	data[22] = 0;
	data[23] = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	return 0;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_joybox_set_softlimit(short card_no, short enable_soft_limit[3], double positive_limit[3], double negative_limit[3])
{
	unsigned char data0[128], rdata[6];
	int val;
	int xpositive = positive_limit[0], ypositive = positive_limit[1], zpositive = positive_limit[2], xnegative = negative_limit[0], ynegative = negative_limit[1], znegative = negative_limit[2];
	short xable = enable_soft_limit[0], yable = enable_soft_limit[1], zable = enable_soft_limit[2];
	data0[0] = 0;
	data0[1] = 0;
	data0[2] = 0;
	data0[3] = 109;
	data0[4] = xable >> 8;
	data0[5] = xable;
	data0[6] = 0;
	data0[7] = 0;
	data0[8] = xpositive >> 24;
	data0[9] = xpositive >> 16;
	data0[10] = xpositive >> 8;
	data0[11] = xpositive;
	data0[12] = xnegative >> 24;
	data0[13] = xnegative >> 16;
	data0[14] = xnegative >> 8;
	data0[15] = xnegative;
	data0[16] = 0;
	data0[17] = 0;
	data0[18] = 0;
	data0[19] = 0;
	data0[20] = 0;
	data0[21] = 0;
	data0[22] = 0;
	data0[23] = 0;
	unsigned char chv = 0;
	for (int i = 0; i < 24; i++) chv += data0[i];
	data0[24] = chv;
	val = udpl.Send_Data(data0, 25);
	if (val != 25)
	{
		return val;
	}
	Sleep(20);
	data0[0] = 0;
	data0[1] = 0;
	data0[2] = 0;
	data0[3] = 110;
	data0[4] = yable >> 8;
	data0[5] = yable;
	data0[6] = 0;
	data0[7] = 0;
	data0[8] = ypositive >> 24;
	data0[9] = ypositive >> 16;
	data0[10] = ypositive >> 8;
	data0[11] = ypositive;
	data0[12] = ynegative >> 24;
	data0[13] = ynegative >> 16;
	data0[14] = ynegative >> 8;
	data0[15] = ynegative;
	data0[16] = 0;
	data0[17] = 0;
	data0[18] = 0;
	data0[19] = 0;
	data0[20] = 0;
	data0[21] = 0;
	data0[22] = 0;
	data0[23] = 0;
	chv = 0;
	for (int i = 0; i < 24; i++) chv += data0[i];
	data0[24] = chv;
	val = udpl.Send_Data(data0, 25);
	if (val != 25)
	{
		return val;
	}
	Sleep(20);
	data0[0] = 0;
	data0[1] = 0;
	data0[2] = 0;
	data0[3] = 111;
	data0[4] = zable >> 8;
	data0[5] = zable;
	data0[6] = 0;
	data0[7] = 0;
	data0[8] = zpositive >> 24;
	data0[9] = zpositive >> 16;
	data0[10] = zpositive >> 8;
	data0[11] = zpositive;
	data0[12] = znegative >> 24;
	data0[13] = znegative >> 16;
	data0[14] = znegative >> 8;
	data0[15] = znegative;
	data0[16] = 0;
	data0[17] = 0;
	data0[18] = 0;
	data0[19] = 0;
	data0[20] = 0;
	data0[21] = 0;
	data0[22] = 0;
	data0[23] = 0;
	chv = 0;
	for (int i = 0; i < 24; i++) chv += data0[i];
	data0[24] = chv;
	val = udpl.Send_Data(data0, 25);
	if (val != 25)
	{
		return val;
	}
	return 0;
}


extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_joybox_enable_check(short card_no)
{
	unsigned char data[128], rdata[2];
	int val;
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 130;
	data[4] = 1;
	for (int i = 5; i < 24; i++) { data[i] = 0; }
	unsigned char chv = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	if (val != 25)
	{
		return val;
	}
	return 0;
}


extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_joybox_disable_check(short card_no)
{
	startjoy = 0;
	unsigned char data[128], rdata[2];
	int val;
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 130;
	data[4] = 0;
	for (int i = 5; i < 24; i++) { data[i] = 0; }
	unsigned char chv = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	if (val != 25)
	{
		return val;
	}
	return 0;
}

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_1234(BOOL sign)
{
	unsigned char data[128], rdata[2];
	int val;
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 60;
	data[4] = 0;
	data[5] = 0;
	data[6] = 0;
	data[7] = sign;
	unsigned char chv = 0;
	for (int i = 0; i < 24; i++) chv += data[i];
	data[24] = chv;
	val = udpl.Send_Data(data, 25);
	if (val != 25)
	{
		return val;
	}
	return 0;
}


extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_CompareLinear(short encoder, short channel, int startPos, int repeatTimes, int interval, short time, short source)
{
	unsigned char data0[128], rdata[2];
	int val;
	data0[0] = 0;
	data0[1] = 0;
	data0[2] = 0;
	data0[3] = 17;
	data0[4] = (char)((encoder >> 8) & 0xFF);
	data0[5] = (char)(encoder & 0xFF);
	data0[6] = (char)((channel >> 8) & 0xFF);
	data0[7] = (char)(channel & 0xFF);
	data0[8] = (char)((startPos >> 24) & 0xFF);
	data0[9] = (char)((startPos >> 16) & 0xFF);
	data0[10] = (char)((startPos >> 8) & 0xFF);
	data0[11] = (char)(startPos & 0xFF);
	data0[12] = (char)((repeatTimes >> 24) & 0xFF);
	data0[13] = (char)((repeatTimes >> 16) & 0xFF);
	data0[14] = (char)((repeatTimes >> 8) & 0xFF);
	data0[15] = (char)(repeatTimes & 0xFF);
	data0[16] = (char)((interval >> 24) & 0xFF);
	data0[17] = (char)((interval >> 16) & 0xFF);
	data0[18] = (char)((interval >> 8) & 0xFF);
	data0[19] = (char)(interval & 0xFF);
	data0[20] = (char)((time >> 8) & 0xFF);
	data0[21] = (char)(time & 0xFF);
	data0[22] = (char)((source >> 8) & 0xFF);
	data0[23] = (char)(source & 0xFF);
	unsigned char chv = 0;
	for (int i = 0; i < 24; i++) chv += data0[i];
	data0[24] = chv;
	val = udpl.Send_Data(data0, 25);
	if (val != 25)
	{
		return val;
	}
	return 0;
}



extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_ClearCaptureStatus(short encoder)
{
	unsigned char data0[128], rdata[2];
	int val;

	for (int i = 0; i < 24; i++)
	{
		data0[i] = 0;
	}

	data0[3] = 36;

	data0[6] = (char)((encoder >> 8) & 0xFF);
	data0[7] = (char)(encoder & 0xFF);

	unsigned char chv = 0;
	for (int i = 0; i < 24; i++)
	{
		chv += data0[i];
	}
	data0[24] = chv;

	val = udpl.Send_Data(data0, 25);

	if (val != 25)
	{
		return 1;
	}

	return 0;
}

static unsigned char g_xyzCmd[25] = { 0 };   // 38 指令包（XYZ 指令）
static short         g_axisNum = 0;          // 轴号 (encoder)
static short         g_cmpPort = 0;          // 高速比较端口: 0=端口1, 1=端口2 (source)

static int* g_port1Data = nullptr;  // 端口1 数据 (45 指令 pBuf1)
static short         g_port1Count = 0;

static int* g_port2Data = nullptr;  // 端口2 数据 (46 指令 pBuf2)
static short         g_port2Count = 0;

#define ZR_MAX_AXIS 8
static int g_axisStartPos[ZR_MAX_AXIS + 1] = { 0 };

static void ZR_FreeSavedPortData()
{
	if (g_port1Data) { delete[] g_port1Data; g_port1Data = nullptr; }
	if (g_port2Data) { delete[] g_port2Data; g_port2Data = nullptr; }
	g_port1Count = 0;
	g_port2Count = 0;
}


extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_CompareData(
	short encoder, short source, short pulseType, short startLevel, short time,
	int* pBuf1, short count1, int* pBuf2, short count2)
{
	int val;

	unsigned char data1[25];
	for (int i = 0; i < 24; i++) data1[i] = 0;

	data1[3] = 38;
	data1[4] = (char)(startLevel & 0xFF);
	data1[5] = (char)(pulseType & 0xFF);
	data1[6] = (char)(source & 0xFF);
	data1[7] = (char)(encoder & 0xFF);
	data1[10] = (char)((time >> 8) & 0xFF);
	data1[11] = (char)(time & 0xFF);
	data1[12] = (char)((count1 >> 8) & 0xFF);
	data1[13] = (char)(count1 & 0xFF);
	data1[14] = (char)((count2 >> 8) & 0xFF);
	data1[15] = (char)(count2 & 0xFF);

	unsigned char chv1 = 0;
	for (int i = 0; i < 24; i++) chv1 += data1[i];
	data1[24] = chv1;

	// ---- 保存参数供 04 本地使用 ----
	memcpy(g_xyzCmd, data1, 25);
	g_axisNum = encoder;
	g_cmpPort = (source == 1) ? 1 : 0;

	for (int ax = 1; ax <= ZR_MAX_AXIS; ax++)
	{
		double posD = 0.0;
		if (ZR_get_encoder((short)(ax - 1), &posD) == 0)
		{
			g_axisStartPos[ax] = (int)posD;
		}
	}
	ZR_FreeSavedPortData();
	if (count1 > 0 && pBuf1 != nullptr) {
		g_port1Data = new int[count1];
		memcpy(g_port1Data, pBuf1, count1 * sizeof(int));
		g_port1Count = count1;
	}
	if (count2 > 0 && pBuf2 != nullptr) {
		g_port2Data = new int[count2];
		memcpy(g_port2Data, pBuf2, count2 * sizeof(int));
		g_port2Count = count2;
	}

	// ---- 下发网口：38 ----
	val = udpl.Send_Data(data1, 25);
	if (val != 25) return 1;
	Sleep(2);

	// ---- 下发网口：45 分包 ----
	if (count1 > 0 && pBuf1 != nullptr)
	{
		int totalRawBytes = count1 * 4;
		int rawOffset = 0;

		while (rawOffset < totalRawBytes) {
			int remainingRaw = totalRawBytes - rawOffset;
			int rawBytesThisPacket = (remainingRaw > 864) ? 864 : remainingRaw;
			int numBlocks = (rawBytesThisPacket + 35) / 36;
			int packetLen = numBlocks * 40 + 4;

			unsigned char* data2 = new unsigned char[packetLen];
			memset(data2, 0, packetLen);

			for (int b = 0; b < numBlocks; ++b) {
				int offset = b * 40;
				data2[offset + 3] = 45;

				int startByte = b * 36;
				for (int i = 0; i < 36; ++i) {
					int currentRawIndex = startByte + i;
					if (currentRawIndex < rawBytesThisPacket) {
						int globalRawIndex = rawOffset + currentRawIndex;
						int intIndex = globalRawIndex / 4;
						int byteOffset = globalRawIndex % 4;
						data2[offset + 4 + i] =
							(unsigned char)((pBuf1[intIndex] >> (8 * (3 - byteOffset))) & 0xFF);
					}
				}
			}

			unsigned int checksum = 0;
			for (int i = 0; i < packetLen - 4; ++i) checksum += data2[i];

			data2[packetLen - 4] = 0;
			data2[packetLen - 3] = 0;
			data2[packetLen - 2] = 0;
			data2[packetLen - 1] = (unsigned char)(checksum & 0xFF);

			val = udpl.Send_Data(data2, packetLen);
			delete[] data2;
			if (val != packetLen) return 1;

			Sleep(5);
			rawOffset += rawBytesThisPacket;
		}
	}

	// ---- 下发网口：46 分包 ----
	if (count2 > 0 && pBuf2 != nullptr)
	{
		int totalRawBytes = count2 * 4;
		int rawOffset = 0;

		while (rawOffset < totalRawBytes) {
			int remainingRaw = totalRawBytes - rawOffset;
			int rawBytesThisPacket = (remainingRaw > 864) ? 864 : remainingRaw;

			int numBlocks = (rawBytesThisPacket + 35) / 36;
			int packetLen = numBlocks * 40 + 4;

			unsigned char* data3 = new unsigned char[packetLen];
			memset(data3, 0, packetLen);

			for (int b = 0; b < numBlocks; ++b) {
				int offset = b * 40;
				data3[offset + 3] = 46;

				int startByte = b * 36;
				for (int i = 0; i < 36; ++i) {
					int currentRawIndex = startByte + i;
					if (currentRawIndex < rawBytesThisPacket) {
						int globalRawIndex = rawOffset + currentRawIndex;
						int intIndex = globalRawIndex / 4;
						int byteOffset = globalRawIndex % 4;
						data3[offset + 4 + i] =
							(unsigned char)((pBuf2[intIndex] >> (8 * (3 - byteOffset))) & 0xFF);
					}
				}
			}

			unsigned int checksum = 0;
			for (int i = 0; i < packetLen - 4; ++i) checksum += data3[i];

			data3[packetLen - 4] = 0;
			data3[packetLen - 3] = 0;
			data3[packetLen - 2] = 0;
			data3[packetLen - 1] = (unsigned char)(checksum & 0xFF);

			val = udpl.Send_Data(data3, packetLen);
			delete[] data3;
			if (val != packetLen) return 1;

			Sleep(5);
			rawOffset += rawBytesThisPacket;
		}
	}

	return 0;
}


extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_GetCaptureRepeatPos(
	short encoder, int* pValue, short startNum, short count)
{
	if (pValue == nullptr) return 1;
	if (count <= 0)        return 0;

	if (encoder < 1 || encoder > ZR_MAX_AXIS)
	{
		for (int i = 0; i < count; i++) pValue[i] = 0;
		return 0;
	}

	// ---- 1. 取该轴的编码器起始值 ----
	int cur = g_axisStartPos[encoder];

	// ---- 2. 不是比较轴 -> 直接返回该轴编码器起始值 ----
	if (encoder != g_axisNum)
	{
		for (int i = 0; i < count; i++) pValue[i] = cur;
		return 0;
	}

	// ---- 3. 是比较轴 -> 用 38 保存的端口数据，正加负减 ----
	const int* portData = (g_cmpPort == 1) ? g_port2Data : g_port1Data;
	const short portCount = (g_cmpPort == 1) ? g_port2Count : g_port1Count;

	if (portData == nullptr || portCount <= 0)
	{
		for (int i = 0; i < count; i++) pValue[i] = cur;
		return 0;
	}

	int baseIdx = (int)startNum - 1;
	if (baseIdx < 0) baseIdx = 0;

	for (int i = 0; i < count; i++)
	{
		int idx = baseIdx + i;
		if (idx < 0 || idx >= portCount) { pValue[i] = cur; continue; }

		int delta = portData[idx];
		if (delta >= 0) cur += delta;
		else            cur -= (-delta);

		pValue[i] = cur;
	}

	return 0;
}

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_GetCaptureRepeatStatus(short encoder, short* pCount)
{
	unsigned char data0[128], rdata[128];
	int val;

	for (int i = 0; i < 24; i++)
	{
		data0[i] = 0;
	}
	data0[3] = 5;

	data0[6] = (char)((encoder >> 8) & 0xFF);
	data0[7] = (char)(encoder & 0xFF);

	unsigned char chv = 0;
	for (int i = 0; i < 24; i++)
	{
		chv += data0[i];
	}
	data0[24] = chv;

	val = udpl.Send_Data(data0, 25);
	if (val != 25)
	{
		return 1;
	}
	val = udpl.Receive_data(rdata, 4);
	if (val != 4)
	{
		return 1;
	}

	if (pCount != nullptr)
	{
		*pCount = (short)(((rdata[2] & 0xFF) << 8) | (rdata[3] & 0xFF));
	}

	return 0;
}

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_SetCaptureRepeat(short encoder, short count)
{
	unsigned char data0[128], rdata[128];
	int val;

	for (int i = 0; i < 24; i++)
	{
		data0[i] = 0;
	}

	data0[3] = 35;

	data0[6] = (char)((encoder >> 8) & 0xFF);
	data0[7] = (char)(encoder & 0xFF);

	data0[10] = (char)((count >> 8) & 0xFF);
	data0[11] = (char)(count & 0xFF);

	unsigned char chv = 0;
	for (int i = 0; i < 24; i++)
	{
		chv += data0[i];
	}
	data0[24] = chv;

	val = udpl.Send_Data(data0, 25);

	if (val != 25)
	{
		return 1;
	}

	return 0;
}

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_SetCaptureMode(short encoder, short mode)
{
	unsigned char data0[128], rdata[128];
	int val;

	for (int i = 0; i < 24; i++)
	{
		data0[i] = 0;
	}

	data0[3] = 34;

	data0[6] = (char)((encoder >> 8) & 0xFF);
	data0[7] = (char)(encoder & 0xFF);

	data0[10] = (char)((mode >> 8) & 0xFF);
	data0[11] = (char)(mode & 0xFF);

	unsigned char chv = 0;
	for (int i = 0; i < 24; i++)
	{
		chv += data0[i];
	}
	data0[24] = chv;

	val = udpl.Send_Data(data0, 25);

	if (val != 25)
	{
		return 1;
	}

	return 0;
}

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_CompareStop()
{
	unsigned char data0[128], rdata[128];
	int val;

	for (int i = 0; i < 24; i++)
	{
		data0[i] = 0;
	}

	data0[3] = 37;

	unsigned char chv = 0;
	for (int i = 0; i < 24; i++)
	{
		chv += data0[i];
	}
	data0[24] = chv;

	val = udpl.Send_Data(data0, 25);

	if (val != 25)
	{
		return 1;
	}

	return 0;
}

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_CompareStatus(short* pStatus, int* pCount)
{
	unsigned char data0[128], rdata[128],nouse[128];
	int val;
	val = udpl.Receive_data(nouse, 40);
	if (val == 10040) {
		unsigned char tmp[2048];
		int tmpRet = udpl.Receive_data(tmp, sizeof(tmp));
		if (tmpRet == 10040) {
			unsigned char big[65507];
			udpl.Receive_data(big, sizeof(big));
		}
	}
	for (int i = 0; i < 24; i++)
	{
		data0[i] = 0;
	}

	data0[3] = 7;

	unsigned char chv = 0;
	for (int i = 0; i < 24; i++)
	{
		chv += data0[i];
	}
	data0[24] = chv; 

	val = udpl.Send_Data(data0, 25);
	if (val != 25)
	{
		return 1;
	}
	Sleep(1);
	val = udpl.Receive_data(rdata, 8);
	if (val != 8)
	{
		return val;
	}

	if (pStatus != nullptr)
	{
		*pStatus = rdata[3];
	}

	if (pCount != nullptr)
	{
		*pCount = (((rdata[4] & 0xFF) << 8) | (rdata[5] & 0xFF));
		*(pCount+ 1) = (((rdata[6] & 0xFF) << 8) | (rdata[7] & 0xFF));
	}

	return 0;
}

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_ComparePulse(short level, short outputType, short time)
{
	unsigned char data0[128];
	int val;

	for (int i = 0; i < 24; i++)
	{
		data0[i] = 0;
	}

	data0[3] = 39;

	data0[7] = (char)(level & 0xFF);

	data0[11] = (char)(outputType & 0xFF);

	data0[14] = (char)((time >> 8) & 0xFF);
	data0[15] = (char)(time & 0xFF);

	unsigned char chv = 0;
	for (int i = 0; i < 24; i++)
	{
		chv += data0[i];
	}
	data0[24] = chv;

	val = udpl.Send_Data(data0, 25);

	if (val != 25)
	{
		return 1;
	}

	return 0;
}

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_2DCompareMode(short chn, short mode)
{
	unsigned char data0[128];
	int val;

	for (int i = 0; i < 24; i++)
	{
		data0[i] = 0;
	}

	data0[3] = 47;

	data0[7] = (char)(chn & 0xFF);

	data0[11] = (char)(mode & 0xFF);

	unsigned char chv = 0;
	for (int i = 0; i < 24; i++)
	{
		chv += data0[i];
	}
	data0[24] = chv;

	val = udpl.Send_Data(data0, 25);

	if (val != 25)
	{
		return 1;
	}

	return 0;
}

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_2DCompareClear(short chn)
{
	unsigned char data0[128];
	int val;

	for (int i = 0; i < 24; i++)
	{
		data0[i] = 0;
	}

	data0[3] = 41;

	data0[7] = (char)(chn & 0xFF);

	unsigned char chv = 0;
	for (int i = 0; i < 24; i++)
	{
		chv += data0[i];
	}
	data0[24] = chv;

	val = udpl.Send_Data(data0, 25);

	if (val != 25)
	{
		return 1;
	}

	return 0;
}

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_2DCompareStop(short chn)
{
	unsigned char data0[128];
	int val;

	for (int i = 0; i < 24; i++)
	{
		data0[i] = 0;
	}

	data0[3] = 42;

	data0[7] = (char)(chn & 0xFF);

	unsigned char chv = 0;
	for (int i = 0; i < 24; i++)
	{
		chv += data0[i];
	}
	data0[24] = chv;

	val = udpl.Send_Data(data0, 25);

	if (val != 25)
	{
		return 1;
	}

	return 0;
}

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_2DCompareSetPrm(short chn, T2DComparePrm * pPrm)
{
	unsigned char data0[128];
	int val;

	if (pPrm == nullptr)
	{
		return 1;
	}

	for (int i = 0; i < 24; i++)
	{
		data0[i] = 0;
	}

	data0[3] = 43;

	data0[8] = (char)(chn & 0xFF);

	// 第5字节 = 数据1：X比较轴选择 (占1字节，下标4)
	data0[4] = (char)(pPrm->encx & 0xFF);

	// 第6字节 = 数据2：Y比较轴选择 (占1字节，下标5)
	data0[5] = (char)(pPrm->ency & 0xFF);

	// 第7字节 = 数据3：比较源 (占1字节，下标6)
	data0[6] = (char)(pPrm->source & 0xFF);

	// 第8字节 = 数据4：输出方式 (占1字节，下标7)
	data0[7] = (char)(pPrm->outputType & 0xFF);

	// 第10字节 = 数据5：起始电平方式 (占1字节，下标9)
	data0[9] = (char)(pPrm->startLevel & 0xFF);

	// 第11~12字节 = 数据6：输出脉冲宽度 (占2字节，下标10, 11)
	data0[10] = (char)((pPrm->time >> 8) & 0xFF);
	data0[11] = (char)(pPrm->time & 0xFF);

	// 第13~14字节 = 数据7：比较范围最大误差 (占2字节，下标12, 13)
	data0[12] = (char)((pPrm->maxerr >> 8) & 0xFF);
	data0[13] = (char)(pPrm->maxerr & 0xFF);

	// 第15~16字节 = 数据8：最优算法阈值 (占2字节，下标14, 15)
	data0[14] = (char)((pPrm->threshold >> 8) & 0xFF);
	data0[15] = (char)(pPrm->threshold & 0xFF);

	unsigned char chv = 0;
	for (int i = 0; i < 24; i++)
	{
		chv += data0[i];
	}
	data0[24] = chv;

	val = udpl.Send_Data(data0, 25);

	if (val != 25)
	{
		return 1;
	}

	return 0;
}

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_2DCompareStart(short chn)
{
	unsigned char data0[128];
	int val;

	for (int i = 0; i < 24; i++)
	{
		data0[i] = 0;
	}

	data0[3] = 48;

	data0[7] = (char)(chn & 0xFF);

	unsigned char chv = 0;
	for (int i = 0; i < 24; i++)
	{
		chv += data0[i];
	}
	data0[24] = chv;

	val = udpl.Send_Data(data0, 25);

	if (val != 25)
	{
		return 1;
	}

	return 0;
}

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_2DCompareStatus(short chn, short* pStatus, int* pCount, short* pFifo, short* pFifoCount, short* pBufCount)
{
	return 0;
}

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_2DCompareData(short chn, short count, T2DCompareData * pBuf, short fifo)
{
	int val;

	// ==========================================
	// 第一条指令：发送基本配置参数 (固定 25 字节)
	// ==========================================
	unsigned char data1[25];
	for (int i = 0; i < 24; i++) data1[i] = 0;

	data1[3] = 44;
	data1[4] = (char)(chn & 0xFF);
	data1[6] = (char)((count >> 8) & 0xFF);
	data1[7] = (char)(count & 0xFF);
	data1[11] = (char)(fifo & 0xFF);

	unsigned char chv1 = 0;
	for (int i = 0; i < 24; i++) chv1 += data1[i];
	data1[24] = chv1;

	val = udpl.Send_Data(data1, 25);
	if (val != 25) return 1;
	Sleep(2);

	// ==========================================
	// 第二条指令：发送 X/Y 轴交替数据 (49指令 - 分包发送)
	// 排布规则：每块40字节 (4字节49指令头 + 36字节交替数据[X0, Y0, X1, Y1...])
	// 包尾附加：4字节校验码
	// ==========================================
	if (count > 0 && pBuf != nullptr)
	{
		// 每个点包含 X(4字节) 和 Y(4字节)，共 8 字节
		int totalRawBytes = count * 8;
		int rawOffset = 0;

		while (rawOffset < totalRawBytes) {
			int remainingRaw = totalRawBytes - rawOffset;
			int rawBytesThisPacket = (remainingRaw > 864) ? 864 : remainingRaw; // 每包最多24块(24*36=864字节原始数据)

			int numBlocks = (rawBytesThisPacket + 35) / 36;
			int packetLen = numBlocks * 40 + 4; // 最大包长 24*40 + 4 = 964 字节

			unsigned char* data2 = new unsigned char[packetLen];
			memset(data2, 0, packetLen);

			for (int b = 0; b < numBlocks; ++b) {
				int offset = b * 40;
				data2[offset + 3] = 49; // 1~4 字节为指令头 (前3字节为0, 第4字节为49)

				int startByte = b * 36;
				for (int i = 0; i < 36; ++i) {
					int currentRawIndex = startByte + i;
					if (currentRawIndex < rawBytesThisPacket) {
						int globalRawIndex = rawOffset + currentRawIndex;

						int intIndex = globalRawIndex / 4;       // 当前是第几个 4字节 (32位int)
						int pointIndex = intIndex / 2;          // 点索引 (pBuf下标)
						int isY = intIndex % 2;                 // 0: X数据, 1: Y数据
						int byteOffset = globalRawIndex % 4;    // 32位数据内的字节偏移 (0:高位 -> 3:低位)

						int val32 = (isY == 0) ? pBuf[pointIndex].px : pBuf[pointIndex].py;

						// 写入当前 40 字节块中对应位置 (大端模式)
						data2[offset + 4 + i] = (unsigned char)((val32 >> (8 * (3 - byteOffset))) & 0xFF);
					}
				}
			}

			// 计算校验和
			unsigned int checksum = 0;
			for (int i = 0; i < packetLen - 4; ++i) {
				checksum += data2[i];
			}

			data2[packetLen - 4] = 0;
			data2[packetLen - 3] = 0;
			data2[packetLen - 2] = 0;
			data2[packetLen - 1] = (unsigned char)(checksum & 0xFF);

			val = udpl.Send_Data(data2, packetLen);
			delete[] data2;
			if (val != packetLen) return 1;

			Sleep(2);
			rawOffset += rawBytesThisPacket;
		}
	}

	return 0;
}