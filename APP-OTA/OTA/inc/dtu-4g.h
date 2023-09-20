#ifndef __DTU_4G_H_
#define __DTU_4G_H_

/* DTUʹ�õĴ��� */
#define DTU_USART huart3

/* Զ�̷��������� */
#define SERVER_CONFIG	"TCP,iot-060a70tq.mqtt.iothub.aliyuncs.com,1883"

/* DTU������Ϣ�ṹ�� */
typedef struct {

    /* DMA�����Ϣ */
	uint16_t			usDMARxLength;			/* DMA�ܽ������ݳ���(DMAʹ��) */
	uint16_t			usDMARxMAXSize;			/* DMA���ջ�������С(DMAʹ��) */
	uint8_t				*ucpDMARxCache;			/* DMA���ջ�����(DMAʹ��) */

	/* ��־λ��Ϣ */
	uint8_t				ucDMARxCplt;			/* DMA������ɱ�־(DMAʹ��) */
}DTU_USART_INFO_T;


/* 声明 */
extern DTU_USART_INFO_T dtu_usart_info;

void DTU_Enter_CMD(void);

void DTU_Exit_CMD(void);

void DTU_Set_Server(void);

void DTU_Usart_Event(uint8_t *data, uint16_t datalen);

#endif // !__DTU-4G_H_

