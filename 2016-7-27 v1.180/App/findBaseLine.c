/****************************************Copyright (c)****************************************************
**                            		skyray-instrument Co.,LTD.
**
**                                 http://www.skyray-instrument.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           findBaseLine.c
** Last modified Date:  2011-02-24
** Last Version:        V1.0
** Descriptions:
**
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "findBaseLine.h"
#define MEASURE_WINDOWS_WIDTH	500
/*
�㼯������x����
�㼯������y����
�㼯�����һ��x����
�㼯�����һ��y����
�㼯��y������Сֵ
�㼯��y�������ֵ
*/
static DATA_TYPE x_centor = 0;
static DATA_TYPE y_centor = 0;
static DATA_TYPE x_end = 0;
static DATA_TYPE y_end = 0;
static DATA_TYPE y_min = 0;
static DATA_TYPE y_max = 0;

/*
͹��������((y_max - y_min) / 10)
͹��������(x_end - x_centor)* (x_end - x_centor)
����͹�������y����
͹������x����
͹������y����
*/
static fp32 convex_func_F = 0;
static fp32 convex_func_N = 0;
static DATA_TYPE add_convex_y[MEASURE_WINDOWS_WIDTH] = {0};
static DATA_TYPE convex_x[MEASURE_WINDOWS_WIDTH] = {0};
static DATA_TYPE convex_y[MEASURE_WINDOWS_WIDTH] = {0};

static DATA_TYPE x_data[MEASURE_WINDOWS_WIDTH] = {0};
static uint16 length;

#define	 FACTOR		3.5          //����͹����Ӱ������

#define  CONVEX_FUNC_VAL(x) (convex_func_F * abs(((x) - x_centor) * ((x) - x_centor)) / convex_func_N)

/**********************************************
func:����data,Ѱ����ֵ������������Сֵ
input��data:����Դ ��length:���ݵ�����
output:min����Сֵ��max:���ֵ
return:0,�޴���-1������
**********************************************/
int find_extrernes(DATA_TYPE *data,uint16 length,DATA_TYPE *min,DATA_TYPE *max)
{
    uint16 i;

    if (NULL == data ||
            NULL == min || NULL == max)
    {
        Print_Debug("�����ָ��!\n");
        return -1;
    }

    if (length <= 0)
    {
        Print_Debug("���ݳ��ȴ���0!\n");
        return -1;
    }

    *min = *max = data[0];
    for (i = 1; i < length; i ++)
    {
        if (data[i] > *max)
        {
            *max = data[i];
        }

        if (data[i] < *min)
        {
            *min = data[i];
        }
    }
    return 0;
}


/************************************************************
func:���ݺ��������ֱ�ߣ�ʹ�����һ����ԭʼ��������ͬ�����ĵ�
input��x_data,���������x���ꡣy_data,���������y���ꡣ
		length,��������ݳ��ȡ�fill_length����Ҫ���ĳ���
		fill_x_data��������ݵ�x����
output:fill_y_data��������ݵ�y����
return:0,�޴���-1������
************************************************************/
int8 fill_line(const DATA_TYPE *x_data,const DATA_TYPE *y_data,uint16 length,
               const DATA_TYPE *fill_x_data,uint16 fill_length,DATA_TYPE *fill_y_data)
{
    // ֱ�ߵ�б��
    fp32 line_k;
    // ֱ�ߵĽؾ�
    fp32 line_b;
    uint16 i = 0,j = 0;

    if (NULL == x_data || NULL == y_data ||
            NULL == fill_x_data || NULL == fill_y_data)
    {
        Print_Debug("�����ָ��!\n");
        return -1;
    }

    if (length <= 0)
    {
        Print_Debug("���ݳ��ȴ���0!\n");
        return -1;
    }


    for (i = 0; i < fill_length; i ++)
    {
        for (j = 1; j < length - 1 ; j ++)
        {
            if ((fill_x_data[i] >= x_data[j - 1]) &&
                    (fill_x_data[i] <= x_data[j]))
            {
                //�ҵ�б��
                if (x_data[j - 1] != x_data[j])
                {
                    line_k = ((fp32)(y_data[j - 1] - y_data[j])) / (x_data[j - 1] - x_data[j]);
                }
                else
                {
                    line_k = 0;
                }
                line_b = y_data[j - 1] - line_k * x_data[j - 1];

                fill_y_data[i] = line_k * fill_x_data[i] + line_b;
                break;
            }
        }
        //���һ����ֱ�Ӻ�͹�����һ�������
        if (j == length - 1)
        {
            if (x_data[j - 1] != x_data[j])
            {
                line_k = ((fp32)(y_data[j - 1] - y_data[j])) / (x_data[j - 1] - x_data[j]);
            }
            else
            {
                line_k = 0;
            }
            line_b = y_data[j - 1] - line_k * x_data[j - 1];

            fill_y_data[i] = line_k * fill_x_data[i] + line_b;
//			fill_y_data[i] = fill_y_data[i - 1];
//			fill_y_data[i] = y_data[j];
        }
    }
    return 0;
}

/**********************************************
func:�ж���������Ƿ�Ϊ���
input��DATA_TYPE first_x,DATA_TYPE first_y,
		DATA_TYPE second_x,DATA_TYPE second_y,
		DATA_TYPE third_x,DATA_TYPE third_y
output:void
return:false,�ҹա�true�����
**********************************************/
bool is_turn_left(DATA_TYPE first_x,DATA_TYPE first_y,
                  DATA_TYPE second_x,DATA_TYPE second_y,
                  DATA_TYPE third_x,DATA_TYPE third_y)
{
    fp32 judge = (fp32)(second_x -  first_x) * (third_y - first_y) -
                 (third_x - first_x) * (second_y - first_y);
    return (judge > 0) ? true : false;
}

/**********************************************
func:����x_data,y_data,Ѱ�����ߵ���͹��
input��x_data����������ꡣy_data:����y����
	   length:���������
output:down_convex_x��͹����x���꣬down_convex_y��͹����y����
		convex_cnt,��͹�����ݸ���
return:0,�޴���-1������
**********************************************/
int8 get_down_convex(int16 *x_data,int16 *y_data,uint16 length,
                     int16 *down_convex_x,int16 *down_convex_y,uint16 *convex_cnt)
{
    int i = 0;
    if (NULL == x_data || NULL == y_data || NULL == down_convex_x ||
            NULL == down_convex_y || NULL == convex_cnt)
    {
        Print_Debug("�����ָ��!\n");
        return -1;
    }
    if (length <= 0)
    {
        Print_Debug("���ݳ��ȴ���0!\n");
        return -1;
    }
    *convex_cnt = 0;

    down_convex_x[0] = x_data[0];
    down_convex_x[1] = x_data[1];
    down_convex_y[0] = y_data[0];
    down_convex_y[1] = y_data[1];
    *convex_cnt = 2;

    for (i = 2; i < length; i ++)
    {
        down_convex_x[*convex_cnt] = x_data[i];
        down_convex_y[*convex_cnt] = y_data[i];
        (*convex_cnt) ++;

        while ((*convex_cnt) >= 3)
        {
            if (!is_turn_left(down_convex_x[(*convex_cnt) - 3],
                              down_convex_y[(*convex_cnt) - 3],
                              down_convex_x[(*convex_cnt) - 2],
                              down_convex_y[(*convex_cnt) - 2],
                              down_convex_x[(*convex_cnt) - 1],
                              down_convex_y[(*convex_cnt) - 1]))
            {
                down_convex_x[(*convex_cnt) - 2] = down_convex_x[(*convex_cnt) - 1];
                down_convex_y[(*convex_cnt) - 2] = down_convex_y[(*convex_cnt) - 1];
                (*convex_cnt) --;
            }
            else
            {
                break;
            }
        }
    }
    return 0;
}

/**********************************************
func:����x_data,y_data,Ѱ�����ߵĻ���
input��x_data����������ꡣy_data:����y����
	   length:���������loop_cnt��ѭ������
output:y_baseline�����ߵ�y����
return:0,�޴���-1������
remarks����������ݱ����ǰ��պ�����˳�����еĵ�
**********************************************/
int8 find_baseline(DATA_TYPE *y_data,uint8 loop_cnt)
{
//	int16 x_Data[500];
//	DATA_TYPE xLength;

    //��ʱ����
//	DATA_TYPE tmp_y[10];
    // ͹�����ݳ���
    uint16 convex_length = 0;
    uint16 i = 0;

//	uint16 *y_baseline;

// 	if (NULL == y_data)
// 	{
//	 	Print_Debug("�����ָ��!\n");
//	 	return -1;
//	}
//  	if (length <= 0)
//  	{
//		Print_Debug("���ݳ��ȴ���0!\n");
//		return -1;
//	}
//	if (loop_cnt <= 0)
//	{
// 	 	Print_Debug("ѭ����������0!\n");
//  	 	return -1;
//  	}

//  	for (i = 0;i < length;i ++)
//  	{
//  		tmp_y[i] = y_data[i];
//	}

    length = GetXData(&runMenuParameter,x_data);

    if(length>MEASURE_WINDOWS_WIDTH)
    {
        return -1;
    }

    while (loop_cnt > 0)
    {
        //��ʼ��ȫ�ֱ���
        find_extrernes(y_data,length,&y_min,&y_max);
        x_centor = x_data[length / 2];
        y_centor = y_data[length / 2];
        x_end = x_data[length - 1];
        y_end = y_data[length - 1];
        convex_func_F = (y_max - y_min) / FACTOR;
        convex_func_N = abs((x_end - x_centor)* (x_end - x_centor));
        if( 0 == convex_func_N )
        {
            return -1;		//����Ϊ0��
        }
        // ��ԭʼ���߼���͹��������
        for (i = 0; i < length; i ++)
        {
            add_convex_y[i] = y_data[i] + CONVEX_FUNC_VAL(x_data[i]);
        }

        if (0 != get_down_convex(x_data,add_convex_y,length,convex_x,convex_y,&convex_length))
        {
            return -1;
        }
        if (0 != fill_line(convex_x,convex_y,convex_length,x_data,length,y_data))
        {
            return -1;
        }
        //�õ�������
        for (i = 0; i < length; i ++)
        {
            y_data[i] = add_convex_y[i] - y_data[i];
        }

        loop_cnt --;
//		if (loop_cnt > 0)
//		{
//			for (i = 0;i < length;i ++)
//			{
//				y_data[i] = y_baseline[i];
//			}
//		}
    }
    return 0;
}
/**********************************************
func:����x_data,y_data,���ߵ����
input��x_data����������ꡣy_data:����y����
	   length:���������
output:���ߵ����
return:0,��
remarks����������ݱ����ǰ��պ�����˳�����еĵ�
**********************************************/
fp32 GetCalculationArea(DATA_TYPE *y_data)
{
    uint16 i;
    uint16 chang,kuan;
    fp32 areaS;
    fp32 areaLine = 0;

    if ((NULL == y_data)||(NULL == x_data))
    {
        return -1;
    }

    length = GetXData(&runMenuParameter,x_data);

    for(i=0; i<length-1; i++)
    {
        chang = (y_data[i+1] + y_data[i])/2;
        kuan = x_data[i+1] - x_data[i];			//������step����

        areaS = (fp32)chang*kuan;

        areaLine += areaS;
    }

    return areaLine;
}

/**********************************************
func:����x_data,y_data,���ߵķ��
input��x_data����������ꡣy_data:����y����
	   length:���������
output:���ߵķ��
return:0,��
remarks����������ݱ����ǰ��պ�����˳�����еĵ�
**********************************************/
fp32 GetCalculationPeak(DATA_TYPE *y_data)
{
    uint16 i;
//	fp32 peak;
    fp32 I_max = 0;

    if ((NULL == y_data)||(NULL == x_data))
    {
        return -1;
    }

    length = GetXData(&runMenuParameter,x_data);

    for(i=0; i<length-3; i++)
    {
        if( y_data[i+2] > I_max )
        {
            I_max =y_data[i+2];
        }
    }

    return(I_max);
}
