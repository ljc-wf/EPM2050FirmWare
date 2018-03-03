/****************************************Copyright (c)****************************************************
**                            Skyray Instrment Co.,LTD.
**
**                         http://www.skyray-instrument.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           main.C
** Last modified Date:  2011-8-3 10:52:24
** Last Version:        V2.5
** Descriptions:        ����������Яʽ�ؽ��������
**
*********************************************************************************************************/

#include 	"global.h"

float  		Wait_Flg = 0;
OS_EVENT 	*OSLcdUsedMutex		=	NULL;	//�����ź���
OS_EVENT 	*OSI2CUsedMutex		=	NULL;
OS_EVENT 	*OSUart0UsedMutex	=	NULL;	//�����ź���
OS_EVENT 	*OSFlashUsedMutex	=	NULL;	//�����ź���

/*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
										�����ջ����
*********************************************************************************************************/
static  OS_STK         AppTask_Start_Stk[APP_TASK_START_STK_SIZE];
static  OS_STK         AppTask_ModbusSlave_Stk[APP_TASK_ModbusSlave_STK_SIZE];
static  OS_STK         AppTask_Meas_Stk[APP_TASK_MEAS_STK_SIZE];
static  OS_STK         AppTask_HistoryMemoryRead_Stk[APP_TASK_HistoryMemoryRead_STK_SIZE];
static  OS_STK         AppTask_DataGather_PineTempCtrl_Stk[APP_TASK_DataGather_PineTempCtrl_STK_SIZE];
static  OS_STK         AppTask_Kbd_Stk[APP_TASK_KBD_STK_SIZE];
static  OS_STK         AppTask_ModbusMaster_Stk[APP_TASK_ModbusMaster_STK_SIZE];
static  OS_STK         AppTask_Refresh_Lcd_AnalogOutput_Stk[APP_TASK_Refresh_Lcd_AnalogOutput_STK_SIZE];
static  OS_STK         AppTask_DTU_Stk[APP_TASK_DTU_STK_SIZE];
static  OS_STK         AppTask_SHELL_Stk[APP_TASK_SHELL_STK_SIZE];

/*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************/

static  void  AppTask_Create       						(void);

static  void  AppTask_Start        						(void        *p_arg);
static  void  AppTask_ModbusSlave      					(void        *p_arg);
static  void  AppTask_Meas         						(void        *p_arg);
static  void  AppTask_HistoryMemoryRead          		(void        *p_arg);
static  void  AppTask_Kbd          						(void        *p_arg);
static  void  AppTask_DataGather_PineTempCtrl 	   		(void 		 *p_arg);
static  void  AppTask_Refresh_Lcd_AnalogOutput 	   		(void 		 *p_arg);
static  void  AppTask_DTU 	   		            		(void 		 *p_arg);
static  void  AppTask_SHELL 	   		            		(void 		 *p_arg);

/*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************/
int main (void)
{

    CPU_INT08U  os_err =   os_err;
    /*
    Disable all the interrupts
    �����������񣬺ͼ�������
    Initialize "uC/OS-II, The Real-Time Kernel".
    */
    CPU_IntDis();
    OSInit();
    /* Create the start task.*/
    os_err = OSTaskCreateExt((void (*)(void *)) AppTask_Start,
                             (void          * ) 0,
                             (OS_STK        * )&AppTask_Start_Stk[APP_TASK_START_STK_SIZE - 1],	 //ջ��
                             (INT8U           ) APP_TASK_START_PRIO,							 //���ȼ�
                             (INT16U          ) APP_TASK_START_PRIO,							 //
                             (OS_STK        * )&AppTask_Start_Stk[0],							 //ջβ
                             (INT32U          ) APP_TASK_START_STK_SIZE,						 //����Ķ�ջ��С
                             (void          * )0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));	 //��ջ�Ƿ����㣬�Ƿ�ʹ�ܶ�ջʹ��������
    /*��ʼ�������*/
    OSStart();               /* Start multitasking (i.e. give control to uC/OS-II).  */

    return (0);
}

/*********************************************************************************************************
*                                          AppTask_Start()
*
* Description : The startup task.  The uC/OS-II ticker should only be initialize once multitasking starts.
*
* Argument(s) : p_arg       Argument passed to 'App_TaskStart()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************/

static  void  AppTask_Start (void *p_arg)
{
    CPU_INT08U   err;
    /*
    ���������
    Initialize BSP functions.
    ����USART2 Timer2���������ʼ����
    modus��س�ʼ��
    */
    (void)p_arg;
    InitDevice1();

    //��pc��ͨ�ŷ�ʽѡ��
    if (!ComminiStyle)
    {
        eMBTCPInit(0);
    }
    else if (ComminiStyle)
    {
        eMBInit(MB_RTU, 0x01, 1, 19200, MB_PAR_NONE);
    }
    eMBEnable();
    /*
    ucosII���Ķ�ʱ����ʼ������֤1s��ӦOS_TICKS_PER_SEC������
    ���� SystemCoreClock�� system_LPC17XX.C����  ��ϵͳ����Ϊ72M
    2����ʼ��
    */
    SysTick_Config(SystemCoreClock/OS_TICKS_PER_SEC - 1);

#if (OS_TASK_STAT_EN > 0)
    /* ϵͳ�����ʼ�� Determine CPU capacity.*/
    OSStatInit();
#endif

#if ((APP_PROBE_COM_EN == DEF_ENABLED) || \
     (APP_OS_PROBE_EN  == DEF_ENABLED))
#endif
    /*
    �ź��� LCD������ʾ���ɰ�����rtc�ٷ���
    LCD������Դ�������ҽ������
    I2C������Դ�������ҽ������
    */
    OSFlashUsedMutex	=	OSMutexCreate(MUTEX_FLASHUSED_PRIO,&err);
    OSUart0UsedMutex	=	OSMutexCreate(MUTEX_UART0USED_PRIO,&err);
    OSLcdUsedMutex		=	OSMutexCreate(MUTEX_LCDUSED_PRIO,&err);
    OSI2CUsedMutex		=	OSMutexCreate(MUTEX_I2CUSED_PRIO,&err);
    /*
    ��������������ubd��UserIFͨ�� ���������
    ����������������
    3����ʼ��
    ���� ��ʾ��ʼ�����
    */

    InitDevice2();
    AppTask_Create();
    BuzzerCtrl(2,5,5);

    /*�������е�*/
    while (DEF_TRUE)
    {
        OSTimeDlyHMSM(0, 0, 3, 0);
    }
}


/*********************************************************************************************************
*                                            AppTask_Create()
*
* Description : Create the application tasks.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : App_Task_Start().
*
* Note(s)     : none.
*********************************************************************************************************/
static  void  AppTask_Create (void)
{
    CPU_INT08U  os_err = os_err;
    /*ModBus�ӻ���������*/    //����λͨ��
    os_err = OSTaskCreateExt((void (*)(void *)) AppTask_ModbusSlave,
                             (void          * ) 0,
                             (OS_STK        * )&AppTask_ModbusSlave_Stk[APP_TASK_ModbusSlave_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_ModbusSlave_PRIO,
                             (INT16U          ) APP_TASK_ModbusSlave_PRIO,
                             (OS_STK        * )&AppTask_ModbusSlave_Stk[0],
                             (INT32U          ) APP_TASK_ModbusSlave_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
    /*��������*/
    os_err = OSTaskCreateExt((void (*)(void *)) AppTask_Kbd,
                             (void          * ) 0,
                             (OS_STK        * )&AppTask_Kbd_Stk[APP_TASK_KBD_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_KBD_PRIO,
                             (INT16U          ) APP_TASK_KBD_PRIO,
                             (OS_STK        * )&AppTask_Kbd_Stk[0],
                             (INT32U          ) APP_TASK_KBD_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
    /*��������*/    //�������̿���
    os_err = OSTaskCreateExt((void (*)(void *)) AppTask_Meas,
                             (void          * ) 0,
                             (OS_STK        * )&AppTask_Meas_Stk[APP_TASK_MEAS_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_MEAS_PRIO,
                             (INT16U          ) APP_TASK_MEAS_PRIO,
                             (OS_STK        * )&AppTask_Meas_Stk[0],
                             (INT32U          ) APP_TASK_MEAS_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));

    /*�����ϴ���Ӧ����*/     //������Ӧ��λ����ȡ�����¼�
    os_err = OSTaskCreateExt((void (*)(void *)) AppTask_HistoryMemoryRead,
                             (void          * ) 0,
                             (OS_STK        * )&AppTask_HistoryMemoryRead_Stk[APP_TASK_HistoryMemoryRead_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_HistoryMemoryRead_PRIO,
                             (INT16U          ) APP_TASK_HistoryMemoryRead_PRIO,
                             (OS_STK        * )&AppTask_HistoryMemoryRead_Stk[0],
                             (INT32U          ) APP_TASK_HistoryMemoryRead_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));

    /*���ݲɼ������ȴ��¶ȿ�������*/      //�����ȡ������ֵ�����ڸ���ADC����ֵ�������¶ȼ�⼰�¶ȿ���
    os_err = OSTaskCreateExt((void (*)(void *)) AppTask_DataGather_PineTempCtrl,
                             (void          * ) 0,
                             (OS_STK        * )&AppTask_DataGather_PineTempCtrl_Stk[APP_TASK_DataGather_PineTempCtrl_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_DataGather_PineTempCtrl_PRIO,
                             (INT16U          ) APP_TASK_DataGather_PineTempCtrl_PRIO,
                             (OS_STK        * )&AppTask_DataGather_PineTempCtrl_Stk[0],
                             (INT32U          ) APP_TASK_DataGather_PineTempCtrl_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
    /*modbus��������*/  //modbus����������ͽӿڰ�ͨ��
//    os_err = OSTaskCreateExt((void (*)(void *)) OSModbusServe,
//                             (void          * ) 0,
//                             (OS_STK        * )&AppTask_ModbusMaster_Stk[APP_TASK_ModbusMaster_STK_SIZE - 1],
//                             (INT8U           ) APP_TASK_ModbusMaster_PRIO,
//                             (INT16U          ) APP_TASK_ModbusMaster_PRIO,
//                             (OS_STK        * )&AppTask_ModbusMaster_Stk[0],
//                             (INT32U          ) APP_TASK_ModbusMaster_STK_SIZE,
//                             (void          * ) 0,
//                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
    /*LCD��ҳ����ˢ�¼�ģ�����ˢ��*/
    os_err = OSTaskCreateExt((void (*)(void *)) AppTask_Refresh_Lcd_AnalogOutput,
                             (void          * ) 0,
                             (OS_STK        * )&AppTask_Refresh_Lcd_AnalogOutput_Stk[APP_TASK_Refresh_Lcd_AnalogOutput_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_Refresh_Lcd_AnalogOutput_PRIO,
                             (INT16U          ) APP_TASK_Refresh_Lcd_AnalogOutput_PRIO,
                             (OS_STK        * )&AppTask_Refresh_Lcd_AnalogOutput_Stk[0],
                             (INT32U          ) APP_TASK_Refresh_Lcd_AnalogOutput_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
    /*DTU������͸�� */
//    os_err = OSTaskCreateExt((void (*)(void *)) AppTask_DTU,
//                             (void          * ) 0,
//                             (OS_STK        * )&AppTask_DTU_Stk[APP_TASK_DTU_STK_SIZE - 1],
//                             (INT8U           ) APP_TASK_DTU_PRIO,
//                             (INT16U          ) APP_TASK_DTU_PRIO,
//                             (OS_STK        * )&AppTask_DTU_Stk[0],
//                             (INT32U          ) APP_TASK_DTU_STK_SIZE,
//                             (void          * ) 0,
//                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
    /*SHELL�˻����ڽ���*/
//    os_err = OSTaskCreateExt((void (*)(void *)) AppTask_SHELL,
//                             (void          * ) 0,
//                             (OS_STK        * )&AppTask_SHELL_Stk[APP_TASK_SHELL_STK_SIZE - 1],
//                             (INT8U           ) APP_TASK_SHELL_PRIO,
//                             (INT16U          ) APP_TASK_SHELL_PRIO,
//                             (OS_STK        * )&AppTask_SHELL_Stk[0],
//                             (INT32U          ) APP_TASK_SHELL_STK_SIZE,
//                             (void          * ) 0,
//                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
}


/*********************************************************************************************************
*                                            AppTask_ModbusSlave()
*
* Description :     //����λ��ͨ������
*
* Argument(s) : p_arg
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************/

static  void  AppTask_ModbusSlave (void *p_arg)
{
    (void)p_arg;
    while (DEF_TRUE)
    {
        /*modbus ͨ��50ms ͨѶһ��*/
        //��pc��ͨ�ŷ�ʽѡ��
        if (!ComminiStyle)
        {
            //�����жϲ�ѯ
            if(WEBxintiao > 60 )//2����û��������  �ر���������
            {
                WEBxintiao = 0;
                Write_W5100(W5100_S0_CR, S_CR_CLOSE);		/* �رն˿ڣ��ȴ����´����� */
                S0_State=0;
                Close_Flg =1;
            }
            W5100_Interrupt_Process();
            if(Close_Flg == 1)
            {
                Socket_Listen(0);
                Close_Flg=0;
            }
            if(S0_State == S_CONN)   //���ӳɹ���־
            {
                S0_State = 0;
            }
            if((S0_Data&S_RECEIVE)==S_RECEIVE)
            {
                S0_Data = 0;			//����λ��־
                LastResDataLen = S_rx_process(0);
                xMBPortEventPost( EV_FRAME_RECEIVED );
            }
        }
        eMBPoll();
        OSTimeDlyHMSM(0, 0, 0,10);
    }
}


/*********************************************************************************************************
*                                            AppTask_Kbd()  //��������
*
* Description : Monitor the state of the push buttons and passes messages to AppTaskUserIF()
*
* Argument(s) : p_arg
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************/
static  void  AppTask_Kbd (void *p_arg)
{
    CPU_INT08U   err;
    (void)p_arg;
    while (DEF_TRUE)
    {
        OSMutexPend(OSLcdUsedMutex,0,&err);
        OSMutexPend(OSI2CUsedMutex,0,&err);
        PorcKey(1);
        OSMutexPost(OSI2CUsedMutex);
        OSMutexPost(OSLcdUsedMutex);
        OSTimeDly(10);
    }
}


/*********************************************************************************************************
*                                            AppTask_Meas()
*
* Description : 	 ��������
*
* Argument(s) : p_arg
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************/
static  void  AppTask_Meas (void *p_arg)
{
    uint8 err;
    (void)p_arg;
    while (DEF_TRUE)
    {
        switch(MeasureStepFlg)
        {
        case 0:  //�Լ켰��ʼ��
            //��������
            if(StopFlg == 0)   //������
            {
                //�ر�
                OSMutexPend(OSUart0UsedMutex,0,&err);
                PumpFlg = 1;
                TTLOutput (TTL_0,PumpFlg,TTL_PUMP);	 //��
                OSMutexPost(OSUart0UsedMutex);

                //����Դ��λ
                OSMutexPend(OSUart0UsedMutex,0,&err);
                TTLOutput (TTL_2,0,TTL_BRAY_Ctrl1);	 //��
                TTLOutput (TTL_2,1,TTL_BRAY_Ctrl2);	 //��
                OSMutexPost(OSUart0UsedMutex);
                ClampRadio = 1;
                OSTimeDlyHMSM(0, 0, C14MotMoveTime,0);	//����ʱ��

                OSMutexPend(OSUart0UsedMutex,0,&err);
                TTLOutput (TTL_2,0,TTL_BRAY_Ctrl1);	 //��
                TTLOutput (TTL_2,0,TTL_BRAY_Ctrl2);	 //��
                OSMutexPost(OSUart0UsedMutex);

                while((TimeType==1 )&&(TimeDate.minute != 0)&&(StopFlg == 0))   //���������ģʽ����ȴ�ϵͳʱ�ӷ�����Ϊ0
                {
                    OSTimeDlyHMSM(0, 0, 10,0);
                }
                //��ֽ
                if(StopFlg == 0)
                {
                    RunPaperStep();
                }
            }
            (!StopFlg)?(MeasureStepFlg++):(MeasureStepFlg = 10);
            break;
        case 1://��հ� �����ظ�����
            showFlg = 0;//������ֵ����������ʾ��
            if(RunStyleFlg == SHISHI)
            {
                //��������8��  ��հ�
                OSMutexPend(OSUart0UsedMutex,0,&err);
                PumpFlg = 0;
                TTLOutput (TTL_0,PumpFlg,TTL_PUMP);	 //��
                FlowOfSamp_B_Set =  CaliParF1*(QiYa*(FlowOfSamp) * (273.15) )/((273.15+TDAIDIN)*101.325);
                DAMCSOutPutSetOne(DA_PUMP,(5.0*FlowOfSamp_B_Set/20.0));
                DAMCSOutPutAll();
                OSMutexPost(OSUart0UsedMutex);
                TimeMeasCount =8 ;	  //���ȶ�
                while((StopFlg==0)&&TimeMeasCount)
                {
                    OSTimeDly(100);//0.5��;
                }
                FujiStartFlg =1;
                MeasureStep();
                (!StopFlg)?(MeasureStepFlg =2):(MeasureStepFlg = 10);
                if(measRecord.I<Mass)//����С���趨ֵ��Ҫ��ֽ��
                {
                    MeasureStepFlg = 4;
                    OSMutexPend(OSUart0UsedMutex,0,&err);
                    PumpFlg = 1;//�رձ�
                    TTLOutput (TTL_0,PumpFlg,TTL_PUMP);	 //��
                    OSMutexPost(OSUart0UsedMutex);
                    FujiStartFlg =0;
                    TV_GK =0 ;
                    TV_BK =0	;
                }
            }
            else if(RunStyleFlg == ZHOUQI)
            {
                MeasureStep();
                (!StopFlg)?(MeasureStepFlg++):(MeasureStepFlg = 10);
            }
            break;
        case 2 :  //����
            FujiStep();
            (!StopFlg)?(MeasureStepFlg++):(MeasureStepFlg = 10);
            break;
        case 3:	//����Ʒ
            MeasureStep();
            (!StopFlg)?(MeasureStepFlg++):(MeasureStepFlg = 10);
            break;
        case 4://��ֽ ������ ̧��ѹ�Ƶ���� ��ֽx���ף�����ѹֽ�����
            OSTimeDlyHMSM(0, 0, 15,50);      //  ��ֹ�ڷ�����ģʽʱֱ�Ӳ����հ���
            (StopFlg)?(MeasureStepFlg++):(MeasureStepFlg = 10);
            if(MeasureStepFlg>4)
            {
                MeasureStepFlg = 0;
            }
            break;
        default:
            MeasureStepFlg = 10;
            OSTimeDlyHMSM(0, 0, 0,50);
            break;
        }
    }
}


/*********************************************************************************************************
*                                            AppTask_HistoryMemoryRead()
*
* Description : ��ʷ���ݶ�ȡ��Ӧ����
*
* Argument(s) : p_arg       .
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************/

static  void  AppTask_HistoryMemoryRead (void *p_arg)
{
    int16 bufHold;
    CPU_INT08U   err = err;
    (void)p_arg;
    while (DEF_TRUE)
    {
        NewLcdVol(MenuId);
        //�˴�������λ���ϴ�flg�ж� �����ﲻ��̫����
        if(OnlineFlg == 1)
        {
            bufHold	= usRegHoldingBuf[1];
            if(bufHold)//Сʱ�����ϴ�
            {
                Wait_Flg ++;
                usRegHoldingBuf[0] = 0x55;	  //æ
                ReadHistory();
                usRegHoldingBuf[0] = 0;
                usRegHoldingBuf[1] = 0;
                CUpdateFlg = 0;
                WEBxintiao = 0;//��⵽���� ����������
            }
            bufHold = usRegHoldingBuf[2];	//��������
            if(bufHold)
            {
                Wait_Flg ++;
                usRegHoldingBuf[0] = 0x55;	  //æ
                ReadHistoryM();
                usRegHoldingBuf[0] = 0;
                usRegHoldingBuf[2] = 0;
                WEBxintiao = 0;//��⵽���� ����������
            }
            bufHold = usRegHoldingBuf[3];	//ϵͳ��־����	�����
            if(bufHold)
            {
                Wait_Flg ++;
                usRegHoldingBuf[0] = 0x55;	  //æ
                ReadHistorySys();
                usRegHoldingBuf[0] = 0;
                usRegHoldingBuf[3] = 0;
                WEBxintiao = 0;//��⵽���� ����������
            }
            bufHold = usRegHoldingBuf[4];	//��ʷ����
            if(bufHold)
            {
                Wait_Flg ++;
                usRegHoldingBuf[0] = 0x55;	  //æ
                ReadHistoryDat();
                usRegHoldingBuf[0] = 0;
                usRegHoldingBuf[4] = 0;
                WEBxintiao = 0;//��⵽���� ����������
            }
            //ϵͳ��־���Ӷ�
            bufHold = usRegHoldingBuf[5];	//ϵͳ��־����	���Ӷ�
            if(bufHold)
            {
                Wait_Flg ++;
                usRegHoldingBuf[0] = 0x55;	  //æ
                ReadHistorySysMin();
                usRegHoldingBuf[0] = 0;
                usRegHoldingBuf[5] = 0;
                SysrecordReadFlg=0;
                WEBxintiao = 0;//��⵽���� ����������
            }
        }
        OSTimeDlyHMSM(0, 0, 0,50);
    }
}

/*
*********************************************************************************************************
*                                            App_TaskBattery()
*
* Description : 	 ���ݲɼ����¶ȿ���
*
* Argument(s) : p_arg       .
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************/

static  void  AppTask_DataGather_PineTempCtrl (void *p_arg)
{
    CPU_INT08U   err,i;
    (void)p_arg;
    while (DEF_TRUE)
    {
        if(TimeCount1s)//rtc��ʱһ�� �ڲ���������1���� ����
        {
            TimeCount1s = 0;
            if(CountStartFlg == 1)
            {
                OSMutexPend(OSUart0UsedMutex,0,&err);
                CountRead();
                OSMutexPost(OSUart0UsedMutex);
            }
            i++;
            if(i>2)
            {
                i = 0;
                if((Wait_Flg/2)<0.5)//���������ϴ�����ͣFPGA������֤�ϴ�������ȷ��
                {
                    NomalMeasure();
                }
                else
                {
                    Wait_Flg = Wait_Flg/2.0;
                }
            }
        }
        OSTimeDlyHMSM(0, 0, 0,50);
    }
}
/*********************************************************************************************************
*                                            AppTask_DTU()  //DTU����
*
* Description : Monitor the state of the push buttons and passes messages to AppTaskUserIF()
*
* Argument(s) : p_arg
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************/
static  void  AppTask_DTU (void *p_arg)
{
    CPU_INT08U   err = err;
    (void)p_arg;
    while (DEF_TRUE)
    {
        static uint16 datbuf[16];
        OSReadHoldReg(1, 0x9c46, 6,datbuf);		//40006
        TSP_GS_M 	= 	TSP_GS_k * (datbuf[0]<<8 | datbuf[1])/10.0;//40006
        PM25_GS_M 	= 	PM25_GS_k * (datbuf[2]<<8 | datbuf[3])/10.0;//40008
        PM10_GS_M 	= 	PM10_GS_k * (datbuf[4]<<8 | datbuf[5])/10.0;//40010
        TSP_GS_H  	+= 	TSP_GS_M;
        PM25_GS_H 	+= 	PM25_GS_M;
        PM10_GS_H 	+= 	PM10_GS_M;
        GS_Cnt	++;
        OSTimeDlyHMSM(0, 0, 1, 0);

        OSReadHoldReg(1, 0x9c5f, 6,datbuf);		//40031
        GS_Cycle 	= datbuf[0]	;//40031
        //PM25_GS_k	= datbuf[3]	;//40034
        //TSP_GS_k 	= datbuf[4] ;//40035
        //PM10_GS_k 	= datbuf[5] ;//40036
        OSTimeDlyHMSM(0, 0, 1, 0);

        //SendStates();
        OSTimeDlyHMSM(0, 0, 1, 0);

        //SendResult();
        OSTimeDlyHMSM(0, 0, 56, 0);
    }
}

/*********************************************************************************************************
*                                            AppTask_SHELL()  //SHELL����
*
* Description : Monitor the state of the push buttons and passes messages to AppTaskUserIF()
*
* Argument(s) : p_arg
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************/
static  void  AppTask_SHELL (void *p_arg)
{
    CPU_INT08U   err = err;
    static int i = 0;
    char ubuf[20];
    (void)p_arg;

    while (DEF_TRUE)
    {
        i++;
        if(i%2 == 0)
        {
            SET_LED3(0);
        }
        else
        {
            SET_LED3(1);
        }

        //Send_test();
       // getShishiLiuliang();
		

        /* ����һ���ַ��� */
        //Ϊ����ͨ��
        //scanf("%s", ubuf);

        /* ��ʾ������ַ��� */
        //for(i=0;i<180;i++)
        //printf("%f\n", Cnt_Record[i].I);

        OSTimeDlyHMSM(0, 0, 0, 200);
    }
}
/*********************************************************************************************************
*                                            OSModbusServe()  //������Modbus������������
*
* Description : Monitor the state of the push buttons and passes messages to AppTaskUserIF()
*
* Argument(s) : p_arg
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************/

extern void OSModbusServe	(void *pdata)
{
    (void)pdata;
    while (1)
    {
        IdleModbus();
    }
}
/*********************************************************************************************************
*                        AppTask_Refresh_Lcd_AnalogOutput()  //LCD��ҳ����ˢ�¼�ģ�����ˢ��
*
* Description : Monitor the state of the push buttons and passes messages to AppTaskUserIF()
*
* Argument(s) : p_arg
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************/


static  void  AppTask_Refresh_Lcd_AnalogOutput(void 		*p_arg) /*LCD��ҳ����ˢ�¼�ģ�����ˢ��*/
{
    CPU_INT08U   err;
    (void)p_arg;
    while (1)
    {
        OSTimeDlyHMSM(0, 0, 1,0);
        if((Wait_Flg/2)<0.5)//���������ϴ�����ͣFPGA������֤�ϴ�������ȷ��
        {
            OSMutexPend(OSLcdUsedMutex,1,&err);
            Menm0Updata();
            OSMutexPost(OSLcdUsedMutex);
//			outputstruct1.RH 			= 		HumiditySample;
//			outputstruct1.flow1 		= 		LiuLiang_GK;
//			outputstruct1.flow2 		= 		LiuLiang_BK;
//			outputstruct1.temperature1 	= 		WenDuT1;
//			outputstruct1.temperature2 	= 		WenDuT2;
//			outputstruct1.temperature3 	= 		WenDuT3;
//			OutputCONC(outputstruct1);
            OSTimeDlyHMSM(0, 0, 1,0);
//			DigitalOutput();
        }
    }
}

#ifdef  DEBUG
/*******************************************************************************
* @brief		Reports the name of the source file and the source line number
* 				where the CHECK_PARAM error has occurred.
* @param[in]	file Pointer to the source file name
* @param[in]    line assert_param error line source number
* @return		None
*******************************************************************************/
void check_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while(1);
}
#endif

/*
 * @}
 */
/*
*********************************************************************************************************
*********************************************************************************************************
*                                          uC/OS-II APP HOOKS
*********************************************************************************************************
*********************************************************************************************************
*/
/*
		���� UCOSII �ӿں���      ��Ϊ��  û���õ�
*/
#if (OS_APP_HOOKS_EN > 0)
/*
*********************************************************************************************************
*                                      TASK CREATION HOOK (APPLICATION)
*
* Description : This function is called when a task is created.
*
* Argument(s) : ptcb   is a pointer to the task control block of the task being created.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  App_TaskCreateHook (OS_TCB *ptcb)
{
#if ((APP_OS_PROBE_EN   == DEF_ENABLED) && \
     (OS_PROBE_HOOKS_EN == DEF_ENABLED))
    OSProbe_TaskCreateHook(ptcb);
#endif
}

/*
*********************************************************************************************************
*                                    TASK DELETION HOOK (APPLICATION)
*
* Description : This function is called when a task is deleted.
*
* Argument(s) : ptcb   is a pointer to the task control block of the task being deleted.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  App_TaskDelHook (OS_TCB *ptcb)
{
    (void)ptcb;
}

/*
*********************************************************************************************************
*                                      IDLE TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskIdleHook(), which is called by the idle task.  This hook
*               has been added to allow you to do such things as STOP the CPU to conserve power.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts are enabled during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 251
void  App_TaskIdleHook (void)
{
}
#endif


/*
*********************************************************************************************************
*                                            TASK RETURN HOOK
*
* Description: This function is called if a task accidentally returns.  In other words, a task should
*              either be an infinite loop or delete itself when done.
*
* Arguments  : ptcb      is a pointer to the task control block of the task that is returning.
*
* Note(s)    : none
*********************************************************************************************************
*/
#if OS_VERSION >= 289
void  OSTaskReturnHook (OS_TCB  *ptcb)
{
    (void)ptcb;
}
#endif
/*
*********************************************************************************************************
*                                        STATISTIC TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskStatHook(), which is called every second by uC/OS-II's
*               statistics task.  This allows your application to add functionality to the statistics task.
*
* Argument(s) : none.
*********************************************************************************************************
*/

void  App_TaskStatHook (void)
{
}

/*
*********************************************************************************************************
*                                        TASK SWITCH HOOK (APPLICATION)
*
* Description : This function is called when a task switch is performed.  This allows you to perform other
*               operations during a context switch.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*
*               (2) It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                   will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the
*                  task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/

#if OS_TASK_SW_HOOK_EN > 0
void  App_TaskSwHook (void)
{
#if ((APP_OS_PROBE_EN   == DEF_ENABLED) && \
     (OS_PROBE_HOOKS_EN == DEF_ENABLED))
    OSProbe_TaskSwHook();
#endif
}
#endif

/*
*********************************************************************************************************
*                                     OS_TCBInit() HOOK (APPLICATION)
*
* Description : This function is called by OSTCBInitHook(), which is called by OS_TCBInit() after setting
*               up most of the TCB.
*
* Argument(s) : ptcb    is a pointer to the TCB of the task being created.
*
* Note(s)     : (1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 204
void  App_TCBInitHook (OS_TCB *ptcb)
{
    (void)ptcb;
}
#endif

/*
*********************************************************************************************************
*                                        TICK HOOK (APPLICATION)
*
* Description : This function is called every tick.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_TIME_TICK_HOOK_EN > 0
void  App_TimeTickHook (void)
{
#if ((APP_OS_PROBE_EN   == DEF_ENABLED) && \
     (OS_PROBE_HOOKS_EN == DEF_ENABLED))
    OSProbe_TickHook();
#endif
}
#endif
#endif