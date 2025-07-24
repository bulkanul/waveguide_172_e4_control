#ifndef SETTING_PROJECT_H
#define SETTING_PROJECT_H

#define PROTOCOL_VERSION "FSLS-2WL-Control-260225\r"

#define RELAY_COUNT		4
#define LAMP_COUNT		3
#define DOOR_COUNT		9

#ifndef __cplusplus
	#define true 1
	#define false 0
#endif
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define DEBUG_ENABLE
#define DEBUG_MESSAGE 1
#define DEBUG_MESSAGE_FORMAT 0 // 0 - tcp,1 - printf

#define DEBUG_OVER_TCP 1

#define SERVER_PORT 7878

#define TCP_QUEUE_BUF_SIZE 1000
#define RECV_OK_TIMEOUT 10000

#define	CAN_IND_REQ 200
#define	ETH_IND_REQ 201

#define INTERFACE_RS232_UART3 300
#define INTERFACE_USB_UART1 301
#define INTERFACE_ETHERNET 302

#endif //SETTING_PROJECT_H
