#ifndef __MQTT_H_
#define __MQTT_H_

#define CLIENTID    "k08lcwgm0Ts.MQTTtest|securemode=2,signmethod=hmacsha256,timestamp=1695198813604|"
#define USERNAME    "MQTTtest&k08lcwgm0Ts"
#define PASSWORD    "1f9be4385dde450bd8a4059af5ffccd25e2a6e9a672dd666d7fb2513c593419c"

/* MQTT控制结构体 */
typedef struct{
    uint8_t     Pack_buff[512];
    uint16_t    MessageID;
    uint16_t    Fixed_len;          //固定报头长度
    uint16_t    Variable_len;       //可变报头长度
    uint16_t    Payload_len;        //负载报文长度
    uint16_t    Remaining_len;      //剩余长度
}MQTT_CB;


void MQTT_ConnectPack(void);

#endif // !__MQTT_H_

