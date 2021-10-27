/*
 * Copyright (C) 2016 - 2019 Xilinx, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include "xparameters.h"
#include "netif/xadapter.h"
#include "platform_config.h"
#include "xil_printf.h"
#include "xgpio.h"
#include "xil_types.h"
//#define pmodADC XPAR_PMODADC_0_S00_AXI_BASEADDR
void Task_WEBServer (void *p);
int main_thread();
void print_echo_app_header();
void echo_application_thread(void *);
//void readReg(void *paramters);

void lwip_init();
#define THREAD_STACKSIZE 1024

static struct netif server_netif;
struct netif *echo_netif;
XGpio LED, SW;
QueueHandle_t queueHandle;
//u16 *pmodAdd = (u16*) pmodADC;
//u16 adcdata;
void print_ip(char *msg, ip_addr_t *ip)
{
	xil_printf(msg);
	xil_printf("%d.%d.%d.%d\n\r", ip4_addr1(ip), ip4_addr2(ip),
			ip4_addr3(ip), ip4_addr4(ip));
}

void
print_ip_settings(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw)
{

	print_ip("Board IP: ", ip);
	print_ip("Netmask : ", mask);
	print_ip("Gateway : ", gw);
}


int main()
{
	int status, status2;
	status = XGpio_Initialize(&LED,0);
	status2 = XGpio_Initialize(&SW,1);
	if((status)||(status2)) xil_printf("GPIO Error");
	XGpio_SetDataDirection(&LED, 1, 0x00);
	XGpio_SetDataDirection(&SW, 1, 0xFF);
	sys_thread_new("main_thrd", (void(*)(void*))main_thread, 0,2048, DEFAULT_THREAD_PRIO);
	vTaskStartScheduler();
	while(1);
	return 0;
}

void network_thread(void *p)
{
    struct netif *netif;
    /* the mac address of the board. this should be unique per board */
    unsigned char mac_ethernet_address[] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };
    ip_addr_t ipaddr, netmask, gw;
    netif = &server_netif;

    xil_printf("\r\n\r\n");
    xil_printf("-----lwIP Socket Mode Echo server Demo Application ------\r\n");

    /* initialize IP addresses to be used */
    IP4_ADDR(&ipaddr,  192, 168, 1, 10);
    IP4_ADDR(&netmask, 255, 255, 255,  0);
    IP4_ADDR(&gw,      192, 168, 1, 1);


    /* print out IP settings of the board */

    print_ip_settings(&ipaddr, &netmask, &gw);
    /* print all application headers */

    /* Add network interface to the netif_list, and set it as default */
    if (!xemac_add(netif, &ipaddr, &netmask, &gw, mac_ethernet_address, PLATFORM_EMAC_BASEADDR))
    {
	xil_printf("Error adding N/W interface\r\n");
	return;
    }
    netif_set_default(netif);

    /* specify that the network if is up */
    netif_set_up(netif);

    /* start packet receive thread - required for lwIP operation */
    sys_thread_new("xemacif_input_thread", (void(*)(void*))xemacif_input_thread, netif,2048, DEFAULT_THREAD_PRIO);
    sys_thread_new("httpd", Task_WEBServer,   0,  2048, DEFAULT_THREAD_PRIO);
    sys_thread_new("echod", echo_application_thread, 0,	2048,DEFAULT_THREAD_PRIO);
//    sys_thread_new("readReg", readReg, 0,	2048,1);
//    xTaskCreate(readReg,(char*)"readReg",   2048,  0,1, 0);
    vTaskDelete(NULL);

    return;
}
//void readReg(void *paramters)
//{
//	while(1)
//	{
//		adcdata  = *(pmodAdd)&0xFFF;
//		XGpio_DiscreteWrite(&LED, 1, adcdata);
//		vTaskDelay(10);
//	}
//
////	vTaskDelete(NULL);
//}
int main_thread()
{
	/* initialize lwIP before calling sys_thread_new */
    lwip_init();

    /* any thread using lwIP should be created using sys_thread_new */
    sys_thread_new("NW_THRD", network_thread, NULL,	THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
    vTaskDelete(NULL);
    return 0;
}

