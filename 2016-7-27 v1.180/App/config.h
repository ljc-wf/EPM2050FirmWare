/****************************************Copyright (c)**************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**
**                                 http://www.zyinside.com
**
**--------------File Info-------------------------------------------------------------------------------
** File Name:          config.h
** Last modified Date: 2006-01-06
** Last Version:       v2.0
** Descriptions:       �û�����ͷ�ļ�
**
**------------------------------------------------------------------------------------------------------
** Created By:         ���ܱ�
** Created date:       2005-12-31
** Version:            v1.0
** Descriptions:       ����
**
**------------------------------------------------------------------------------------------------------
** Modified by:        �ʴ�
** Modified date:      2006-01-06
** Version:            v2.0
** Descriptions:       �޸�����S3C2410
**
**------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Descriptions:
**
********************************************************************************************************/
#ifndef _CONFIG_H
#define _CONFIG_H

typedef unsigned char  uint8;                   // �޷���8λ���ͱ���
typedef signed   char  int8;                    // �з���8λ���ͱ���
typedef unsigned short uint16;                  // �޷���16λ���ͱ���
typedef signed   short int16;                   // �з���16λ���ͱ���
typedef unsigned int   uint32;                  // �޷���32λ���ͱ���
typedef signed   int   int32;                   // �з���32λ���ͱ���
typedef float          fp32;                    // �����ȸ�����(32λ����)
typedef double         fp64;                    // ˫���ȸ�����(64λ����)


/********************************/
/*      uC/OS-II���������      */
/********************************/

#include   "INCLUDES.H"


/********************************/
/*        ARM���������         */
/********************************/
// ��һ������Ķ�
#include    <stdio.h>
#include    <ctype.h>
#include    <stdlib.h>


/********************************/
/*      Ӧ�ó�������            */
/********************************/
#include    <stdio.h>
#include    <ctype.h>
#include    <stdlib.h>
#include    <setjmp.h>
#include    <rt_misc.h>

// ���¸�����Ҫ�Ķ�
//#include    "uart.h"

/********************************/
/*       �û������ļ�           */
/********************************/
// ���¸�����Ҫ�Ķ�
/**********************************
** ZLG/MODBUS TRU���ͷ�ļ�������
***********************************/
#define 	UCOSII						// ��uC/OS-II��ʹ�ñ����붨��ú�
#include	"MBMaster.h"					// MODBUS�ӻ�ջͷ�ļ�

void IniUART1(uint32 bps);					// Modbusʹ�õĴ��п�
void  UART1_Exception(void);				// Modbusʹ�ô��пڵ��жϷ�����

void TimersInit(void);						// Modbusʹ�ö�ʱ����ʼ��
void T15_Exception(void);					// Modbus T15��ʱ���жϷ�����
void T35_Exception(void);					// Modbus T35��ʱ���жϷ�����
void T10ms_Exception(void);				// Modbus T10MS��ʱ���жϷ�����



/********************************/
/*       �û���������           */
/********************************/
// ���¸�����Ҫ�Ķ�
extern uint8 OSWriteSingleReg(uint8  ID,
                              uint16 OutAddress,
                              uint16 OutValue);



#endif


/********************************************************************************************************
**                            End Of File
********************************************************************************************************/