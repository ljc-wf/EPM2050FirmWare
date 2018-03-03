#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_i2c.h"
#include "modbus2.h"


float ModbusAddr=1;//本机Modbus地址
//uint8_t Modbus_Receive_Cout=0;//接收缓冲区计数位
uint8_t Modbus_Receive_CoutFlag=0;//接收计数标志位
//uint8_t Modbus_Receive_Buffer[20]={0};//接收缓冲区
uint8_t Modbus_Send_Bytes=7;
uint8_t Modbus_Sent_Buffer[MODSEDCOUNT]= {'a','b','c','d'}; //发送缓冲区
int8_t 	T35Flag=7;//3.5T标志位
int8_t 	T15Flag=3;//1.5T标志位
uint8_t Receive_End=0;//一帧数据停顿时间
uint8_t ModbusReceive=1;//接收标志
uint8_t ModbusSent=0;//发送标志
uint8_t TFlag=0;//产生0.5T间隔时间标志位
uint8_t Modbus_Receive_CRCcouter=0;
//uint8_t Coil_state[10]={0};
uint8_t InputReg[(MODBUSRC-1)]= {0,2,0,3,0,4,0,5,0,6};
uint8_t HodingReg[MODBUSTC-1]= {0,1,2,3,4,5,6,7};
//uint16_t HodingSet[11]={0xffff,1,1,0xffff,0xffff,0xffff,0xffff,1,2,3,6000};

uint8_t CalFlag=0xff;
float ModbusBoadRate=0;
float ModbusParityStop=2;
uint32_t Baud_rate;
UART_PARITY_Type Parity;
UART_STOPBIT_Type Stopbits;

uint8_t Serial_H=0x00;
uint8_t Serial_M=0x00;
uint8_t Serial_L=0x00;

//字地址 0 - 255
//位地址 0 - 255

/* CRC 高位字节值表 */
const uint8_t auchCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
} ;

/* CRC低位字节值表*/
const uint8_t auchCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
} ;



uint16_t getCRC16(volatile uint8_t *puchMsg, uint16_t usDataLen)
{
    uint8_t uchCRCHi = 0xFF ; //高CRC字节初始化
    uint8_t uchCRCLo = 0xFF ; // 低CRC 字节初始化
    uint32_t uIndex ; // CRC循环中的索引

    while (usDataLen--)
    {
        uIndex = uchCRCHi ^ *puchMsg++ ;
        uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ;
        uchCRCLo = auchCRCLo[uIndex] ;
    }
    return (uchCRCHi << 8 | uchCRCLo) ;
}

/*
void Modbus_Function_1(void)
{
	unsigned char  a, b,c;
	uint16 i;
	unsigned int temp;
	Modbus_Sent_Buffer[1]=0x01;
	if((Modbus_Receive_Buffer[4]==0)&&(Modbus_Receive_Buffer[5]<6))
	{
		if((Modbus_Receive_Buffer[2]==0)&&(Modbus_Receive_Buffer[3]<6)&&((Modbus_Receive_Buffer[3]+Modbus_Receive_Buffer[5])<6 ))
		{
			a=Modbus_Receive_Buffer[3]>>3; //
			b=Modbus_Receive_Buffer[3]&0x07; //
			c=Modbus_Receive_Buffer[5]+b; //
			Modbus_Sent_Buffer[2]=Modbus_Receive_Buffer[5]>>3; //
			if(Modbus_Receive_Buffer[5]&0x07) //
			Modbus_Sent_Buffer[2]+=1;
			Modbus_Send_Bytes=Modbus_Sent_Buffer[2]+3;
			if(c&0x07)
			{
				c=(c>>3)+1;
			}
            else
			{
				c=c>>3;
			}
			for(i=0;i<c;i++)
			{
				temp=Coil_state[a];
				a++;
				temp+=(Coil_state[a]<<8);
				temp>>=b;
				Modbus_Sent_Buffer[i+3]=temp&0xff;
			}
			Modbus_Sent_Buffer[i+3]=temp>>8;
			Modbus_Sent_Buffer[i+3]<<=(8-Modbus_Receive_Buffer[5]&0x07);
			Modbus_Sent_Buffer[i+3]>>=(8-Modbus_Receive_Buffer[5]&0x07);
		}
		else
		{
			Modbus_Sent_Buffer[1]=0x81;
			Modbus_Sent_Buffer[2]=2;
			Modbus_Send_Bytes=3;
		}
	}
	else
	{
		Modbus_Sent_Buffer[1]=0x81;
		Modbus_Sent_Buffer[2]=3;
		Modbus_Send_Bytes=3;
	}
}
*/

void Modbus_Function_4(void)
{
    uint16 i=0;
    uint8_t j=uart3RingBuffer.rx[5];
    uint8_t k=uart3RingBuffer.rx[3];
    if((uart3RingBuffer.rx[4]==0)&&(uart3RingBuffer.rx[5]<MODBUSRC)&&(uart3RingBuffer.rx[5]!=0))
    {
        if((uart3RingBuffer.rx[2]==0)&&(uart3RingBuffer.rx[3]<(MODBUSRC-1))&&((uart3RingBuffer.rx[3]+uart3RingBuffer.rx[5])<MODBUSRC ))
        {
            for(i=0; i<j; i++)
            {
                Modbus_Sent_Buffer[i*2+3]=InputReg[k*2];
                Modbus_Sent_Buffer[i*2+4]=InputReg[k*2+1];
                k++;
            }
            Modbus_Sent_Buffer[1]=0x04;
            Modbus_Sent_Buffer[2]=j*2;
            Modbus_Send_Bytes=j*2+3;
        }
        else
        {
            Modbus_Sent_Buffer[1]=0x84;
            Modbus_Sent_Buffer[2]=0x03;
            Modbus_Send_Bytes=3;
        }
    }
    else
    {
        Modbus_Sent_Buffer[1]=0x84;
        Modbus_Sent_Buffer[2]=0x03;
        Modbus_Send_Bytes=3;
    }
}

void Modbus_Function_3(void)
{
    uint16 i=0;
    uint8_t j=uart3RingBuffer.rx[5];	//寄存器个数低位
    uint8_t k=uart3RingBuffer.rx[3];	 //起始地址
    if((uart3RingBuffer.rx[4]==0)&&(uart3RingBuffer.rx[5]<MODBUSTC)&&(uart3RingBuffer.rx[5]!=0))
    {
        if((uart3RingBuffer.rx[2]==0)&&(uart3RingBuffer.rx[3]<(MODBUSTC-1))&&((uart3RingBuffer.rx[3]+uart3RingBuffer.rx[5])<(MODBUSTC)))
        {
            for(i=0; i<j; i++)
            {
                Modbus_Sent_Buffer[i*2+3]=HodingReg[k*2];
                Modbus_Sent_Buffer[i*2+4]=HodingReg[k*2+1];
                k++;
            }
            Modbus_Sent_Buffer[1]=0x03;
            Modbus_Sent_Buffer[2]=j*2;
            Modbus_Send_Bytes=j*2+3;
        }
        else
        {
            Modbus_Sent_Buffer[1]=0x83;
            Modbus_Sent_Buffer[2]=0x03;
            Modbus_Send_Bytes=3;
        }
    }
    else
    {
        Modbus_Sent_Buffer[1]=0x83;
        Modbus_Sent_Buffer[2]=0x03;
        Modbus_Send_Bytes=3;
    }
}
//	写单个寄存器
void Modbus_Function_6(void)
{
    uint16 i=0;
    uint8_t j=uart3RingBuffer.rx[3];
    CalFlag=j;
    if((uart3RingBuffer.rx[2]==0)&&(uart3RingBuffer.rx[3]<MODBUSTC))
    {
        for(i=0; i<4; i++)
        {
            Modbus_Sent_Buffer[i+2]=uart3RingBuffer.rx[i+2];
        }
        HodingReg[j*2]=uart3RingBuffer.rx[4];
//		C08_WriteByte(MOFFSETADD+j*2,dat);
        HodingReg[j*2+1]=uart3RingBuffer.rx[5];
//		C08_WriteByte(MOFFSETADD+j*2+1,dat);
        /*
        ModbusAddr=HodingReg[1]+(fp32)HodingReg[0]*256;
        Mode=HodingReg[3];
        Filter=HodingReg[5];
        CalUser5g=HodingReg[9]+(fp32)HodingReg[8]*256;
        CalUser10g=HodingReg[11]+(fp32)HodingReg[10]*256;
        CalUser20g=HodingReg[13]+(fp32)HodingReg[12]*256;
        Boad=HodingReg[15];
        */
        Modbus_Sent_Buffer[1]=0x06;
        Modbus_Send_Bytes=6;
    }
    else
    {
        Modbus_Sent_Buffer[1]=0x86;
        Modbus_Sent_Buffer[2]=0x03;
        Modbus_Send_Bytes=3;
    }

}

void Modbus_Function_16(void)
{
    uint8_t i=0,k= 0;
//	uint8_t dat=0;
    uint8_t j=uart3RingBuffer.rx[3];
    if((uart3RingBuffer.rx[2]==0)&&(uart3RingBuffer.rx[3]<((MODBUSTC-1)/2+1)))//&&(uart3RingBuffer.rx[5]==2))  //后面要改
    {
        for(i=0; i<4; i++)
        {
            Modbus_Sent_Buffer[i+2]=uart3RingBuffer.rx[i+2];
        }
//		HodingReg[j*2]=uart3RingBuffer.rx[7];
        for(i = 0,k=0; i<uart3RingBuffer.rx[5]; i++)
        {
            HodingReg[(i+j)*2] = uart3RingBuffer.rx[7+k++];
            HodingReg[(i+j)*2+1] = uart3RingBuffer.rx[7+k++];
        }


//		HodingReg[j*2]=uart3RingBuffer.rx[7];
//		HodingReg[j*2+1]=uart3RingBuffer.rx[8];
//		HodingReg[j*2+2]=uart3RingBuffer.rx[9];
//		HodingReg[j*2+3]=uart3RingBuffer.rx[10];

        Modbus_Sent_Buffer[1]=0x10;
        Modbus_Send_Bytes=6;
    }
    else
    {
        Modbus_Sent_Buffer[1]=0x86;
        Modbus_Sent_Buffer[2]=0x03;
        Modbus_Send_Bytes=3;
    }

}

void Modbus_Function_17(void)
{
    Modbus_Sent_Buffer[1]=0x11;
    Modbus_Sent_Buffer[2]=0x00;
    Modbus_Sent_Buffer[3]=0x20;
    Modbus_Sent_Buffer[4]=0x08;
    Modbus_Sent_Buffer[5]=0x01;//version
    Modbus_Sent_Buffer[6]=0x02;
    Modbus_Sent_Buffer[7]=Serial_H;//Serial
    Modbus_Sent_Buffer[8]=Serial_M;
    Modbus_Sent_Buffer[9]=Serial_L;
    Modbus_Send_Bytes=10;

}
//void Modbus_Function_ff(void)
//{
//	uint8_t cd;
//	cd=uart3RingBuffer.rx[2];
//	if(uart3RingBuffer.rx[2]==1)
//	{
//		ModbusAddr=uart3RingBuffer.rx[3];
//		C08_WriteFp(MODBUSADD,FpConvertFpMod(ModbusAddr));
//	}
//	else if(uart3RingBuffer.rx[2]==2)
//	{
//		Serial_H=uart3RingBuffer.rx[3];
//		Serial_M=uart3RingBuffer.rx[4];
//		Serial_L=uart3RingBuffer.rx[5];
//
//		C08_WriteByte(SERIAL_H,Serial_H);
//		C08_WriteByte(SERIAL_M,Serial_M);
//		C08_WriteByte(SERIAL_L,Serial_L);
//	}
//	Modbus_Sent_Buffer[1]=0xff;
//	Modbus_Sent_Buffer[2]=cd;
//	Modbus_Send_Bytes=3;
//}


void Modbus_Communication(void)
{
    uint16_t crcresult;
    uint8_t temp[2];

    NVIC_DisableIRQ(UART3_IRQn);
    crcresult= getCRC16(uart3RingBuffer.rx,Modbus_Receive_CRCcouter-2);
    temp[1]=(crcresult >> 8) & 0xff;
    temp[0]=crcresult & 0xff;
    if((uart3RingBuffer.rx[Modbus_Receive_CRCcouter-1]==temp[0])&&(uart3RingBuffer.rx[Modbus_Receive_CRCcouter-2]==temp[1]))
    {
        switch(uart3RingBuffer.rx[1])
        {
        case 0x01:
//				Modbus_Function_1();
            break;
        case 0x02:
//				Modbus_Function_2();
            break;
        case 0x03:
            Modbus_Function_3();
            break;
        case 0x04:
            Modbus_Function_4();
            break;
        case 0x05:
//				Modbus_Function_5();
            break;
        case 0x06:
            Modbus_Function_6();
            break;
        case 0x10:
            Modbus_Function_16();
            break;
        case 0x11:
            Modbus_Function_17();
            break;
        case 0xff:
            //	Modbus_Function_ff();
            break;
        default:
        {
            Modbus_Sent_Buffer[1]=Modbus_Sent_Buffer[1]+0x80;
            Modbus_Sent_Buffer[2]=1;
            Modbus_Send_Bytes=3;
        }
        }
    }
    else
    {
        ////show   error!
        Modbus_Sent_Buffer[1]=0xFF;
        Modbus_Sent_Buffer[2]=3;
        Modbus_Send_Bytes=5;

    }
    Modbus_Sent_Buffer[0]=(uint8_t)ModbusAddr;
    crcresult= getCRC16(Modbus_Sent_Buffer,Modbus_Send_Bytes);
    Modbus_Sent_Buffer[Modbus_Send_Bytes]=(crcresult >> 8) & 0xff;
    Modbus_Sent_Buffer[Modbus_Send_Bytes+1]=crcresult & 0xff;
    Modbus_Send_Bytes=Modbus_Send_Bytes+2;

    UART_Send((LPC_UART_TypeDef *)LPC_UART3, Modbus_Sent_Buffer, Modbus_Send_Bytes,BLOCKING);
//	putsUART1(Modbus_Sent_Buffer,Modbus_Send_Bytes);
//	TIM_Cmd(LPC_TIM0,ENABLE);
    NVIC_EnableIRQ(UART3_IRQn);
}

void ModbusProc(void)
{
//	if(TFlag == 1)//处理时间到
//	{
//		TFlag = 0;
    if(ModbusReceive == 1)//串口开始接收数据
    {
        if(T15Flag>0)
        {
            T15Flag--;//1.5T的变量
        }
        if(T35Flag>0)
        {
            T35Flag--;//3.5T的变量
        }
        if(T15Flag == 0)//1.5T的时间到了，记录长度
        {
            //Modbus_Receive_CoutFlag = Modbus_Receive_Cout;
            Modbus_Receive_CoutFlag =uart3RingBuffer.rx_head;
        }
        if(T35Flag == 0)//3.5T的时间到
        {
            //判断是不是发给本机的数据
            //if(Modbus_Receive_Buffer[0]== ModbusAddr)
            //{

            if((uart3RingBuffer.rx[0]== (uint8_t)ModbusAddr)||(uart3RingBuffer.rx[0]==0))
            {
                //判断接收的过程中有没有多余帧消息
                //if((Modbus_Receive_CoutFlag == Modbus_Receive_Cout)&&Modbus_Receive_Cout)
                //{
                if((Modbus_Receive_CoutFlag == uart3RingBuffer.rx_head)&&uart3RingBuffer.rx_head)
                {
                    Receive_End = 7;//一帧数据停顿时间3.5T
                    ModbusSent = 1;//可以发送
                    ModbusReceive = 0;//接收关闭

                }
                else
                {
                    //Modbus_Receive_Cout = 0;
                    __BUF_RESET(uart3RingBuffer.rx_head);
                    TIM_Cmd(LPC_TIM2,DISABLE);	//关定时器2中断
                }
            }
            else
            {
                __BUF_RESET(uart3RingBuffer.rx_head);
                TIM_Cmd(LPC_TIM2,DISABLE);	//关定时器2中断
                //Modbus_Receive_Cout = 0;
            }
        }
    }
    if(Receive_End > 0)//一桢数据结束，延长>T3.5发送
    {
        Receive_End--;
        if(Receive_End == 0)
        {
//				DelayMs(2);
//				Modbus_Receive_CRCcouter=Modbus_Receive_Cout;
            Modbus_Receive_CRCcouter=uart3RingBuffer.rx_head;
//				Modbus_Receive_Cout = 0;

            __BUF_RESET(uart3RingBuffer.rx_head);
            Modbus_Communication();//
            TIM_Cmd(LPC_TIM2,DISABLE);	//关定时器2中断
            ModbusReceive =1;
            ModbusSent = 0;
        }
    }
//	}
}


//主动发送的命令  读输入寄存器
void   ReadInputR()
{



}