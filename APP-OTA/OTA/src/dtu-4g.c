#include "main.h"
#include "stdio.h"
#include "usart.h"
#include "string.h"
#include "dtu-4g.h"
#include "mqtt.h"

/* 定义一个串口DMA数据接收结构体 */
DTU_USART_INFO_T dtu_usart_info = {
    .usDMARxMAXSize = 256,
    .ucDMARxCplt = 0,
}; 

/**
 * @brief DTU进入指令模式
 * 
 */
void DTU_Enter_CMD(void)
{
    while(1)
    {
        u3_printf("+++");
        HAL_Delay(1000);

        /* 等待DTU回应'a' */
        if(dtu_usart_info.ucDMARxCplt)
		{
			dtu_usart_info.ucDMARxCplt = 0;	//标志位清零
            if(dtu_usart_info.ucpDMARxCache[0] == 'a')
            {
                u1_printf("退出透传模式\r\n");
                u3_printf("a"); //回复DTU
                break;
            }
        }   
    }
}

/**
 * @brief DTU退出指令模式
 * 
 */
void DTU_Exit_CMD(void)
{
    u3_printf("AT+ENTM\r\n");
}

/**
 * @brief 处理DTU的串口得到的数据
 * 
 * @param data 数据指针
 * @param datalen 数据长度
 */
void DTU_Usart_Event(uint8_t *data,uint16_t datalen)
{
    //进入指令模式的应答
    if((datalen == 5) && !memcmp(data,"+ok\r\n",sizeof("+ok\r\n")))
    {
        u1_printf("进入指令模式，设置服务器\r\n");

        //服务器配置

        //退出CMD模式
        DTU_Exit_CMD();
    }

    //退出指令模式的应答
    else if((datalen == 15) && !memcmp(data,"AT+ENTM\r\n\r\nOK\r\n",sizeof("AT+ENTM\r\n\r\nOK\r\n")))
    {
        u1_printf("退出指令模式\r\n");
    }
}

