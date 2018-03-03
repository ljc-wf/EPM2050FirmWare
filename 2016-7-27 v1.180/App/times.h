/*********************************************************************************************************
  Copyright (C), 2011-8-3  Skyray Instrment Co.,LTD.
  File name:      	times.h
  Author:   		方卫龙    	Version:	V2.5        	Date:	2011-8-3 11:37:43
  Description:    	定时器0,1，2,3初始化，及其中断服务函数

	Others:
	Function List:  // 主要函数列表，每条记录应包括函数名及功能简要说明
			extern volatile void DelayMs(uint32_t timeTick);延时1ms
			extern void Time0Init(void);初始化1ms中断 用于蜂鸣
			extern void Time2Init(uint32_t matchValue);用于DA输出间隔延时
			extern void Time1Init(uint32_t matchValue);用于AD输出间隔延时
			extern void Time3Init(void);初始化中断,用于系统服务 modbus
	History:
	  	1. Date:  	2011-8-3 12:52:43
		   Author: 	何顺杰
		   Modification: 	规范了注释
************************************************************************************************************/
#ifndef __TIMES_H
#define __TIMES_H

/*********************************************************************************************************
**               头文件包含
*********************************************************************************************************/
#include "global.h"

/*********************************************************************************************************
**               C++代码兼容
*********************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
    /*  __cplusplus                 */
    extern uint32 iCountTime2;
// extern uint32 iCountTime2AD;
// extern uint8  iCountTime2Flag;
// extern uint8 iCountTime1Flag;

    extern uint16 	BPtimer;	//蜂鸣器时间位
    extern uint16  	BPcount;	//蜂鸣器时间变化位
    extern uint16  	BPoffTime;	//蜂鸣器开始间
    extern uint16  	BPonTime;	//蜂鸣器关时间
    /*********************************************************************************************************
    * Function name:	    DelayMs
    * Descriptions:	    	延时1ms
    * input parameters:    	timeTick
    * output parameters:   	无
    * Returned value:      	无
    *********************************************************************************************************/
    extern volatile void DelayMs(uint32_t timeTick);

    /*********************************************************************************************************
    * Function name:	    Time0Init
    * Descriptions:	    	初始化1ms中断
    * input parameters:    	无
    * output parameters:   	无
    * Returned value:      	无
    *********************************************************************************************************/
    extern void Time0Init(void);


    /*********************************************************************************************************
    * Function name:	    Time1Init
    * Descriptions:	    	初始化中断,用于从机LED发射
    * input parameters:    	无
    * output parameters:   	无
    * Returned value:      	无
    *********************************************************************************************************/
    extern void Time1Init(uint32_t matchValue);

    /*********************************************************************************************************
    * Function name:	    Time2Init
    * Descriptions:	    	初始化中断,用于从机modbus
    * input parameters:    	无
    * output parameters:   	无
    * Returned value:      	无
    *********************************************************************************************************/
    extern void Time2Init(void);

    /*********************************************************************************************************
    * Function name:	    Time3Init
    * Descriptions:	    	初始化中断,用于系统服务 modbus
    * input parameters:    	无
    * output parameters:   	无
    * Returned value:      	无
    *********************************************************************************************************/
    extern void Time3Init(void);
    extern int8 T15Flag,T35Flag,T10msFlag;
    extern uint8_t T15CountEn;
    extern uint8_t T35CountEn;
    extern uint8_t T10msCountEn;

#ifdef __cplusplus
}
#endif                                                                  /*  __cplusplus                 */

#endif

/********************************************************************************************************
  End Of File
********************************************************************************************************/

