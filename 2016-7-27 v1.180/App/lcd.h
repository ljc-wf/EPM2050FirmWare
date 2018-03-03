/*********************************************************************************************************
Copyright (C), 2011-8-3 ,  Skyray Instrment Co.,LTD.
File name:      	LCD.h
Author:             方卫龙       Version:       V2.5 Date:      2011-8-3 11:37:43
Description:

Others:
Function List:
		extern void SetVLCD(uint8 dat);	//对比度
		extern void LcdClr(void);		   //清屏
		extern void ClrDisBuf(void);			 //清除buf
		//显示 各个大小图片
		extern void pic56x56(uint8 x,uint8 y,const uint8 *p);
		extern void pic40x16(uint8 x,uint8 y,const uint8 *p);
		extern void Write240x160(const uint8 *p);

		extern uint8 LcdUpdata(uint8 dat[20][240]);	//更新显示
		extern uint8 LcdReadByte(uint8 x,uint8 y);	   //读写
		extern void LcdWriteByte(uint8 x,uint8 y,uint8 data);
		//显示不同字体的字符串
		extern void DisBuf168(uint8 x,uint8 y,uint8 const p[]);
		extern void DisBuf126(uint8 x,uint8 y,uint8 const p[]);
		extern void DisBuf1212(uint8 x,uint8 y,uint8 const p[]);
		extern void DisBuf1616(uint8 x,uint8 y,uint8 const p[]);
		extern void DisBuf2412(uint8 x,uint8 y,uint8 const p[]);
		extern void DisCList12(uint8 x,uint8 y,uint8 const dat[21]);
		extern void DisCList16(uint8 x,uint8 y,uint8 dat[21]);
		extern void DisEList126(uint8 x,uint8 y,uint8 const dat[21]);
		extern void DisEList2412(uint8 x,uint8 y,uint8 const dat[36]);
		//区域显示操作
		extern void ReverseVideo(uint8 area[]);
		extern void CReverseVideo(uint8 const area[]);
		extern void CleanVideo(uint8 area[]);
		extern void DarwButton(uint8 x,uint8 y);
		extern void DarwScrollBar(uint8 tdat,uint8 ndat);
		extern void DrawRectangular(uint8 area[]);
		extern void DarwHLine(uint8 x1,uint8 x2,uint8 y);
		extern void DarwVLine(uint8 x1,uint8 x2,uint8 y);
		extern void DisEList168(uint8 x,uint8 y,uint8 dat[21]);
		extern void DisStr168(uint8 x,uint8 y,char *dat);
		extern void DisCStr16(uint8 x,uint8 y,uint8 *dat);
		//应用函数 显示时间 日期 数值
		extern void DisTime(uint16 x,uint8 y,TimeDat dat,uint8 Font);
		extern void DisDate(uint16 x,uint8 y,TimeDat dat,uint8 Font);
		extern void DisValue(fp32 fValuel,uint16 x,uint8 y,uint8 font,uint8 length,uint8 decimal,uint8 DisSign);
		extern int8 GetNumLen(fp32 fValuel);
		extern uint32 Num10n(uint8 n);

History:
   1. Date:                2011-8-3 12:52:43
       Author:             何顺杰
	   Modification:    规范了注释
************************************************************************************************************/
#ifndef __LCD_H
#define __LCD_H

/*********************************************************************************************************
**               头文件包含
*********************************************************************************************************/
#include "global.h"

/*********************************************************************************************************
**               C++代码兼容
*********************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif                                                                  /*  __cplusplus                 */

    /*********************************************************************************************************
    **                    240*160 LCD的控制端口
    *********************************************************************************************************/

#define 	DATA_BUS	(0XFFul<<24)			//P1.24--P1.31

#define		CS			(1ul<<19)			//P1.19
#define		CD			(1ul<<20)			//P1.20;
#define		RD			(1ul<<21)			//P1.21;
#define 	WR			(1ul<<22)			//p1.22;
#define 	RST			(1ul<<23)			//p1.23;

#define		DATA_OUT		GPIO_SetDir(1, DATA_BUS,1)
#define		DATA_IN			GPIO_SetDir(1, DATA_BUS,0)
#define		DATA_PIN		GPIO_ReadValue(1, DATA_BUS)
#define		MASK_ON  		FIO_SetMask(1, DATA_BUS,1)
#define		MASK_OFF  		FIO_SetMask(1, DATA_BUS,0)



#define 	CMD_OUT 		GPIO_SetDir(1, (CD|RD|WR|RST|CS),1)		//控制口全为输出
#define  	CD_H 			GPIO_SetValue(1, CD)			//P1.17--CD 置高。
#define  	CD_L 			GPIO_ClearValue(1, CD)			//P1.17--CD 置低

#define  	WR_H 			GPIO_SetValue(1, WR)			//P1.15--WR 置高。
#define  	WR_L 			GPIO_ClearValue(1, WR)			//P1.15--WR 置L。

#define  	CS_H 			GPIO_SetValue(1, CS)			//P1.10--CS 置高。
#define  	CS_L 			GPIO_ClearValue(1, CS)			//P1.10--CS 置L。

#define  	RD_H 			GPIO_SetValue(1, RD)			//P1.16--RD 置高。  
#define  	RD_L 			GPIO_ClearValue(1, RD)			//P1.16--RD 置L。

#define  	RES_H 			GPIO_SetValue(1, RST)			//P1.14--RES 置高。  
#define  	RES_L 			GPIO_ClearValue(1, RST)			//P1.14--RES 置L。
//typedef struct
//{
//	uint8 font;
//	uint8 length;
//	uint8 decimal;
//	uint8 DisSign;
//}
//DISDATSTR;
//
//DISDATSTR  DisDat1;
    extern uint8 volatile DatLength;

    extern uint8  DisBuf[21][240];
    /*********************************************************************************************************
    ** 函数名称: LCDIOIint
    ** 函数功能: lcd引脚初始化
    ** 入口参数: 无
    ** 返 回 值: 无
    ** 说    明:
    *********************************************************************************************************/
    extern void LCDIOIint(void);

    /*********************************************************************************************************
    ** Function name:       lcd_write
    ** Descriptions:        lcd写数据 或命令
    ** input parameters:    cmd 0 数据 1 命令
    						DATA  数据或命令
    ** output parameters:
    ** Returned value:      无
    *********************************************************************************************************/
    extern void LCDWrite(uint8 cmd,uint8 data);

    /*********************************************************************************************************
    ** Function name:       Init1698
    ** Descriptions:        初始lcd
    ** input parameters:
    ** output parameters:
    ** Returned value:      无
    *********************************************************************************************************/
    extern void InitLCD1698(void);

    /*********************************************************************************************************
    * Function name:        LcdClr
    * Descriptions:         LCD清屏
    * input parameters:     无
    * output parameters:    无
    * Returned value:       无
    *********************************************************************************************************/
    extern void LcdClr(void);

    /*********************************************************************************************************
    * Function name:  SetVLCD
    * Descriptions:   对比度设置
    * input parameters: dat
    * output parameters:
    * Returned value:
    *********************************************************************************************************/
    extern void SetVLCD(uint8 dat);

    /*********************************************************************************************************
    * Function name:  ClrDisBuf
    * Descriptions:   显示buf清零
    * input parameters:
    * output parameters:
    * Returned value:
    *********************************************************************************************************/
    extern void ClrDisBuf(void);

    /*********************************************************************************************************
    ** Function name:      words56x56
    ** Descriptions:        56*56显示
    ** input parameters:    x,y 地址
    ** output parameters:   *p字模指针
    ** Returned value:      无
    *********************************************************************************************************/
    extern void pic56x56(uint8 x,uint8 y,const uint8 *p);

    /*********************************************************************************************************
    ** Function name:      pic40x16
    ** Descriptions:        40*16显示
    ** input parameters:    x,y 地址
    ** output parameters:   *p字模指针
    ** Returned value:      无
    *********************************************************************************************************/
    extern void pic40x16(uint8 x,uint8 y,const uint8 *p);

    /*********************************************************************************************************
    * Function name:LcdUpdata
    * Descriptions:   更新显示 将buf的值显示出来
    * input parameters:
    * output parameters:
    * Returned value:
    *********************************************************************************************************/
    extern uint8 LcdUpdata(uint8 dat[20][240]);

    /*********************************************************************************************************
    * Function name:  LcdReadByte
    * Descriptions:   读出1byte数
    * input parameters:
    * output parameters:
    * Returned value:
    *********************************************************************************************************/
    extern uint8 LcdReadByte(uint8 x,uint8 y);

    /*********************************************************************************************************
    * Function name:   LcdWriteByte
    * Descriptions:    写入1byte数
    * input parameters:
    * output parameters:
    * Returned value:
    *********************************************************************************************************/
    extern void LcdWriteByte(uint8 x,uint8 y,uint8 data);

    /*********************************************************************************************************
    ** Function name:      DisBuf168
    ** Descriptions:        16 8显示
    ** input parameters:    x,y 地址
    ** output parameters:   *p字模指针
    ** Returned value:      无
    *********************************************************************************************************/
    extern void DisBuf168(uint8 x,uint8 y,uint8 const p[]);

    /*********************************************************************************************************
    ** Function name:      DisBuf126
    ** Descriptions:        12*6显示
    ** input parameters:    x,y 地址
    ** output parameters:   *p字模指针
    ** Returned value:      无
    *********************************************************************************************************/
    extern void DisBuf126(uint8 x,uint8 y,uint8 const p[]);

    /*********************************************************************************************************
    ** Function name:      	DisBuf1212
    ** Descriptions:        12*12显示
    ** input parameters:    x,y 地址
    ** output parameters:   *p字模指针
    ** Returned value:      无
    *********************************************************************************************************/
    extern void DisBuf1212(uint8 x,uint8 y,uint8 const p[]);

    /*********************************************************************************************************
    ** Function name:     	DisBuf1616
    ** Descriptions:        16 16显示
    ** input parameters:    x,y 地址
    ** output parameters:   *p字模指针
    ** Returned value:      无
    *********************************************************************************************************/
//extern void DisBuf1616(uint8 x,uint8 y,uint8  p[]);

    /*********************************************************************************************************
    ** Function name:      DisBuf2412
    ** Descriptions:        2412显示
    ** input parameters:    x,y 地址
    ** output parameters:   *p字模指针
    ** Returned value:      无
    *********************************************************************************************************/
    extern void DisBuf2412(uint8 x,uint8 y,uint8 const p[]);

    /*********************************************************************************************************
    ** Function name:      	DisCList12
    ** Descriptions:        显示一串字符
    ** input parameters:    x,y 地址
    ** output parameters:   dat字符串
    ** Returned value:      无
    *********************************************************************************************************/
    extern void DisCList12(uint8 x,uint8 y,uint8 const dat[21]);

    /*********************************************************************************************************
    ** Function name:      	DisCList16
    ** Descriptions:        显示一串字符
    ** input parameters:    x,y 地址
    ** output parameters:   dat字符串
    ** Returned value:      无
    *********************************************************************************************************/
    extern void DisCList16(uint8 x,uint8 y,uint8 const dat[21]);

    /*********************************************************************************************************
    ** Function name:       DisEList126
    ** Descriptions:        显示 一串字符窜  12 6
    ** input parameters:    x,y 地址
    ** output parameters:   字模指针
    ** Returned value:      无
    *********************************************************************************************************/
    extern void DisEList126(uint8 x,uint8 y,uint8 const dat[21]);

    /*********************************************************************************************************
    ** Function name:        DisEList2412
    ** Descriptions:        显示 一串字符窜
    ** input parameters:    x,y 地址
    ** output parameters:   字模指针
    ** Returned value:      无
    *********************************************************************************************************/
    extern void DisEList2412(uint8 x,uint8 y,uint8 const dat[36]);

    /*********************************************************************************************************
    ** Function name:       ReverseVideo
    ** Descriptions:        对话框反白
    ** input parameters:    对话框地址
    ** output parameters:
    ** Returned value:      无
    *********************************************************************************************************/
    extern void ReverseVideo(uint8 area[]);

    /*********************************************************************************************************
    ** Function name:    CReverseVideo
    ** Descriptions:        显示菜单标题
    ** input parameters:     汉字字符组
    ** output parameters:
    ** Returned value:      无
    *********************************************************************************************************/
    extern void CReverseVideo(uint8 const area[]);

    /*********************************************************************************************************
    ** Function name:    CleanVideo
    ** Descriptions:        区域清屏
    ** input parameters:    清屏的地址范围数组
    ** output parameters:
    ** Returned value:      无
    *********************************************************************************************************/
    extern void CleanVideo(uint8 area[]);

    /*********************************************************************************************************
    ** Function name:       DarwButton
    ** Descriptions:        显示 纽扣 图标
    ** input parameters:    x,y 地址
    ** output parameters:
    ** Returned value:      无
    *********************************************************************************************************/
    extern void DarwButton(uint8 x,uint8 y);

    /*********************************************************************************************************
    ** Function name:    DarwScrollBar
    ** Descriptions:        显示 滚动条
    ** input parameters:    tdat 当前数	  ndat 总数
    ** output parameters:
    ** Returned value:      无
    *********************************************************************************************************/
    extern void DarwScrollBar(uint16 tdat,uint16 ndat);

    /*********************************************************************************************************
    * Function name:   DrawRectangular
    * Descriptions:    画一对话框边
    * input parameters:
    * output parameters:
    * Returned value:
    *********************************************************************************************************/
    extern void DrawRectangular(uint8 const area[]);

    /*********************************************************************************************************
    * Function name: DarwHLine
    * Descriptions:  画一条横线
    * input parameters: x1,x2,y  y坐标，x起始和终止
    * output parameters:
    * Returned value:
    *********************************************************************************************************/
    extern void DarwHLine(uint8 x1,uint8 x2,uint8 y);

    /*********************************************************************************************************
    * Function name: DarwVLine
    * Descriptions:  画一条竖线
    * input parameters:  x,y11,y2  x坐标，y起始和终止
    * output parameters:
    * Returned value:
    *********************************************************************************************************/
    extern void DarwVLine(uint8 x1,uint8 x2,uint8 y);

    /*********************************************************************************************************
    ** Function name:       DisEList168
    ** Descriptions:        显示 一串字符窜  168
    ** input parameters:    x,y 地址
    ** output parameters:   字模指针
    ** Returned value:      无
    *********************************************************************************************************/
    extern void DisEList168(uint8 x,uint8 y,uint8 dat[21]);

    /*********************************************************************************************************
    ** Function name:       DisStr168
    ** Descriptions:        显示一行12*8文字
    						dat的元素依次为各显示文字在hzk1212k1的偏移量
    ** input parameters:    x,y 地址
    ** output parameters:   字模指针
    ** Returned value:      无
    *********************************************************************************************************/
    extern void DisStr168(uint8 x,uint8 y,char *dat);

    /*********************************************************************************************************
    ** Function name:       DisCStr16
    ** Descriptions:        显示 一串字符窜
    ** input parameters:    x,y 地址
    ** output parameters:   字模指针
    ** Returned value:      无
    *********************************************************************************************************/
    extern void DisCStr16(uint8 x,uint8 y,const uint8 *dat);


    /*********************************************************************************************************
    ** Function name: DisTime
    ** Descriptions:        显示 倒计时
    ** input parameters:    x,y 地址 font显示大小
    ** output parameters:
    ** Returned value:      无
    *********************************************************************************************************/
    extern void DisTime(uint16 x,uint8 y,TimeDat dat,uint8 Font);


    /*********************************************************************************************************
    ** Function name:       DisDate
    ** Descriptions:        显示 日期
    ** input parameters:    x,y 地址 font 样式
    ** output parameters:
    ** Returned value:      无
    *********************************************************************************************************/
    extern void DisDate(uint16 x,uint8 y,TimeDat dat,uint8 Font);

    /*********************************************************************************************************
    * Function name: DisValue
    * Descriptions:  显示结果
    * input parameters:  fp32 fValuel, 值
    					uint16 x,uint8 y, 显示地址
    					uint8 font,		  显示大小档
    					uint8 length,	  显示位数
    					uint8 decimal,	  小数点后的个数
    					uint8 DisSign     有整数部分？
    * output parameters:
    * Returned value:
    *********************************************************************************************************/

    extern void DisValue(fp32 fValuel,uint16 x,uint8 y,uint8 font,uint8 length,uint8 decimal,uint8 DisSign);

    /*********************************************************************************************************
    ** Function name:      write_row_strip
    ** Descriptions:       画一大图	 320*240
    ** input parameters:    x:x地址
    						y:y地址
    ** output parameters:
    ** Returned value:      无
    *********************************************************************************************************/
    extern void Write240x160(const uint8 *p);

    /*********************************************************************************************************
    * Function name:   GetNumLen
    * Descriptions:     取得数据长度
    * input parameters:  fValuel 数据
    * output parameters:
    * Returned value:
    *********************************************************************************************************/
    extern int8 GetNumLen(fp32 fValuel);

    /*********************************************************************************************************
    * Function name:  Num10n
    * Descriptions://10的n次方
    * input parameters:
    * output parameters:
    * Returned value:
    *********************************************************************************************************/
    extern uint32 Num10n(uint8 n);

    /*********************************************************************************************************
    ** Function name:       DisConfigState
    ** Descriptions:       设置状态单个显示
    ** input parameters:    OnOff 开关量状态 0关 1 开  信号状态时无用
    						dat 数据值  开关状态时无用
    						CurrId 当前meuID
    						showplace当在第一页时应该的显示位置
    						type 状态类型 0开关类型 1 数据类型
    ** output parameters:
    ** Returned value:      无
    *********************************************************************************************************/
    extern void DisConfigState1(uint8 OnOff,fp32 dat,uint16 lan_offset,uint8 showplace,uint8 type,uint8 *danwei);
    /*********************************************************************************************************
    ** Function name:       DisConfigState
    ** Descriptions:       设置状态显示
    ** input parameters:
    ** output parameters:
    ** Returned value:      无
    *********************************************************************************************************/

    extern void DisConfigState(void);

    /*********************************************************************************************************
    ** Function name:      Disppb_cn1212
    ** Descriptions:       显示汉字单位 微克每升
    ** input parameters:
    ** output parameters:
    ** Returned value:      无
    *********************************************************************************************************/
    extern void Disppb_cn1212(uint8 x,uint8 y);

    /*********************************************************************************************************
    ** Function name:      Disppb_En1616
    ** Descriptions:       显示英文单位 ug/l
    ** input parameters:
    ** output parameters:
    ** Returned value:      无
    *********************************************************************************************************/
    extern void Disppb_En1616(uint8 x,uint8 y);
    /*********************************************************************************************************
    ** Function name:      Disppb_cn1212
    ** Descriptions:       显示英文单位 ug/l
    ** input parameters:
    ** output parameters:
    ** Returned value:      无
    *********************************************************************************************************/
    extern void Disppb_En1212(uint8 x,uint8 y);
    extern void WriteCode1212(uint8 x,uint8 y,uint8 const dat[40]);
    extern void Disppb_En2412(uint8 x,uint8 y);
    extern void DisUg_M3_En2412(uint8 x,uint8 y);
    extern void ShowCoord(void);
    extern uint8 	RunStyleFlg	,HeatOn,TempCompensateFlag,PumpFlg ,HeatFlg,	ClampMotorFlg,ClampMotor,ClampRadio, RunMotor,  MeasureFlg;
    extern fp32 Humidity,HumiditySample	,CycleOfSampSet,MaxHeatTemp,MinHeatPower,FlowOfSampOut,FlowOfSampOut2,FlowOfSamp_B,FlowOfSamp_B_Set,FlowOfSamp ,
           HighV ,BiaoPianTest,CycleOfSamp,BiaoPian,WenDuT3,WenDuT1,WenDuT2,ShiDu,
           QiYa,LiuLiang_GK,LiuLiang_BK, Pump,Mass,PowerV1,PowerV2,PowerV3,PowerV4,TCQWenDuT4/* 探测器温度 */,CYCShiDu,TestTimes/* 标膜回测测试 */;
    extern uint8 keyEnterFlg;
    extern void WriteMyWord1212(uint8 x,uint8 y,uint8 const dat[40]) ;
    extern int8 OffsetFlg	;
    extern  float MoniTongDao ,ShuzhiTongDao;
    extern  fp32  MoniZhi,	 ShuzhiZhi;

#ifdef __cplusplus
}
#endif                                                                  /*  __cplusplus                 */

#endif

/********************************************************************************************************
  End Of File
********************************************************************************************************/

