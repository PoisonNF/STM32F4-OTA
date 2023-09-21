#include "main.h"
#include "stdio.h"
#include "usart.h"
#include "string.h"
#include "dtu-4g.h"
#include "mqtt.h"

/* ���ڱ���Ƿ������Ϸ����� */
static uint8_t Connect_Status = 0; 

/**
 * @brief DTU����ָ��ģʽ
 * 
 */
void DTU_Enter_CMD(void)
{
    u1_printf("����ָ��ģʽ...\r\n");
    while(1)
    {
        u3_printf("+++");
        HAL_Delay(1000);

        /* �ȴ�DTU��Ӧ'a' */
        if(usart_info.ucDMARxCplt)
		{
			usart_info.ucDMARxCplt = 0;	//��־λ����
            if(usart_info.ucpDMARxCache[0] == 'a')
            {
                u1_printf("�˳�͸��ģʽ\r\n");
                u3_printf("a"); //�ظ�DTU
                break;
            }
        }   
    }
}

/**
 * @brief DTU�˳�ָ��ģʽ
 * 
 */
void DTU_Exit_CMD(void)
{
    u3_printf("AT+ENTM\r\n");
}

/**
 * @brief DTU���������������
 * 
 * @param data ����ָ��
 * @param datalen ���ݳ���
 * @return uint8_t 
 */
uint8_t DTU_SendData(uint8_t *data,uint16_t datalen)
{
    HAL_StatusTypeDef ret;
    ret = HAL_UART_Transmit(&DTU_USART,data,datalen,0xffff);
    if(ret == HAL_OK)   return 1;   //���ͳɹ� 
    else return 0;                  //����ʧ��
}

/**
 * @brief DTU����Զ�̷�����
 * 
 */
void DTU_Set_Server(void)
{
    u3_printf("AT+SOCKA=%s\r\n",SERVER_CONFIG);     //���÷�������ַ�Ͷ˿�
    HAL_Delay(30);

    u3_printf("AT+SOCKAEN=ON\r\n");                 //����SOCKA
    HAL_Delay(30);

    u3_printf("AT+SOCKBEN=OFF\r\n");                //�ر�SOCKB
    HAL_Delay(30);

    u3_printf("AT+SOCKCEN=OFF\r\n");                //�ر�SOCKC
    HAL_Delay(30);
    
    u3_printf("AT+SOCKDEN=OFF\r\n");                //�ر�SOCKD
    HAL_Delay(30);

    u3_printf("AT+HEART=ON,NET,USER,60,C000\r\n");  //��������
    HAL_Delay(30);

    u3_printf("AT+S\r\n");                          //��������
    HAL_Delay(30);
}

/**
 * @brief ����DTU�Ĵ��ڵõ�������
 * 
 * @param data ����ָ��
 * @param datalen ���ݳ���
 */
void DTU_Usart_Event(uint8_t *data,uint16_t datalen)
{
    //����ָ��ģʽ��Ӧ��
    if((datalen == 5) && !memcmp(data,"+ok\r\n",sizeof("+ok\r\n")))
    {
        u1_printf("����ָ��ģʽ�����÷�����...\r\n");

        //����������
        DTU_Set_Server();

        //�˳�CMDģʽ
        DTU_Exit_CMD();
    }

    //�˳�ָ��ģʽ��Ӧ��
    else if((datalen == 15) && !memcmp(data,"AT+ENTM\r\r\nOK\r\n",sizeof("AT+ENTM\r\r\nOK\r\n")))
    {
        u1_printf("�˳�ָ��ģʽ!\r\n");
    }

    //���÷��������
    else if(!memcmp(data,"USR-DR152",sizeof("USR-DR152")))
    {
        u1_printf("Connect������...\r\n");
        MQTT_ConnectPack();
    }

    //���յ�CONNACK����
    else if((datalen == 4) && (data[0] == 0x20))
    {
        u1_printf("�յ�CONNACK!\r\n");
        if(data[3] == 0x00){
            u1_printf("Connect�������ɹ�!\r\n");
            Connect_Status= 1;      //���ӳɹ���־λ
            MQTT_SubscribePack("/k08lcwgm0Ts/MQTTtest/user/get");
            MQTT_SubscribePack("/ota/device/upgrade/k08lcwgm0Ts/MQTTtest");
            HAL_Delay(500);
            MQTT_SendOTAVersion();  //���͵�ǰOTA�İ汾
        }else{
            u1_printf("Connect������ʧ��!\r\n");
            NVIC_SystemReset();
        }
    }

    //���յ�SUBACK����
    else if(Connect_Status &&(datalen == 5) && (data[0] == 0x90))
    {
        u1_printf("�յ�SUBACK!\r\n");

        //������һ���ֽ�Ϊ0x00����0x01�����ͳɹ�
        if((data[datalen-1] == 0x00) || (data[datalen-1] == 0x01)){
            u1_printf("Subscribe���ĳɹ�!\r\n");
            //MQTT_UnSubscribePack("/sys/k08lcwgm0Ts/MQTTtest/thing/service/property/set");
        }else{
            u1_printf("Subscribe����ʧ��!\r\n");
            NVIC_SystemReset();
        }
    }

    //���յ�UNSUBACK����
    else if(Connect_Status && (data[0] == 0xB0) && data[1] == 0x02)
    {
        u1_printf("�յ�UNSUBACK!\r\n");
        u1_printf("UnSubscribe���ĳɹ�!\r\n");
    }

    //���յ��ȼ�0��Publish����
    else if(Connect_Status && (data[0] == 0x30))
    {
        u1_printf("�յ��ȼ�0��Publish����!\r\n");
        MQTT_DealPublishData(data,datalen);
        if(strstr((const char*)Aliyun_mqtt.CMD_buff,"{\"switch1\":0}"))
        {
            u1_printf("�رտ���\r\n");
            MQTT_PublishDataQos0("/k08lcwgm0Ts/MQTTtest/user/post","{\"params\":}{\"switch1\":0}");
        }
        if(strstr((const char*)Aliyun_mqtt.CMD_buff,"{\"switch1\":1}"))
        {
            u1_printf("�򿪿���\r\n");
            MQTT_PublishDataQos0("/k08lcwgm0Ts/MQTTtest/user/post","{\"params\":}{\"switch1\":1}");
        }

        MQTT_GetOTAInfo((char*)Aliyun_mqtt.CMD_buff);
    }

    //���յ�PUBACK���ģ�QoS1��
    else if(Connect_Status && (data[0] == 0x40))
    {
        u1_printf("�յ�PUBACK!\r\n");
    }

    //���յ�d0 00 �����豸����
    else if(Connect_Status && (datalen == 2) && (data[0] == 0xd0) && (data[1] == 0x00))
    {
        u1_printf("�豸���!\r\n");
    }
}

