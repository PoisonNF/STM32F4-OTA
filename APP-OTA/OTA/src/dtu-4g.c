#include "main.h"
#include "stdio.h"
#include "usart.h"
#include "string.h"
#include "dtu-4g.h"
#include "mqtt.h"

/* 定义一个串口DMA数据接收结构体 */
DTU_USART_INFO_T dtu_usart_info = {
    .usDMARxMAXSize = 500,
    .ucDMARxCplt = 0,
}; 

/**
 * @brief DTU进入指令模式
 * 
 */
void DTU_Enter_CMD(void)
{
    u1_printf("进入指令模式...\r\n");
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
 * @brief DTU向服务器发送数据
 * 
 * @param data 数据指针
 * @param datalen 数据长度
 * @return uint8_t 
 */
uint8_t DTU_SendData(uint8_t *data,uint16_t datalen)
{
    HAL_StatusTypeDef ret;
    ret = HAL_UART_Transmit(&DTU_USART,data,datalen,0xffff);
    if(ret == HAL_OK)   return 1;   //发送成功 
    else return 0;                  //发送失败
}

/**
 * @brief DTU设置远程服务器
 * 
 */
void DTU_Set_Server(void)
{
    u3_printf("AT+SOCKA=%s\r\n",SERVER_CONFIG);     //设置服务器地址和端口
    HAL_Delay(30);

    u3_printf("AT+SOCKAEN=ON\r\n");                 //开启SOCKA
    HAL_Delay(30);

    u3_printf("AT+SOCKBEN=OFF\r\n");                //关闭SOCKB
    HAL_Delay(30);

    u3_printf("AT+SOCKCEN=OFF\r\n");                //关闭SOCKC
    HAL_Delay(30);
    
    u3_printf("AT+SOCKDEN=OFF\r\n");                //关闭SOCKD
    HAL_Delay(30);

    u3_printf("AT+HEART=ON,NET,USER,60,C000\r\n");  //设置心跳
    HAL_Delay(30);

    u3_printf("AT+S\r\n");                          //保存配置
    HAL_Delay(30);
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
        u1_printf("进入指令模式，设置服务器...\r\n");

        //服务器配置
        DTU_Set_Server();

        //退出CMD模式
        DTU_Exit_CMD();
    }

    //退出指令模式的应答
    else if((datalen == 15) && !memcmp(data,"AT+ENTM\r\r\nOK\r\n",sizeof("AT+ENTM\r\r\nOK\r\n")))
    {
        u1_printf("退出指令模式!\r\n");
    }

    //配置服务器完成
    else if(!memcmp(data,"USR-DR152",sizeof("USR-DR152")))
    {
        u1_printf("Connect服务器...\r\n");
        MQTT_ConnectPack();
    }

    //接收到20 02 00 00代表Connect成功
    else if((datalen == 4) && (data[0] == 0x20) && (data[1] == 0x02) && (data[2] == 0x00) && (data[3] == 0x00))
    {
        u1_printf("Connect服务器成功!\r\n");
    }

    //接收到d0 00 代表设备保活
    else if((datalen == 2) && (data[0] == 0xd0) && (data[1] == 0x00))
    {
        u1_printf("设备存活!\r\n");
    }
}

