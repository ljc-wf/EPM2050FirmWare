/****************************************Copyright (c)****************************************************
**                            		skyray-instrument Co.,LTD.
**
**                                 http://www.skyray-instrument.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           ad.c
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
#include "ad.h"

fp32 GainRes[8]= {RES1,RES2,RES3,RES4,RES5,RES6,RES7,RES8};  // //�������浵����

/*********************************************************************************************************
* Function name:        DelayAD
* Descriptions:         ����AD����ʱ����,�ڲ�����
* input parameters:     ��ʱ������
* output parameters:    ��
* Returned value:       ��
*********************************************************************************************************/
volatile void DelayAD(int32_t ulTime)
{
    while (ulTime--);
}
/*********************************************************************************************************
* Function name:        ADIOInit
* Descriptions:         AD��IO��ʼ��
* input parameters:     ��
* output parameters:    ��
* Returned value:       ��
*********************************************************************************************************/
void ADIOInit(void)
{
    GPIO_SetDir(1,(SCK_7606|RST_7606),1);
    GPIO_SetDir(3,(CS_7606|CONVST_7606),1);
    GPIO_SetDir(1,(SDO_7606|CONV_BUSY),0);
}

/*********************************************************************************************************
* Function Name:        ADRst
* Description:          AD��λ
* Input:                ��
* Output:               ��
* Return:               ��
*********************************************************************************************************/
void ADRst(void)
{
    SET_RST_7606(0);	  //AD��λ
    SET_RST_7606(1);
    DelayAD(10);
    SET_RST_7606(0);
    DelayAD(50);
}

/*********************************************************************************************************
* Function Name:        GetADValue
* Description:          AD����
* Input:                ��
* Output:               ��
* Return:               ��
*********************************************************************************************************/
uint16 GetADValue(void)
{
    uint16 i,Outwait =500;
    uint16 value = 0;
    uint32 GPOValue = 0;

    SET_CS_7606(1);
    SET_SCK_7606(1);

    SET_CONVST_7606(1);
    SET_CONVST_7606(0);
    DelayAD(30);		   //2us����
    SET_CONVST_7606(1);
    DelayAD(50);		   //3us����

    while(GPIO_ReadValue(1) & CONV_BUSY)		//�ȴ�ת�����
    {
        Outwait --;			  //��ֹ���޵ȴ�
        if(0 ==Outwait)
            break;
    }
    SET_CS_7606(0);

    for(i=0; i<16; i++)
    {
        DelayAD(1);
        SET_SCK_7606(0);
        DelayAD(1);
        GPOValue = (GPIO_ReadValue(1)&SDO_7606);
        if(GPOValue)
        {
            value |=(0x8000>>i);
        }
        SET_SCK_7606(1);

    }

    return value;
}

/*********************************************************************************************************
* Function Name:        GetADVoltage
* Description:          AD����һ����ĵ�ѹֵ
* Input:                ��
* Output:               ��
* Return:               ��
*********************************************************************************************************/
fp32 GetADVoltage(uint8 point)
{
    uint8 i;
    fp32 arrVoltage[32];
    fp32 voltage =0.0;

    if(point > 32)
    {
        point = 32;
    }

    for(i=0; i<point; i++)
    {
        //arrVoltage[i] = ((int16)GetADValue())*30.5176/GainRes[currentGain];		   // 10,000*100/32,768 = 30.517578125 mv/lsb
        arrVoltage[i] = (int16)GetADValue();									  //ֱ�Ӽ�¼ADת��ֵ
    }
    voltage = AverageSelectFp(arrVoltage,point);
    return voltage;
}

/*********************************************************************************************************
* Function Name:        JudgeCurrent
* Description:          �ж�����ʱ�򳬹�320uA,������������*100����֮����*10��ͬʱusRegInputBuf[98]=100����10
* Input:                *usRegHoldingBuf
* Output:               ��
* Return:               ��
*********************************************************************************************************/
void JudgeCurrent(int16 *buffer,uint32 length)
{
    uint16 i;
    //����������320uA,�����*10��usRegInputBuf[98]=10
    if(V_for_I_max*0.3052/GainRes[currentGain] >320.0)
    {
        for(i = 0; i < length ; i++)
        {
            buffer[i+100] =	buffer[i+100]*3.05176/GainRes[currentGain];
        }
        usRegInputBuf[98]=10;
    }
    //������С��320uA,�����*100��usRegInputBuf[98]=100
    else
    {
        for(i = 0; i < length ; i++)
        {
            buffer[i+100] =	buffer[i+100]*30.5176/GainRes[currentGain];
        }
        usRegInputBuf[98]=100;
    }
}