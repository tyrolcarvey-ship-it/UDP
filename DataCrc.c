#include "DataCrc.h"

int Crc_Data(unsigned char* m_Data)//回应触摸屏函数
{
     unsigned char Return_data[10];
     int ii,len;
     Return_data[0]=m_Data[0];	//地址高位
     Return_data[1]=m_Data[1];	//地址低位
     Return_data[2]=m_Data[2];
     Return_data[3]=m_Data[3];
     Return_data[4]=m_Data[4];
	 len=( m_Data[3]<<8)+ m_Data[4];
     ii=CRC16(Return_data,len);
     Return_data[5]=ii/256;
     Return_data[6]=ii%256;
	 return ii;
}


