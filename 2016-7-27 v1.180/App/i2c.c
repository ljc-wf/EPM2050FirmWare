/****************************************Copyright (c)****************************************************
**                            		skyray-instrument Co.,LTD.
**
**                                 http://www.skyray-instrument.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           i2c.c
** Last modified Date:  2011-02-24
** Last Version:        V1.0
** Descriptions:
**
**--------------------------------------------------------------------------------------------------------
** Created By:          ������
** Created date:        2011-02-24
** Version:             V1.0
** Descriptions:        First version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:         ������
** Modified date:       2011-02-24
** Version:             V1.X
** Descriptions:
**
*********************************************************************************************************/

#include "i2c.h"


/*Flag Status (RESET = 0, SET = !RESET)*/
__IO FlagStatus complete_M;

/*********************************************************************************************************
* Function name:	    I2C1Init
* Descriptions:	    	I2C1��ʼ��
* input parameters:    	��
* output parameters:   	��
* Returned value:      	��
*********************************************************************************************************/
void I2C1Init(void)
{
    PINSEL_CFG_Type PinCfg;

    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = 0;
    PinCfg.Funcnum = 3;
    PinCfg.Portnum = 0;
    PinCfg.Pinnum = 0;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = 1;
    PINSEL_ConfigPin(&PinCfg);

    // Initialize Slave I2C peripheral
    I2C_Init(I2CDEV, 10000);
    /* preemption = 1, sub-priority = 0 */
    NVIC_SetPriority(I2C1_IRQn, ((PRIO_GROUP_I2C<<3)|PRIO_I2C));
    /* Enable Master I2C operation */
    I2C_Cmd(I2CDEV, ENABLE);
}

/*********************************************************************************************************
* Function name:	    I2C1_IRQHandler
* Descriptions:	    	Main I2C2 interrupt handler sub-routine
* input parameters:    	None
* output parameters:   	None
* Returned value:      	None
*********************************************************************************************************/
void I2C1_IRQHandler(void)
{
    I2C_MasterHandler(I2CDEV);
    if (I2C_MasterTransferComplete(I2CDEV))
    {
        complete_M = SET;
    }
}


/*********************************************************************************************************
* Function name:	    C1025_WriteByte
* Descriptions:	    	��1025ָ����ַ��д��һ���ֽ�
* input parameters:    	ads		��ַ
*						dat		����
* output parameters:   	��
* Returned value:      	��
*********************************************************************************************************/
uint8_t C1025_WriteByte(uint32 ads,uint8_t dat)
{
    uint8_t wrdat[3];
    uint8_t CtrlByte;
    I2C_M_SETUP_Type txsetup;

    complete_M = RESET;
    if (ads>0x0FFFF)
        CtrlByte=0xAA;//�����ⲿ���߸��ġ�
    else
        CtrlByte=0xA2;//�����ⲿ���߸��ġ�
    wrdat[0]=(uint8_t)(ads>>8);
    wrdat[1]=(uint8_t)ads;
    wrdat[2]=dat;
    txsetup.sl_addr7bit = (CtrlByte>>1);
    txsetup.tx_data = wrdat;
    txsetup.tx_length = sizeof(wrdat);
    txsetup.rx_data = NULL;
    txsetup.rx_length = 0;
    txsetup.retransmissions_max = 3;
    Delay(6);	   //����ʡ���������š�
    if (I2C_MasterTransferData(I2CDEV, &txsetup, I2C_TRANSFER_POLLING) == SUCCESS)
    {
        return (TRUE);
    } else {
        return (FALSE);
    }
//	I2C_MasterTransferData(I2CDEV, &txsetup, I2C_TRANSFER_INTERRUPT);
//	while (complete_M == RESET);
//	return TRUE;
}

/*********************************************************************************************************
* Function name:	    C1025_ReadByte
* Descriptions:	    	��1025ָ����ַ��ȡһ���ֽ�
* input parameters:    	ads		��ַ
* output parameters:   	��
* Returned value:      	��ȡ������
*********************************************************************************************************/
uint8_t C1025_ReadByte(uint32 ads)
{
    uint8_t CtrlByte;
    I2C_M_SETUP_Type rxsetup;
    uint8_t a[2];
    uint8_t rddat[1];

    complete_M = RESET;
    if (ads>0x0FFFF)
        CtrlByte=0xAA;
    else
        CtrlByte=0xA2;
    a[0]=(uint8_t)(ads>>8);
    a[1]=(uint8_t)ads;
    rxsetup.sl_addr7bit = (CtrlByte>>1);
    rxsetup.tx_data = a;	// Get address to read at writing address
    rxsetup.tx_length = sizeof(a);
    rxsetup.rx_data = rddat;
    rxsetup.rx_length = sizeof(rddat);
    rxsetup.retransmissions_max = 3;
    Delay(1);
    if (I2C_MasterTransferData(I2CDEV, &rxsetup, I2C_TRANSFER_POLLING) == SUCCESS)
    {

        return (rddat[0]);
    }
    else
    {
        return (FALSE);
    }
//	I2C_MasterTransferData(I2CDEV, &rxsetup, I2C_TRANSFER_INTERRUPT);
//	while (complete_M == RESET);
//	return rddat[0];
}

/*********************************************************************************************************
* Function name:	    WriteData16
* Descriptions:	    	��1025ָ����ַ��д��һ����
* input parameters:    	ads		��ַ
*						dat		����
* output parameters:   	��
* Returned value:      	��
*********************************************************************************************************/
uint16_t WriteData16(uint32 ads,uint16_t dat)
{
    C1025_WriteByte(ads,dat);
    C1025_WriteByte(ads+1,dat>>8);
    return 0;//ReadData16(ads);
}

/*********************************************************************************************************
* Function name:	    C1025_ReadByte
* Descriptions:	    	��1025ָ����ַ��ȡһ����
* input parameters:    	ads		��ַ
* output parameters:   	��
* Returned value:      	��ȡ������
*********************************************************************************************************/
uint16_t ReadData16(uint32 ads)
{
    uint16_t dat;
    dat =C1025_ReadByte(ads);
    dat|=(((uint16_t)C1025_ReadByte(++ads))<<8);
    return dat;
}

/*********************************************************************************************************
* Function name:	    WriteData32
* Descriptions:	    	��1025ָ����ַ��д��һ��˫��
* input parameters:    	ads		��ַ
*						dat		����
* output parameters:   	��
* Returned value:      	��
*********************************************************************************************************/
uint32_t WriteData32(uint32 ads,uint32_t dat)
{
    C1025_WriteByte(ads,dat);
    C1025_WriteByte(ads+1,dat>>8);
    C1025_WriteByte(ads+2,dat>>16);
    C1025_WriteByte(ads+3,dat>>24);
    return 0;//ReadData32(ads);
}

/*********************************************************************************************************
* Function name:	    C1025_ReadByte
* Descriptions:	    	��1025ָ����ַ��ȡһ����
* input parameters:    	ads		��ַ
* output parameters:   	��
* Returned value:      	��ȡ������
*********************************************************************************************************/
uint32_t ReadData32(uint32 ads)
{
    uint32_t dat;
    dat =C1025_ReadByte(ads);
    dat|=(((uint32_t)C1025_ReadByte(++ads))<<8);
    dat|=(((uint32_t)C1025_ReadByte(++ads))<<16);
    dat|=(((uint32_t)C1025_ReadByte(++ads))<<24);

    return dat;
}

/*********************************************************************************************************
* Function name:	    WriteFp32
* Descriptions:	    	��1025ָ����ַ��д��һ��������
* input parameters:    	ads		��ַ
*						dat		����
* output parameters:   	��
* Returned value:      	��
*********************************************************************************************************/
float WriteFp32(uint32 ads,float dat)
{
    uint8_t a[4];

    FloatToChArray(dat,a);
    C1025_WriteByte(ads,a[0]);

    C1025_WriteByte(ads+1,a[1]);
    C1025_WriteByte(ads+2,a[2]);
    C1025_WriteByte(ads+3,a[3]);
    return 0;//ReadFp32(ads);
}

/*********************************************************************************************************
* Function name:	    ReadFp32
* Descriptions:	    	��1025ָ����ַ��ȡһ��������
* input parameters:    	ads		��ַ
* output parameters:   	��
* Returned value:      	��ȡ������
*********************************************************************************************************/
float ReadFp32(uint32 ads)
{
    uint8_t a[4];
    a[0]=C1025_ReadByte(ads);
    a[1]=C1025_ReadByte(++ads);
    a[2]=C1025_ReadByte(++ads);
    a[3]=C1025_ReadByte(++ads);
    return ChArrayToFloat(a);
}
