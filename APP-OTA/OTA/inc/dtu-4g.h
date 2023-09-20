#ifndef __DTU_4G_H_
#define __DTU_4G_H_

#define DTU_4G_USART huart3

/* 串口接收结构体 */
typedef struct {

    /* DMA相关信息 */
	uint16_t			usDMARxLength;			/* DMA总接收数据长度(DMA使用) */
	uint16_t			usDMARxMAXSize;			/* DMA接收缓冲区大小(DMA使用) */
	uint8_t				*ucpDMARxCache;			/* DMA接收缓冲区(DMA使用) */

	/* 标志位信息 */
	uint8_t				ucDMARxCplt;			/* DMA接收完成标志(DMA使用) */
}DTU_USART_INFO_T;


/* 声明 */
extern DTU_USART_INFO_T dtu_usart_info;

void DTU_Set_ServerInfo(void);

void DTU_USART_Event(uint8_t *data, uint16_t datalen);

#endif // !__DTU-4G_H_

