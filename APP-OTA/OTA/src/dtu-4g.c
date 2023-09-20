#include "main.h"
#include "stdio.h"
#include "usart.h"
#include "string.h"
#include "dtu-4g.h"
#include "mqtt.h"

/* ����һ������DMA���ݽ��սṹ�� */
DTU_USART_INFO_T dtu_usart_info = {
    .usDMARxMAXSize = 500,
    .ucDMARxCplt = 0,
}; 

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
        if(dtu_usart_info.ucDMARxCplt)
		{
			dtu_usart_info.ucDMARxCplt = 0;	//��־λ����
            if(dtu_usart_info.ucpDMARxCache[0] == 'a')
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

    //���յ�20 02 00 00����Connect�ɹ�
    else if((datalen == 4) && (data[0] == 0x20) && (data[1] == 0x02) && (data[2] == 0x00) && (data[3] == 0x00))
    {
        u1_printf("Connect�������ɹ�!\r\n");
    }

    //���յ�d0 00 �����豸����
    else if((datalen == 2) && (data[0] == 0xd0) && (data[1] == 0x00))
    {
        u1_printf("�豸���!\r\n");
    }
}

