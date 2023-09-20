#include "main.h"
#include "stdio.h"
#include "usart.h"
#include "string.h"
#include "dtu-4g.h"
#include "mqtt.h"

/* ����һ������DMA���ݽ��սṹ�� */
DTU_USART_INFO_T dtu_usart_info = {
    .usDMARxMAXSize = 256,
    .ucDMARxCplt = 0,
}; 

/**
 * @brief DTU����ָ��ģʽ
 * 
 */
void DTU_Enter_CMD(void)
{
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
        u1_printf("����ָ��ģʽ�����÷�����\r\n");

        //����������

        //�˳�CMDģʽ
        DTU_Exit_CMD();
    }

    //�˳�ָ��ģʽ��Ӧ��
    else if((datalen == 15) && !memcmp(data,"AT+ENTM\r\n\r\nOK\r\n",sizeof("AT+ENTM\r\n\r\nOK\r\n")))
    {
        u1_printf("�˳�ָ��ģʽ\r\n");
    }
}

