#include"Measure.h"
volatile int32 CaliFlg;
float CalParam_I1[3],CalParam_I2[3];  //У׼��ǿ����
float CalParam_U[3];                  //У׼ֵ
float CalParam_avr_U;                  //У׼Ʒ��ֵֵ
float CalParam_max_U;                  //У׼���ֵֵֵ
float CalParam_min_U;                  //У׼��Сֵֵֵ
float CalParam_AARD;                   //������
float CheParam_I1[10],CheParam_I2[10];  //����������
float CheParam_U[10];                   //�������ֵ
float CheParam_avr_U;                   //�������ƽ��ֵ
float CheParam_AARD;                   //������

#define AREA 6.8358
DAStruct DAStructTable[NUN_DA]=
{
    {
        0,
        2.5/1.002,
    },
    {
        1,
        2.5/1.002,
    },
    {
        2,
        1.22/1.002,
    },
    {
        3,
        2/1.002,
    },

};

ADStruct ADStructTable[NUN_AD]=
{   //ͨ�� ���Ŵ��������ɼ�������ֵҪ��������Ŵ�����
    {
        0,
        1*1.002,
    },
    {
        1,
        2,
    },
    {
        2,
        2,
    },
    {
        3,
        3.35,//4.92,yuan3.43
    },
    {
        4,
        2,
    },
    {
        5,
        2,
    },
    {
        6,
        1*1.002,
    },
    {
        7,
        1,
    },
    {
        8,
        1,
    },
    {
        9,
        1,
    },
    {
        10,
        1,
    },
    {
        11,
        1*1.002,
    },
    {
        12,
        1,
    },
    {
        13,
        1,
    },
    {
        14,
        1,
    },
    {
        15,
        1,
    },


};

static SensorStruct SensorStructTable[NUN_SENSOR]=
{
    {   //���غ���JWSK-6������ʪ�ȴ�����
        CGQ_WENDU1_KL,      //�������¶ȴ�����4~20ma��Ӧ0.8v~4v��Ӧ-40��~120V
        {50,-80},
        1.0,
        0.0,
    },

    {   //����CWS18-06������ʪ�ȴ�����
        CGQ_WENDU1_XY,    //�������¶ȴ�����4~20ma��Ӧ0.8v~4v��Ӧ-20��~60V
        //{31.25,-65},//0.8v~4v��Ӧ-40��~60V
        {25,-40},
        1.0,
        0.0,
    },

    {   //���غ���JWSK-6������ʪ�ȴ�����
        CGQ_CYK_SHIDU_KL,//������ʪ�ȴ�����4~20ma��Ӧ0.8v~4v��Ӧ0%~100%
        {31.25,-25},
        1,
        0.0,
    },
    {   //����CWS18������ʪ�ȴ�����
        CGQ_CYK_SHIDU_XY, //������ʪ�ȴ�����4~20ma��Ӧ0.8v~4v��Ӧ0%~100%
        {31.25,-25},
        1,
        0.0,
    },

    {   //T2
        CGQ_WENDU2,      //���ȹ��¶ȴ�����pt100��1V��5V ��Ӧ -50�浽100��
        {37.5,-87.5},
        1.0,
        0.0,
    },
    {   //T3
        CGQ_WENDU3,     //���Բ��¶ȴ�����pt100��1V��5V ��Ӧ -50�浽100��
        {37.5,-87.5},
        1.0,
        0.0,
    },
    {   //RH2
        CGQ_SHIDU,      //���Բ�ʪ�ȴ�����HM1500 1V~4V ��Ӧ 0%~100% ���������ֲṫʽ
        {39.1,-42.5},
        1,
        0.0,
    },
    {
        CGQ_LIULIANG,
        {4.0,0},
        1,
        0.0,
    },
    {
        CGQ_LIULIANG_HONEYWELL,
        {5.0,-5},
        1,
        0.0,
    },
//	{
//		CGQ_QIYA,
//		{22.957,10.56},//�ֲ��������⣬��ʽ�Ƴ�����-10.56 ������ֵ�������10.56�� Ȼ���ԴҲ��4.84.
//		1,
//		0.0,
//	},
//
    {
        CGQ_QIYA,
        {22.217,10.56},//д���˼��㹫ʽ�������Ǹ���vss���е���
        1,
        0.0,
    },

    {   //T4
        CGQ_TCQ_WENDU,    //̽����������pt100��1V��5V ��Ӧ -50�浽100��
        {37.5,-87.5},
        1,
        0.0,
    }
};



float FindOriginalDat(uint8 SensorName)
{
    int i;
    for(i = 0; i<NUN_SENSOR; i++)
    {
        if(SensorStructTable[i].SensorName == SensorName )
        {
            break;
        }
    }
    return SensorStructTable[i].OriginalDat;
}

void SetCaliDat(uint8 SensorName,float CaliDat)
{
    int i;
    for(i = 0; i<NUN_SENSOR; i++)
    {
        if(SensorStructTable[i].SensorName == SensorName )
        {
            break;
        }
    }
    SensorStructTable[i].CaliDat = 	CaliDat;
}

float LoadK(uint8 SensorName,float k)
{
    int i=0;
    if((SensorName>NUN_SENSOR))
    {
        return -1;
    }
    for(i=0; i<NUN_SENSOR; i++)
    {
        if(SensorName == SensorStructTable[i].SensorName)
        {
            SensorStructTable[i].Modulus[0] = k;
            return 0;
        }
    }
    return -1;
}

float GetADValue(uint8 ADChannel)   //����ͨ������ֵ������Ӧ�Ŵ���
{
    int i;//ADRangesel=2;
    float dat;
    if((ADChannel>NUN_AD))
    {
        return -1;
    }

    for(i=0; i<NUN_AD; i++)
    {
        if(ADChannel == ADStructTable[i].Channel)
        {
            dat = addat[ADChannel]*ADStructTable[i].Gain;
            return dat;
        }
    }
    return -1;
}


float GetMeasureValue(uint8 SensorName,uint8 ADChannel)    //��������ѹֵת��Ϊʵ�ʴ�������������ֵ
{
    int i;//ADRangesel=2;
    float dat;
    if((SensorName>NUN_SENSOR)||(ADChannel>NUN_AD))
    {
        return -1;
    }

    for(i=0; i<NUN_AD; i++) //����ͨ������ֵ������Ӧ�Ŵ���
    {
        if(ADChannel == ADStructTable[i].Channel)
        {
            dat = addat[ADChannel]*ADStructTable[i].Gain;//���ѹֵ
            break;
        }
    }

    for(i=0; i<NUN_SENSOR; i++)
    {
        if(SensorName == SensorStructTable[i].SensorName)
        {
            SensorStructTable[i].OriginalDat = (dat*SensorStructTable[i].Modulus[0])+(SensorStructTable[i].Modulus[1]);
            if(JIAOZHUN_ON)
            {
                dat = SensorStructTable[i].OriginalDat* SensorStructTable[i].CaliDat;
            }
            else
            {
                dat =  SensorStructTable[i].OriginalDat	;
            }
            break;
        }
    }
    return dat;
}


//��ƬУ׼
void Calibration(void)
{
    uint32 slen;
    uint8 err;
    uint8 i;
    float ParOldU;
    area[0] = 0;
    area[1] = 25;
    area[2] = 239;
    area[3] = 159;


    //����Դ��λ
    OSMutexPend(OSUart0UsedMutex,0,&err);
    TTLOutput (TTL_2,1,TTL_BRAY_Ctrl1);	 //��
    TTLOutput (TTL_2,0,TTL_BRAY_Ctrl2);	 //��
    OSMutexPost(OSUart0UsedMutex);

    OSTimeDlyHMSM(0, 0, C14MotMoveTime,0);	//����ʱ��

    OSMutexPend(OSUart0UsedMutex,0,&err);
    TTLOutput (TTL_2,0,TTL_BRAY_Ctrl1);	 //��
    TTLOutput (TTL_2,0,TTL_BRAY_Ctrl2);	 //��
    OSMutexPost(OSUart0UsedMutex);

    switch (CaliFlg)
    {
        //�����ɿ�ѹֽ���
    case 0:
        CleanVideo(area);

        DisCList16(80,30,"��һ�α궨");

        slen = StringLen(NextPrompt[24+NePrOffset]); //
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[24+NePrOffset]);   //�ɿ������ʾ
        LcdUpdata(DisBuf);
        songkaiMoto();
        CleanVideo(area);
        //��ȡ����ֽ �����Ƭ1
        DisCList16(80,30,"��һ�α궨");
        slen = StringLen(NextPrompt[25+NePrOffset]);
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[25+NePrOffset]);   //ȡ����ֽ��ʾ
        prompt(0);	// ��ʾ ȷ��
        LcdUpdata(DisBuf);
        CaliFlg = 1;

        break;
    case 1:
        //��ʾ����200s
        CleanVideo(area);
        DisCList16(80,30,"��һ�α궨");

        slen = StringLen(NextPrompt[30+NePrOffset]);   //   ����ѹ�������	�Ժ�
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[30+NePrOffset]);
        LcdUpdata(DisBuf);
        yajingMoto();

        CleanVideo(area);
        DisCList16(80,30,"��һ�α궨");

        slen = StringLen(NextPrompt[27+NePrOffset]); //   ����200s	�Ժ�
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[27+NePrOffset]);
        LcdUpdata(DisBuf);
        OSMutexPend(OSUart0UsedMutex,0,&err);
        CountCtrl(COUNT_START);	//��������
        OSMutexPost(OSUart0UsedMutex);
        TimeKeyCount = JIAOZHUN_BIAOPIAN_TEST_TIME*50;
        CountStartFlg =1;
        while(TimeKeyCount)
        {
            CleanVideo(area);
            DisCList16(80,30,"��һ�α궨");
            DisDat.length = 5;
            DisDat.decimal = 0;
            DisDat.DisSign = 0;
            DisDat.font = 2;
            DisValue2(CountNow,96,96, &DisDat ,"");
            DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[27+NePrOffset]);
            DisValue2((int)(TimeKeyCount/50),16*2+(DIS_AREA_X-slen*8)/2,70, &DisDat ,"");

            LcdUpdata(DisBuf);
            OSTimeDly(100);//0.5��;
        }
        CleanVideo(area);
        OSMutexPend(OSUart0UsedMutex,0,&err);
        CountRead();
        OSMutexPost(OSUart0UsedMutex);
        CountStartFlg = 0;
        //����ƽ��ֵ
        EeorMear = (CountTotal<50)?(EeorMear|0x01):(EeorMear&0xfe);		   //̽��������

        CalParam_I1[0] =  CountTotal / CountTimes;
//			OSMutexPend(OSUart0UsedMutex,0,&err);
//			CountCtrl(COUNT_END);
//			OSMutexPost(OSUart0UsedMutex);
        //�����ɿ�ѹֽ���
        CleanVideo(area);
        DisCList16(80,30,"��һ�α궨");
        slen = StringLen(NextPrompt[24+NePrOffset]); //
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[24+NePrOffset]);
        LcdUpdata(DisBuf);
        songkaiMoto();
        CleanVideo(area);
        DisCList16(80,30,"��һ�α궨");
        slen = StringLen(NextPrompt[26+NePrOffset]); //  �����2
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[26+NePrOffset]);
        prompt(2);	// ��ʾ ȷ��
        LcdUpdata(DisBuf);
        CaliFlg = 2;
        break;


    case 2:
        //
        CleanVideo(area);
        DisCList16(80,30,"��һ�α궨");
        slen = StringLen(NextPrompt[30+NePrOffset]); //   ����ѹ�������	�Ժ�
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[30+NePrOffset]);
        LcdUpdata(DisBuf);
        yajingMoto();
        CleanVideo(area);
        DisCList16(80,30,"��һ�α궨");
        slen = StringLen(NextPrompt[27+NePrOffset]); //   ����200s
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[27+NePrOffset]);
        LcdUpdata(DisBuf);
        OSMutexPend(OSUart0UsedMutex,0,&err);
        CountCtrl(COUNT_START);	//��������
        OSMutexPost(OSUart0UsedMutex);
        TimeKeyCount = JIAOZHUN_BIAOPIAN_TEST_TIME*50;//20msһ�� 5s
        CountStartFlg =1;

        while(TimeKeyCount)
        {
            CleanVideo(area);
            DisCList16(80,30,"��һ�α궨");
            DisDat.length = 5;
            DisDat.decimal = 0;
            DisDat.DisSign = 0;
            DisDat.font = 2;
            DisValue2(CountNow,96,96, &DisDat ,"");
            DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[27+NePrOffset]);
            DisValue2((int)(TimeKeyCount/50),16*2+(DIS_AREA_X-slen*8)/2,70, &DisDat ,"");

            LcdUpdata(DisBuf);
            OSTimeDly(100);//0.5��;
        }
        CleanVideo(area);
        OSMutexPend(OSUart0UsedMutex,0,&err);
        CountRead();
        OSMutexPost(OSUart0UsedMutex);
        CountStartFlg = 0;
        //����ƽ��ֵ
        EeorMear = (CountTotal<50)?(EeorMear|0x01):(EeorMear&0xfe);		   //̽��������

        CalParam_I2[0] =  CountTotal / CountTimes;
//			OSMutexPend(OSUart0UsedMutex,0,&err);
//			CountCtrl(COUNT_END);
//			OSMutexPost(OSUart0UsedMutex);
        CalParam_U[0] = 1000.0*log(CalParam_I1[0]/CalParam_I2[0])*AREA/(BiaoPian*10);//cm2/mg
        DisCList16(80,30,"��һ�α궨");
        slen = StringLen("��һ�α궨���,�밴ȷ����"); //
        DisCList16((DIS_AREA_X-slen*8)/2,80,"��һ�α궨���,�밴ȷ����");

        WriteCode1212(43,60,"I1:       I2:       u1: ");
        DisValue(CalParam_I1[0],61,60,1,5,0,1);
        DisValue(CalParam_I2[0],121,60,1,5,0,1);
        DisValue(CalParam_U[0],181,60,1,4,3,1);

        prompt(0);	// ��ʾ ȷ��

        CaliFlg = 3;

        break;
    case 3:    //�ڶ��α궨

        //���� ��ֵ����ʾ
// 			WriteFp32(JIAOZHUN_BIAOPIAN,Parg_u);
// 			WriteCaliParTime(JIAOZHUN_BIAOPIAN_TIME,&TimeDate);
        CleanVideo(area);

        DisCList16(80,30,"�ڶ��α궨");

        slen = StringLen(NextPrompt[24+NePrOffset]); //
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[24+NePrOffset]);   //�ɿ������ʾ
        LcdUpdata(DisBuf);
        songkaiMoto();
        CleanVideo(area);
        //��ȡ����ֽ �����Ƭ1
        DisCList16(80,30,"�ڶ��α궨");
        slen = StringLen("��ȡ�±�Ƭ,����ȷ����");
        DisCList16((DIS_AREA_X-slen*8)/2,70,"��ȡ�±�Ƭ,����ȷ����");   //ȡ����ֽ��ʾ
        prompt(0);	// ��ʾ ȷ��
        LcdUpdata(DisBuf);
        CaliFlg = 4;
        break;
    case 4:
        //��ʾ����200s
        CleanVideo(area);
        DisCList16(80,30,"�ڶ��α궨");

        slen = StringLen(NextPrompt[30+NePrOffset]);   //   ����ѹ�������	�Ժ�
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[30+NePrOffset]);
        LcdUpdata(DisBuf);
        yajingMoto();

        CleanVideo(area);
        DisCList16(80,30,"�ڶ��α궨");

        slen = StringLen(NextPrompt[27+NePrOffset]); //   ����200s	�Ժ�
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[27+NePrOffset]);
        LcdUpdata(DisBuf);
        OSMutexPend(OSUart0UsedMutex,0,&err);
        CountCtrl(COUNT_START);	//��������
        OSMutexPost(OSUart0UsedMutex);
        TimeKeyCount = JIAOZHUN_BIAOPIAN_TEST_TIME*50;
        CountStartFlg =1;
        while(TimeKeyCount)
        {
            CleanVideo(area);
            DisCList16(80,30,"�ڶ��α궨");
            DisDat.length = 5;
            DisDat.decimal = 0;
            DisDat.DisSign = 0;
            DisDat.font = 2;
            DisValue2(CountNow,96,96, &DisDat ,"");
            DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[27+NePrOffset]);
            DisValue2((int)(TimeKeyCount/50),16*2+(DIS_AREA_X-slen*8)/2,70, &DisDat ,"");

            LcdUpdata(DisBuf);
            OSTimeDly(100);//0.5��;
        }
        CleanVideo(area);
        OSMutexPend(OSUart0UsedMutex,0,&err);
        CountRead();
        OSMutexPost(OSUart0UsedMutex);
        CountStartFlg = 0;
        //����ƽ��ֵ
        EeorMear = (CountTotal<50)?(EeorMear|0x01):(EeorMear&0xfe);		   //̽��������

        CalParam_I1[1] =  CountTotal / CountTimes;
//			OSMutexPend(OSUart0UsedMutex,0,&err);
//			CountCtrl(COUNT_END);
//			OSMutexPost(OSUart0UsedMutex);
        //�����ɿ�ѹֽ���
        CleanVideo(area);
        DisCList16(80,30,"�ڶ��α궨");
        slen = StringLen(NextPrompt[24+NePrOffset]); //
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[24+NePrOffset]);
        LcdUpdata(DisBuf);
        songkaiMoto();
        CleanVideo(area);
        DisCList16(80,30,"�ڶ��α궨");
        slen = StringLen(NextPrompt[26+NePrOffset]); //  �����2
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[26+NePrOffset]);
        prompt(2);	// ��ʾ ȷ��
        LcdUpdata(DisBuf);
        CaliFlg = 5;
        break;
    case 5:
        CleanVideo(area);
        DisCList16(80,30,"�ڶ��α궨");
        slen = StringLen(NextPrompt[30+NePrOffset]); //   ����ѹ�������	�Ժ�
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[30+NePrOffset]);
        LcdUpdata(DisBuf);
        yajingMoto();
        CleanVideo(area);
        DisCList16(80,30,"�ڶ��α궨");
        slen = StringLen(NextPrompt[27+NePrOffset]); //   ����200s
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[27+NePrOffset]);
        LcdUpdata(DisBuf);
        OSMutexPend(OSUart0UsedMutex,0,&err);
        CountCtrl(COUNT_START);	//��������
        OSMutexPost(OSUart0UsedMutex);
        TimeKeyCount = JIAOZHUN_BIAOPIAN_TEST_TIME*50;//20msһ�� 5s
        CountStartFlg =1;

        while(TimeKeyCount)
        {
            CleanVideo(area);
            DisCList16(80,30,"�ڶ��α궨");
            DisDat.length = 5;
            DisDat.decimal = 0;
            DisDat.DisSign = 0;
            DisDat.font = 2;
            DisValue2(CountNow,96,96, &DisDat ,"");
            DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[27+NePrOffset]);
            DisValue2((int)(TimeKeyCount/50),16*2+(DIS_AREA_X-slen*8)/2,70, &DisDat ,"");

            LcdUpdata(DisBuf);
            OSTimeDly(100);//0.5��;
        }
        CleanVideo(area);
        OSMutexPend(OSUart0UsedMutex,0,&err);
        CountRead();
        OSMutexPost(OSUart0UsedMutex);
        CountStartFlg = 0;
        //����ƽ��ֵ
        EeorMear = (CountTotal<50)?(EeorMear|0x01):(EeorMear&0xfe);		   //̽��������

        CalParam_I2[1] =  CountTotal / CountTimes;
//			OSMutexPend(OSUart0UsedMutex,0,&err);
//			CountCtrl(COUNT_END);
//			OSMutexPost(OSUart0UsedMutex);
        CalParam_U[1] = 1000.0*log(CalParam_I1[1]/CalParam_I2[1])*AREA/(BiaoPian*10);//cm2/mg

        DisCList16(80,30,"�ڶ��α궨");
        slen = StringLen("�ڶ��α궨���,�밴ȷ����"); //
        DisCList16((DIS_AREA_X-slen*8)/2,80,"�ڶ��α궨���,�밴ȷ����");

        WriteCode1212(43,60,"I1:       I2:       u2: ");
        DisValue(CalParam_I1[1],61,60,1,5,0,1);
        DisValue(CalParam_I2[1],121,60,1,5,0,1);
        DisValue(CalParam_U[1],181,60,1,4,3,1);

        prompt(0);	// ��ʾ ȷ��

        CaliFlg = 6;
        break;
    case 6:    //�����α궨

        //���� ��ֵ����ʾ
// 			WriteFp32(JIAOZHUN_BIAOPIAN,Parg_u);
// 			WriteCaliParTime(JIAOZHUN_BIAOPIAN_TIME,&TimeDate);
        CleanVideo(area);

        DisCList16(80,30,"�����α궨");

        slen = StringLen(NextPrompt[24+NePrOffset]); //
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[24+NePrOffset]);   //�ɿ������ʾ
        LcdUpdata(DisBuf);
        songkaiMoto();
        CleanVideo(area);
        //��ȡ����ֽ �����Ƭ1
        DisCList16(80,30,"�����α궨");
        slen = StringLen("��ȡ�±�Ƭ,����ȷ����");
        DisCList16((DIS_AREA_X-slen*8)/2,70,"��ȡ�±�Ƭ,����ȷ����");   //ȡ����ֽ��ʾ
        prompt(0);	// ��ʾ ȷ��
        LcdUpdata(DisBuf);
        CaliFlg = 7;
        break;
    case 7:
        //��ʾ����200s
        CleanVideo(area);
        DisCList16(80,30,"�����α궨");

        slen = StringLen(NextPrompt[30+NePrOffset]);   //   ����ѹ�������	�Ժ�
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[30+NePrOffset]);
        LcdUpdata(DisBuf);
        yajingMoto();

        CleanVideo(area);
        DisCList16(80,30,"�����α궨");

        slen = StringLen(NextPrompt[27+NePrOffset]); //   ����200s	�Ժ�
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[27+NePrOffset]);
        LcdUpdata(DisBuf);
        OSMutexPend(OSUart0UsedMutex,0,&err);
        CountCtrl(COUNT_START);	//��������
        OSMutexPost(OSUart0UsedMutex);

        TimeKeyCount = JIAOZHUN_BIAOPIAN_TEST_TIME*50;

        CountStartFlg =1;
        while(TimeKeyCount)
        {
            CleanVideo(area);
            DisCList16(80,30,"�����α궨");
            DisDat.length = 5;
            DisDat.decimal = 0;
            DisDat.DisSign = 0;
            DisDat.font = 2;
            DisValue2(CountNow,96,96, &DisDat ,"");
            DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[27+NePrOffset]);
            DisValue2((int)(TimeKeyCount/50),16*2+(DIS_AREA_X-slen*8)/2,70, &DisDat ,"");

            LcdUpdata(DisBuf);
            OSTimeDly(100);//0.5��;
        }
        CleanVideo(area);
        OSMutexPend(OSUart0UsedMutex,0,&err);
        CountRead();
        OSMutexPost(OSUart0UsedMutex);
        CountStartFlg = 0;
        //����ƽ��ֵ
        EeorMear = (CountTotal<50)?(EeorMear|0x01):(EeorMear&0xfe);		   //̽��������

        CalParam_I1[2] =  CountTotal / CountTimes;
//			OSMutexPend(OSUart0UsedMutex,0,&err);
//			CountCtrl(COUNT_END);
//			OSMutexPost(OSUart0UsedMutex);
        //�����ɿ�ѹֽ���
        CleanVideo(area);
        DisCList16(80,30,"�����α궨");
        slen = StringLen(NextPrompt[24+NePrOffset]); //
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[24+NePrOffset]);
        LcdUpdata(DisBuf);
        songkaiMoto();
        CleanVideo(area);
        DisCList16(80,30,"�����α궨");
        slen = StringLen(NextPrompt[26+NePrOffset]); //  �����2
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[26+NePrOffset]);
        prompt(2);	// ��ʾ ȷ��
        LcdUpdata(DisBuf);
        CaliFlg = 8;
        break;
    case 8:
        CleanVideo(area);
        DisCList16(80,30,"�����α궨");
        slen = StringLen(NextPrompt[30+NePrOffset]); //   ����ѹ�������	�Ժ�
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[30+NePrOffset]);
        LcdUpdata(DisBuf);
        yajingMoto();
        CleanVideo(area);
        DisCList16(80,30,"�����α궨");
        slen = StringLen(NextPrompt[27+NePrOffset]); //   ����200s
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[27+NePrOffset]);
        LcdUpdata(DisBuf);

        OSMutexPend(OSUart0UsedMutex,0,&err);
        CountCtrl(COUNT_START);	//��������
        OSMutexPost(OSUart0UsedMutex);
        TimeKeyCount = JIAOZHUN_BIAOPIAN_TEST_TIME*50;//20msһ�� 5s

        CountStartFlg =1;

        while(TimeKeyCount)
        {
            CleanVideo(area);
            DisCList16(80,30,"�����α궨");
            DisDat.length = 5;
            DisDat.decimal = 0;
            DisDat.DisSign = 0;
            DisDat.font = 2;
            DisValue2(CountNow,96,96, &DisDat ,"");
            DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[27+NePrOffset]);
            DisValue2((int)(TimeKeyCount/50),16*2+(DIS_AREA_X-slen*8)/2,70, &DisDat ,"");

            LcdUpdata(DisBuf);
            OSTimeDly(100);//0.5��;
        }

        CleanVideo(area);

        OSMutexPend(OSUart0UsedMutex,0,&err);
        CountRead();
        OSMutexPost(OSUart0UsedMutex);
        CountStartFlg = 0;
        //����ƽ��ֵ
        EeorMear = (CountTotal<50)?(EeorMear|0x01):(EeorMear&0xfe);		   //̽��������

        CalParam_I2[2] =  CountTotal / CountTimes;
//			OSMutexPend(OSUart0UsedMutex,0,&err);
//			CountCtrl(COUNT_END);
//			OSMutexPost(OSUart0UsedMutex);
        CalParam_U[2] = 1000.0*log(CalParam_I1[2]/CalParam_I2[2])*AREA/(BiaoPian*10);//cm2/mg

        ParOldU = 	ReadFp32(JIAOZHUN_BIAOPIAN);

        CalParam_avr_U = (CalParam_U[0]+CalParam_U[1]+CalParam_U[2])/3;          //����Ʒ��ֵ

        CalParam_max_U = CalParam_U[0];
        CalParam_min_U = CalParam_U[0];

        for (i=0; i<2; i++)
        {

            if(CalParam_max_U < CalParam_U[i+1])
            {
                CalParam_max_U = CalParam_U[i+1];
            }

            if(CalParam_min_U > CalParam_U[i+1])
            {
                CalParam_min_U = CalParam_U[i+1];
            }
        }

        CalParam_AARD =100*(CalParam_max_U - CalParam_min_U )/CalParam_avr_U;       //���ƫ��ٷֱ�


        WriteCode1212(10,22,"��һ�β���:I1:      I2:      u1: ");
        WriteCode1212(10,42,"�ڶ��β���:I1:      I2:      u2: ");
        WriteCode1212(10,62,"�����β���:I1:      I2:      u2: ");
        WriteCode1212(10,82,"uƽ��ֵ:          u������:     %");
        WriteCode1212(10,102,"��ʷУ׼��¼:     ");

        slen = StringLen("�Ƿ����uֵ ������?");                  //
        DisCList16((DIS_AREA_X-slen*8)/2,122,"�Ƿ����uֵ?");
        //��һ�β��Բ�����ʾ
        DisValue(CalParam_I1[0],92,22,1,5,0,1);
        DisValue(CalParam_I2[0],146,22,1,5,0,1);
        DisValue(CalParam_U[0],200,22,1,4,3,1);
        //�ڶ��β��Բ�����ʾ
        DisValue(CalParam_I1[1],92,42,1,5,0,1);
        DisValue(CalParam_I2[1],146,42,1,5,0,1);
        DisValue(CalParam_U[1],200,42,1,4,3,1);
        //�����β��Բ�����ʾ
        DisValue(CalParam_I1[2],92,62,1,5,0,1);
        DisValue(CalParam_I2[2],146,62,1,5,0,1);
        DisValue(CalParam_U[2],200,62,1,4,3,1);
        //uƷ��ֵ��ʾ
        DisValue(CalParam_avr_U,53,82,1,4,3,1);
        //������
        DisValue(CalParam_AARD,180,82,1,3,1,1);
        //��ʷУ׼��¼
        DisValue(ParOldU,60,102,1,4,3,1);

        prompt(0);	// ��ʾ ȷ��
        LcdUpdata(DisBuf);
        //����EEPROM
        //��ʾ

        CaliFlg = 9;
        break;
    case 9:

        //���� ��ֵ����ʾ
        Parg_u = CalParam_avr_U;
        WriteFp32(JIAOZHUN_BIAOPIAN,Parg_u);
        WriteCaliParTime(JIAOZHUN_BIAOPIAN_TIME,&TimeDate);
        CaliFlg = 10;
        break;
    default:
        break;
    }

    //����Դ��λ
    OSMutexPend(OSUart0UsedMutex,0,&err);
    TTLOutput (TTL_2,0,TTL_BRAY_Ctrl1);	 //
    TTLOutput (TTL_2,1,TTL_BRAY_Ctrl2);	 //
    OSMutexPost(OSUart0UsedMutex);

    OSTimeDlyHMSM(0, 0, C14MotMoveTime,0);	//����ʱ��

    OSMutexPend(OSUart0UsedMutex,0,&err);
    TTLOutput (TTL_2,0,TTL_BRAY_Ctrl1);	 //
    TTLOutput (TTL_2,0,TTL_BRAY_Ctrl2);	 //
    OSMutexPost(OSUart0UsedMutex);
    // �������Ƭ��Ӧ  ֵ��  ȷ������  ���²����� ȷ�����Զ�����У׼���������档
}
//У׼Ĥ�����Բ���
void 	Calibration2(void)
{
    uint32 slen;
    uint8 err;
    uint8 i;
    uint8 j;
    float temp;
    float ParOldU;

    //��ƬУ׼���Ա���TestTimes   by yxq
    //  CheckCount
    //����Դ��λ
    OSMutexPend(OSUart0UsedMutex,0,&err);
    TTLOutput (TTL_2,1,TTL_BRAY_Ctrl1);	 //��
    TTLOutput (TTL_2,0,TTL_BRAY_Ctrl2);	 //��
    OSMutexPost(OSUart0UsedMutex);

    OSTimeDlyHMSM(0, 0, C14MotMoveTime,0);	//����ʱ��

    OSMutexPend(OSUart0UsedMutex,0,&err);
    TTLOutput (TTL_2,0,TTL_BRAY_Ctrl1);	 //��
    TTLOutput (TTL_2,0,TTL_BRAY_Ctrl2);	 //��
    OSMutexPost(OSUart0UsedMutex);

    switch (CaliFlg)
    {
        //�����ɿ�ѹֽ��� ��
    case 0:
        area[0] = 0;
        area[1] = 25;
        area[2] = 239;
        area[3] = 159;
        CleanVideo(area);      //�����ʾ����
        //��һ����ʾ��������ʾ
        DisCList16(80,25,"��  �β���");

        DisDat.length = 1;
        DisDat.decimal = 0;
        DisDat.DisSign = 0;
        DisDat.font = 2;
        DisValue2((CheckCount+1),96,25, &DisDat ,"");

        area[0] = 0;      //
        area[1] = 45;
        area[2] = 239;
        area[3] = 159;


        slen = StringLen(NextPrompt[24+NePrOffset]); //
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[24+NePrOffset]);  //��ʾ�ɿ���ֽ�����ʾ
        LcdUpdata(DisBuf);
        songkaiMoto();       //ִ���ɿ���ֽ�������
        CleanVideo(area);
        //��ȡ����ֽ �����Ƭ1
        slen = StringLen(NextPrompt[25+NePrOffset]);
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[25+NePrOffset]);  //��ʾȥ����ֽ��ʾ
        prompt(0);	// ��ʾ ȷ��
        LcdUpdata(DisBuf);
        CaliFlg = 1;
        break;
    case 1:
        //��ʾ����200s
        CleanVideo(area);

        slen = StringLen(NextPrompt[30+NePrOffset]); //   ����ѹ�������	�Ժ�
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[30+NePrOffset]);
        LcdUpdata(DisBuf);
        yajingMoto();           //ѹ�����

        CleanVideo(area);
        slen = StringLen(NextPrompt[27+NePrOffset]); //   ����200s	�Ժ�
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[27+NePrOffset]);  //����ʱ����
        LcdUpdata(DisBuf);
        OSMutexPend(OSUart0UsedMutex,0,&err);
        CountCtrl(COUNT_START);	             //������������
        OSMutexPost(OSUart0UsedMutex);
        TimeKeyCount = JIAOZHUN_BIAOPIAN_TEST_TIME*50;
        CountStartFlg =1;
        while(TimeKeyCount)
        {
            CleanVideo(area);
            DisDat.length = 5;
            DisDat.decimal = 0;
            DisDat.DisSign = 0;
            DisDat.font = 2;
            DisValue2(CountNow,96,96, &DisDat ,"");
            DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[27+NePrOffset]);
            DisValue2((int)(TimeKeyCount/50),16*2+(DIS_AREA_X-slen*8)/2,70, &DisDat ,"");

            LcdUpdata(DisBuf);
            OSTimeDly(100);//0.5��;
        }
        CleanVideo(area);
        OSMutexPend(OSUart0UsedMutex,0,&err);
        CountRead();        //��ȡ����ֵ
        OSMutexPost(OSUart0UsedMutex);
        CountStartFlg = 0;
        //����ƽ��ֵ
        EeorMear = (CountTotal<50)?(EeorMear|0x01):(EeorMear&0xfe);		   //̽��������

        CheParam_I1[CheckCount] =  CountTotal / CountTimes;           //�հ׼�����
//			OSMutexPend(OSUart0UsedMutex,0,&err);
//			CountCtrl(COUNT_END);
//			OSMutexPost(OSUart0UsedMutex);
        //�����ɿ�ѹֽ���
        CleanVideo(area);
        slen = StringLen(NextPrompt[24+NePrOffset]); //
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[24+NePrOffset]);  //�ɿ������ʾ
        LcdUpdata(DisBuf);
        songkaiMoto();
        CleanVideo(area);
        slen = StringLen(NextPrompt[26+NePrOffset]); //  �����2
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[26+NePrOffset]);
        prompt(2);	// ��ʾ ȷ��
        LcdUpdata(DisBuf);
        CaliFlg = 2;
        break;


    case 2:
        //
        CleanVideo(area);

        slen = StringLen(NextPrompt[30+NePrOffset]); //   ����ѹ�������	�Ժ�
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[30+NePrOffset]);
        LcdUpdata(DisBuf);
        yajingMoto();
        CleanVideo(area);
        slen = StringLen(NextPrompt[27+NePrOffset]); //   ����200s
        DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[27+NePrOffset]);
        LcdUpdata(DisBuf);
        OSMutexPend(OSUart0UsedMutex,0,&err);
        CountCtrl(COUNT_START);	//��������
        OSMutexPost(OSUart0UsedMutex);
        TimeKeyCount = JIAOZHUN_BIAOPIAN_TEST_TIME*50;//20msһ�� 5s
        CountStartFlg =1;

        while(TimeKeyCount)
        {
            CleanVideo(area);
            DisDat.length = 5;
            DisDat.decimal = 0;
            DisDat.DisSign = 0;
            DisDat.font = 2;
            DisValue2(CountNow,96,96, &DisDat ,"");
            DisCList16((DIS_AREA_X-slen*8)/2,70,NextPrompt[27+NePrOffset]);
            DisValue2((int)(TimeKeyCount/50),16*2+(DIS_AREA_X-slen*8)/2,70, &DisDat ,"");

            LcdUpdata(DisBuf);
            OSTimeDly(100);//0.5��;
        }
        CleanVideo(area);
        OSMutexPend(OSUart0UsedMutex,0,&err);
        CountRead();
        OSMutexPost(OSUart0UsedMutex);
        CountStartFlg = 0;
        //����ƽ��ֵ
        EeorMear = (CountTotal<50)?(EeorMear|0x01):(EeorMear&0xfe);		   //̽��������

        CheParam_I2[CheckCount] =  CountTotal / CountTimes;        //��Ƭ������
//			OSMutexPend(OSUart0UsedMutex,0,&err);
//			CountCtrl(COUNT_END);
//			OSMutexPost(OSUart0UsedMutex);
// 			Parg_u = 1000.0*log(CalParam_I1/CalParam_I2)*AREA/(BiaoPian*10);//cm2/mg
        CheParam_U[CheckCount] = 100.0*log(CheParam_I1[CheckCount]/CheParam_I2[CheckCount])*AREA/Parg_u;//cm2/mg

        DisCList16(30,60,"���β���:I1:     I2:");
        DisCList16(30,80,"          U:");

        prompt(0);	// ��ʾ ȷ��
        DisValue(CheParam_I1[CheckCount],126,60,1,5,0,1);
        DisValue(CheParam_I2[CheckCount],190,60,1,5,0,1);
        DisValue(CheParam_U[CheckCount],126,80,1,4,3,1);
        LcdUpdata(DisBuf);
        //����EEPROM
        //��ʾ

//			CaliFlg = 3;
        CheckCount++;

        if(CheckCount < TestTimes )
        {
            CaliFlg = 0;
        }
        else
        {
            CaliFlg = 3;    //������ʾ����
        }

        break;

    case 3:       //��ʾ���в��Խ��,����������Ա�׼ƫ��
        //����Ʒ��ֵ
        temp = 0;
        for (i=0; i<TestTimes; i++)
        {
            temp += CheParam_U[i];
        }
        CheParam_avr_U = temp / TestTimes ;
        //������Ա�׼ƫ��
        if(TestTimes ==1)
        {
            CheParam_AARD = 0.0 ;
        }
        else
        {
            temp = 0;
            for (i=0; i<TestTimes; i++)
            {
                temp += ((CheParam_U[i]-CheParam_avr_U)*(CheParam_U[i]-CheParam_avr_U));
            }
            CheParam_AARD = 100*(sqrt((1.0/(TestTimes-1))*temp))/CheParam_avr_U;  //��Ա�׼ƫ��ٷֱ�
        }

        area[0] = 0;
        area[1] = 25;
        area[2] = 239;
        area[3] = 159;
        CleanVideo(area);

        j = 0 ;
        for(i=0; i<TestTimes; i++)
        {
            if((i % 2)) //�ڶ���
            {
                WriteCode1212(140,20+17*j,"u : ");

                DisValue(i,146,20+17*j,1,1,0,0);
                DisValue(CheParam_U[i],164,20+17*j,1,4,3,1);
                j++;
            }
            else   //��һ��
            {
                WriteCode1212(40,20+17*j,"u  : ");
                DisValue(i,46,20+17*j,1,1,0,0);
                DisValue(CheParam_U[i],64,20+17*j,1,4,3,1);
            }

        }

        WriteCode1212(20,20+17*5,"ƽ��ֵ:    ");
        DisValue(CheParam_avr_U,62,20+17*5,1,4,3,1);

        ParOldU = 	ReadFp32(JIAOZHUN_BIAOPIAN);
        WriteCode1212(140,20+17*5,"��¼ֵ:    ");
        DisValue(ParOldU,185,20+17*5,1,4,3,1);


        WriteCode1212(20,20+17*6,"��Ա�׼ƫ��:       %");
        DisValue(CheParam_AARD,98,20+17*6,1,3,1,1);


        prompt(0);	// ��ʾ ȷ��

        CaliFlg = 4;
        break;

    case 4:
        CaliFlg = 5;
        break;

    default:
        break;
    }

    //����Դ��λ
    OSMutexPend(OSUart0UsedMutex,0,&err);
    TTLOutput (TTL_2,0,TTL_BRAY_Ctrl1);
    TTLOutput (TTL_2,1,TTL_BRAY_Ctrl2);
    OSMutexPost(OSUart0UsedMutex);

    OSTimeDlyHMSM(0, 0, C14MotMoveTime,0);	//����ʱ��

    OSMutexPend(OSUart0UsedMutex,0,&err);
    TTLOutput (TTL_2,0,TTL_BRAY_Ctrl1);
    TTLOutput (TTL_2,0,TTL_BRAY_Ctrl2);
    OSMutexPost(OSUart0UsedMutex);
    // �������Ƭ��Ӧ  ֵ��  ȷ������  ���²����� ȷ�����Զ�����У׼���������档
}
