// NetUdpDll.h : NetUdpDll DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "heads.h"

//motionpara mpara;

class CNetUdpDllApp : public CWinApp
{
public:
	CNetUdpDllApp();

	// 重写
public:
	virtual BOOL InitInstance();
	DECLARE_MESSAGE_MAP()
};
/**************************************************************
*函数名：  InitNetUdp
*入口参数：Addr,网络输入输出地址及端口配置
*出口参数：无
*函数返回：成功为0，否则为标准错误码
*函数功能：初始化网络
*/
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT InitNetUdp(struct NET_UDPIPADD Addr);
//初始化网络
extern "C" __declspec(dllexport)DWORD ZR_InitNetUdp(int* val, LPSTR ip);//修改后初始化网络
extern "C" __declspec(dllexport)void   PASCAL EXPORT  Dupclose();
//关闭网络
extern "C" __declspec(dllexport)DWORD   PASCAL EXPORT  ZR_Dupclose();//修改后关闭网络

/**************************************************************
*函数名： SendMotorDataNet
*入口参数：参见结构体说明
*出口参数：无
*函数返回：0成功；其它值为错误码
*函数功能：XYZ三轴数据
*/
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT SendMotorDataNet(motionpara mparas);
/**************************************************************
*函数名： SendMotorDataNetQ
*入口参数：参见结构体说明
*出口参数：无
*函数返回：0成功;其它值为错误码
*函数功能：Q轴数据及速度数据
*/

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT SendMotorDataNetQ(motionpara mparas,int dir);
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT SendMotorDataNetY(motionpara mparas,int dir);
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT SetMotorParasNetQ(motionpara mparas);
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT SetMotorParasNetY(motionpara mparas);
/**************************************************************
*函数名： SendMotorDataNetS
*入口参数：参见结构体说明
*出口参数：无
*函数返回：0成功;其它值为错误码
*函数功能：S轴数据及速度数据
*/
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT SendMotorDataNetS(motionpara mparas);
/**************************************************************
*函数名：   SetMotorStartVal
*入口参数：x,y,z三轴初始值
*出口参数：无
*函数返回：0成功返回;其它值为错误码
*函数功能：关闭网络
*/

extern "C" __declspec(dllexport)void PASCAL EXPORT SetMotorStartVal(int x, int y, int z);
/**************************************************************
*函数名：  SetMotorStartValQ
*入口参数：q轴初始数据;
*出口参数：无
*函数返回：无返回
*函数功能：Q轴初始化
*/
extern "C" __declspec(dllexport)void PASCAL EXPORT SetMotorStartValQ(int q);
/**************************************************************
*函数名：  SetMotorStartValS
*入口参数：S轴初始数据;
*出口参数：无
*函数返回：无返回
*函数功能：S轴初始化
*/
extern "C" __declspec(dllexport)void PASCAL EXPORT SetMotorStartValS(int q);
/**************************************************************
*函数名：  SetMotorParas
*入口参数：Mdata,马达参数指针；Ldata，其它数据指针;
*出口参数：无
*函数返回：0成功;其它为错误码
*函数功能：向下位机发送马达运行参数
*/
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT SetMotorParasNet(motionpara mparas);
/**************************************************************
*函数名：  SetProtNet
*入口参数：需要设置的端口数据
*出口参数：无
*函数返回：返回：0成功；其它为错误码
*函数功能：设置输出口状态
*/
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT SetProtNet(int portsta);
/**************************************************************
*函数名：  GetProtNet
*入口参数：无
*出口参数：10个32位数据，0.X轴计数值；1.Y轴计数值；2.Z轴计数值；3.Q轴计数值；4.S轴计数值；5.X轴编码器值；6.Y轴编码器值；7.Z轴编码器值；8.保留；9：高31-29保留
*          28：x电机运行状：0-停止；1-正在运行；27：z电机运行状态：保留；25-16：电机前后限位状态；15-0：外部输入端口状态
*函数返回：返回：0成功；其它值错误，查询错误码
*函数功能：获取板卡数据
*/
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT GetProtNet(int* portsta);
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT GetIntArray(int* portsta, int length);
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT MotorStop(unsigned char* data);
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT MotorStopQ(unsigned char* data);
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT MotorStopY(unsigned char* data);
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT MotornoStop();


extern "C" __declspec(dllexport)DWORD PASCAL EXPORT SendMotorMove(motionpara mparas, int dir, int index);

//单轴运动
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_Uniaxialmove(short axis, double start_vel, double max_vel, double t_acc);
//插补运动
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_Interpolation_move(short card_no, short axis1, short axis2, double x_distance, double y_distance, double start_vel, double max_vel, double t_acc, double t_dec);
//相对运动
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_relative_move(short axis, double distance, double start_vel, double max_vel, double t_acc, double t_dec);
//绝对运动
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_Absolute_move(short axis, double distance, double start_vel, double max_vel, double t_acc, double t_dec);
//插补不记原点
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_Interpolate_move(short card_no, short axis1, short axis2, double x_distance, double y_distance, double start_vel, double max_vel, double t_acc, double t_dec);

//轴运动状态,1激活
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_axis_state(short axis);
//轴限位改变  待添加  启不启用限位
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_axis_limit(short axis, short el_mode);
//轴使能 
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_axis_enable(short axis, short on_off);
//轴报警
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_axis_alarm(short axis, short alm_logic, short alm_mode);
//全部急停  emg_enable=1有效
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_axis_stop(short card, short emg_enable, short emg_logic);
//减速停止
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_slow_stop(short axis, double t_dec);
//三轴急停
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_qk_stop(short axis);


//读取单轴的计数器
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_get_counter(short axis, int* cmd);
//设置单轴的计数器  //待添加
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_set_counter(short axis, int cmd);

//读取单轴的编码器
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_get_encoder(short axis, double* postion);//,int *adj);
//设置单轴的编码器 
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_set_encoder(short axis, double postion);

//设置光栅尺的高低电平  待添加
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_set_ruler(short axis, short ez_logic, short ez_mode);

//设置光栅尺的方向
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_ruler_direction(short axis, short direction);

//清除控制卡参数   待添加
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_clear(short card_no);

//轴专用IO (轴限位,轴报警)
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_get_axis_io(short axis, int* axis_status);

//轴通用do 输出
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_set_axis_do(short card_no, short io_number, short io_level);

//轴通用do 输出
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_get_axis_do(short card_no, short io_number, int* io_level);

//轴通用di 输人
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_get_axis_di(short card, short io, int* io_level);

//遥感打开关闭
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_stick_able(short card_no, int enable);


//改变轴方向
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_set_Change_Axis(short axis, short pulse_mode);

//光栅尺零点信号
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_get_latch_flag(short card_no, int* flag);

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_pls_iptmode(short axis, short mode, short logic);

//回零
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_ReturnZero(short axis, double start_vel, double max_vel, double t_acc, bool flag);
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_ReturnZero2(double start_vel0, double start_vel1, double start_vel2, double t_acc, bool flag);

//修改ip
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_Changeip(BYTE tempip[]);

//电机参数
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_AxisCofig(short card_no, short axis, double pulse_per_revolution, double pitch, double encoder_resolution, double encoder_direction);

//摇杆停止
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_emg_stop();

//暂不加
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_GetControllerState(short card_no, int* state);

//遥感急停
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_set_joy_emg(short card_no, short enable, short logic);

//摇杆速度
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_joybox_set_profile(short card_no, double low_speed[3], double mid_speed[3], double high_speed[3], double acc[3]);

//遥感最大小
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_joybox_set_allconfig(short card_no, short min_value[3], short mid_value[3], short max_value[3], short reserve_flag[3]);

//遥感短距离
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_joybox_set_joglen(short card_no, short low_distance[3], short mid_distance[3], short high_distance[3]);

//遥感死区
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_joybox_set_deadband(short card_no, double dead_band[3]);

//正负最大脉冲
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_joybox_set_softlimit(short card_no, short enable_soft_limit[3], double positive_limit[3], double negative_limit[3]);

//遥感连接状态
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT  ZR_joybox_get_if_connceted(short card_no, short* connect_flag);

//获取遥感当前值(校准)
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT  ZR_joybox_get_checkdata(short card_no, short id, short* xdata, short* ydata, short* zdata);

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT  ZR_joybox_disable_check(short card_no);
extern "C" __declspec(dllexport)DWORD PASCAL EXPORT  ZR_joybox_enable_check(short card_no);

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT  ZR_1234(BOOL sign);

void log_with_time(const CString& message);

extern "C" __declspec(dllexport)DWORD PASCAL EXPORT ZR_CompareLinear(short encoder, short channel, int startPos, int repeatTimes, int interval, short time, short source);


struct T2DCompareData
{
	int px;
	int py;
};

struct T2DComparePrm
{
	short encx;
	short ency;
	short source;
	short outputType;
	short startLevel;
	short time;
	short maxerr;
	short threshold;
};

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_ClearCaptureStatus(short encoder);

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_GetCaptureRepeatPos(short encoder, int* pValue, short startNum, short count);

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_GetCaptureRepeatStatus(short encoder, short* pCount);

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_SetCaptureRepeat(short encoder, short count);

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_SetCaptureMode(short encoder, short mode);

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_CompareStop();

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_CompareStatus(short* pStatus, int* pCount);

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_CompareData(short encoder, short source, short pulseType, short startLevel, short time, int* pBuf1, short count1, int* pBuf2, short count2);

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_ComparePulse(short level, short outputType, short time);

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_2DCompareMode(short chn, short mode);

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_2DCompareClear(short chn);

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_2DCompareStop(short chn);

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_2DCompareSetPrm(short chn, T2DComparePrm * pPrm);

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_2DCompareStart(short chn);

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_2DCompareStatus(short chn, short* pStatus, int* pCount, short* pFifo, short* pFifoCount, short* pBufCount);

extern "C" __declspec(dllexport) short PASCAL EXPORT ZR_2DCompareData(short chn, short count, T2DCompareData * pBuf, short fifo);