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

void DTU_Printf(const uint8_t *data,uint16_t datalen)
{
    HAL_UART_Transmit(&DTU_4G_USART,data,datalen,1000);
}

void DTU_Set_ServerInfo(void)
{
    DTU_Printf("+++",sizeof("+++"));
}

/**
 * @brief 处理DTU的串口得到的数据
 * 
 * @param data 数据指针
 * @param datalen 数据长度
 */
void DTU_USART_Event(uint8_t *data,uint16_t datalen)
{

}

