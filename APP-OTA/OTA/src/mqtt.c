#include "main.h"
#include "stdio.h"
#include "usart.h"
#include "string.h"
#include "mqtt.h"
#include "dtu-4g.h"

MQTT_CB Aliyun_mqtt;

/**
 * @brief MQTT的Connect报文
 * 
 */
void MQTT_ConnectPack(void)
{
    Aliyun_mqtt.MessageID = 1;
    Aliyun_mqtt.Fixed_len = 1;
    Aliyun_mqtt.Variable_len = 10;
    Aliyun_mqtt.Payload_len = 2 + strlen(CLIENTID) + 2 + strlen(USERNAME) + 2 + strlen(PASSWORD);   //其中的2都表示为长度
    Aliyun_mqtt.Remaining_len = Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len;

    /* 固定报头 */
    Aliyun_mqtt.Pack_buff[0] = 0x10;    //connect报头

    //判断剩余长度是一个字节还是两个字节
    do{
        if(Aliyun_mqtt.Remaining_len/128 == 0)
            Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = Aliyun_mqtt.Remaining_len;
        else
            Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len] = (Aliyun_mqtt.Remaining_len%128) | 0x80;

        Aliyun_mqtt.Fixed_len++;
        Aliyun_mqtt.Remaining_len = Aliyun_mqtt.Remaining_len/128;
    }while(Aliyun_mqtt.Remaining_len);

    /* 可变报头 */
    Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len + 0] = 0x00;
    Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len + 1] = 0x04;
    Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len + 2] = 0x4D;    //'M'
    Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len + 3] = 0x51;    //'Q'
    Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len + 4] = 0x54;    //'T'
    Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len + 5] = 0x54;    //'T'
    Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len + 6] = 0x04;    //协议级别04
    Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len + 7] = 0xC2;    //连接标志
    Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len + 8] = 0x00;    //保持连接时间高字节（100）
    Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len + 9] = 0x64;    //保持连接时间低字节（100）

    /* 有效负载 */
    Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len + 10] = strlen(CLIENTID)/256;
    Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len + 11] = strlen(CLIENTID)%256;
    memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len + 12],CLIENTID,strlen(CLIENTID));

    Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len + 12 + strlen(CLIENTID)] = strlen(USERNAME)/256;
    Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len + 13 + strlen(CLIENTID)] = strlen(USERNAME)%256;
    memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len + 14 + strlen(CLIENTID)],USERNAME,strlen(USERNAME));

    Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len + 14 + strlen(CLIENTID) + strlen(USERNAME)] = strlen(PASSWORD)/256;
    Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len + 15 + strlen(CLIENTID) + strlen(USERNAME)] = strlen(PASSWORD)%256;
    memcpy(&Aliyun_mqtt.Pack_buff[Aliyun_mqtt.Fixed_len + 16 + strlen(CLIENTID) + strlen(USERNAME)],PASSWORD,strlen(PASSWORD));

    //测试输出数据
    // for(int i = 0;i < (Aliyun_mqtt.Fixed_len + Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len);i++)
    //     u1_printf("%02x ",Aliyun_mqtt.Pack_buff[i]);
    //HAL_UART_Transmit(&DTU_USART,Aliyun_mqtt.Pack_buff,Aliyun_mqtt.Fixed_len + Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len,0xffff);

    /* 使用DTU发送Connect报文给服务器 */
    if(DTU_SendData(Aliyun_mqtt.Pack_buff,Aliyun_mqtt.Fixed_len + Aliyun_mqtt.Variable_len + Aliyun_mqtt.Payload_len))
        u1_printf("Connect报文发送成功,等待服务器回应\r\n");

}

