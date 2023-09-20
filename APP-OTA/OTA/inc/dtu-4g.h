#ifndef __DTU_4G_H_
#define __DTU_4G_H_

/* DTU使用的串口 */
#define DTU_USART huart3

/* 远程服务器配置 */
#define SERVER_CONFIG	"TCP,iot-060a70tq.mqtt.iothub.aliyuncs.com,1883"

/* DTU串口信息结构体 */
typedef struct {

    /* DMA相关信息 */
	uint16_t			usDMARxLength;			/* DMA总接收数据长度(DMA使用) */
	uint16_t			usDMARxMAXSize;			/* DMA接收缓冲区大小(DMA使用) */
	uint8_t				*ucpDMARxCache;			/* DMA接收缓冲区(DMA使用) */

	/* 标志位信息 */
	uint8_t				ucDMARxCplt;			/* DMA接收完成标志(DMA使用) */
}DTU_USART_INFO_T;


/* 澹版槑 */
extern DTU_USART_INFO_T dtu_usart_info;

void DTU_Enter_CMD(void);

void DTU_Exit_CMD(void);

void DTU_Set_Server(void);

void DTU_Usart_Event(uint8_t *data, uint16_t datalen);

#endif // !__DTU-4G_H_

