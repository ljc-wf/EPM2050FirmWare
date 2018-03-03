

#include "TCPIP.H"

#define		SCK_W5100			(1 << 15)
#define		SO_W5100			(1 << 17)
#define		SI_W5100			(1 << 18)
#define		CE_W5100			(1 << 16)
#define		SET_SCK_W5100(x)	((x) ? GPIO_SetValue(0, SCK_W5100): GPIO_ClearValue(0, SCK_W5100))
#define		SET_SO_W5100(x)	((x) ? GPIO_SetValue(0, SO_W5100): GPIO_ClearValue(0, SO_W5100))
#define		SET_SI_W5100(x)	((x) ? GPIO_SetValue(0, SI_W5100): GPIO_ClearValue(0, SI_W5100))
#define		SET_CE_W5100(x)	((x) ? GPIO_SetValue(0, CE_W5100): GPIO_ClearValue(0, CE_W5100))

unsigned char Rx_Buffer[256];			/* �˿ڽ������ݻ����� */
unsigned char Tx_Buffer[256];			/* �˿ڷ������ݻ����� */



uint8 Close_Flg;
/* �˿����ݻ����� */
// unsigned char Rx_Buffer[W5100_BUFSIZE];			/* �˿ڽ������ݻ����� */
// unsigned char Tx_Buffer[W5100_BUFSIZE];			/* �˿ڷ������ݻ����� */

/* ��������Ĵ��� */
unsigned char Gateway_IP[4]= {192,168,0,1};	     		/* Gateway IP Address */
unsigned char Sub_Mask[4]= {255,255,255,0};				/* Subnet Mask */
unsigned char Phy_Addr[6]= {0x00,0x08,0xDC,0x01,0x02,0x03};  			/* Physical Address */
unsigned char IP_Addr[4]= {192,168,0,160};				/* Loacal IP Address */

unsigned char S0_Port[2]= {0x13,0x88}; 		    	/* Socket0 Port number 5000 */
unsigned char S0_DIP[4]= {192,168,0,10};				/* Socket0 Destination IP Address */
unsigned char S0_DPort[2]= {0x13,0x88}; 				/* Socket0 Destination Port number 5000*/


unsigned char S0_State=0;				/* Socket0 state recorder */

unsigned char S0_Data;					/* Socket0 receive data and transmit OK */
unsigned short LastResDataLen;
unsigned char W5100_Interrupt;

/* UDP Destionation IP address and Port number */
unsigned char UDP_DIPR[4];
unsigned char UDP_DPORT[2];

unsigned char BUF[]= {"���ӳɹ�"};

/****************������������*********************/
void static Delay__(unsigned int  x);
void DelayNS (INT32U  uiDly);
void MSPIInit(void);
void MSPISendData(uint8 uiData);
INT8U MSPIRcvByte (void);
unsigned char Read_W5100(unsigned short addr);
void Write_W5100(unsigned short addr, unsigned char dat);
void W5100_Init(void);
unsigned char Socket_Init(SOCKET s);
unsigned char Socket_Connect(SOCKET s);
unsigned char Socket_Listen(SOCKET s);
unsigned char Socket_UDP(SOCKET s);
unsigned short S_rx_process(SOCKET s);
unsigned char S_tx_process(uint8 s, unsigned int size);
void vPrintString(unsigned char *ucpString);
void W5100_Interrupt_Process(void);

//////////////////////////////////////////////////
static void Delay__(unsigned int  x)
{
    unsigned int i;
    for(i=0; i<x; i++)
    {
        ;
    }
}
/*********************************************************************************************************
** Function name:		DelayNS
** Descriptions:		��������ʱ
** input parameters:    uiDly��ʱ����ֵ��ֵԽ����ʱԽ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void DelayNS (INT32U  uiDly)
{
    INT32U  i;

    for(; uiDly > 0; uiDly--) {
        for(i = 0; i < 50000; i++);
    }
}




void SPIInit_w5100(void)
{
    PINSEL_CFG_Type PinCfg;
    __IO FlagStatus exitflag;
    PinCfg.Funcnum = 0;
    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = 0;
    PinCfg.Portnum = 0;
    PinCfg.Pinnum = 15;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = 16;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = 18;
    PINSEL_ConfigPin(&PinCfg);

    GPIO_SetDir(0, SCK_W5100|SI_W5100|CE_W5100, 1);
    GPIO_SetDir(0, SO_W5100, 0);
    SET_CE_W5100(1);
    SET_SCK_W5100(0);

}

void MSPISendData(uint8 dat)
{
    int32 i;
    SET_SCK_W5100(0);
    for(i=0; i<8; i++)
    {
        if(dat&0x80)
        {
            SET_SI_W5100(1);
        }
        else
        {
            SET_SI_W5100(0);
        }
        SET_SCK_W5100(1);

        dat <<=1  ;
        SET_SCK_W5100(0);

    }
}
uint8 MSPIRcvByte(void)
{
    uint8 i=0,in=0;
// 	for(i=0;i<8;i++)
// 	{
// 		in = in<<1;
// 		if(GPIO_ReadValue(0)&SO)
// 		{
// 			in |= 0x01;
// 		}
// 		SET_SCK(1);
// 		SET_SCK(0);
// 	}
    SET_SCK_W5100(0);

    for(i=0; i<8; i++)
    {
        SET_SCK_W5100(1);
        in<<=1;
        if(LPC_GPIO0->FIOPIN&SO_W5100)
        {
            in |= 0x01;
        }
        SET_SCK_W5100(0);
        DelayAD(2);
    }



    return in;
}


/*********************************************************************************************************
** Function name:		MSPISendData
** Descriptions:		SPI��ģʽ��������
** input parameters:    uiData�� ��Ҫ���͵�����
** output parameters:   ��
** Returned value:      S0SPDR�� ����ֵΪ��ȡ������
*********************************************************************************************************/


/*********************************************************************************************************
** Function name:		SPI_RcvByte
** Descriptions:		��SPI���߽���1�ֽ�����
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/



unsigned char Read_W5100(unsigned short addr)
{
    unsigned char i;
    SET_CE_W5100(0);

// 	FIO0CLR    = SLAVE_CS;
    /* ���Ͷ����� */
    MSPISendData(0x0f);

    /* ���͵�ַ */
    MSPISendData(addr/256);
    MSPISendData(addr);

    /* ��ȡ���� */
    i= MSPIRcvByte();
    SET_CE_W5100(1);

// 	FIO0SET    = SLAVE_CS;

    return i;
}

void Write_W5100(unsigned short addr, unsigned char dat)
{
// 	FIO0CLR    = SLAVE_CS;		/*  ƬѡSSP	*/
    SET_CE_W5100(0);

    /* ����д���� */
    MSPISendData(0xf0);

    /* ���͵�ַ */
    MSPISendData(addr/256);
    MSPISendData(addr);

    /* д������ */
    MSPISendData(dat);
    SET_CE_W5100(1);
//     FIO0SET    = SLAVE_CS;
}


/*------------------------------------------------------------------------------
						W5100��ʼ������
��ʹ��W5100֮ǰ����W5100��ʼ��
------------------------------------------------------------------------------*/
void W5100_Init(void)
{
    unsigned char i;

    Write_W5100(W5100_MODE,MODE_RST);		/*����λW5100*/

    Delay__(100);

    /*��������(Gateway)��IP��ַ��4�ֽ� */
    /*ʹ�����ؿ���ʹͨ��ͻ�������ľ��ޣ�ͨ�����ؿ��Է��ʵ��������������Internet*/
    for(i=0; i<4; i++)
        Write_W5100(W5100_GAR+i,Gateway_IP[i]);			/*Gateway_IPΪ4�ֽ�unsigned char����,�Լ�����*/
    for(i=0; i<4; i++)
        Gateway_IP[i]=Read_W5100(W5100_GAR+i);
    /*������������(MASK)ֵ��4�ֽڡ���������������������*/
    for(i=0; i<4; i++)
        Write_W5100(W5100_SUBR+i,Sub_Mask[i]);			/*SUB_MASKΪ4�ֽ�unsigned char����,�Լ�����*/

    /*����������ַ��6�ֽڣ�����Ψһ��ʶ�����豸��������ֵַ
    �õ�ֵַ��Ҫ��IEEE���룬����OUI�Ĺ涨��ǰ3���ֽ�Ϊ���̴��룬�������ֽ�Ϊ��Ʒ���
    ����Լ�����������ַ��ע���һ���ֽڱ���Ϊż��*/
    for(i=0; i<6; i++)
        Write_W5100(W5100_SHAR+i,Phy_Addr[i]);			/*PHY_ADDR6�ֽ�unsigned char����,�Լ�����*/

    /*���ñ�����IP��ַ��4���ֽ�
    ע�⣬����IP�����뱾��IP����ͬһ�����������򱾻����޷��ҵ�����*/
    for(i=0; i<4; i++)
        Write_W5100(W5100_SIPR+i,IP_Addr[i]);			/*IP_ADDRΪ4�ֽ�unsigned char����,�Լ�����*/

    /*���÷��ͻ������ͽ��ջ������Ĵ�С���ο�W5100�����ֲ�*/
    Write_W5100(W5100_RMSR,0x55);		/*Socket Rx memory size=2k*/
    Write_W5100(W5100_TMSR,0x55);		/*Socket Tx mempry size=2k*/

    /* ��������ʱ�䣬Ĭ��Ϊ2000(200ms) */
    Write_W5100(W5100_RTR,0x07);
    Write_W5100(W5100_RTR+1,0xd0);

    /* �������Դ�����Ĭ��Ϊ8�� */
    Write_W5100(W5100_RCR,8);

    /* �����жϣ��ο�W5100�����ֲ�ȷ���Լ���Ҫ���ж�����
    IMR_CONFLICT��IP��ַ��ͻ�쳣�ж�
    IMR_UNREACH��UDPͨ��ʱ����ַ�޷�������쳣�ж�
    ������Socket�¼��жϣ�������Ҫ���� */
    Write_W5100(W5100_IMR,(IMR_CONFLICT|IMR_UNREACH|IMR_S0_INT));
}

/******************************************************************************
						Socket��ʼ��
����ɹ��򷵻�true, ���򷵻�false��sΪ��ʼ���˿ں�
-----------------------------------------------------------------------------*/
unsigned char Socket_Init(SOCKET s)
{
    unsigned char i;

    Write_W5100((W5100_S0_MR+s*0x100),S_MR_TCP);/*����socket0ΪTCPģʽ*/

    Write_W5100((W5100_S0_CR+s*0x100),S_CR_OPEN);/*��socket0*/

    i=Read_W5100(W5100_S0_SSR+s*0x100);
    if(i!=S_SSR_INIT)
    {
        Write_W5100((W5100_S0_CR+s*0x100),S_CR_CLOSE);	/*�򿪲��ɹ����ر�Socket��Ȼ�󷵻�*/
        return FALSE;
    }

    /*������ؼ���ȡ���ص�������ַ*/

    for(i=0; i<4; i++)
        Write_W5100((W5100_S0_DIPR+s*0x100+i),IP_Addr[i]+1);	/*��Ŀ�ĵ�ַ�Ĵ���д���뱾��IP��ͬ��IPֵ*/

    Write_W5100((W5100_S0_CR+s*0x100),S_CR_CONNECT);		/*��socket0��TCP����*/

    Delay__(20);

    i=Read_W5100(W5100_S0_DHAR+s*0x100);	/*��ȡĿ��������������ַ���õ�ַ�������ص�ַ*/
    //������ַ��6λ������һλ

    Write_W5100((W5100_S0_CR+s*0x100),S_CR_CLOSE);/*�ر�socket0*/

    if(i==0xff)
    {
        /**********û���ҵ����ط���������û�������ط������ɹ�����***********/
        /**********              �Լ����Ӵ�������                ***********/
    }

    /*���÷�Ƭ���ȣ��ο�W5100�����ֲᣬ��ֵ���Բ��޸�*/
    Write_W5100((W5100_S0_MSS+s*0x100),0x00);		/*����Ƭ�ֽ���=16*/
    Write_W5100((W5100_S0_MSS+s*0x100+1),0x10);


    return TRUE;
}

/*-----------------------------------------------------------------------------
                           ����SocketΪ�ͻ�����Զ�̷���������
������Socket�����ڿͻ���ģʽʱ�����øó�����Զ�̷�������������
������óɹ��򷵻�true�����򷵻�false
����������Ӻ���ֳ�ʱ�жϣ��������������ʧ�ܣ���Ҫ���µ��øó�������
�ó���ÿ����һ�Σ��������������һ������
------------------------------------------------------------------------------*/
unsigned char Socket_Connect(SOCKET s)
{
    unsigned char i;

    Write_W5100((W5100_S0_MR+s*0x100),(S_MR_TCP|S_MR_MC));			/* ����SocketΪTCPģʽ */
    /* Ϊ�������Ӧ�ٶȣ���ND/MCλ��λ */

    Write_W5100((W5100_S0_PORT+s*0x100),S0_Port[0]);		/*���ñ���source�Ķ˿ں�*/
    Write_W5100((W5100_S0_PORT+s*0x100+1),S0_Port[1]);

    for(i=0; i<4; i++)										/*����Զ������IP��ַ������������IP��ַ*/
    {
        Write_W5100((W5100_S0_DIPR+s*0x100+i),S0_DIP[i]);	/*D_IP_ADDRΪ4�ֽ�unsigned char���飬�Լ�����*/
    }

    Write_W5100((W5100_S0_DPORT+s*0x100),S0_DPort[0]);		/*Socket��Ŀ�Ķ˿ں�*/
    Write_W5100((W5100_S0_DPORT+s*0x100+1),S0_DPort[0]);	/*DPORTΪunisgned �ͣ��Լ�����*/


    Write_W5100((W5100_S0_CR+s*0x100),S_CR_OPEN);			/*��Socket*/

    i=Read_W5100(W5100_S0_SSR+s*0x100);
    if(i!=S_SSR_INIT)
    {
        Write_W5100((W5100_S0_CR+s*0x100),S_CR_CLOSE);		/*�򿪲��ɹ����ر�Socket��Ȼ�󷵻�*/
        return FALSE;
    }
    Write_W5100((W5100_S0_CR+s*0x100),S_CR_CONNECT);		/*����SocketΪConnectģʽ*/

    return TRUE;

    /*���������Socket�Ĵ����ӹ������������Ƿ���Զ�̷������������ӣ�����Ҫ�ȴ�Socket�жϣ�
    ���ж�Socket�������Ƿ�ɹ����ο�W5100�����ֲ��Socket�ж�״̬*/
}
/*-----------------------------------------------------------------------------
                   ����Socket��Ϊ�������ȴ�Զ������������
������Socket�����ڷ�����ģʽʱ�����øó��򣬵ȵ�Զ������������
������óɹ��򷵻�true, ���򷵻�false
�ó���ֻ����һ�Σ���ʹW5100����Ϊ������ģʽ
-----------------------------------------------------------------------------*/
unsigned char Socket_Listen(SOCKET s)
{
    unsigned char i;

    Write_W5100((W5100_S0_MR+s*0x100),(S_MR_TCP|S_MR_MC));			/*����SocketΪTCPģʽ*/
    /* Ϊ�������Ӧ�ٶȣ���ND/MCλ��λ */

    Write_W5100((W5100_S0_PORT+s*0x100),S0_Port[0]);		/*���ñ���source�Ķ˿ں�*/
    Write_W5100((W5100_S0_PORT+s*0x100+1),S0_Port[1]);


    Write_W5100((W5100_S0_CR+s*0x100),S_CR_OPEN);			/*��Socket*/

    i=Read_W5100(W5100_S0_SSR+s*0x100);
    if(i!=S_SSR_INIT)
    {
        Write_W5100((W5100_S0_CR+s*0x100),S_CR_CLOSE);		/*�򿪲��ɹ����ر�Socket��Ȼ�󷵻�*/
        return FALSE;
    }


    Write_W5100((W5100_S0_CR+s*0x100), S_CR_LISTEN);		/*����SocketΪ����ģʽ*/

    if(Read_W5100(W5100_S0_SSR+s*0x100)!=S_SSR_LISTEN)
    {
        Write_W5100((W5100_S0_CR+s*0x100), S_CR_CLOSE);		/*���ò��ɹ����ر�Socket��Ȼ�󷵻�*/
        return FALSE;
    }

    return TRUE;

    /*���������Socket�Ĵ򿪺�������������������Զ�̿ͻ����Ƿ������������ӣ�����Ҫ�ȴ�Socket�жϣ�
    ���ж�Socket�������Ƿ�ɹ����ο�W5100�����ֲ��Socket�ж�״̬
    �ڷ���������ģʽ����Ҫ����Ŀ��IP��Ŀ�Ķ˿ں�*/
}
/*-----------------------------------------------------------------------------
					����SocketΪUDPģʽ
���Socket������UDPģʽ�����øó�����UDPģʽ�£�Socketͨ�Ų���Ҫ��������
������óɹ��򷵻�true, ���򷵻�false
�ó���ֻ����һ�Σ���ʹW5100����ΪUDPģʽ
-----------------------------------------------------------------------------*/
unsigned char Socket_UDP(SOCKET s)
{

    Write_W5100((W5100_S0_MR+s*0x100), S_MR_UDP);		/*����SocketΪUDPģʽ*/

    Write_W5100((W5100_S0_PORT+s*0x100),S0_Port[0]);		/*���ñ���source�Ķ˿ں�*/
    Write_W5100((W5100_S0_PORT+s*0x100+1),S0_Port[1]);

    Write_W5100((W5100_S0_CR+s*0x100), S_CR_OPEN);		/*��Socket*/

    if(Read_W5100(W5100_S0_SSR+s*0x100)!=S_SSR_UDP)
    {
        Write_W5100((W5100_S0_CR+s*0x100), S_CR_CLOSE);	/*�򿪲��ɹ����ر�Socket��Ȼ�󷵻�*/
        return FALSE;
    }
    else
        return TRUE;

    /*���������Socket�Ĵ򿪺�UDPģʽ���ã�������ģʽ��������Ҫ��Զ��������������
    ��ΪSocket����Ҫ�������ӣ������ڷ�������ǰ����������Ŀ������IP��Ŀ��Socket�Ķ˿ں�
    ���Ŀ������IP��Ŀ��Socket�Ķ˿ں��ǹ̶��ģ������й�����û�иı䣬��ôҲ��������������*/
}

/******************************************************************************
                              ����Socket���պͷ��͵�����
******************************************************************************/
/*-----------------------------------------------------------------------------
���Socket�����������ݵ��жϣ������øó�����д���
�ó���Socket�Ľ��յ������ݻ��浽Rx_buffer�����У������ؽ��յ������ֽ���
-----------------------------------------------------------------------------*/
unsigned short S_rx_process(SOCKET s)
{
    unsigned short i,j;
    unsigned short rx_size,rx_offset;

    /*��ȡ�������ݵ��ֽ���*/
    rx_size=Read_W5100(W5100_S0_RX_RSR+s*0x100);
    rx_size*=256;
    rx_size+=Read_W5100(W5100_S0_RX_RSR+s*0x100+1);

    /*��ȡ���ջ�������ƫ����*/
    rx_offset=Read_W5100(W5100_S0_RX_RR+s*0x100);
    rx_offset*=256;
    rx_offset+=Read_W5100(W5100_S0_RX_RR+s*0x100+1);

    i=rx_offset/S_RX_SIZE;				/*����ʵ�ʵ�����ƫ������S0_RX_SIZE��Ҫ��ǰ��#define�ж���*/
    /*ע��S_RX_SIZE��ֵ��W5100_Init()������W5100_RMSR��ȷ��*/
    rx_offset=rx_offset-i*S_RX_SIZE;

    j=W5100_RX+s*S_RX_SIZE+rx_offset;		/*ʵ��������ַΪW5100_RX+rx_offset*/
    for(i=0; i<rx_size; i++)
    {
        if(rx_offset>=S_RX_SIZE)
        {
            j=W5100_RX+s*S_RX_SIZE;
            rx_offset=0;
        }
        Rx_Buffer[i]=Read_W5100(j);		/*�����ݻ��浽Rx_buffer������*/
        j++;
        rx_offset++;
    }

    /*������һ��ƫ����*/
    rx_offset=Read_W5100(W5100_S0_RX_RR+s*0x100);
    rx_offset*=256;
    rx_offset+=Read_W5100(W5100_S0_RX_RR+s*0x100+1);

    rx_offset+=rx_size;
    Write_W5100((W5100_S0_RX_RR+s*0x100), (rx_offset/256));
    Write_W5100((W5100_S0_RX_RR+s*0x100+1), rx_offset);

    Write_W5100((W5100_S0_CR+s*0x100), S_CR_RECV);			/*����RECV����ȵ���һ�ν���*/

    return rx_size;								/*���ؽ��յ������ֽ���*/
}
/*-----------------------------------------------------------------------------
���Ҫͨ��Socket�������ݣ������øó���
Ҫ���͵����ݻ�����Tx_buffer��, size����Ҫ���͵��ֽڳ���
-----------------------------------------------------------------------------*/

unsigned char S_tx_process(SOCKET s, unsigned int size)
{
    unsigned short i,j;
    unsigned short tx_free_size,tx_offset;

    /*�����UDPģʽ,�����ڴ�����Ŀ��������IP�Ͷ˿ں�*/
    if((Read_W5100(W5100_S0_MR+s*0x100)&0x0f)==0x02)
    {
        for(i=0; i<4; i++)			/* ����Ŀ������IP*/
            Write_W5100((W5100_S0_DIPR+s*0x100+i), UDP_DIPR[i]);

        Write_W5100((W5100_S0_DPORT+s*0x100), UDP_DPORT[0]);
        Write_W5100((W5100_S0_DPORT+s*0x100+1), UDP_DPORT[1]);
    }

    /*��ȡ������ʣ��ĳ���*/
    tx_free_size=Read_W5100(W5100_S0_TX_FSR+s*0x100);
    tx_free_size*=256;
    tx_free_size+=Read_W5100(W5100_S0_TX_FSR+s*0x100+1);
    if(tx_free_size<size)						/*���ʣ����ֽڳ���С�ڷ����ֽڳ���,�򷵻�*/
        return FALSE;

    /*��ȡ���ͻ�������ƫ����*/
    tx_offset=Read_W5100(W5100_S0_TX_WR+s*0x100);
    tx_offset*=256;
    tx_offset+=Read_W5100(W5100_S0_TX_WR+s*0x100+1);

    i=tx_offset/S_TX_SIZE;					/*����ʵ�ʵ�����ƫ������S0_TX_SIZE��Ҫ��ǰ��#define�ж���*/
    /*ע��S0_TX_SIZE��ֵ��W5100_Init()������W5100_TMSR��ȷ��*/
    tx_offset=tx_offset-i*S_TX_SIZE;
    j=W5100_TX+s*S_TX_SIZE+tx_offset;		/*ʵ��������ַΪW5100_TX+tx_offset*/

    for(i=0; i<size; i++)
    {
        if(tx_offset>=S_TX_SIZE)
        {
            j=W5100_TX+s*S_TX_SIZE;
            tx_offset=0;
        }
        Write_W5100(j,Tx_Buffer[i]);						/*��Tx_buffer�������е�����д�뵽���ͻ�����*/
        j++;
        tx_offset++;
    }

    /*������һ�ε�ƫ����*/
    tx_offset=Read_W5100(W5100_S0_TX_WR+s*0x100);
    tx_offset*=256;
    tx_offset+=Read_W5100(W5100_S0_TX_WR+s*0x100+1);


    tx_offset+=size;
    Write_W5100((W5100_S0_TX_WR+s*0x100),(tx_offset/256));
    Write_W5100((W5100_S0_TX_WR+s*0x100+1),tx_offset);

    Write_W5100((W5100_S0_CR+s*0x100), S_CR_SEND);			/*����SEND����,��������*/

    return TRUE;								/*���سɹ�*/
}


void vPrintString(unsigned char *ucpString)
{
    unsigned int  i=0;
    while(ucpString[i]!=0x00)
    {
        Tx_Buffer[i]=ucpString[i];
        i++;
    }
    S_tx_process(0,i);
}

/******************************************************************************
					W5100�жϴ���������
******************************************************************************/
void W5100_Interrupt_Process(void)
{
    unsigned char i,j;

    W5100_Interrupt=0;

    i=Read_W5100(W5100_IR);
    Write_W5100(W5100_IR, (i&0xf0));					/*��д����жϱ�־*/

    if((i & IR_CONFLICT) == IR_CONFLICT)	 	/*IP��ַ��ͻ�쳣�������Լ����Ӵ���*/
    {

    }

    if((i & IR_UNREACH) == IR_UNREACH)			/*UDPģʽ�µ�ַ�޷������쳣�������Լ����Ӵ���*/
    {

    }

    /* Socket�¼����� */
    if((i & IR_S0_INT) == IR_S0_INT)
    {
        j=Read_W5100(W5100_S0_IR);
        Write_W5100(W5100_S0_IR, j);		/* ��д���жϱ�־ */

        if(j&S_IR_CON)				/* ��TCPģʽ��,Socket0�ɹ����� */
        {
            S0_State|=S_CONN;
        }
        if(j&S_IR_DISCON)				/* ��TCPģʽ��Socket�Ͽ����Ӵ������Լ����Ӵ��� */
        {
            Write_W5100(W5100_S0_CR, S_CR_CLOSE);		/* �رն˿ڣ��ȴ����´����� */
            S0_State=0;
            Close_Flg =1;
        }
        if(j&S_IR_SENDOK)				/* Socket0���ݷ�����ɣ������ٴ�����S_tx_process()������������ */
        {
            S0_Data|=S_TRANSMITOK;
        }
        if(j&S_IR_RECV)				/* Socket���յ����ݣ���������S_rx_process()���� */
        {
            S0_Data|=S_RECEIVE;
        }
        if(j&S_IR_TIMEOUT)			/* Socket���ӻ����ݴ��䳬ʱ���� */
        {
            Write_W5100(W5100_S0_CR, S_CR_CLOSE);		/* �رն˿ڣ��ȴ����´����� */
            S0_State=0;
            Close_Flg =1;
        }
    }

    /* Socket1�¼����� */
    if((i&IR_S1_INT)==IR_S1_INT)
    {
        j=Read_W5100(W5100_S1_IR);
        Write_W5100(W5100_S1_IR, j);		/* ��д���жϱ�־ */
    }

    /* Socket2�¼����� */
    if((i&IR_S2_INT)==IR_S2_INT)
    {
        j=Read_W5100(W5100_S2_IR);
        Write_W5100(W5100_S2_IR, j);		/*��д���жϱ�־ */

    }

    /* Socket3�¼����� */
    if((i&IR_S3_INT)==IR_S3_INT)
    {
        j=Read_W5100(W5100_S3_IR);
        Write_W5100(W5100_S3_IR, j);		/* ��д���жϱ�־ */
    }
}


/*********************************************************************************************************
**                            End Of File
**********************************************************************************************************/