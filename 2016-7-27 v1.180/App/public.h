/*********************************************************************************************************
Copyright (C), 2011-8-3 ,  Skyray Instrment Co.,LTD.
File name:      	public.h
Author:             ������       Version:       V2.5 Date:      2011-8-3 11:37:43
Description:    	���õĳ��ú�����

Others:
Function List:
		extern volatile void Delay100uS(uint32 dly);
		extern volatile void DelayuS(uint32 dly);
		extern volatile void Delay(uint32 dly);
		extern void Uint16ToMod(uint8 *arr,uint16 dat);//modbus�ã��ֱ��ֽ�
		extern uint16 ModToUint16(uint8 *arr);//�ֽڱ���
		extern uint8 LongIntegerToBit(uint32 ulSource,uint8 *pTarget);//���ֱ��ֽ�
		extern uint8 IntegerToBitAscii(uint32 dat,uint8 *pArr);//���α�Ϊasciiֵ
		extern void disSeondTime(uint16 dat,uint8 *pArr);//��ʱ��ֿ���������
		extern void FloatToChArray(fp32 Source,uint8 *Target);//������ֽ�
		extern fp32 ChArrayToFloat(uint8 *Source);//�ֽڱ両��
		extern fp32 Int16ArrayToFloat(uint16 *Source);//�ֱ両��
		extern uint32 AverageSelectUint16(uint16 *dat,uint8 length);//������������ȡ�м��ƽ��ֵ
		extern fp32 AverageSelectFp(fp32 *dat,uint8 length);//������������ȡ�м��ƽ��ֵ
		extern fp32 MovingAverage(int16 *dat,uint32 length,uint8 num);//�ƶ�ƽ��
		extern uint16 LzgCStrlen(uint8 const s[]);//���� ���鳤��
		extern uint16 LzgChineselen(uint8 const s[]);
		extern void StrCopyStr (uint8 *arr1,uint8 *arr2,uint8 length);//����length ���ȵ��ַ���
History:
   1. Date:                2011-8-3 12:52:43
       Author:             ��˳��
	   Modification:    �淶��ע��
************************************************************************************************************/
#ifndef __PUBLIC_H
#define __PUBLIC_H

/*********************************************************************************************************
**               ͷ�ļ�����
*********************************************************************************************************/
#include "global.h"

/*********************************************************************************************************
**               C++�������
*********************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif                                                                  /*  __cplusplus                 */

    extern volatile uint16 V_for_I_max;
    /*********************************************************************************************************
    * Function name:        Delay10uS
    * Descriptions:         С��ʱ����
    * input parameters:     dly
    * output parameters:    ��
    * Returned value:       ��
    *********************************************************************************************************/
    extern volatile void Delay100uS(uint32 dly);

    /*********************************************************************************************************
    * Function name:        DelayuS
    * Descriptions:         С��ʱ����
    * input parameters:     dly
    * output parameters:    ��
    * Returned value:       ��
    *********************************************************************************************************/
    extern volatile void DelayuS(uint32 dly);

    /*********************************************************************************************************
    * Function name:        Delay
    * Descriptions:         С��ʱ����
    * input parameters:     dly
    * output parameters:    ��
    * Returned value:       ��
    *********************************************************************************************************/
    extern volatile void Delay(uint32 dly);

    /*********************************************************************************************************
    * Function name:        Uint16ToMod
    * Descriptions:         modbus�ã��ֱ��ֽ�
    * input parameters:     *arr		�ֽ�ָ��
    *						dat			Ҫ�仯����
    * output parameters:    ��
    * Returned value:       ��
    *********************************************************************************************************/
    extern void Uint16ToMod(uint8 *arr,uint16 dat);

    /*********************************************************************************************************
    * Function name:        ModToUint16
    * Descriptions:         modbus�ã��ֽڱ���
    * input parameters:     *arr		�ֽ�ָ��
    * output parameters:    ��
    * Returned value:       ��
    *********************************************************************************************************/
    extern uint16 ModToUint16(uint8 *arr);

    /*********************************************************************************************************
    * Function name:        LongIntegerToBit
    * Descriptions:         ���ֱ��ֽ�
    * input parameters:     ulSource	����
    * output parameters:    *pTarget	�ֽ�����
    * Returned value:       0
    *********************************************************************************************************/
    extern uint8 LongIntegerToBit(uint32 ulSource,uint8 *pTarget);

    /*********************************************************************************************************
    * Function name:  IntegerToBitAscii
    * Descriptions:    ���α�Ϊasciiֵ
    * input parameters: uint16 dat,uint8 *pArr  ����  ��Ӧ��ascii
    * output parameters:     ��
    * Returned value:          ��
    *********************************************************************************************************/
    extern uint8 IntegerToBitAscii(uint32 dat,uint8 *pArr);

    /*********************************************************************************************************
    * Function name:   disSeondTime
    * Descriptions:    ��ʱ��ֿ���������
    * input parameters: uint16 dat,uint8 *pArr  ʱ��  ����
    * output parameters:     ��
    * Returned value:          ��
    *********************************************************************************************************/
    extern void disSeondTime(uint16 dat,uint8 *pArr);

    /*********************************************************************************************************
    * Function name:        FloatToChArray
    * Descriptions:         ������ֽ�
    * input parameters:     Source	������
    * output parameters:    *Target	�ֽ�����
    * Returned value:       ��
    *********************************************************************************************************/
    extern void FloatToChArray(fp32 Source,uint8 *Target);
    /*********************************************************************************************************
    * Function name:        Float64ToChArray
    * Descriptions:         ������ֽ�
    * input parameters:     Source	������
    * output parameters:    *Target	�ֽ�����
    * Returned value:       ��
    *********************************************************************************************************/
    extern void Float64ToChArray(fp64 Source,uint8 *Target);





    /*********************************************************************************************************
    * Function name:        ChArrayToFloat
    * Descriptions:         �ֽڱ両��
    * input parameters:     *Source	�ֽ�����
    * output parameters:    ��
    * Returned value:       ������
    *********************************************************************************************************/
    extern fp32 ChArrayToFloat(uint8 *Source);

    /*********************************************************************************************************
    * Function name:        Int16ArrayToFloat
    * Descriptions:         �ֱ両��
    * input parameters:     *Source	������
    * output parameters:    ��
    * Returned value:       ������
    *********************************************************************************************************/
    extern fp32 Int16ArrayToFloat(uint16 *Source);

    /*********************************************************************************************************
    * Function name:        AverageSelectUint16
    * Descriptions:         ������������ȡ�м��ƽ��ֵ
    * input parameters:     ����ָ�룬���鳤��
    * output parameters:    ��
    * Returned value:       ƽ��ֵ
    *********************************************************************************************************/
    extern uint32 AverageSelectUint16(uint16 *dat,uint8 length);

    /*********************************************************************************************************
    * Function name:        AverageSelectFp
    * Descriptions:         ������������ȡ�м��ƽ��ֵ
    * input parameters:     ����ָ�룬���鳤��
    * output parameters:    ��
    * Returned value:       ƽ��ֵ
    *********************************************************************************************************/
    extern fp32 AverageSelectFp(fp32 *dat,uint8 length);

    /**********************************************************	********
    * Function name:        MovingAverage
    * Descriptions:         �ƶ�ƽ��
    * input parameters:     ����ָ�룬���鳤�ȣ��ƶ���Ŀ
    * output parameters:    ��
    * Returned value:       ���ֵ
    ****************************************************************************/
    extern fp32 MovingAverage(int16 *dat,uint32 length,uint8 num);

    /**********************************************************	********
    * Function name:        LzgCStrlen
    * Descriptions:         ���� ���鳤��
    * input parameters:     ����ָ�룬
    * output parameters:    ��
    * Returned value:       ����
    ****************************************************************************/
    extern uint16 LzgCStrlen(uint8 const s[]);

    /**********************************************************	********
    * Function name:        LzgCStrlen
    * Descriptions:         ���� �����ַ�����
    * input parameters:     ����ָ�룬
    * output parameters:    ��
    * Returned value:       ����
    ****************************************************************************/
    extern uint16 LzgChineselen(uint8 const s[]);

    /*********************************************************************************************************
    * Function name:      StringLen
    * Descriptions:       �����ַ�������
    * input parameters:    s
    * output parameters:
    * Returned value:    �����ַ��ֽ���
    *********************************************************************************************************/
    extern uint16 StringLen(uint8 const s[]);
    /***************************************************************************
    * Function name:        StrCopyStr
    * Descriptions:         ����length ���ȵ��ַ���
    * input parameters:     ����ָ��1������ָ��2 ����
    * output parameters:    ��
    * Returned value:       ��
    ****************************************************************************/
    extern void StrCopyStr (uint8 *arr1,const uint8 *arr2,uint8 length);

#ifdef __cplusplus
}
#endif                                                                  /*  __cplusplus                 */

#endif

/********************************************************************************************************
  End Of File
********************************************************************************************************/