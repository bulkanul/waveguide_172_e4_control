#ifndef SERVER_DEF
#define SERVER_DEF

#include "global.h"
#ifndef TB_DEF
#include "lwip/tcp.h"
#include "ethernetif.h"
#include "api.h"
#include "lwip/tcp.h"
#include "lwip/stats.h"
#endif
#include "stdio.h"

void server_handler(QueueHandle_t* tcp_rx_data_queue,QueueHandle_t* indication_queue);
struct netconn* getRemoteClient();

#endif //SERVER_DEF
