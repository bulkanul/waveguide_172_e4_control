#include "server.h"
#include "tools.h"
extern ip4_addr_t     ipaddr;
extern QueueHandle_t link_queue;

struct netconn *ms_server_inst   = NULL;
struct netconn *ms_remote_client = NULL;

void server_handler(QueueHandle_t* tcp_rx_data_queue,QueueHandle_t* indication_queue){

	char debug_buffer[200];
	
	err_t bind_err   = ERR_CONN;
	err_t accept_err = ERR_CONN;
	err_t listen_err = ERR_CONN;
	err_t recv_err   = ERR_CONN;
	
	struct netbuf* buf;

	uint8_t link_state = 0;
	uint8_t tmp;
	uint32_t recv_ok_timeout_last_tick = 0;
//	char tmp_char;
/* Infinite loop */
	for(;;)
	{
		if( xQueueReceive(link_queue,&tmp,0) == pdPASS )
		{
			debug_message(DEBUG_MESSAGE,debug_buffer,"Ethernet cable state changet to: %i\r\n",tmp);
			link_state = tmp;
		}
		if(link_state == 1)
		{
			if(ms_server_inst == NULL)
			{
				ms_server_inst = netconn_new(NETCONN_TCP);
			}

			if(bind_err != ERR_OK)
			{
				bind_err = netconn_bind(ms_server_inst, NULL, SERVER_PORT);
				debug_message(DEBUG_MESSAGE,debug_buffer,"Bind static IP Address: %u.%u.%u.%u\r\n", (uint8_t)((uint32_t)(ipaddr.addr)&0x000000ff),
																									(uint8_t)(((uint32_t)(ipaddr.addr)&0x0000ff00)>>8),
																									(uint8_t)(((uint32_t)(ipaddr.addr)&0x00ff0000)>>16),
																									(uint8_t)(((uint32_t)(ipaddr.addr)&0xff000000)>>24));
				debug_message(DEBUG_MESSAGE,debug_buffer,"Bind port: %i\r\n",SERVER_PORT);
			}
			else
			{
				if(listen_err != ERR_OK)
				{
					listen_err = netconn_listen(ms_server_inst);
					debug_message(DEBUG_MESSAGE,debug_buffer,"Whait for client connecting to: %i ...\r\n",SERVER_PORT);
				}
				else
				{
					if(accept_err != ERR_OK)
					{
						netconn_set_nonblocking(ms_server_inst,1);
						netconn_set_recvtimeout(ms_server_inst,200);

						accept_err = netconn_accept(ms_server_inst, &ms_remote_client);

						if(accept_err == ERR_OK)
						{
							set_console_mode(DISABLE);
							netconn_set_recvtimeout(ms_remote_client,200);
							debug_message(DEBUG_MESSAGE,debug_buffer,"Connection established whith: %u.%u.%u.%u\r\n", (uint8_t)((uint32_t)(ms_remote_client->pcb.ip->remote_ip.addr)&0x000000ff),
																								  (uint8_t)(((uint32_t)(ms_remote_client->pcb.ip->remote_ip.addr)&0x0000ff00)>>8),
																								  (uint8_t)(((uint32_t)(ms_remote_client->pcb.ip->remote_ip.addr)&0x00ff0000)>>16),
																							      (uint8_t)(((uint32_t)(ms_remote_client->pcb.ip->remote_ip.addr)&0xff000000)>>24));
						  recv_ok_timeout_last_tick = osKernelSysTick();
						}
					}
					else
					{
						set_interface_in(INTERFACE_ETHERNET);
						recv_err = netconn_recv(ms_remote_client, &buf);
						if(buf != NULL)
						{
							if(recv_err == ERR_OK && buf->p->len != 0)
							{
// buffer processing put here begin
								if(uxQueueMessagesWaiting((*tcp_rx_data_queue)) + buf->p->len < TCP_QUEUE_BUF_SIZE )
								for(int i = 0; i < buf->p->len; i ++)
								{
									if(xQueueSend(*tcp_rx_data_queue,(buf->p->payload + i),0) == pdPASS)
									{
// OK processing
									}
									else
									{
// ERROR processing
									}
								}
// buffer processing put here end

								tmp = ETH_IND_REQ;
								if(xQueueSend(*indication_queue,&tmp,0) != pdPASS)
								{
//Обработка ошибки
								}
								netbuf_next(buf);
								netbuf_delete(buf);
								recv_ok_timeout_last_tick = osKernelSysTick();
							}
						}
						else
						{
							if(get_console_mode() == ENABLE)
							{
								if(osKernelSysTick() - recv_ok_timeout_last_tick > 3600000)
								{
									recv_err = ERR_CLSD;
								}
							}
							else
							{
								if(osKernelSysTick() - recv_ok_timeout_last_tick > RECV_OK_TIMEOUT)
								{
									recv_err = ERR_CLSD;
								}
							}
						}

						if(recv_err == ERR_CONN || recv_err == ERR_CLSD)
						{
							debug_message(DEBUG_MESSAGE,debug_buffer,"Client DISCONNECTED\r\n");
							accept_err = recv_err;
//							if(ms_remote_client != NULL)
//							{
								netbuf_delete(buf);
								netconn_close(ms_remote_client);
								netconn_delete(ms_remote_client);
								netconn_close(ms_server_inst);
								netconn_delete(ms_server_inst);
								ms_remote_client = NULL;
								ms_server_inst = NULL;
//							}
							bind_err = ERR_CONN;
							accept_err = ERR_CONN;
							listen_err = ERR_CONN;
							recv_err   = ERR_CONN;
							osDelay(1);
						}
					}
				}
			}
		}
		else
		{
//			if(ms_remote_client != NULL)
//			{
				//struct tcp_pcb* tcp = ms_remote_client->pcb.tcp;
				netbuf_delete(buf);
				netconn_close(ms_remote_client);
				netconn_delete(ms_remote_client);
				netconn_close(ms_server_inst);
				netconn_delete(ms_server_inst);
				ms_remote_client = NULL;
				ms_server_inst = NULL;
				//if(tcp)
				//tcp_abort(tcp);
//			}
			// if(ms_server_inst != NULL)
			// {
			// 	netconn_close(ms_server_inst);
			// 	netconn_delete(ms_server_inst);
			// 	ms_server_inst = NULL;
			// }
			bind_err = ERR_CONN;
			accept_err = ERR_CONN;
			listen_err = ERR_CONN;
			recv_err   = ERR_CONN;
			osDelay(1);
		}
	}
}

void ethernetif_notify_conn_changed(struct netif *netif){
	uint8_t link_state;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if (netif_is_link_up(netif)) {
		link_state = 1;
		netif_set_up(netif);
	} else {
		link_state = 0;
		netif_set_down(netif);
	}
	xQueueSendFromISR(link_queue, &link_state, &xHigherPriorityTaskWoken);
}

struct netconn* getRemoteClient(){
	return ms_remote_client;
}
