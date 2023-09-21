#include "main.h"
#include "stdio.h"
#include "usart.h"
#include "string.h"
#include "dtu-4g.h"
#include "mqtt.h"

/* 用于标记是否连接上服务器 */
static uint8_t Connect_Status = 0; 

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
        if(usart_info.ucDMARxCplt)
		{
			usart_info.ucDMARxCplt = 0;	//标志位清零
            if(usart_info.ucpDMARxCache[0] == 'a')
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

    //接收到CONNACK报文
    else if((datalen == 4) && (data[0] == 0x20))
    {
        u1_printf("收到CONNACK!\r\n");
        if(data[3] == 0x00){
            u1_printf("Connect服务器成功!\r\n");
            Connect_Status= 1;      //连接成功标志位
            MQTT_SubscribePack("/k08lcwgm0Ts/MQTTtest/user/get");
            MQTT_SubscribePack("/ota/device/upgrade/k08lcwgm0Ts/MQTTtest");
            HAL_Delay(500);
            MQTT_SendOTAVersion();  //发送当前OTA的版本
        }else{
            u1_printf("Connect服务器失败!\r\n");
            NVIC_SystemReset();
        }
    }

    //接收到SUBACK报文
    else if(Connect_Status &&(datalen == 5) && (data[0] == 0x90))
    {
        u1_printf("收到SUBACK!\r\n");

        //如果最后一个字节为0x00或者0x01代表发送成功
        if((data[datalen-1] == 0x00) || (data[datalen-1] == 0x01)){
            u1_printf("Subscribe报文成功!\r\n");
            //MQTT_UnSubscribePack("/sys/k08lcwgm0Ts/MQTTtest/thing/service/property/set");
        }else{
            u1_printf("Subscribe报文失败!\r\n");
            NVIC_SystemReset();
        }
    }

    //接收到UNSUBACK报文
    else if(Connect_Status && (data[0] == 0xB0) && data[1] == 0x02)
    {
        u1_printf("收到UNSUBACK!\r\n");
        u1_printf("UnSubscribe报文成功!\r\n");
    }

    //接收到等级0的Publish报文
    else if(Connect_Status && (data[0] == 0x30))
    {
        u1_printf("收到等级0的Publish报文!\r\n");
        MQTT_DealPublishData(data,datalen);
        if(strstr((const char*)Aliyun_mqtt.CMD_buff,"{\"switch1\":0}"))
        {
            u1_printf("关闭开关\r\n");
            MQTT_PublishDataQos0("/k08lcwgm0Ts/MQTTtest/user/post","{\"params\":}{\"switch1\":0}");
        }
        if(strstr((const char*)Aliyun_mqtt.CMD_buff,"{\"switch1\":1}"))
        {
            u1_printf("打开开关\r\n");
            MQTT_PublishDataQos0("/k08lcwgm0Ts/MQTTtest/user/post","{\"params\":}{\"switch1\":1}");
        }

        MQTT_GetOTAInfo((char*)Aliyun_mqtt.CMD_buff);
    }

    //接收到PUBACK报文（QoS1）
    else if(Connect_Status && (data[0] == 0x40))
    {
        u1_printf("收到PUBACK!\r\n");
    }

    //接收到d0 00 代表设备保活
    else if(Connect_Status && (datalen == 2) && (data[0] == 0xd0) && (data[1] == 0x00))
    {
        u1_printf("设备存活!\r\n");
    }
}

