#pragma once
extern "C" __declspec(dllexport)typedef struct NET_UDPIPADD
{
	CString inaddr;//本机IP地址
	UINT     inProt;//本机端口号
	CString outaddr;//目标机IP地址
	UINT     outProt;//目标机端口号
};

typedef struct BZLPoint
{
	int x;
	int y;
	int z;
	int q;

};
typedef struct motionpara
{
	double x;
	double y;
	double z;
	float q;
	float s;
	int speed;//速度
	int Nullspeed;//空走速度
	int acceleration;//加速度
	int Nullacceleration;//空走加速度
	BOOL motiontype;//1:为出激光;0:不出激光
	int laserSpace;//激光触发间隔
	int xweekpul;//X马达旋转一周的脉冲个数,与马达驱动器设置有关
	int yweekpul;//Y马达旋转一周的脉冲个数,与马达驱动器设置有关
	int zweekpul;//Z马达旋转一周的脉冲个数,与马达驱动器设置有关
	int weekpul;//QS马达旋转一周的脉冲个数,与马达驱动器设置有关
	double xrailspace;//X导轨一周距离
	double yrailspace;//Y导轨一周距离
	double zrailspace;//Z导轨一周距离
	double railspace;//QS导轨一周距离
	BOOL motorEn;//暂时停止
	BOOL motorstop;//全部停止
	BOOL linerazero[3];//编码器过零停止;0:X,1:Y;2:Z;置‘0’时为有效
	BOOL motorrst[3];

};

// CLPC1758DllApp
// 有关此类实现的信息，请参阅 LPC1758Dll.cpp
//
extern "C" __declspec(dllexport)typedef struct PlatformData
{
	float Standardx;//基准坐标X
	float Standardy;//基准坐标Y
	float Standardq;//基准角度
	float Objx;//对象坐标X
	float Objy;//对象坐标Y
	float Objq;//对象角度
	float Radius;//平台半径
	float ParaX1;//平台制造参数
	float ParaX2;//平台制造参数
	float ParaY1;//平台制造参数
};
extern "C" __declspec(dllexport)typedef struct MotorParaData
{
	int XY_MotorLen;//马达单周长度
	int XY_MotorPul;//马达单周脉冲数
	int XY_MotorSeep;//马达速度
	int XY_NULLSeep; //马达空走速度
	int XY_Acceleration;
};
extern "C" __declspec(dllexport)typedef struct LaserData
{
	float PwmFrequency;//单位K 0-100K
	int   PwmDutyratio;//点空比,值 ：0-100
	int   Power;//激光功率
	int   OffLaserTime;//关光延时
	int   OnLaserTime;//开光延时
	int   RedEn;
	BOOL  OutLaser;//激光测量
	int Laser_outs;
};

