/****************************************Copyright (c)****************************************************
**                            		skyray-instrument Co.,LTD.
**
**                                 http://www.skyray-instrument.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           da.c
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
#include "da.h"

/*********************************************************************************************************
* Function name:        DelayDa
* Descriptions:         ����DA����ʱ����,�ڲ�����
* input parameters:     ��ʱ������
* output parameters:    ��
* Returned value:       ��
*********************************************************************************************************/
volatile void DelayDa(int32_t ulTime)
{
//	int32_t i=0;

    while (ulTime--)
    {
//		for (i = 0; i < 500; i++);
    }
}

/*********************************************************************************************************
* Function name:        DaIOInit
* Descriptions:         DA��IO��ʼ��
* input parameters:     ��
* output parameters:    ��
* Returned value:       ��
*********************************************************************************************************/
void DaIOInit(void)
{
    GPIO_SetDir(1,(SDI_8831|CS_8831|SCK_8831|LDAC_8831),1);
}

/*********************************************************************************************************
* Function name:        DA8831Output
* Descriptions:         DA������ƺ���
* input parameters:     ��
* output parameters:    ��
* Returned value:       ��
*********************************************************************************************************/
void DA8831Output(uint16 digit)
{
    uint8 i;

    SET_LDAC_8831(1);
    DelayDa(1);
    SET_CS_8831(0);
    DelayDa(1);
    SET_SCK_8831(1);
    DelayDa(1);
    for(i=0; i<16; i++)
    {
        SET_SCK_8831(0);
        DelayDa(1);
        if((digit&0x8000)==0x8000)
        {
            SET_SDI_8831(1);
        }
        else
        {
            SET_SDI_8831(0);
        }
        DelayDa(1);
        SET_SCK_8831(1);
        DelayDa(1);
        digit=digit<<1;
    }
    SET_CS_8831(1);
    DelayDa(1);
    SET_LDAC_8831(0);
    DelayDa(5);
    SET_LDAC_8831(1);
    DelayDa(5);
}

/*********************************************************************************************************
* Function name:        AnalogOutput
* Descriptions:         ģ���ѹ�������
* input parameters:     analogVoltage: ģ���ѹֵ mV
* output parameters:    ��
* Returned value:       ��
*********************************************************************************************************/
void AnalogOutput(fp32 analogVoltage)
{
    fp32 voltage;

    if(analogVoltage>2500.0)
    {
        analogVoltage = 2500.0;
    }
    if(analogVoltage<-2500.0)
    {
        analogVoltage = -2500.0;
    }
    voltage = analogVoltage + 2500.0;
    DA8831Output((uint16)(voltage/5000.0*65535));
}
